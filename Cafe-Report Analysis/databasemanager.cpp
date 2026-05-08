#include "databasemanager.h"

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {}

DatabaseManager::~DatabaseManager()
{
    disconnect();
}

bool DatabaseManager::connect()
{
    if (m_db.isOpen()) return true;

    m_db = QSqlDatabase::addDatabase("QODBC");

    QString connectionString = QString(
        "DRIVER={ODBC Driver 17 for SQL Server};"
        "SERVER=localhost;"
        "DATABASE=CafeManagerDB;"
        "Trusted_Connection=Yes;"
        );

    m_db.setDatabaseName(connectionString);

    if (!m_db.open()) {
        qDebug() << "❌ Database Connection Failed:" << m_db.lastError().text();
        return false;
    }

    qDebug() << "✅ Database Connected Successfully!";
    return true;
}

void DatabaseManager::disconnect()
{
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << "Database Disconnected.";
    }
}

bool DatabaseManager::isConnected() const
{
    return m_db.isOpen();
}

QSqlDatabase DatabaseManager::database() const
{
    return m_db;
}