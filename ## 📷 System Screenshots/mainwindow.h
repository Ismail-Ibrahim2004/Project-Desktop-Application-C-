#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QButtonGroup>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onMenuButtonClicked(int index);
    void onLogoutClicked();

private:
    Ui::MainWindow *ui;

    QWidget *m_centralWidget;
    QHBoxLayout *m_mainLayout;

    QFrame *m_sidebar;
    QButtonGroup *m_menuButtonGroup;
    QPushButton *m_dashboardBtn;
    QPushButton *m_productsBtn;
    QPushButton *m_ordersBtn;
    QPushButton *m_inventoryBtn;
    QPushButton *m_employeesBtn;
    QPushButton *m_reportsBtn;
    QPushButton *m_settingsBtn;
    QPushButton *m_logoutBtn;

    QStackedWidget *m_contentStack;

    void setupUI();
    void setupSidebar();
    void setupContentArea();
    void applyStyles();
    QPushButton* createMenuButton(const QString &text, const QString &iconChar);
};

#endif // MAINWINDOW_H