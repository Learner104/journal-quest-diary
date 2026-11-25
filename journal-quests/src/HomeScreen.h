#pragma once

#include "Models.h"
#include "GardenWidget.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>

class HomeScreen : public QWidget {
    Q_OBJECT
public:
    explicit HomeScreen(QWidget* parent = nullptr);

    void refresh();
    void applyTheme();

signals:
    void openEntry(qint64 entryId);
    void newEntryRequested();
    void openRandomMemory();

private slots:
    void onRecentItemClicked(QListWidgetItem* item);

private:
    void setupUI();

    QLabel* m_greetingLabel = nullptr;
    QLabel* m_dateLabel = nullptr;
    QLabel* m_streakLabel = nullptr;
    QLabel* m_totalWordsLabel = nullptr;
    QLabel* m_totalEntriesLabel = nullptr;
    QLabel* m_xpLabel = nullptr;
    GardenWidget* m_gardenWidget = nullptr;
    QPushButton* m_continueBtn = nullptr;
    QPushButton* m_randomMemoryBtn = nullptr;
    QPushButton* m_freezeBtn = nullptr;
    QList<QFrame*> m_statCards;
    QList<QLabel*> m_statTitleLabels;

    // Motivational Quote Card
    QFrame* m_quoteCard = nullptr;
    QLabel* m_quoteTitle = nullptr;
    QLabel* m_quoteText = nullptr;
    QLabel* m_quoteAuthor = nullptr;

    // Recent Memories
    QLabel* m_recentTitle = nullptr;
    QListWidget* m_recentList = nullptr;

    qint64 m_lastEntryId = -1;
};
