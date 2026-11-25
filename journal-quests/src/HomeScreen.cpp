#include "HomeScreen.h"
#include "DatabaseManager.h"
#include "ThemeManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QFrame>
#include <QScrollArea>
#include <QRandomGenerator>

struct QuoteItem {
    QString quote;
    QString author;
};

static const QVector<QuoteItem> s_motivationalQuotes = {
    {"“Fill your paper with the breathings of your heart.”", "William Wordsworth"},
    {"“Writing in a journal will clarify thoughts, unlock emotions, and give you peace.”", "Robin Sharma"},
    {"“Keep a notebook. Travel with it, eat with it, sleep with it. Slap into it every stray thought.”", "Jack London"},
    {"“In the journal I do not just express myself more openly than I could to any person; I create myself.”", "Susan Sontag"},
    {"“What you write today is the sacred soil for what blooms in your life tomorrow.”", "Sanctuary Musings"},
    {"“Small daily deeds done consistently build great kingdoms of peace.”", "Marcus Aurelius"},
    {"“You do not write because you want to say something, you write because you have something to say.”", "F. Scott Fitzgerald"}
};

HomeScreen::HomeScreen(QWidget* parent) : QWidget(parent) {
    setupUI();
    applyTheme();
    refresh();
}

void HomeScreen::applyTheme() {
    const auto& t = ThemeManager::instance().currentTheme();
    setStyleSheet(QString("background: %1;").arg(t.bgApp));

    if (m_greetingLabel) m_greetingLabel->setStyleSheet(QString("font-size: 28px; font-weight: bold; color: %1; font-family: 'Georgia', serif;").arg(t.textPrimary));
    if (m_dateLabel) m_dateLabel->setStyleSheet(QString("font-size: 14px; color: %1; font-weight: 500;").arg(t.textSecondary));

    if (m_continueBtn) {
        m_continueBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: #FFFFFF; font-size: 14px; font-weight: bold; padding: 10px 20px; border-radius: 8px; border: none; } "
            "QPushButton:hover { background: %2; }"
        ).arg(t.accent).arg(t.accentHover));
    }

    if (m_randomMemoryBtn) {
        m_randomMemoryBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: %2; font-size: 13px; font-weight: bold; padding: 10px 18px; border-radius: 8px; border: 1px solid %3; } "
            "QPushButton:hover { background: %4; }"
        ).arg(t.bgCard).arg(t.textPrimary).arg(t.border).arg(t.dropdownHover));
    }

    if (m_freezeBtn) {
        m_freezeBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: %2; font-size: 13px; font-weight: bold; padding: 10px 16px; border-radius: 8px; border: 1px solid %3; } "
            "QPushButton:hover { background: %4; }"
        ).arg(t.bgCard).arg(t.textPrimary).arg(t.border).arg(t.dropdownHover));
    }

    // Dynamic stat card theming: noticeably larger and softly elevated with clean solid borders
    QColor cardBgColor(t.bgCard);
    QColor cardBorderColor(t.border);
    if (t.isDark) {
        cardBgColor = cardBgColor.lighter(135);
        cardBorderColor = cardBorderColor.lighter(115);
    } else {
        cardBgColor = cardBgColor.lighter(105);
    }

    QString cardStyle = QString(
        "QFrame { background: %1; border: 1px solid %2; border-radius: 12px; } "
    ).arg(cardBgColor.name()).arg(cardBorderColor.name());

    for (QFrame* card : m_statCards) {
        if (card) {
            card->setStyleSheet(cardStyle);
            card->setFixedHeight(88);
        }
    }
    for (QLabel* lbl : m_statTitleLabels) {
        if (lbl) lbl->setStyleSheet(QString("font-size: 12px; font-weight: 600; color: %1; text-transform: uppercase; letter-spacing: 0.5px;").arg(t.textSecondary));
    }

    if (m_streakLabel) m_streakLabel->setStyleSheet(QString("font-size: 24px; font-weight: bold; color: %1;").arg(t.textPrimary));
    if (m_totalWordsLabel) m_totalWordsLabel->setStyleSheet(QString("font-size: 24px; font-weight: bold; color: %1;").arg(t.textPrimary));
    if (m_totalEntriesLabel) m_totalEntriesLabel->setStyleSheet(QString("font-size: 24px; font-weight: bold; color: %1;").arg(t.textPrimary));
    if (m_xpLabel) m_xpLabel->setStyleSheet(QString("font-size: 24px; font-weight: bold; color: %1;").arg(t.textPrimary));

    // Motivational quote card styling
    if (m_quoteCard) {
        m_quoteCard->setStyleSheet(QString(
            "QFrame { background: %1; border: 1px solid %2; border-radius: 12px; } "
        ).arg(cardBgColor.name()).arg(cardBorderColor.name()));
    }
    if (m_quoteTitle) m_quoteTitle->setStyleSheet(QString("font-size: 11px; font-weight: bold; color: %1; text-transform: uppercase; letter-spacing: 0.5px;").arg(t.textSecondary));
    if (m_quoteText) m_quoteText->setStyleSheet(QString("font-size: 14px; font-style: italic; color: %1; font-family: 'Georgia', serif; line-height: 140%;").arg(t.textPrimary));
    if (m_quoteAuthor) m_quoteAuthor->setStyleSheet(QString("font-size: 12px; font-weight: bold; color: %1;").arg(t.accent));

    // Recent Memories section styling
    if (m_recentTitle) m_recentTitle->setStyleSheet(QString("font-size: 17px; font-weight: bold; color: %1; font-family: 'Georgia', serif; margin-top: 6px;").arg(t.textPrimary));

    if (m_recentList) {
        m_recentList->setStyleSheet(QString(
            "QListWidget { background: %1; border: 1px solid %2; border-radius: 8px; padding: 6px; outline: none; } "
            "QListWidget::item { padding: 10px 12px; border-bottom: 1px solid %2; border-radius: 6px; color: %3; outline: none; } "
            "QListWidget::item:hover { background: %4; } "
            "QListWidget::item:selected { background: %5; color: %3; font-weight: bold; outline: none; }"
        ).arg(t.bgCard).arg(t.border).arg(t.textPrimary).arg(t.dropdownHover).arg(t.selectionBg));
    }

    if (m_gardenWidget) m_gardenWidget->update();
}

void HomeScreen::setupUI() {
    setStyleSheet("background: #FAF7F2;");

    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* container = new QWidget(scrollArea);
    QVBoxLayout* mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(40, 32, 40, 32);
    mainLayout->setSpacing(20);

    // 1. Header greeting & date
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QVBoxLayout* greetLayout = new QVBoxLayout();

    m_greetingLabel = new QLabel("Welcome to your sanctuary", container);
    m_greetingLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #3A2E25; font-family: 'Georgia', serif;");

    m_dateLabel = new QLabel(QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy"), container);
    m_dateLabel->setStyleSheet("font-size: 14px; color: #8A7B6D; font-weight: 500;");

    greetLayout->addWidget(m_greetingLabel);
    greetLayout->addWidget(m_dateLabel);
    headerLayout->addLayout(greetLayout);
    headerLayout->addStretch();

    // Action buttons
    m_continueBtn = new QPushButton("✨ Continue Writing", container);
    m_continueBtn->setCursor(Qt::PointingHandCursor);
    connect(m_continueBtn, &QPushButton::clicked, this, [this]() {
        if (m_lastEntryId > 0) {
            emit openEntry(m_lastEntryId);
        } else {
            emit newEntryRequested();
        }
    });

    m_randomMemoryBtn = new QPushButton("🎲 Random Memory", container);
    m_randomMemoryBtn->setCursor(Qt::PointingHandCursor);
    connect(m_randomMemoryBtn, &QPushButton::clicked, this, &HomeScreen::openRandomMemory);

    m_freezeBtn = new QPushButton("🧊 Streak Freeze", container);
    m_freezeBtn->setCursor(Qt::PointingHandCursor);
    m_freezeBtn->setToolTip("Protect your writing streak if you missed a day");
    connect(m_freezeBtn, &QPushButton::clicked, this, [this]() {
        UserStats stats = DatabaseManager::instance().getUserStats();
        if (stats.freezeActiveToday) {
            return;
        }
        if (stats.streakFreezes > 0) {
            DatabaseManager::instance().useStreakFreeze();
            refresh();
        }
    });

    headerLayout->addWidget(m_freezeBtn);
    headerLayout->addSpacing(8);
    headerLayout->addWidget(m_randomMemoryBtn);
    headerLayout->addSpacing(8);
    headerLayout->addWidget(m_continueBtn);
    mainLayout->addLayout(headerLayout);

    // 2. Garden Card (The Visual Gamification)
    m_gardenWidget = new GardenWidget(container);
    mainLayout->addWidget(m_gardenWidget);

    // 3. Stats Strip
    QHBoxLayout* statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(14);
    auto makeStatCard = [this, container, statsLayout](const QString& title, QLabel*& valueLabel, const QString& icon) {
        QFrame* card = new QFrame(container);
        m_statCards.append(card);
        QVBoxLayout* l = new QVBoxLayout(card);
        l->setSpacing(6);
        l->setContentsMargins(16, 14, 16, 14);

        QLabel* t = new QLabel(icon + " " + title, card);
        m_statTitleLabels.append(t);
        valueLabel = new QLabel("0", card);

        l->addWidget(t);
        l->addWidget(valueLabel);
        statsLayout->addWidget(card);
    };

    makeStatCard("Streak", m_streakLabel, "🔥");
    makeStatCard("Words Written", m_totalWordsLabel, "✍️");
    makeStatCard("Total Entries", m_totalEntriesLabel, "📖");
    makeStatCard("Experience Points", m_xpLabel, "⭐");
    mainLayout->addLayout(statsLayout);

    // 4. Motivational Quote Card
    m_quoteCard = new QFrame(container);
    QVBoxLayout* qLayout = new QVBoxLayout(m_quoteCard);
    qLayout->setContentsMargins(18, 14, 18, 14);
    qLayout->setSpacing(4);

    m_quoteTitle = new QLabel("💡 Daily Reflection & Inspiration", m_quoteCard);
    m_quoteText = new QLabel(m_quoteCard);
    m_quoteText->setWordWrap(true);
    m_quoteAuthor = new QLabel(m_quoteCard);
    m_quoteAuthor->setAlignment(Qt::AlignRight);

    // Pick random quote
    int qIdx = QRandomGenerator::global()->bounded(s_motivationalQuotes.size());
    m_quoteText->setText(s_motivationalQuotes.at(qIdx).quote);
    m_quoteAuthor->setText("— " + s_motivationalQuotes.at(qIdx).author);

    qLayout->addWidget(m_quoteTitle);
    qLayout->addWidget(m_quoteText);
    qLayout->addWidget(m_quoteAuthor);
    mainLayout->addWidget(m_quoteCard);

    // 5. Recent Memories Section (Brought back!)
    m_recentTitle = new QLabel("Recent Memories", container);
    mainLayout->addWidget(m_recentTitle);

    m_recentList = new QListWidget(container);
    m_recentList->setMinimumHeight(170);
    connect(m_recentList, &QListWidget::itemClicked, this, &HomeScreen::onRecentItemClicked);
    mainLayout->addWidget(m_recentList);

    scrollArea->setWidget(container);
    rootLayout->addWidget(scrollArea);
}

void HomeScreen::refresh() {
    UserStats stats = DatabaseManager::instance().getUserStats();

    // Time-based warm greeting
    int hour = QTime::currentTime().hour();
    QString greeting = "Good morning";
    if (hour >= 12 && hour < 17) greeting = "Good afternoon";
    else if (hour >= 17 && hour < 22) greeting = "Good evening";
    else if (hour >= 22 || hour < 5) greeting = "Midnight quietude";

    m_greetingLabel->setText(greeting + ", wanderer.");
    m_dateLabel->setText(QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy"));

    if (stats.freezeActiveToday) {
        m_freezeBtn->setText("🧊 Freeze Active Today");
        m_freezeBtn->setEnabled(false);
    } else {
        m_freezeBtn->setText(QString("🧊 Freeze Streak (%1 left)").arg(stats.streakFreezes));
        m_freezeBtn->setEnabled(stats.streakFreezes > 0);
    }

    QString streakText = QString("%1 Day%2").arg(stats.currentStreak).arg(stats.currentStreak == 1 ? "" : "s");
    if (stats.freezeActiveToday) {
        streakText += " ❄️";
    }
    m_streakLabel->setText(streakText);
    m_totalWordsLabel->setText(QString::number(stats.totalWordsWritten));
    m_totalEntriesLabel->setText(QString::number(stats.totalEntriesWritten));
    m_xpLabel->setText(QString("%1 XP").arg(stats.totalXp));

    m_gardenWidget->setGardenLevel(stats.gardenLevel);
    m_gardenWidget->setVisible(stats.gamificationEnabled);

    // Populate recent entries
    if (m_recentList) {
        m_recentList->clear();
        QVector<DiaryEntry> entries = DatabaseManager::instance().getAllEntries(false);
        if (!entries.isEmpty()) {
            m_lastEntryId = entries.first().id;
        } else {
            m_lastEntryId = -1;
        }

        int count = 0;
        for (const auto& e : entries) {
            if (count++ >= 6) break;

            QString itemText = QString("%1  •  %2\n%3  |  %4 words  |  %5")
                                   .arg(e.title.isEmpty() ? "Untitled Entry" : e.title)
                                   .arg(e.createdAt.toString("MMM d, yyyy"))
                                   .arg(e.mood)
                                   .arg(e.wordCount)
                                   .arg(e.category);

            QListWidgetItem* item = new QListWidgetItem(itemText, m_recentList);
            item->setData(Qt::UserRole, e.id);
        }

        if (entries.isEmpty()) {
            QListWidgetItem* emptyItem = new QListWidgetItem("🌱 No entries yet. Click 'Continue Writing' or 'New Entry' to plant your first thought.", m_recentList);
            emptyItem->setFlags(Qt::NoItemFlags);
        }
    }
}

void HomeScreen::onRecentItemClicked(QListWidgetItem* item) {
    if (!item) return;
    qint64 id = item->data(Qt::UserRole).toLongLong();
    if (id > 0) {
        emit openEntry(id);
    }
}
