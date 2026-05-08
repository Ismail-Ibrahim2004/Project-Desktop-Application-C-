#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "reportspage.h"

#include <QApplication>
#include <QScreen>
#include <QStatusBar>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // إخفاء Status Bar و Menu Bar
    if (statusBar()) statusBar()->hide();
    if (menuBar()) menuBar()->hide();

    setWindowTitle("Café Manager");
    resize(900, 700);  // ✨ الحجم الجديد

    // توسيط النافذة في الشاشة
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    setupUI();
    applyStyles();

    // اختيار Reports افتراضياً
    m_reportsBtn->setChecked(true);
    m_contentStack->setCurrentIndex(5);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QHBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    setupSidebar();
    setupContentArea();
}

void MainWindow::setupSidebar()
{
    m_sidebar = new QFrame(this);
    m_sidebar->setObjectName("sidebar");
    m_sidebar->setFixedWidth(200);  // ✨ تصغير الـ Sidebar

    QVBoxLayout *sidebarLayout = new QVBoxLayout(m_sidebar);
    sidebarLayout->setContentsMargins(12, 18, 12, 18);
    sidebarLayout->setSpacing(6);

    // ===== Logo Section =====
    QWidget *logoWidget = new QWidget();
    QHBoxLayout *logoLayout = new QHBoxLayout(logoWidget);
    logoLayout->setContentsMargins(3, 0, 3, 0);
    logoLayout->setSpacing(10);

    QLabel *logoIcon = new QLabel("☕");
    logoIcon->setObjectName("logoIcon");
    logoIcon->setFixedSize(40, 40);
    logoIcon->setAlignment(Qt::AlignCenter);

    QWidget *logoTextWidget = new QWidget();
    QVBoxLayout *logoTextLayout = new QVBoxLayout(logoTextWidget);
    logoTextLayout->setContentsMargins(0, 0, 0, 0);
    logoTextLayout->setSpacing(0);

    QLabel *logoTitle = new QLabel("Café Manager");
    logoTitle->setObjectName("logoTitle");

    QLabel *logoSubtitle = new QLabel("Admin");
    logoSubtitle->setObjectName("logoSubtitle");

    logoTextLayout->addWidget(logoTitle);
    logoTextLayout->addWidget(logoSubtitle);

    logoLayout->addWidget(logoIcon);
    logoLayout->addWidget(logoTextWidget);
    logoLayout->addStretch();

    sidebarLayout->addWidget(logoWidget);
    sidebarLayout->addSpacing(20);

    // ===== Menu Buttons =====
    m_menuButtonGroup = new QButtonGroup(this);
    m_menuButtonGroup->setExclusive(true);

    m_dashboardBtn = createMenuButton("Dashboard", "▦");
    m_productsBtn  = createMenuButton("Products", "🛍");
    m_ordersBtn    = createMenuButton("Orders", "🛒");
    m_inventoryBtn = createMenuButton("Inventory", "📦");
    m_employeesBtn = createMenuButton("Employees", "👥");
    m_reportsBtn   = createMenuButton("Reports", "📊");
    m_settingsBtn  = createMenuButton("Settings", "⚙");

    sidebarLayout->addWidget(m_dashboardBtn);
    sidebarLayout->addWidget(m_productsBtn);
    sidebarLayout->addWidget(m_ordersBtn);
    sidebarLayout->addWidget(m_inventoryBtn);
    sidebarLayout->addWidget(m_employeesBtn);
    sidebarLayout->addWidget(m_reportsBtn);
    sidebarLayout->addWidget(m_settingsBtn);

    m_menuButtonGroup->addButton(m_dashboardBtn, 0);
    m_menuButtonGroup->addButton(m_productsBtn, 1);
    m_menuButtonGroup->addButton(m_ordersBtn, 2);
    m_menuButtonGroup->addButton(m_inventoryBtn, 3);
    m_menuButtonGroup->addButton(m_employeesBtn, 4);
    m_menuButtonGroup->addButton(m_reportsBtn, 5);
    m_menuButtonGroup->addButton(m_settingsBtn, 6);

    connect(m_menuButtonGroup, &QButtonGroup::idClicked,
            this, &MainWindow::onMenuButtonClicked);

    sidebarLayout->addStretch();

    m_logoutBtn = new QPushButton("⎋  Logout");
    m_logoutBtn->setObjectName("logoutBtn");
    m_logoutBtn->setCursor(Qt::PointingHandCursor);
    m_logoutBtn->setFixedHeight(40);

    connect(m_logoutBtn, &QPushButton::clicked,
            this, &MainWindow::onLogoutClicked);

    sidebarLayout->addWidget(m_logoutBtn);

    m_mainLayout->addWidget(m_sidebar);
}

QPushButton* MainWindow::createMenuButton(const QString &text, const QString &iconChar)
{
    QPushButton *btn = new QPushButton(QString("  %1   %2").arg(iconChar, text));
    btn->setObjectName("menuButton");
    btn->setCheckable(true);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedHeight(38);  // ✨ تصغير شوية
    return btn;
}

void MainWindow::setupContentArea()
{
    m_contentStack = new QStackedWidget(this);
    m_contentStack->setObjectName("contentStack");

    QStringList pageNames = {"Dashboard", "Products", "Orders", "Inventory", "Employees"};
    for (const QString &name : pageNames) {
        QWidget *page = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(page);
        QLabel *label = new QLabel(name + " - Coming Soon");
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 22px; color: #8B6F47; font-weight: bold;");
        layout->addWidget(label);
        m_contentStack->addWidget(page);
    }

    // ✨ Reports Page
    ReportsPage *reportsPage = new ReportsPage(this);
    m_contentStack->addWidget(reportsPage);

    // Settings (placeholder)
    QWidget *settingsPage = new QWidget();
    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsPage);
    QLabel *settingsLabel = new QLabel("Settings - Coming Soon");
    settingsLabel->setAlignment(Qt::AlignCenter);
    settingsLabel->setStyleSheet("font-size: 22px; color: #8B6F47; font-weight: bold;");
    settingsLayout->addWidget(settingsLabel);
    m_contentStack->addWidget(settingsPage);

    m_mainLayout->addWidget(m_contentStack);
}

void MainWindow::onMenuButtonClicked(int index)
{
    m_contentStack->setCurrentIndex(index);
}

void MainWindow::onLogoutClicked()
{
    close();
}

void MainWindow::applyStyles()
{
    QString styles = R"(
        QMainWindow {
            background-color: #FAF6F1;
        }

        #sidebar {
            background-color: #8B5A2B;
            border: none;
        }

        #logoIcon {
            background-color: #E67E22;
            border-radius: 8px;
            color: white;
            font-size: 18px;
        }

        #logoTitle {
            color: white;
            font-size: 14px;
            font-weight: bold;
        }

        #logoSubtitle {
            color: rgba(255, 255, 255, 0.7);
            font-size: 11px;
        }

        #menuButton {
            background-color: transparent;
            color: rgba(255, 255, 255, 0.85);
            border: none;
            border-radius: 8px;
            text-align: left;
            padding-left: 10px;
            font-size: 13px;
            font-weight: 500;
        }

        #menuButton:hover {
            background-color: rgba(255, 255, 255, 0.08);
            color: white;
        }

        #menuButton:checked {
            background-color: #E67E22;
            color: white;
            font-weight: bold;
        }

        #logoutBtn {
            background-color: transparent;
            color: rgba(255, 255, 255, 0.85);
            border: none;
            border-top: 1px solid rgba(255, 255, 255, 0.1);
            text-align: left;
            padding-left: 12px;
            font-size: 13px;
        }

        #logoutBtn:hover {
            color: white;
            background-color: rgba(255, 255, 255, 0.05);
        }

        #contentStack {
            background-color: #FAF6F1;
        }
    )";

    setStyleSheet(styles);
}