#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupSidebar();
    void setupMainContent();
    void populateTableData();
    QWidget* createStatusBadge(const QString &status);

    // عناصر الواجهة الأساسية
    QWidget *centralWidget;
    QWidget *sidebar;
    QWidget *mainContentArea;
    QTableWidget *orderTable;
    QLineEdit *searchField;
    QComboBox *statusFilter;
};

#endif // MAINWINDOW_H