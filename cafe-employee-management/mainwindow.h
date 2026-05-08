#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMap>
#include "database.h"
#include "dashboardwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onAddEmployee();
    void onEditEmployee(const QString &id);
    void onDeleteEmployee(const QString &id);
    void onSearch(const QString &text);
    void refreshTable(const QList<Employee> &employees);
    void showPage(const QString &pageName);

private:
    void setupUi();
    QWidget*      buildSidebar();
    QWidget*      buildEmployeesPage();
    QWidget*      buildHeader();
    QWidget*      buildSearchBar();
    QTableWidget* buildTable();

    void populateRow(int row, const Employee &emp);
    QPushButton* makeNavButton(const QString &label);
    void setActiveNav(const QString &pageName);

    QStackedWidget  *m_stack       = nullptr;
    DashboardWidget *m_dashboard   = nullptr;
    QTableWidget    *m_table       = nullptr;
    QLineEdit       *m_search      = nullptr;

    QMap<QString, QPushButton*> m_navButtons;
    QString m_activePage;
};
