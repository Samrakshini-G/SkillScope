#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <memory>

/**
 * @brief Singleton manager handling SQLite database lifecycle, schema initialization,
 *        and foreign key constraints.
 */
class DatabaseManager {
public:
    static DatabaseManager& instance();

    // Prevent copying and assignment
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool openDatabase(const QString& customPath = QString());
    void closeDatabase();
    bool isOpen() const;

    bool initializeSchema();
    bool seedSampleData();

    QSqlDatabase& database();
    QString lastError() const;
    QString getDatabasePath() const;

private:
    DatabaseManager();
    ~DatabaseManager();

    bool executeQuery(const QString& queryStr);
    QString determineDatabaseFilePath() const;

    QSqlDatabase m_db;
    QString m_lastError;
    QString m_dbPath;
    bool m_initialized;
};

#endif // DATABASEMANAGER_H
