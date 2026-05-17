#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "widgets/sidebar.h"
#include "pages/dashboardpage.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPageChanged(int pageIndex);
    void onLogout();

private:
    void setupUI();
    void connectDatabase();

    Sidebar *m_sidebar;
    QStackedWidget *m_stackedWidget;
    DashboardPage *m_dashboardPage;

    // Placeholder pages (to be implemented later)
    QWidget *m_productsPage;
    QWidget *m_ordersPage;
    QWidget *m_inventoryPage;
    QWidget *m_employeesPage;
    QWidget *m_reportsPage;
    QWidget *m_settingsPage;

    QWidget* createPlaceholderPage(const QString &pageName);
};

#endif // MAINWINDOW_H