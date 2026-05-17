#include "adminwindow.h"
#include "../database/databasemanager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QApplication>
#include "pages/employeespage.h"

AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();

    setWindowTitle("Cafe Manager - Admin Panel");
    resize(1400, 850);
    setMinimumSize(1200, 700);

    setStyleSheet("QMainWindow { background-color: #FAF6F1; }");
}

AdminWindow::~AdminWindow() {}

void AdminWindow::setUserInfo(const QString &username, const QString &role)
{
    m_username = username;
    m_role = role;
    setWindowTitle(QString("Cafe Manager - %1 (%2)").arg(username, role));
}

void AdminWindow::setupUI()
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
    m_productsPage = new ProductsPage();
    m_ordersPage = new OrdersPage();
    m_inventoryPage = new InventoryPage();
    m_employeesPage = new EmployeesPage();
    m_reportsPage = new ReportsPage();
    m_settingsPage = new SettingsPage();

    m_stackedWidget->addWidget(m_dashboardPage);
    m_stackedWidget->addWidget(m_productsPage);
    m_stackedWidget->addWidget(m_ordersPage);
    m_stackedWidget->addWidget(m_inventoryPage);
    m_stackedWidget->addWidget(m_employeesPage);
    m_stackedWidget->addWidget(m_reportsPage);
    m_stackedWidget->addWidget(m_settingsPage);

    m_stackedWidget->setCurrentIndex(0);
    mainLayout->addWidget(m_stackedWidget);

    setCentralWidget(centralWidget);

    // Connect signals
    connect(m_sidebar, &Sidebar::pageChanged, this, &AdminWindow::onPageChanged);
    connect(m_sidebar, &Sidebar::logoutClicked, this, &AdminWindow::onLogout);
}

void AdminWindow::onPageChanged(int pageIndex)
{
    if (pageIndex >= 0 && pageIndex < m_stackedWidget->count()) {
        m_stackedWidget->setCurrentIndex(pageIndex);

        if (pageIndex == 0) {
            m_dashboardPage->refreshData();
        }
    }
}

void AdminWindow::onLogout()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Logout",
        "Are you sure you want to logout?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        this->close();
    }
}

QWidget* AdminWindow::createPlaceholderPage(const QString &pageName)
{
    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #FAF6F1;");

    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *label = new QLabel(pageName);
    label->setStyleSheet("font-size: 36px; font-weight: bold; "
                         "color: #CCCCCC; background: transparent;");
    label->setAlignment(Qt::AlignCenter);

    QLabel *sublabel = new QLabel("Coming Soon...");
    sublabel->setStyleSheet("font-size: 16px; color: #DDDDDD; "
                            "background: transparent;");
    sublabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(label);
    layout->addWidget(sublabel);

    return page;
}