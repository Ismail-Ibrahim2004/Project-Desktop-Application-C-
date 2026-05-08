#pragma once

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include "database.h"

class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(QWidget *parent = nullptr);

    void refresh();

private:
    void setupUi();
    QWidget* buildStatCard(const QString &title, const QString &objectName,
                           QLabel *&valueOut);
    QWidget* buildRoleRow(const QString &role, int count, int total);
    QWidget* buildRecentTable(const QList<Employee> &employees);

    QWidget *m_rolesSection   = nullptr;
    QWidget *m_recentSection  = nullptr;

    QLabel  *m_totalVal    = nullptr;
    QLabel  *m_activeVal   = nullptr;
    QLabel  *m_inactiveVal = nullptr;
    QLabel  *m_rolesVal    = nullptr;

    QVBoxLayout *m_rolesLayout  = nullptr;
    QVBoxLayout *m_recentLayout = nullptr;
};
