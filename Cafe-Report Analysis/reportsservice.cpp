#include "reportsservice.h"
#include "databasemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>

ReportsService& ReportsService::instance()
{
    static ReportsService instance;
    return instance;
}

ReportsService::ReportsService(QObject *parent) : QObject(parent) {}

// ==================== Dashboard Stats ====================
DashboardStats ReportsService::getDashboardStats()
{
    DashboardStats stats = {0};

    QSqlQuery query;

    // ===== إحصائيات اليوم =====
    query.prepare(R"(
        SELECT
            ISNULL(SUM(TotalAmount), 0) AS Revenue,
            COUNT(*) AS OrdersCount,
            ISNULL(AVG(TotalAmount), 0) AS AvgOrderValue
        FROM Orders
        WHERE CAST(OrderDate AS DATE) = CAST(GETDATE() AS DATE)
    )");

    if (query.exec() && query.next()) {
        stats.dailyRevenue = query.value(0).toDouble();
        stats.ordersToday = query.value(1).toInt();
        stats.avgOrderValue = query.value(2).toDouble();
    } else {
        qDebug() << "Error getting today stats:" << query.lastError().text();
    }

    // ===== عدد المنتجات المباعة اليوم =====
    query.prepare(R"(
        SELECT ISNULL(SUM(od.Quantity), 0) AS ItemsSold
        FROM OrderDetails od
        JOIN Orders o ON od.OrderID = o.OrderID
        WHERE CAST(o.OrderDate AS DATE) = CAST(GETDATE() AS DATE)
    )");

    if (query.exec() && query.next()) {
        stats.itemsSold = query.value(0).toInt();
    }

    // ===== مقارنة بأمس =====
    double yesterdayRevenue = 0;
    int yesterdayOrders = 0;
    double yesterdayAvg = 0;
    int yesterdayItems = 0;

    query.prepare(R"(
        SELECT
            ISNULL(SUM(TotalAmount), 0),
            COUNT(*),
            ISNULL(AVG(TotalAmount), 0)
        FROM Orders
        WHERE CAST(OrderDate AS DATE) = CAST(DATEADD(DAY, -1, GETDATE()) AS DATE)
    )");

    if (query.exec() && query.next()) {
        yesterdayRevenue = query.value(0).toDouble();
        yesterdayOrders = query.value(1).toInt();
        yesterdayAvg = query.value(2).toDouble();
    }

    query.prepare(R"(
        SELECT ISNULL(SUM(od.Quantity), 0)
        FROM OrderDetails od
        JOIN Orders o ON od.OrderID = o.OrderID
        WHERE CAST(o.OrderDate AS DATE) = CAST(DATEADD(DAY, -1, GETDATE()) AS DATE)
    )");

    if (query.exec() && query.next()) {
        yesterdayItems = query.value(0).toInt();
    }

    // حساب نسبة التغيير
    auto calcChange = [](double today, double yesterday) -> double {
        if (yesterday == 0) return 0;
        return ((today - yesterday) / yesterday) * 100.0;
    };

    stats.revenueChangePercent = calcChange(stats.dailyRevenue, yesterdayRevenue);
    stats.ordersChangePercent = calcChange(stats.ordersToday, yesterdayOrders);
    stats.avgOrderChangePercent = calcChange(stats.avgOrderValue, yesterdayAvg);
    stats.itemsChangePercent = calcChange(stats.itemsSold, yesterdayItems);

    return stats;
}

// ==================== Best Sellers ====================
QList<BestSellerData> ReportsService::getTopBestSellers(int limit)
{
    QList<BestSellerData> result;

    QSqlQuery query;
    query.prepare(QString(R"(
        SELECT TOP (%1)
            p.ProductName,
            SUM(od.Quantity) AS TotalSold,
            SUM(od.Subtotal) AS TotalRevenue
        FROM OrderDetails od
        JOIN Products p ON od.ProductID = p.ProductID
        GROUP BY p.ProductName
        ORDER BY TotalSold DESC
    )").arg(limit));

    if (query.exec()) {
        while (query.next()) {
            BestSellerData data;
            data.productName = query.value(0).toString();
            data.totalOrders = query.value(1).toInt();
            data.totalRevenue = query.value(2).toDouble();
            result.append(data);
        }
    } else {
        qDebug() << "Error getting best sellers:" << query.lastError().text();
    }

    return result;
}

// ==================== Sales By Category ====================
QList<CategorySalesData> ReportsService::getSalesByCategory()
{
    QList<CategorySalesData> result;
    double totalRevenue = 0;

    QSqlQuery query;
    query.prepare(R"(
        SELECT
            p.Category,
            SUM(od.Subtotal) AS CategoryRevenue
        FROM OrderDetails od
        JOIN Products p ON od.ProductID = p.ProductID
        GROUP BY p.Category
        ORDER BY CategoryRevenue DESC
    )");

    if (query.exec()) {
        while (query.next()) {
            CategorySalesData data;
            data.category = query.value(0).toString();
            data.revenue = query.value(1).toDouble();
            totalRevenue += data.revenue;
            result.append(data);
        }

        // حساب النسب المئوية
        for (auto &item : result) {
            item.percentage = (totalRevenue > 0) ? (item.revenue / totalRevenue * 100.0) : 0;
        }
    } else {
        qDebug() << "Error getting sales by category:" << query.lastError().text();
    }

    return result;
}

// ==================== Last 7 Days Revenue ====================
QList<DailyRevenueData> ReportsService::getLast7DaysRevenue()
{
    QList<DailyRevenueData> result;

    QSqlQuery query;
    query.prepare(R"(
        SELECT
            CAST(OrderDate AS DATE) AS OrderDay,
            ISNULL(SUM(TotalAmount), 0) AS Revenue
        FROM Orders
        WHERE OrderDate >= DATEADD(DAY, -6, CAST(GETDATE() AS DATE))
          AND OrderDate < DATEADD(DAY, 1, CAST(GETDATE() AS DATE))
        GROUP BY CAST(OrderDate AS DATE)
        ORDER BY OrderDay
    )");

    QStringList dayNames = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    if (query.exec()) {
        while (query.next()) {
            DailyRevenueData data;
            QDate date = query.value(0).toDate();
            int dayOfWeek = date.dayOfWeek(); // 1=Monday, 7=Sunday

            // تحويل لاسم اليوم
            int idx = (dayOfWeek == 7) ? 0 : dayOfWeek;
            data.dayName = dayNames[idx];
            data.revenue = query.value(1).toDouble();
            result.append(data);
        }
    } else {
        qDebug() << "Error getting last 7 days revenue:" << query.lastError().text();
    }

    return result;
}

// ==================== Last 6 Months Orders ====================
QList<MonthlyOrdersData> ReportsService::getLast6MonthsOrders()
{
    QList<MonthlyOrdersData> result;

    QSqlQuery query;
    query.prepare(R"(
        SELECT
            YEAR(OrderDate) AS Yr,
            MONTH(OrderDate) AS Mo,
            COUNT(*) AS OrdersCount
        FROM Orders
        WHERE OrderDate >= DATEADD(MONTH, -5, CAST(GETDATE() AS DATE))
        GROUP BY YEAR(OrderDate), MONTH(OrderDate)
        ORDER BY Yr, Mo
    )");

    QStringList monthNames = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    if (query.exec()) {
        while (query.next()) {
            MonthlyOrdersData data;
            int month = query.value(1).toInt();
            data.monthName = monthNames[month];
            data.ordersCount = query.value(2).toInt();
            result.append(data);
        }
    } else {
        qDebug() << "Error getting monthly orders:" << query.lastError().text();
    }

    return result;
}