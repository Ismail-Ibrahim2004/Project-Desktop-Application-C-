#include "sidebar.h"
#include <QPainter>
#include <QFont>
#include <QHBoxLayout>

// =====================================================
// SidebarButton
// =====================================================

SidebarButton::SidebarButton(const QString &iconPath, const QString &text, QWidget *parent)
    : QPushButton(parent), m_iconPath(iconPath)
{
    setFixedHeight(48);
    setCursor(Qt::PointingHandCursor);
    setCheckable(true);
    setLayoutDirection(Qt::LeftToRight);

    // Icon من الـ Resources
    if (!iconPath.isEmpty()) {
        setIcon(QIcon(iconPath));
        setIconSize(QSize(20, 20));
    }

    setText("  " + text);

    QFont btnFont("Segoe UI", 11);
    btnFont.setWeight(QFont::Medium);
    setFont(btnFont);

    setActive(false);
}

void SidebarButton::setActive(bool active)
{
    m_active = active;

    if (active) {
        setStyleSheet(R"(
            QPushButton {
                border: none;
                border-radius: 10px;
                padding-left: 14px;
                padding-right: 10px;
                text-align: left;
                color: #FFFFFF;
                background-color: #E8882F;
                font-weight: 600;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #E8882F;
            }
        )");
    } else {
        setStyleSheet(R"(
            QPushButton {
                border: none;
                border-radius: 10px;
                padding-left: 14px;
                padding-right: 10px;
                text-align: left;
                color: #FFFFFF;
                background-color: transparent;
                font-weight: 500;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: rgba(255, 255, 255, 0.10);
            }
        )");
    }
    update();
}

void SidebarButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);
}

// =====================================================
// Sidebar
// =====================================================

Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void Sidebar::setupUI()
{
    setFixedWidth(265);
    setLayoutDirection(Qt::LeftToRight);

    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor("#6B4423"));
    setPalette(pal);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 22, 15, 22);
    mainLayout->setSpacing(5);

    // ============================================
    // Logo Section
    // ============================================
    QWidget *logoContainer = new QWidget();
    logoContainer->setStyleSheet("background: transparent;");
    QHBoxLayout *logoLayout = new QHBoxLayout(logoContainer);
    logoLayout->setContentsMargins(5, 5, 5, 5);
    logoLayout->setSpacing(12);

    // Logo Icon from Resources
    QLabel *logoIcon = new QLabel();
    logoIcon->setFixedSize(45, 45);
    logoIcon->setAlignment(Qt::AlignCenter);
    logoIcon->setPixmap(
        QPixmap(":/icons/coffee-cup (1).png")
            .scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    logoIcon->setStyleSheet(R"(
        QLabel {
            background-color: #E8882F;
            border-radius: 10px;
            padding: 6px;
        }
    )");

    // Title Container
    QWidget *textContainer = new QWidget();
    textContainer->setStyleSheet("background: transparent;");
    QVBoxLayout *logoTextLayout = new QVBoxLayout(textContainer);
    logoTextLayout->setContentsMargins(0, 0, 0, 0);
    logoTextLayout->setSpacing(0);

    QLabel *titleLabel = new QLabel("Cafe Manager");
    QFont titleFont("Segoe UI", 14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #FFFFFF; background: transparent;");

    QLabel *subtitleLabel = new QLabel("Admin");
    QFont subFont("Segoe UI", 10);
    subtitleLabel->setFont(subFont);
    subtitleLabel->setStyleSheet("color: rgba(255, 255, 255, 0.7); background: transparent;");

    logoTextLayout->addWidget(titleLabel);
    logoTextLayout->addWidget(subtitleLabel);

    logoLayout->addWidget(logoIcon);
    logoLayout->addWidget(textContainer);
    logoLayout->addStretch();

    mainLayout->addWidget(logoContainer);
    mainLayout->addSpacing(30);

    // ============================================
    // Menu Buttons with Real Icons
    // ============================================
    m_menuLayout = new QVBoxLayout();
    m_menuLayout->setSpacing(4);
    m_menuLayout->setContentsMargins(0, 0, 0, 0);

    createMenuButton(":/icons/dashboard.png",           "Dashboard",  Dashboard);
    createMenuButton(":/icons/product-management.png",  "Products",   Products);
    createMenuButton(":/icons/checkout.png",            "Orders",     Orders);
    createMenuButton(":/icons/product.png",             "Inventory",  Inventory);
    createMenuButton(":/icons/employee.png",            "Employees",  Employees);
    createMenuButton(":/icons/bar-chart.png",           "Reports",    Reports);
    createMenuButton(":/icons/settings.png",            "Settings",   Settings);

    mainLayout->addLayout(m_menuLayout);

    if (!m_buttons.isEmpty()) {
        m_buttons[0]->setActive(true);
    }

    mainLayout->addStretch();

    // ============================================
    // Logout Button with Icon
    // ============================================
    QPushButton *logoutBtn = new QPushButton("  Logout");
    logoutBtn->setFixedHeight(48);
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setIcon(QIcon(":/icons/logout.png"));
    logoutBtn->setIconSize(QSize(20, 20));

    QFont logoutFont("Segoe UI", 11);
    logoutFont.setWeight(QFont::Medium);
    logoutBtn->setFont(logoutFont);

    logoutBtn->setStyleSheet(R"(
        QPushButton {
            border: none;
            border-radius: 10px;
            padding-left: 14px;
            padding-right: 10px;
            text-align: left;
            color: #FFFFFF;
            background-color: transparent;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: rgba(255, 100, 100, 0.20);
            color: #FFAAAA;
        }
    )");

    connect(logoutBtn, &QPushButton::clicked, this, &Sidebar::logoutClicked);
    mainLayout->addWidget(logoutBtn);
}

void Sidebar::createMenuButton(const QString &iconPath, const QString &text, Page page)
{
    SidebarButton *btn = new SidebarButton(iconPath, text);
    m_buttons.append(btn);
    m_menuLayout->addWidget(btn);

    connect(btn, &QPushButton::clicked, this, [this, page]() {
        setActivePage(page);
        emit pageChanged(static_cast<int>(page));
    });
}

void Sidebar::setActivePage(Page page)
{
    m_currentPage = page;
    for (int i = 0; i < m_buttons.size(); i++) {
        m_buttons[i]->setActive(i == static_cast<int>(page));
    }
}