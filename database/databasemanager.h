#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <QList>
#include <QMap>

struct OrderRecord {
    QString orderId;
    QString employeeName;
    QString orderDate;
    QString orderTime;
    double total;
    QString status;
};

struct DashboardStats {
    double totalRevenue;
    double revenueGrowth;
    int totalOrders;
    double ordersGrowth;
    int activeEmployees;
    int workingNow;
    QString topSellingProduct;
    int topSellingCount;
};

struct WeeklySalesData {
    QList<double> dailySales;
    QList<int> dailyOrders;
};

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager& instance();

    QSqlDatabase getDatabase() const;

    bool connectToDatabase(const QString &server = "localhost",
                           const QString &database = "CafeManagerDB",
                           const QString &username = "",
                           const QString &password = "");

    bool isConnected() const;
    void closeConnection();

    // ===== Login =====
    bool checkLogin(const QString &username,
                    const QString &password,
                    const QString &role);

    // ===== Dashboard Queries =====
    DashboardStats getDashboardStats();
    WeeklySalesData getWeeklySalesData();
    QList<OrderRecord> getRecentOrders(int limit = 5);

    // ===== CRUD - Orders =====
    bool insertOrder(const QString &employeeName,
                     const QString &date,
                     const QString &time,
                     double total,
                     const QString &status);

    bool updateOrderStatus(const QString &orderId,
                           const QString &status);

    bool deleteOrder(const QString &orderId);

    // ===== CRUD - Products =====
    bool insertProduct(const QString &name,
                       double price,
                       const QString &category);

    bool updateProduct(int id,
                       const QString &name,
                       double price,
                       const QString &category);

    bool deleteProduct(int id);

    // ===== CRUD - Employees =====
    bool insertEmployee(const QString &name,
                        bool isActive);

    bool updateEmployee(int id, const QString &name,
                        bool isActive, const QString &role = "Employee");

    bool deleteEmployee(int id);

    // ===== Errors =====
    QString lastError() const;

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_db;
    QString m_lastError;

    void setError(const QString &error);
};

#endif // DATABASEMANAGER_H