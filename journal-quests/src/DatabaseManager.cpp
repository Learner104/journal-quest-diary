#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

DatabaseManager::DatabaseManager() {}

DatabaseManager::~DatabaseManager() {
    close();
}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager s_instance;
    return s_instance;
}

bool DatabaseManager::init(const QString& dbPath) {
    QString targetPath = dbPath;
    if (targetPath.isEmpty()) {
        QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (dataDir.isEmpty()) {
            dataDir = QDir::homePath() + "/.journal-quest";
        } else {
            dataDir += "/journal-quest";
        }
        QDir dir(dataDir);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        targetPath = dir.filePath("journal_quest.sqlite");
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE", "journal_connection");
    m_db.setDatabaseName(targetPath);

    if (!m_db.open()) {
        qWarning() << "Failed to open SQLite database:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery q(m_db);
    // Table: entries
    bool ok = q.exec(
        "CREATE TABLE IF NOT EXISTS entries ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "content_html TEXT,"
        "content_plain TEXT,"
        "category TEXT DEFAULT 'Daily',"
        "tags TEXT,"
        "mood TEXT DEFAULT 'Peaceful',"
        "weather TEXT DEFAULT 'Sunny',"
        "is_favorite INTEGER DEFAULT 0,"
        "is_trash INTEGER DEFAULT 0,"
        "original_docx_path TEXT,"
        "created_at TEXT,"
        "updated_at TEXT,"
        "word_count INTEGER DEFAULT 0"
        ");"
    );
    if (!ok) {
        qWarning() << "Failed to create entries table:" << q.lastError().text();
    }

    // Table: user_stats
    ok = q.exec(
        "CREATE TABLE IF NOT EXISTS user_stats ("
        "id INTEGER PRIMARY KEY CHECK (id = 1),"
        "total_xp INTEGER DEFAULT 0,"
        "current_streak INTEGER DEFAULT 0,"
        "last_journal_date TEXT,"
        "total_words INTEGER DEFAULT 0,"
        "total_entries INTEGER DEFAULT 0,"
        "garden_level INTEGER DEFAULT 0,"
        "gamification_enabled INTEGER DEFAULT 1,"
        "streak_freezes INTEGER DEFAULT 2,"
        "freeze_active_date TEXT"
        ");"
    );
    if (!ok) {
        qWarning() << "Failed to create user_stats table:" << q.lastError().text();
    }

    // Add columns if migrating existing db
    q.exec("ALTER TABLE user_stats ADD COLUMN streak_freezes INTEGER DEFAULT 2;");
    q.exec("ALTER TABLE user_stats ADD COLUMN freeze_active_date TEXT;");

    // Ensure initial row for user_stats
    q.exec("INSERT OR IGNORE INTO user_stats (id, total_xp, current_streak, total_words, total_entries, garden_level, gamification_enabled, streak_freezes) VALUES (1, 0, 0, 0, 0, 0, 1, 2);");

    return true;
}

void DatabaseManager::close() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

qint64 DatabaseManager::createEntry(DiaryEntry& entry) {
    if (!m_db.isOpen()) return -1;

    QDateTime now = QDateTime::currentDateTime();
    if (!entry.createdAt.isValid()) entry.createdAt = now;
    entry.updatedAt = now;

    QSqlQuery q(m_db);
    q.prepare("INSERT INTO entries (title, content_html, content_plain, category, tags, mood, weather, is_favorite, is_trash, original_docx_path, created_at, updated_at, word_count) "
              "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(entry.title.isEmpty() ? "Untitled Entry" : entry.title);
    q.addBindValue(entry.contentHtml);
    q.addBindValue(entry.contentPlainText);
    q.addBindValue(entry.category);
    q.addBindValue(entry.tags.join(","));
    q.addBindValue(entry.mood);
    q.addBindValue(entry.weather);
    q.addBindValue(entry.isFavorite ? 1 : 0);
    q.addBindValue(entry.isTrash ? 1 : 0);
    q.addBindValue(entry.originalDocxPath);
    q.addBindValue(entry.createdAt.toString(Qt::ISODate));
    q.addBindValue(entry.updatedAt.toString(Qt::ISODate));
    q.addBindValue(entry.wordCount);

    if (q.exec()) {
        entry.id = q.lastInsertId().toLongLong();
        recordJournalEntryCreated(entry.wordCount);
        return entry.id;
    } else {
        qWarning() << "createEntry failed:" << q.lastError().text();
        return -1;
    }
}

bool DatabaseManager::updateEntry(const DiaryEntry& entry) {
    if (!m_db.isOpen() || entry.id <= 0) return false;

    QSqlQuery q(m_db);
    q.prepare("UPDATE entries SET title = ?, content_html = ?, content_plain = ?, category = ?, tags = ?, mood = ?, weather = ?, is_favorite = ?, is_trash = ?, original_docx_path = ?, updated_at = ?, word_count = ? WHERE id = ?");
    q.addBindValue(entry.title);
    q.addBindValue(entry.contentHtml);
    q.addBindValue(entry.contentPlainText);
    q.addBindValue(entry.category);
    q.addBindValue(entry.tags.join(","));
    q.addBindValue(entry.mood);
    q.addBindValue(entry.weather);
    q.addBindValue(entry.isFavorite ? 1 : 0);
    q.addBindValue(entry.isTrash ? 1 : 0);
    q.addBindValue(entry.originalDocxPath);
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    q.addBindValue(entry.wordCount);
    q.addBindValue(entry.id);

    return q.exec();
}

bool DatabaseManager::getEntry(qint64 id, DiaryEntry& outEntry) {
    if (!m_db.isOpen()) return false;

    QSqlQuery q(m_db);
    q.prepare("SELECT id, title, content_html, content_plain, category, tags, mood, weather, is_favorite, is_trash, original_docx_path, created_at, updated_at, word_count FROM entries WHERE id = ?");
    q.addBindValue(id);

    if (q.exec() && q.next()) {
        outEntry.id = q.value(0).toLongLong();
        outEntry.title = q.value(1).toString();
        outEntry.contentHtml = q.value(2).toString();
        outEntry.contentPlainText = q.value(3).toString();
        outEntry.category = q.value(4).toString();
        outEntry.tags = q.value(5).toString().split(',', Qt::SkipEmptyParts);
        outEntry.mood = q.value(6).toString();
        outEntry.weather = q.value(7).toString();
        outEntry.isFavorite = q.value(8).toInt() == 1;
        outEntry.isTrash = q.value(9).toInt() == 1;
        outEntry.originalDocxPath = q.value(10).toString();
        outEntry.createdAt = QDateTime::fromString(q.value(11).toString(), Qt::ISODate);
        outEntry.updatedAt = QDateTime::fromString(q.value(12).toString(), Qt::ISODate);
        outEntry.wordCount = q.value(13).toInt();
        return true;
    }
    return false;
}

static DiaryEntry entryFromQuery(QSqlQuery& q) {
    DiaryEntry e;
    e.id = q.value(0).toLongLong();
    e.title = q.value(1).toString();
    e.contentHtml = q.value(2).toString();
    e.contentPlainText = q.value(3).toString();
    e.category = q.value(4).toString();
    e.tags = q.value(5).toString().split(',', Qt::SkipEmptyParts);
    e.mood = q.value(6).toString();
    e.weather = q.value(7).toString();
    e.isFavorite = q.value(8).toInt() == 1;
    e.isTrash = q.value(9).toInt() == 1;
    e.originalDocxPath = q.value(10).toString();
    e.createdAt = QDateTime::fromString(q.value(11).toString(), Qt::ISODate);
    e.updatedAt = QDateTime::fromString(q.value(12).toString(), Qt::ISODate);
    e.wordCount = q.value(13).toInt();
    return e;
}

QVector<DiaryEntry> DatabaseManager::getAllEntries(bool includeTrash) {
    QVector<DiaryEntry> list;
    if (!m_db.isOpen()) return list;

    QSqlQuery q(m_db);
    QString sql = "SELECT id, title, content_html, content_plain, category, tags, mood, weather, is_favorite, is_trash, original_docx_path, created_at, updated_at, word_count FROM entries ";
    if (!includeTrash) {
        sql += "WHERE is_trash = 0 ";
    }
    sql += "ORDER BY created_at DESC";

    if (q.exec(sql)) {
        while (q.next()) {
            list.append(entryFromQuery(q));
        }
    }
    return list;
}

QVector<DiaryEntry> DatabaseManager::getEntriesByCategory(const QString& category) {
    QVector<DiaryEntry> list;
    if (!m_db.isOpen()) return list;

    QSqlQuery q(m_db);
    q.prepare("SELECT id, title, content_html, content_plain, category, tags, mood, weather, is_favorite, is_trash, original_docx_path, created_at, updated_at, word_count FROM entries WHERE is_trash = 0 AND category = ? ORDER BY created_at DESC");
    q.addBindValue(category);

    if (q.exec()) {
        while (q.next()) {
            list.append(entryFromQuery(q));
        }
    }
    return list;
}

QVector<DiaryEntry> DatabaseManager::getFavoriteEntries() {
    QVector<DiaryEntry> list;
    if (!m_db.isOpen()) return list;

    QSqlQuery q(m_db);
    if (q.exec("SELECT id, title, content_html, content_plain, category, tags, mood, weather, is_favorite, is_trash, original_docx_path, created_at, updated_at, word_count FROM entries WHERE is_trash = 0 AND is_favorite = 1 ORDER BY created_at DESC")) {
        while (q.next()) {
            list.append(entryFromQuery(q));
        }
    }
    return list;
}

QVector<DiaryEntry> DatabaseManager::getTrashEntries() {
    QVector<DiaryEntry> list;
    if (!m_db.isOpen()) return list;

    QSqlQuery q(m_db);
    if (q.exec("SELECT id, title, content_html, content_plain, category, tags, mood, weather, is_favorite, is_trash, original_docx_path, created_at, updated_at, word_count FROM entries WHERE is_trash = 1 ORDER BY updated_at DESC")) {
        while (q.next()) {
            list.append(entryFromQuery(q));
        }
    }
    return list;
}

QVector<DiaryEntry> DatabaseManager::getWordDocuments() {
    QVector<DiaryEntry> list;
    if (!m_db.isOpen()) return list;

    QSqlQuery q(m_db);
    if (q.exec("SELECT id, title, content_html, content_plain, category, tags, mood, weather, is_favorite, is_trash, original_docx_path, created_at, updated_at, word_count FROM entries WHERE is_trash = 0 AND original_docx_path IS NOT NULL AND original_docx_path != '' ORDER BY updated_at DESC")) {
        while (q.next()) {
            list.append(entryFromQuery(q));
        }
    }
    return list;
}

QVector<DiaryEntry> DatabaseManager::searchEntries(const QString& query) {
    QVector<DiaryEntry> list;
    if (!m_db.isOpen()) return list;

    QSqlQuery q(m_db);
    QString pattern = "%" + query + "%";
    q.prepare("SELECT id, title, content_html, content_plain, category, tags, mood, weather, is_favorite, is_trash, original_docx_path, created_at, updated_at, word_count FROM entries "
              "WHERE is_trash = 0 AND (title LIKE ? OR content_plain LIKE ? OR tags LIKE ? OR category LIKE ?) ORDER BY created_at DESC");
    q.addBindValue(pattern);
    q.addBindValue(pattern);
    q.addBindValue(pattern);
    q.addBindValue(pattern);

    if (q.exec()) {
        while (q.next()) {
            list.append(entryFromQuery(q));
        }
    }
    return list;
}

bool DatabaseManager::moveToTrash(qint64 id) {
    if (!m_db.isOpen() || id <= 0) return false;
    QSqlQuery q(m_db);
    q.prepare("UPDATE entries SET is_trash = 1, updated_at = ? WHERE id = ?");
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    q.addBindValue(id);
    return q.exec();
}

bool DatabaseManager::restoreFromTrash(qint64 id) {
    if (!m_db.isOpen() || id <= 0) return false;
    QSqlQuery q(m_db);
    q.prepare("UPDATE entries SET is_trash = 0, updated_at = ? WHERE id = ?");
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    q.addBindValue(id);
    return q.exec();
}

bool DatabaseManager::permanentDeleteEntry(qint64 id) {
    if (!m_db.isOpen() || id <= 0) return false;
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM entries WHERE id = ?");
    q.addBindValue(id);
    return q.exec();
}

UserStats DatabaseManager::getUserStats() {
    UserStats stats;
    if (!m_db.isOpen()) return stats;

    QSqlQuery q(m_db);
    if (q.exec("SELECT total_xp, current_streak, last_journal_date, total_words, total_entries, garden_level, gamification_enabled, streak_freezes, freeze_active_date FROM user_stats WHERE id = 1")) {
        if (q.next()) {
            stats.totalXp = q.value(0).toInt();
            stats.currentStreak = q.value(1).toInt();
            stats.lastJournalDate = QDate::fromString(q.value(2).toString(), Qt::ISODate);
            stats.totalWordsWritten = q.value(3).toInt();
            stats.totalEntriesWritten = q.value(4).toInt();
            stats.gardenLevel = q.value(5).toInt();
            stats.gamificationEnabled = q.value(6).toInt() == 1;
            stats.streakFreezes = q.value(7).toInt();
            QDate freezeDate = QDate::fromString(q.value(8).toString(), Qt::ISODate);
            stats.freezeActiveToday = (freezeDate == QDate::currentDate());
        }
    }
    return stats;
}

bool DatabaseManager::saveUserStats(const UserStats& stats) {
    if (!m_db.isOpen()) return false;

    QSqlQuery q(m_db);
    q.prepare("UPDATE user_stats SET total_xp = ?, current_streak = ?, last_journal_date = ?, total_words = ?, total_entries = ?, garden_level = ?, gamification_enabled = ?, streak_freezes = ?, freeze_active_date = ? WHERE id = 1");
    q.addBindValue(stats.totalXp);
    q.addBindValue(stats.currentStreak);
    q.addBindValue(stats.lastJournalDate.toString(Qt::ISODate));
    q.addBindValue(stats.totalWordsWritten);
    q.addBindValue(stats.totalEntriesWritten);
    q.addBindValue(stats.gardenLevel);
    q.addBindValue(stats.gamificationEnabled ? 1 : 0);
    q.addBindValue(stats.streakFreezes);
    q.addBindValue(stats.freezeActiveToday ? QDate::currentDate().toString(Qt::ISODate) : QString());
    return q.exec();
}

bool DatabaseManager::useStreakFreeze() {
    UserStats stats = getUserStats();
    if (stats.streakFreezes <= 0 || stats.freezeActiveToday) {
        return false;
    }
    stats.streakFreezes -= 1;
    stats.freezeActiveToday = true;
    // If user missed yesterday, protect their streak by setting lastJournalDate to yesterday
    QDate today = QDate::currentDate();
    if (!stats.lastJournalDate.isValid() || stats.lastJournalDate < today.addDays(-1)) {
        stats.lastJournalDate = today.addDays(-1);
    }
    return saveUserStats(stats);
}

bool DatabaseManager::addStreakFreeze(int count) {
    UserStats stats = getUserStats();
    stats.streakFreezes += count;
    return saveUserStats(stats);
}

void DatabaseManager::updateGardenLevel(UserStats& stats) {
    int total = stats.totalEntriesWritten;
    if (total >= 30) {
        stats.gardenLevel = 4; // Flourishing Sanctuary
    } else if (total >= 15) {
        stats.gardenLevel = 3; // Blooming Garden
    } else if (total >= 5) {
        stats.gardenLevel = 2; // Growing Plant
    } else if (total >= 1) {
        stats.gardenLevel = 1; // Sprouting Seed
    } else {
        stats.gardenLevel = 0; // Dormant Soil
    }
}

void DatabaseManager::recordJournalEntryCreated(int wordCount) {
    UserStats stats = getUserStats();
    QDate today = QDate::currentDate();

    // Streak calculation
    if (!stats.lastJournalDate.isValid()) {
        stats.currentStreak = 1;
    } else if (stats.lastJournalDate == today) {
        // Already journaled today, streak remains unchanged
    } else if (stats.lastJournalDate == today.addDays(-1)) {
        stats.currentStreak += 1;
    } else {
        // Streak broken
        stats.currentStreak = 1;
    }
    stats.lastJournalDate = today;

    // XP calculation: 25 XP per entry + 1 XP per 10 words + streak bonus (10 * streak)
    int gainedXp = 25 + (wordCount / 10) + (stats.currentStreak * 10);
    stats.totalXp += gainedXp;
    stats.totalWordsWritten += wordCount;
    stats.totalEntriesWritten += 1;

    updateGardenLevel(stats);
    saveUserStats(stats);
}

QStringList DatabaseManager::getCategories() {
    QStringList cats = {"Daily", "Reflections", "Creative", "Travel", "Ideas", "Memories"};
    if (!m_db.isOpen()) return cats;

    QSqlQuery q(m_db);
    if (q.exec("SELECT DISTINCT category FROM entries WHERE is_trash = 0 AND category IS NOT NULL AND category != ''")) {
        while (q.next()) {
            QString c = q.value(0).toString();
            if (!cats.contains(c)) {
                cats.append(c);
            }
        }
    }
    return cats;
}

bool DatabaseManager::exportBackup(const QString& jsonFilePath) {
    QVector<DiaryEntry> entries = getAllEntries(true);
    UserStats stats = getUserStats();

    QJsonObject root;
    root["version"] = "1.0";
    root["exported_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonObject statsObj;
    statsObj["total_xp"] = stats.totalXp;
    statsObj["current_streak"] = stats.currentStreak;
    statsObj["last_journal_date"] = stats.lastJournalDate.toString(Qt::ISODate);
    statsObj["total_words"] = stats.totalWordsWritten;
    statsObj["total_entries"] = stats.totalEntriesWritten;
    statsObj["garden_level"] = stats.gardenLevel;
    statsObj["gamification_enabled"] = stats.gamificationEnabled;
    root["stats"] = statsObj;

    QJsonArray entriesArr;
    for (const auto& e : entries) {
        QJsonObject o;
        o["title"] = e.title;
        o["content_html"] = e.contentHtml;
        o["content_plain"] = e.contentPlainText;
        o["category"] = e.category;
        o["tags"] = QJsonArray::fromStringList(e.tags);
        o["mood"] = e.mood;
        o["weather"] = e.weather;
        o["is_favorite"] = e.isFavorite;
        o["is_trash"] = e.isTrash;
        o["original_docx_path"] = e.originalDocxPath;
        o["created_at"] = e.createdAt.toString(Qt::ISODate);
        o["updated_at"] = e.updatedAt.toString(Qt::ISODate);
        o["word_count"] = e.wordCount;
        entriesArr.append(o);
    }
    root["entries"] = entriesArr;

    QFile file(jsonFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool DatabaseManager::importBackup(const QString& jsonFilePath) {
    QFile file(jsonFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return false;

    QJsonObject root = doc.object();
    if (root.contains("stats") && root["stats"].isObject()) {
        QJsonObject statsObj = root["stats"].toObject();
        UserStats stats;
        stats.totalXp = statsObj["total_xp"].toInt();
        stats.currentStreak = statsObj["current_streak"].toInt();
        stats.lastJournalDate = QDate::fromString(statsObj["last_journal_date"].toString(), Qt::ISODate);
        stats.totalWordsWritten = statsObj["total_words"].toInt();
        stats.totalEntriesWritten = statsObj["total_entries"].toInt();
        stats.gardenLevel = statsObj["garden_level"].toInt();
        stats.gamificationEnabled = statsObj["gamification_enabled"].toBool(true);
        saveUserStats(stats);
    }

    if (root.contains("entries") && root["entries"].isArray()) {
        QJsonArray arr = root["entries"].toArray();
        for (const auto& val : arr) {
            if (!val.isObject()) continue;
            QJsonObject o = val.toObject();
            DiaryEntry e;
            e.title = o["title"].toString();
            e.contentHtml = o["content_html"].toString();
            e.contentPlainText = o["content_plain"].toString();
            e.category = o["category"].toString("Daily");
            QJsonArray tagsArr = o["tags"].toArray();
            for (const auto& t : tagsArr) {
                e.tags.append(t.toString());
            }
            e.mood = o["mood"].toString("Peaceful");
            e.weather = o["weather"].toString("Sunny");
            e.isFavorite = o["is_favorite"].toBool(false);
            e.isTrash = o["is_trash"].toBool(false);
            e.originalDocxPath = o["original_docx_path"].toString();
            e.createdAt = QDateTime::fromString(o["created_at"].toString(), Qt::ISODate);
            e.updatedAt = QDateTime::fromString(o["updated_at"].toString(), Qt::ISODate);
            e.wordCount = o["word_count"].toInt(0);

            createEntry(e);
        }
    }
    return true;
}
