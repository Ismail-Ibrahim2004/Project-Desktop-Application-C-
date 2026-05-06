#ifndef REPORTSSERVICE_H
#define REPORTSSERVICE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QPair>
#include <QDate>

// Struct لبيانات أكتر المنتجات مبيعاً
struct BestSellerData {
    QString productName;
    int totalOrders;
    double totalRevenue;
};

// Struct للمبيعات حسب الفئة
struct CategorySalesData {
    QString category;
    double revenue;
    double percentage;
};

// Struct للإيرادات اليومية
struct DailyRevenueData {
    QString dayName;
    double revenue;
};

// Struct للطلبات الشهرية
struct MonthlyOrdersData {
    QString monthName;
    int ordersCount;
};

// Struct لإحصائيات الكروت
struct DashboardStats {
    double dailyRevenue;
    int ordersToday;
    double avgOrderValue;
    int itemsSold;

    double revenueChangePercent;
    double ordersChangePercent;
    double avgOrderChangePercent;
    double itemsChangePercent;
};

class ReportsService : public QObject
{
    Q_OBJECT
public:
    static ReportsService& instance();

    // الإحصائيات الرئيسية (الكروت)
    DashboardStats getDashboardStats();

    // أكتر 5 منتجات مبيعاً
    QList<BestSellerData> getTopBestSellers(int limit = 5);

    // المبيعات حسب الفئة
    QList<CategorySalesData> getSalesByCategory();

    // الإيرادات لآخر 7 أيام
    QList<DailyRevenueData> getLast7DaysRevenue();

    // الطلبات لآخر 6 شهور
    QList<MonthlyOrdersData> getLast6MonthsOrders();

private:
    explicit ReportsService(QObject *parent = nullptr);

    ReportsService(const ReportsService&) = delete;
    ReportsService& operator=(const ReportsService&) = delete;
};

#endif // REPORTSSERVICE_H