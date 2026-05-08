#include "database.h"

Database& Database::instance()
{
    static Database db;
    return db;
}

bool Database::initialize()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("cafe_management.db");

    if (!m_db.open()) {
        qDebug() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query;
    bool ok = query.exec(
        "CREATE TABLE IF NOT EXISTS employees ("
        "  id      VARCHAR(20)  PRIMARY KEY,"
        "  name    VARCHAR(100) NOT NULL,"
        "  role    VARCHAR(50),"
        "  email   VARCHAR(100),"
        "  status  VARCHAR(20)  DEFAULT 'Active'"
        ");"
    );

    if (!ok) {
        qDebug() << "Failed to create employees table:" << query.lastError().text();
        return false;
    }

    QSqlQuery check;
    check.exec("SELECT COUNT(*) FROM employees;");
    if (check.next() && check.value(0).toInt() == 0) {
        QStringList seeds = {
            "INSERT INTO employees VALUES ('EMP-001','Maria Santos','Barista','maria@cafe.com','Active');",
            "INSERT INTO employees VALUES ('EMP-002','James Cruz','Cashier','james@cafe.com','Active');",
            "INSERT INTO employees VALUES ('EMP-003','Ana Reyes','Shift Manager','ana@cafe.com','Active');",
            "INSERT INTO employees VALUES ('EMP-004','Luis Garcia','Barista','luis@cafe.com','Inactive');",
            "INSERT INTO employees VALUES ('EMP-005','Sofia Mendez','Barista','sofia@cafe.com','Active');",
            "INSERT INTO employees VALUES ('EMP-006','Carlos Tan','Kitchen Staff','carlos@cafe.com','Active');",
            "INSERT INTO employees VALUES ('EMP-007','Rosa Lim','Cashier','rosa@cafe.com','Inactive');"
        };
        for (const auto &s : seeds) {
            QSqlQuery q;
            q.exec(s);
        }
    }

    return true;
}

bool Database::isOpen() const
{
    return m_db.isOpen();
}

static Employee rowToEmployee(QSqlQuery &q)
{
    Employee e;
    e.id     = q.value("id").toString();
    e.name   = q.value("name").toString();
    e.role   = q.value("role").toString();
    e.email  = q.value("email").toString();
    e.status = q.value("status").toString();
    return e;
}

QList<Employee> Database::fetchAllEmployees()
{
    QList<Employee> list;
    QSqlQuery q("SELECT id, name, role, email, status FROM employees ORDER BY name;");
    while (q.next())
        list.append(rowToEmployee(q));
    return list;
}

QList<Employee> Database::searchEmployees(const QString &keyword)
{
    QList<Employee> list;
    QSqlQuery q;
    q.prepare("SELECT id, name, role, email, status FROM employees "
              "WHERE name LIKE :kw OR id LIKE :kw ORDER BY name;");
    q.bindValue(":kw", "%" + keyword + "%");
    q.exec();
    while (q.next())
        list.append(rowToEmployee(q));
    return list;
}

EmployeeStats Database::fetchStats()
{
    EmployeeStats stats;

    QSqlQuery total("SELECT COUNT(*) FROM employees;");
    if (total.next()) stats.total = total.value(0).toInt();

    QSqlQuery active("SELECT COUNT(*) FROM employees WHERE status='Active';");
    if (active.next()) stats.active = active.value(0).toInt();

    QSqlQuery inactive("SELECT COUNT(*) FROM employees WHERE status='Inactive';");
    if (inactive.next()) stats.inactive = inactive.value(0).toInt();

    QSqlQuery roles("SELECT role, COUNT(*) as cnt FROM employees GROUP BY role ORDER BY cnt DESC;");
    while (roles.next())
        stats.byRole[roles.value("role").toString()] = roles.value("cnt").toInt();

    return stats;
}

bool Database::addEmployee(const Employee &emp)
{
    QSqlQuery q;
    q.prepare("INSERT INTO employees (id, name, role, email, status) "
              "VALUES (:id, :name, :role, :email, :status);");
    q.bindValue(":id",     emp.id);
    q.bindValue(":name",   emp.name);
    q.bindValue(":role",   emp.role);
    q.bindValue(":email",  emp.email);
    q.bindValue(":status", emp.status);
    if (!q.exec()) {
        qDebug() << "addEmployee error:" << q.lastError().text();
        return false;
    }
    return true;
}

bool Database::updateEmployee(const Employee &emp)
{
    QSqlQuery q;
    q.prepare("UPDATE employees SET name=:name, role=:role, email=:email, status=:status "
              "WHERE id=:id;");
    q.bindValue(":id",     emp.id);
    q.bindValue(":name",   emp.name);
    q.bindValue(":role",   emp.role);
    q.bindValue(":email",  emp.email);
    q.bindValue(":status", emp.status);
    if (!q.exec()) {
        qDebug() << "updateEmployee error:" << q.lastError().text();
        return false;
    }
    return true;
}

bool Database::deleteEmployee(const QString &id)
{
    QSqlQuery q;
    q.prepare("DELETE FROM employees WHERE id=:id;");
    q.bindValue(":id", id);
    if (!q.exec()) {
        qDebug() << "deleteEmployee error:" << q.lastError().text();
        return false;
    }
    return true;
}
