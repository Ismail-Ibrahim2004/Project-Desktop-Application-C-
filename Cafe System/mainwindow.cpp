#include "mainwindow.h"
#include "database/databasemanager.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    connectDatabase();
    setupUI();

    setWindowTitle("Café Manager");
    resize(1400, 850);
    setMinimumSize(1200, 700);

    // Set window style
    setStyleSheet("QMainWindow { background-color: #FAF6F1; }");
}

MainWindow::~MainWindow()
{
    DatabaseManager::instance().closeConnection();
}

void MainWindow::connectDatabase()
{
    DatabaseManager &db = DatabaseManager::instance();

    // Try to connect - will use demo data if connection fails
    bool connected = db.connectToDatabase(
        "localhost",          // SQL Server instance
        "CafeManagerDB",     // Database name
        "",                  // Username (empty = Windows Auth)
        ""                   // Password
        );

    if (!connected) {
        qWarning() << "Running with demo data - database not connected";
        // Don't show error to user - just use demo data
    }
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar
    m_sidebar = new Sidebar();
    mainLayout->addWidget(m_sidebar);

    // Stacked Widget for pages
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->setStyleSheet("background-color: #FAF6F1;");

    // Create pages
    m_dashboardPage = new DashboardPage();
    m_productsPage = createPlaceholderPage("Products");
    m_ordersPage = createPlaceholderPage("Orders");
    m_inventoryPage = createPlaceholderPage("Inventory");
    m_employeesPage = createPlaceholderPage("Employees");
    m_reportsPage = createPlaceholderPage("Reports");
    m_settingsPage = createPlaceholderPage("Settings");

    m_stackedWidget->addWidget(m_dashboardPage);   // 0
    m_stackedWidget->addWidget(m_productsPage);     // 1
    m_stackedWidget->addWidget(m_ordersPage);       // 2
    m_stackedWidget->addWidget(m_inventoryPage);    // 3
    m_stackedWidget->addWidget(m_employeesPage);    // 4
    m_stackedWidget->addWidget(m_reportsPage);      // 5
    m_stackedWidget->addWidget(m_settingsPage);     // 6

    m_stackedWidget->setCurrentIndex(0);

    mainLayout->addWidget(m_stackedWidget);

    setCentralWidget(centralWidget);

    // Connect signals
    connect(m_sidebar, &Sidebar::pageChanged, this, &MainWindow::onPageChanged);
    connect(m_sidebar, &Sidebar::logoutClicked, this, &MainWindow::onLogout);
}

void MainWindow::onPageChanged(int pageIndex)
{
    if (pageIndex >= 0 && pageIndex < m_stackedWidget->count()) {
        m_stackedWidget->setCurrentIndex(pageIndex);

        // Refresh data when switching to dashboard
        if (pageIndex == 0) {
            m_dashboardPage->refreshData();
        }
    }
}

void MainWindow::onLogout()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Logout",
        "Are you sure you want to logout?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        this->close();   // ⭐ هيقفل النافذة بس، والـ Login هيظهر تلقائياً
    }
}

QWidget* MainWindow::createPlaceholderPage(const QString &pageName)
{
    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #FAF6F1;");

    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *label = new QLabel(pageName);
    label->setStyleSheet(R"(
        font-size: 36px;
        font-weight: bold;
        color: #CCCCCC;
        background: transparent;
    )");
    label->setAlignment(Qt::AlignCenter);

    QLabel *sublabel = new QLabel("Coming Soon...");
    sublabel->setStyleSheet(R"(
        font-size: 16px;
        color: #DDDDDD;
        background: transparent;
    )");
    sublabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(label);
    layout->addWidget(sublabel);

    return page;
}