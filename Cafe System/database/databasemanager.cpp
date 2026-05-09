#include "databasemanager.h"
#include <QSqlRecord>
#include <QDate>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    closeConnection();
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::connectToDatabase(const QString &server,
                                        const QString &database,
                                        const QString &username,
                                        const QString &password)
{
    // ⭐ تحقق إذا كان فيه connection موجود بالفعل
    if (QSqlDatabase::contains("cafe_manager_connection")) {
        // لو موجود، استخدمه
        m_db = QSqlDatabase::database("cafe_manager_connection");
        if (m_db.isOpen()) {
            qDebug() << "Using existing database connection";
            return true;
        }
    } else {
        // لو مش موجود، أنشئ واحد جديد بـ name مخصص
        m_db = QSqlDatabase::addDatabase("QODBC", "cafe_manager_connection");
    }

    // Connection string for SQL Server
    QString connectionString;

    if (username.isEmpty()) {
        // Windows Authentication
        connectionString = QString(
                               "DRIVER={SQL Server};"
                               "SERVER=%1;"
                               "DATABASE=%2;"
                               "Trusted_Connection=yes;"
                               ).arg(server, database);
    } else {
        // SQL Server Authentication
        connectionString = QString(
                               "DRIVER={SQL Server};"
                               "SERVER=%1;"
                               "DATABASE=%2;"
                               "UID=%3;"
                               "PWD=%4;"
                               ).arg(server, database, username, password);
    }

    m_db.setDatabaseName(connectionString);

    if (!m_db.open()) {
        setError("Failed to connect to database: " + m_db.lastError().text());
        qCritical() << m_lastError;
        return false;
    }

    qDebug() << "✓ Database connected successfully!";
    return true;
}

bool DatabaseManager::isConnected() const
{
    return m_db.isOpen();
}

void DatabaseManager::closeConnection()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
    // Remove the connection from Qt's database registry
    QString connectionName = m_db.connectionName();
    m_db = QSqlDatabase();  // ← اقطع المرجع أولاً

    if (!connectionName.isEmpty() && QSqlDatabase::contains(connectionName)) {
        QSqlDatabase::removeDatabase(connectionName);
    }

}

void DatabaseManager::setError(const QString &error)
{
    m_lastError = error;
    qWarning() << "DB Error:" << error;
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

// ===== Dashboard Queries =====

DashboardStats DatabaseManager::getDashboardStats()
{
    DashboardStats stats;
    stats.totalRevenue = 0;
    stats.revenueGrowth = 0;
    stats.totalOrders = 0;
    stats.ordersGrowth = 0;
    stats.activeEmployees = 0;
    stats.workingNow = 0;
    stats.topSellingProduct = "N/A";
    stats.topSellingCount = 0;

    if (!isConnected()) {
        // Return demo data when not connected
        stats.totalRevenue = 42580.0;
        stats.revenueGrowth = 12.5;
        stats.totalOrders = 1248;
        stats.ordersGrowth = 8.2;
        stats.activeEmployees = 24;
        stats.workingNow = 4;
        stats.topSellingProduct = "Cappuccino";
        stats.topSellingCount = 342;
        return stats;
    }

    QSqlQuery query(m_db);

    // Total Revenue this month
    query.prepare(R"(
        SELECT ISNULL(SUM(Total), 0) as TotalRevenue
        FROM Orders
        WHERE MONTH(OrderDate) = MONTH(GETDATE())
        AND YEAR(OrderDate) = YEAR(GETDATE())
    )");
    if (query.exec() && query.next()) {
        stats.totalRevenue = query.value("TotalRevenue").toDouble();
    }

    // Revenue last month for growth calculation
    query.prepare(R"(
        SELECT ISNULL(SUM(Total), 0) as LastMonthRevenue
        FROM Orders
        WHERE MONTH(OrderDate) = MONTH(DATEADD(MONTH, -1, GETDATE()))
        AND YEAR(OrderDate) = YEAR(DATEADD(MONTH, -1, GETDATE()))
    )");
    if (query.exec() && query.next()) {
        double lastMonth = query.value("LastMonthRevenue").toDouble();
        if (lastMonth > 0) {
            stats.revenueGrowth = ((stats.totalRevenue - lastMonth) / lastMonth) * 100.0;
        }
    }

    // Total Orders this month
    query.prepare(R"(
        SELECT COUNT(*) as TotalOrders
        FROM Orders
        WHERE MONTH(OrderDate) = MONTH(GETDATE())
        AND YEAR(OrderDate) = YEAR(GETDATE())
    )");
    if (query.exec() && query.next()) {
        stats.totalOrders = query.value("TotalOrders").toInt();
    }

    // Orders last month for growth
    query.prepare(R"(
        SELECT COUNT(*) as LastMonthOrders
        FROM Orders
        WHERE MONTH(OrderDate) = MONTH(DATEADD(MONTH, -1, GETDATE()))
        AND YEAR(OrderDate) = YEAR(DATEADD(MONTH, -1, GETDATE()))
    )");
    if (query.exec() && query.next()) {
        int lastMonthOrders = query.value("LastMonthOrders").toInt();
        if (lastMonthOrders > 0) {
            stats.ordersGrowth = ((double)(stats.totalOrders - lastMonthOrders) / lastMonthOrders) * 100.0;
        }
    }

    // Active Employees
    query.prepare("SELECT COUNT(*) as ActiveCount FROM Employees WHERE IsActive = 1");
    if (query.exec() && query.next()) {
        stats.activeEmployees = query.value("ActiveCount").toInt();
    }

    // Working now
    query.prepare("SELECT COUNT(*) as WorkingCount FROM Employees WHERE IsActive = 1 AND IsWorking = 1");
    if (query.exec() && query.next()) {
        stats.workingNow = query.value("WorkingCount").toInt();
    }

    // Top selling product this month
    query.prepare(R"(
        SELECT TOP 1 p.ProductName, COUNT(od.ProductID) as SalesCount
        FROM OrderDetails od
        INNER JOIN Products p ON od.ProductID = p.ProductID
        INNER JOIN Orders o ON od.OrderID = o.OrderID
        WHERE MONTH(o.OrderDate) = MONTH(GETDATE())
        AND YEAR(o.OrderDate) = YEAR(GETDATE())
        GROUP BY p.ProductName
        ORDER BY SalesCount DESC
    )");
    if (query.exec() && query.next()) {
        stats.topSellingProduct = query.value("ProductName").toString();
        stats.topSellingCount = query.value("SalesCount").toInt();
    }

    return stats;
}

WeeklySalesData DatabaseManager::getWeeklySalesData()
{
    WeeklySalesData data;
    data.dailySales = {4200, 3800, 5200, 4800, 6500, 7500, 6200};
    data.dailyOrders = {3500, 4200, 4500, 4800, 5500, 7000, 6500};

    if (!isConnected()) {
        return data;
    }

    // Reset
    data.dailySales.clear();
    data.dailyOrders.clear();

    QSqlQuery query(m_db);

    // Get sales for each day of current week (Monday to Sunday)
    for (int i = 2; i <= 8; i++) {  // DATEPART weekday: 2=Mon, 8=Sun
        int dayNum = (i == 8) ? 1 : i; // Sunday is 1 in SQL Server

        query.prepare(R"(
            SELECT ISNULL(SUM(Total), 0) as DaySales, COUNT(*) as DayOrders
            FROM Orders
            WHERE DATEPART(WEEKDAY, OrderDate) = ?
            AND DATEPART(WEEK, OrderDate) = DATEPART(WEEK, GETDATE())
            AND YEAR(OrderDate) = YEAR(GETDATE())
        )");
        query.addBindValue(dayNum);

        if (query.exec() && query.next()) {
            data.dailySales.append(query.value("DaySales").toDouble());
            data.dailyOrders.append(query.value("DayOrders").toInt());
        } else {
            data.dailySales.append(0);
            data.dailyOrders.append(0);
        }
    }

    return data;
}

QList<OrderRecord> DatabaseManager::getRecentOrders(int limit)
{
    QList<OrderRecord> orders;

    if (!isConnected()) {
        // Demo data
        orders.append({"#ORD-001", "John Doe", "2026-03-07", "10:30 AM", 24.50, "completed"});
        orders.append({"#ORD-002", "Jane Smith", "2026-03-07", "11:15 AM", 18.75, "completed"});
        orders.append({"#ORD-003", "Mike Johnson", "2026-03-07", "11:45 AM", 32.00, "completed"});
        orders.append({"#ORD-004", "Sarah Brown", "2026-03-07", "12:30 PM", 15.25, "pending"});
        orders.append({"#ORD-005", "John Doe", "2026-03-07", "01:00 PM", 42.50, "completed"});
        return orders;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT TOP (?)
            o.OrderID,
            e.EmployeeName,
            CONVERT(VARCHAR(10), o.OrderDate, 120) as OrderDate,
            FORMAT(CAST(o.OrderTime AS DATETIME), 'hh:mm tt') as OrderTime,
            o.Total,
            o.Status
        FROM Orders o
        LEFT JOIN Employees e ON o.EmployeeID = e.EmployeeID
        ORDER BY o.OrderDate DESC, o.OrderTime DESC
    )");
    query.addBindValue(limit);

    if (query.exec()) {
        while (query.next()) {
            OrderRecord record;
            record.orderId = QString("#ORD-%1").arg(query.value("OrderID").toInt(), 3, 10, QChar('0'));
            record.employeeName = query.value("EmployeeName").toString();
            record.orderDate = query.value("OrderDate").toString();
            record.orderTime = query.value("OrderTime").toString();
            record.total = query.value("Total").toDouble();
            record.status = query.value("Status").toString().toLower();
            orders.append(record);
        }
    } else {
        setError("Failed to get recent orders: " + query.lastError().text());
    }

    return orders;
}

// ===== CRUD - Orders =====

bool DatabaseManager::insertOrder(const QString &employeeName, const QString &date,
                                  const QString &time, double total, const QString &status)
{
    if (!isConnected()) {
        setError("Database not connected");
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO Orders (EmployeeID, OrderDate, OrderTime, Total, Status)
        VALUES (
            (SELECT TOP 1 EmployeeID FROM Employees WHERE EmployeeName = ?),
            ?, ?, ?, ?
        )
    )");
    query.addBindValue(employeeName);
    query.addBindValue(date);
    query.addBindValue(time);
    query.addBindValue(total);
    query.addBindValue(status);

    if (!query.exec()) {
        setError("Insert order failed: " + query.lastError().text());
        return false;
    }
    return true;
}

bool DatabaseManager::updateOrderStatus(const QString &orderId, const QString &status)
{
    if (!isConnected()) {
        setError("Database not connected");
        return false;
    }

    // Extract numeric ID from "#ORD-001" format
    QString numericId = orderId;
    numericId.remove("#ORD-");

    QSqlQuery query(m_db);
    query.prepare("UPDATE Orders SET Status = ? WHERE OrderID = ?");
    query.addBindValue(status);
    query.addBindValue(numericId.toInt());

    if (!query.exec()) {
        setError("Update order failed: " + query.lastError().text());
        return false;
    }
    return true;
}

bool DatabaseManager::deleteOrder(const QString &orderId)
{
    if (!isConnected()) {
        setError("Database not connected");
        return false;
    }

    QString numericId = orderId;
    numericId.remove("#ORD-");

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM Orders WHERE OrderID = ?");
    query.addBindValue(numericId.toInt());

    if (!query.exec()) {
        setError("Delete order failed: " + query.lastError().text());
        return false;
    }
    return true;
}

// ===== CRUD - Products =====

bool DatabaseManager::insertProduct(const QString &name, double price, const QString &category)
{
    if (!isConnected()) { setError("Database not connected"); return false; }

    QSqlQuery query(m_db);
    query.prepare("INSERT INTO Products (ProductName, Price, Category) VALUES (?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(price);
    query.addBindValue(category);

    if (!query.exec()) {
        setError("Insert product failed: " + query.lastError().text());
        return false;
    }
    return true;
}

bool DatabaseManager::updateProduct(int id, const QString &name, double price, const QString &category)
{
    if (!isConnected()) { setError("Database not connected"); return false; }

    QSqlQuery query(m_db);
    query.prepare("UPDATE Products SET ProductName=?, Price=?, Category=? WHERE ProductID=?");
    query.addBindValue(name);
    query.addBindValue(price);
    query.addBindValue(category);
    query.addBindValue(id);

    if (!query.exec()) {
        setError("Update product failed: " + query.lastError().text());
        return false;
    }
    return true;
}

bool DatabaseManager::deleteProduct(int id)
{
    if (!isConnected()) { setError("Database not connected"); return false; }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM Products WHERE ProductID=?");
    query.addBindValue(id);

    if (!query.exec()) {
        setError("Delete product failed: " + query.lastError().text());
        return false;
    }
    return true;
}

// ===== CRUD - Employees =====

bool DatabaseManager::insertEmployee(const QString &name, bool isActive)
{
    if (!isConnected()) { setError("Database not connected"); return false; }

    QSqlQuery query(m_db);
    query.prepare("INSERT INTO Employees (EmployeeName, IsActive, IsWorking) VALUES (?, ?, 0)");
    query.addBindValue(name);
    query.addBindValue(isActive ? 1 : 0);

    if (!query.exec()) {
        setError("Insert employee failed: " + query.lastError().text());
        return false;
    }
    return true;
}

bool DatabaseManager::updateEmployee(int id, const QString &name, bool isActive)
{
    if (!isConnected()) { setError("Database not connected"); return false; }

    QSqlQuery query(m_db);
    query.prepare("UPDATE Employees SET EmployeeName=?, IsActive=? WHERE EmployeeID=?");
    query.addBindValue(name);
    query.addBindValue(isActive ? 1 : 0);
    query.addBindValue(id);

    if (!query.exec()) {
        setError("Update employee failed: " + query.lastError().text());
        return false;
    }
    return true;
}

bool DatabaseManager::deleteEmployee(int id)
{
    if (!isConnected()) { setError("Database not connected"); return false; }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM Employees WHERE EmployeeID=?");
    query.addBindValue(id);

    if (!query.exec()) {
        setError("Delete employee failed: " + query.lastError().text());
        return false;
    }
    return true;
}
// ===== Login Function =====

bool DatabaseManager::checkLogin(const QString &username, const QString &password, const QString &role)
{
    if (!isConnected()) {
        setError("Database not connected");
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT role FROM users WHERE username = ? AND password = ? AND role = ?");
    query.addBindValue(username);
    query.addBindValue(password);
    query.addBindValue(role);

    if (!query.exec()) {
        setError("Login query failed: " + query.lastError().text());
        return false;
    }

    return query.next();
}