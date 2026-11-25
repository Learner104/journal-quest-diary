#pragma once

#include "Models.h"
#include <QObject>
#include <QSqlDatabase>
#include <QVector>
#include <memory>

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    static DatabaseManager& instance();

    bool init(const QString& dbPath = QString());
    void close();

    // Entries CRUD
    qint64 createEntry(DiaryEntry& entry);
    bool updateEntry(const DiaryEntry& entry);
    bool getEntry(qint64 id, DiaryEntry& outEntry);
    QVector<DiaryEntry> getAllEntries(bool includeTrash = false);
    QVector<DiaryEntry> getEntriesByCategory(const QString& category);
    QVector<DiaryEntry> getFavoriteEntries();
    QVector<DiaryEntry> getTrashEntries();
    QVector<DiaryEntry> getWordDocuments();
    QVector<DiaryEntry> searchEntries(const QString& query);
    bool moveToTrash(qint64 id);
    bool restoreFromTrash(qint64 id);
    bool permanentDeleteEntry(qint64 id);

    // Gamification & Stats
    UserStats getUserStats();
    bool saveUserStats(const UserStats& stats);
    void recordJournalEntryCreated(int wordCount);
    bool useStreakFreeze();
    bool addStreakFreeze(int count = 1);

    // Categories
    QStringList getCategories();

    // Backup & Restore
    bool exportBackup(const QString& jsonFilePath);
    bool importBackup(const QString& jsonFilePath);

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_db;
    void updateGardenLevel(UserStats& stats);
};
