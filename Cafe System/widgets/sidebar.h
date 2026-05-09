#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QList>

class SidebarButton : public QPushButton
{
    Q_OBJECT
public:
    SidebarButton(const QString &iconPath, const QString &text, QWidget *parent = nullptr);
    void setActive(bool active);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool m_active = false;
    QString m_iconPath;
};

class Sidebar : public QWidget
{
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);

    enum Page {
        Dashboard = 0,
        Products,
        Orders,
        Inventory,
        Employees,
        Reports,
        Settings
    };

    void setActivePage(Page page);

signals:
    void pageChanged(int pageIndex);
    void logoutClicked();

private:
    void setupUI();
    void createMenuButton(const QString &icon, const QString &text, Page page);

    QVBoxLayout *m_menuLayout;
    QList<SidebarButton*> m_buttons;
    Page m_currentPage = Dashboard;
};

#endif // SIDEBAR_H