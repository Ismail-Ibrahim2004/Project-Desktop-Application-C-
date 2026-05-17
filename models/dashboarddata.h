#ifndef DASHBOARDDATA_H
#define DASHBOARDDATA_H

#include <QString>
#include <QList>
#include "../database/databasemanager.h"

class DashboardDataModel
{
public:
    DashboardDataModel();

    void refresh();

    double totalRevenue() const;
    double revenueGrowth() const;
    int totalOrders() const;
    double ordersGrowth() const;
    int activeEmployees() const;
    int workingNow() const;
    QString topSellingProduct() const;
    int topSellingCount() const;

    QList<double> weeklySales() const;
    QList<int> weeklyOrders() const;
    QList<OrderRecord> recentOrders() const;

private:
    DashboardStats m_stats;
    WeeklySalesData m_weeklyData;
    QList<OrderRecord> m_recentOrders;
};

#endif // DASHBOARDDATA_H