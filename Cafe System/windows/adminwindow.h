#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

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
    QWidget *m_productsPage;
    QWidget *m_ordersPage;
    QWidget *m_inventoryPage;
    QWidget *m_employeesPage;
    QWidget *m_reportsPage;
    QWidget *m_settingsPage;

    QString m_username;
    QString m_role;
};

#endif // ADMINWINDOW_H