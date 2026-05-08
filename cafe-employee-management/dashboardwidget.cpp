#include "dashboardwidget.h"
#include <QScrollArea>
#include <QSizePolicy>

DashboardWidget::DashboardWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    refresh();
}

void DashboardWidget::setupUi()
{
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(24);

    // ── Page header ──────────────────────────────────────
    QWidget *header = new QWidget;
    QHBoxLayout *hLay = new QHBoxLayout(header);
    hLay->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *titleLay = new QVBoxLayout;
    titleLay->setSpacing(4);
    QLabel *title = new QLabel("Dashboard");
    title->setObjectName("pageTitle");
    QLabel *sub = new QLabel("Overview of your cafe staff");
    sub->setObjectName("pageSubtitle");
    titleLay->addWidget(title);
    titleLay->addWidget(sub);

    hLay->addLayout(titleLay);
    hLay->addStretch();
    root->addWidget(header);

    // ── Stat cards row ───────────────────────────────────
    QWidget *cards = new QWidget;
    QHBoxLayout *cLay = new QHBoxLayout(cards);
    cLay->setContentsMargins(0, 0, 0, 0);
    cLay->setSpacing(16);

    cLay->addWidget(buildStatCard("Total Employees", "cardTotal",    m_totalVal));
    cLay->addWidget(buildStatCard("Active Staff",    "cardActive",   m_activeVal));
    cLay->addWidget(buildStatCard("Inactive",        "cardInactive", m_inactiveVal));
    cLay->addWidget(buildStatCard("Roles",           "cardRoles",    m_rolesVal));

    root->addWidget(cards);

    // ── Bottom two columns ───────────────────────────────
    QWidget *bottom = new QWidget;
    QHBoxLayout *bLay = new QHBoxLayout(bottom);
    bLay->setContentsMargins(0, 0, 0, 0);
    bLay->setSpacing(20);

    // Roles breakdown panel
    m_rolesSection = new QWidget;
    m_rolesSection->setObjectName("dashPanel");
    m_rolesLayout = new QVBoxLayout(m_rolesSection);
    m_rolesLayout->setContentsMargins(20, 20, 20, 20);
    m_rolesLayout->setSpacing(14);
    QLabel *rolesTitle = new QLabel("Staff by Role");
    rolesTitle->setObjectName("panelTitle");
    m_rolesLayout->addWidget(rolesTitle);
    bLay->addWidget(m_rolesSection, 1);

    // Recent employees panel
    m_recentSection = new QWidget;
    m_recentSection->setObjectName("dashPanel");
    m_recentLayout = new QVBoxLayout(m_recentSection);
    m_recentLayout->setContentsMargins(20, 20, 20, 20);
    m_recentLayout->setSpacing(10);
    QLabel *recentTitle = new QLabel("Employee Roster");
    recentTitle->setObjectName("panelTitle");
    m_recentLayout->addWidget(recentTitle);
    bLay->addWidget(m_recentSection, 1);

    root->addWidget(bottom, 1);
}

QWidget* DashboardWidget::buildStatCard(const QString &title,
                                        const QString &objectName,
                                        QLabel *&valueOut)
{
    QWidget *card = new QWidget;
    card->setObjectName(objectName);

    QVBoxLayout *lay = new QVBoxLayout(card);
    lay->setContentsMargins(20, 20, 20, 20);
    lay->setSpacing(8);

    valueOut = new QLabel("—");
    valueOut->setObjectName("cardValue");

    QLabel *lbl = new QLabel(title);
    lbl->setObjectName("cardLabel");

    lay->addWidget(valueOut);
    lay->addWidget(lbl);
    lay->addStretch();

    card->setMinimumHeight(110);
    return card;
}

QWidget* DashboardWidget::buildRoleRow(const QString &role, int count, int total)
{
    QWidget *row = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(row);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(4);

    QHBoxLayout *topRow = new QHBoxLayout;
    QLabel *roleLbl = new QLabel(role);
    roleLbl->setObjectName("roleRowLabel");
    QLabel *cntLbl = new QLabel(QString::number(count));
    cntLbl->setObjectName("roleRowCount");
    topRow->addWidget(roleLbl);
    topRow->addStretch();
    topRow->addWidget(cntLbl);

    QProgressBar *bar = new QProgressBar;
    bar->setObjectName("roleBar");
    bar->setRange(0, qMax(total, 1));
    bar->setValue(count);
    bar->setTextVisible(false);
    bar->setFixedHeight(8);

    lay->addLayout(topRow);
    lay->addWidget(bar);
    return row;
}

void DashboardWidget::refresh()
{
    EmployeeStats stats = Database::instance().fetchStats();

    m_totalVal->setText(QString::number(stats.total));
    m_activeVal->setText(QString::number(stats.active));
    m_inactiveVal->setText(QString::number(stats.inactive));
    m_rolesVal->setText(QString::number(stats.byRole.size()));

    // Rebuild roles layout (keep title, remove old rows)
    while (m_rolesLayout->count() > 1) {
        QLayoutItem *item = m_rolesLayout->takeAt(1);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    for (auto it = stats.byRole.constBegin(); it != stats.byRole.constEnd(); ++it)
        m_rolesLayout->addWidget(buildRoleRow(it.key(), it.value(), stats.total));
    m_rolesLayout->addStretch();

    // Rebuild recent roster (keep title, remove old rows)
    while (m_recentLayout->count() > 1) {
        QLayoutItem *item = m_recentLayout->takeAt(1);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    QList<Employee> all = Database::instance().fetchAllEmployees();
    for (const auto &emp : all) {
        QWidget *row = new QWidget;
        row->setObjectName("rosterRow");
        QHBoxLayout *rLay = new QHBoxLayout(row);
        rLay->setContentsMargins(10, 8, 10, 8);
        rLay->setSpacing(12);

        QLabel *nameLbl = new QLabel(emp.name);
        nameLbl->setObjectName("rosterName");

        QLabel *roleLbl = new QLabel(emp.role);
        roleLbl->setObjectName("roleBadge");
        roleLbl->setAlignment(Qt::AlignCenter);
        roleLbl->setContentsMargins(8, 3, 8, 3);

        QLabel *statusLbl = new QLabel(emp.status);
        statusLbl->setObjectName(emp.status == "Active" ? "statusActive" : "statusInactive");
        statusLbl->setAlignment(Qt::AlignCenter);
        statusLbl->setContentsMargins(8, 3, 8, 3);

        rLay->addWidget(nameLbl, 1);
        rLay->addWidget(roleLbl);
        rLay->addWidget(statusLbl);
        m_recentLayout->addWidget(row);
    }
    m_recentLayout->addStretch();
}
