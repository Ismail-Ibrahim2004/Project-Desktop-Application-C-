#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QList>
#include <QMap>

struct Employee {
    QString id;
    QString name;
    QString role;
    QString email;
    QString status;
};

struct EmployeeStats {
    int total    = 0;
    int active   = 0;
    int inactive = 0;
    QMap<QString, int> byRole;
};

class Database
{
public:
    static Database& instance();

    bool initialize();
    bool isOpen() const;

    QList<Employee>  fetchAllEmployees();
    QList<Employee>  searchEmployees(const QString &keyword);
    EmployeeStats    fetchStats();

    bool addEmployee(const Employee &emp);
    bool updateEmployee(const Employee &emp);
    bool deleteEmployee(const QString &id);

private:
    Database() = default;
    QSqlDatabase m_db;
};
