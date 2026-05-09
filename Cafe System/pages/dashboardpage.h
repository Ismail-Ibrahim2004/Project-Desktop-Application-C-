#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QWidget>
#include <QTableWidget>
#include "../widgets/statcard.h"
#include "../widgets/chartwidget.h"
#include "../widgets/barchartwidget.h"
#include "../models/dashboarddata.h"

class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(QWidget *parent = nullptr);

    void refreshData();

private:
    void setupUI();
    void setupRecentOrdersTable();
    void populateStats();
    void populateCharts();
    void populateOrders();
    void styleStatusLabel(QLabel *label, const QString &status);

    // Stat cards
    StatCard *m_revenueCard;
    StatCard *m_ordersCard;
    StatCard *m_employeesCard;
    StatCard *m_topProductCard;

    // Charts
    ChartWidget *m_lineChart;
    BarChartWidget *m_barChart;

    // Table
    QTableWidget *m_recentOrdersTable;

    // Data model
    DashboardDataModel *m_dataModel;
};

#endif // DASHBOARDPAGE_H