#include "dashboarddata.h"

DashboardDataModel::DashboardDataModel()
{
    refresh();
}

void DashboardDataModel::refresh()
{
    DatabaseManager &db = DatabaseManager::instance();
    m_stats = db.getDashboardStats();
    m_weeklyData = db.getWeeklySalesData();
    m_recentOrders = db.getRecentOrders(5);
}

double DashboardDataModel::totalRevenue() const { return m_stats.totalRevenue; }
double DashboardDataModel::revenueGrowth() const { return m_stats.revenueGrowth; }
int DashboardDataModel::totalOrders() const { return m_stats.totalOrders; }
double DashboardDataModel::ordersGrowth() const { return m_stats.ordersGrowth; }
int DashboardDataModel::activeEmployees() const { return m_stats.activeEmployees; }
int DashboardDataModel::workingNow() const { return m_stats.workingNow; }
QString DashboardDataModel::topSellingProduct() const { return m_stats.topSellingProduct; }
int DashboardDataModel::topSellingCount() const { return m_stats.topSellingCount; }
QList<double> DashboardDataModel::weeklySales() const { return m_weeklyData.dailySales; }
QList<int> DashboardDataModel::weeklyOrders() const { return m_weeklyData.dailyOrders; }
QList<OrderRecord> DashboardDataModel::recentOrders() const { return m_recentOrders; }