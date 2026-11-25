#pragma once

#include <QString>
#include <QDateTime>
#include <QStringList>

struct DiaryEntry {
    qint64 id = -1;
    QString title;
    QString contentHtml;
    QString contentPlainText;
    QString category = "Daily";       // e.g. "Personal", "Work", "Reflections", "Ideas"
    QStringList tags;
    QString mood = "Peaceful";        // e.g. "Peaceful", "Joyful", "Reflective", "Focused", "Melancholy", "Inspired"
    QString weather = "Sunny";        // e.g. "Sunny", "Rainy", "Cloudy", "Windy", "Snowy", "Starry Night"
    bool isFavorite = false;
    bool isTrash = false;
    QString originalDocxPath;         // Set if imported or bound to an external docx
    QDateTime createdAt;
    QDateTime updatedAt;
    int wordCount = 0;
};

struct UserStats {
    int totalXp = 0;
    int currentStreak = 0;
    QDate lastJournalDate;
    int totalWordsWritten = 0;
    int totalEntriesWritten = 0;
    int gardenLevel = 0;             // 0 = Seed, 1 = Sprout, 2 = Budding, 3 = Blooming, 4 = Flourishing Sanctuary
    bool gamificationEnabled = true;
    int streakFreezes = 2;           // Available streak freezes
    bool freezeActiveToday = false;  // Whether a freeze was used today
};
