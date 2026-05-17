#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H
#include "../pages/reportspage.h"
#include "../pages/productspage.h"
#include "../pages/inventorypage.h"
#include "../pages/Orderspage.h"
#include "../pages/settingspage.h"
#include <QMainWindow>
#include <QStackedWidget>
#include "../widgets/sidebar.h"
#include "../pages/dashboardpage.h"

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();

    void setUserInfo(const QString &username, const QString &role);

private slots:
    void onPageChanged(int pageIndex);
    void onLogout();

private:
    void setupUI();
    QWidget* createPlaceholderPage(const QString &pageName);

    Sidebar *m_sidebar;
    QStackedWidget *m_stackedWidget;
    DashboardPage *m_dashboardPage;
    ProductsPage *m_productsPage;
    OrdersPage *m_ordersPage;
    InventoryPage *m_inventoryPage;
    QWidget *m_employeesPage;
    ReportsPage *m_reportsPage;
    SettingsPage *m_settingsPage;
    QString m_username;
    QString m_role;
};

#endif // ADMINWINDOW_H