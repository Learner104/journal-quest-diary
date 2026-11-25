#include "UtilitiesView.h"
#include "ThemeManager.h"
#include "DatabaseManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QRandomGenerator>

static const QStringList s_writingPrompts = {
    "What made you smile or pause with gratitude today?",
    "Describe a challenge you faced recently and what it taught you.",
    "If you could whisper advice to yourself from five years ago, what would you say?",
    "What is something simple in your daily surroundings that brings you peace?",
    "Write about a dream, goal, or wild idea that has been lingering in your mind.",
    "Describe today's weather and how it mirrors your inner mood.",
    "Who is someone you appreciate having in your life right now, and why?",
    "What is one tiny victory from this week you haven't celebrated yet?",
    "If today was a chapter in a novel about your life, what would its title be?",
    "What is one thing you need to let go of to feel lighter?"
};

UtilitiesView::UtilitiesView(QWidget* parent) : QWidget(parent) {
    setupUI();
    applyTheme();

    m_focusTimer = new QTimer(this);
    connect(m_focusTimer, &QTimer::timeout, this, &UtilitiesView::onTimerTick);

    onGenerateWritingPrompt();
}

void UtilitiesView::setupUI() {
    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(36, 28, 36, 28);
    rootLayout->setSpacing(16);

    // Header
    m_headerLabel = new QLabel("Utilities & Daily Tools", this);
    m_headerLabel->setStyleSheet("font-size: 26px; font-weight: bold; font-family: 'Georgia', serif;");
    rootLayout->addWidget(m_headerLabel);

    m_subHeaderLabel = new QLabel("Your quiet companion for focused writing sessions, calendar reflections, and writing prompts.", this);
    m_subHeaderLabel->setStyleSheet("font-size: 13px;");
    rootLayout->addWidget(m_subHeaderLabel);

    // Scroll area for tools
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* container = new QWidget(scrollArea);
    QHBoxLayout* columnsLayout = new QHBoxLayout(container);
    columnsLayout->setContentsMargins(0, 8, 0, 8);
    columnsLayout->setSpacing(20);

    // LEFT COLUMN: Calendar & Date-based entries
    m_calendarCard = new QWidget(container);
    QVBoxLayout* calLayout = new QVBoxLayout(m_calendarCard);
    calLayout->setContentsMargins(18, 18, 18, 18);
    calLayout->setSpacing(12);

    m_calendarTitle = new QLabel("📅 Journal Calendar", m_calendarCard);
    m_calendarTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
    calLayout->addWidget(m_calendarTitle);

    m_calendar = new QCalendarWidget(m_calendarCard);
    m_calendar->setGridVisible(true);
    m_calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    connect(m_calendar, &QCalendarWidget::clicked, this, &UtilitiesView::onCalendarDateSelected);
    calLayout->addWidget(m_calendar);

    m_dateSummaryLabel = new QLabel("Entries on selected date:", m_calendarCard);
    m_dateSummaryLabel->setStyleSheet("font-size: 13px; font-weight: bold; margin-top: 6px;");
    calLayout->addWidget(m_dateSummaryLabel);

    m_dateEntriesList = new QListWidget(m_calendarCard);
    m_dateEntriesList->setMinimumHeight(150);
    connect(m_dateEntriesList, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        if (!item) return;
        qint64 id = item->data(Qt::UserRole).toLongLong();
        if (id > 0) emit entrySelected(id);
    });
    calLayout->addWidget(m_dateEntriesList);

    columnsLayout->addWidget(m_calendarCard, 1);

    // RIGHT COLUMN: Focus Timer + Writing Prompts
    QVBoxLayout* rightColLayout = new QVBoxLayout();
    rightColLayout->setSpacing(20);

    // 1. Focus Timer Card
    m_timerCard = new QWidget(container);
    QVBoxLayout* timerLayout = new QVBoxLayout(m_timerCard);
    timerLayout->setContentsMargins(18, 18, 18, 18);
    timerLayout->setSpacing(12);

    m_timerTitle = new QLabel("⏱️ Focus & Writing Timer", m_timerCard);
    m_timerTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
    timerLayout->addWidget(m_timerTitle);

    QHBoxLayout* presetLayout = new QHBoxLayout();
    QLabel* presetLabel = new QLabel("Preset:", m_timerCard);
    m_timerPresetCombo = new QComboBox(m_timerCard);
    m_timerPresetCombo->addItem("25 min (Pomodoro Focus)", 25);
    m_timerPresetCombo->addItem("15 min (Quick Reflection)", 15);
    m_timerPresetCombo->addItem("5 min (Sprint / Brain Dump)", 5);
    m_timerPresetCombo->addItem("45 min (Deep Work)", 45);
    m_timerPresetCombo->addItem("5 min (Rest / Break)", 5);
    connect(m_timerPresetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UtilitiesView::onTimerPresetChanged);

    presetLayout->addWidget(presetLabel);
    presetLayout->addWidget(m_timerPresetCombo, 1);
    timerLayout->addLayout(presetLayout);

    m_timerDisplay = new QLabel("25:00", m_timerCard);
    m_timerDisplay->setAlignment(Qt::AlignCenter);
    m_timerDisplay->setStyleSheet("font-size: 46px; font-weight: bold; font-family: monospace; margin: 12px 0;");
    timerLayout->addWidget(m_timerDisplay);

    QHBoxLayout* timerBtnsLayout = new QHBoxLayout();
    m_timerStartBtn = new QPushButton("▶ Start Focus", m_timerCard);
    m_timerStartBtn->setCursor(Qt::PointingHandCursor);
    connect(m_timerStartBtn, &QPushButton::clicked, this, &UtilitiesView::onTimerToggle);

    m_timerResetBtn = new QPushButton("↺ Reset", m_timerCard);
    m_timerResetBtn->setCursor(Qt::PointingHandCursor);
    connect(m_timerResetBtn, &QPushButton::clicked, this, &UtilitiesView::onTimerReset);

    timerBtnsLayout->addWidget(m_timerStartBtn, 2);
    timerBtnsLayout->addWidget(m_timerResetBtn, 1);
    timerLayout->addLayout(timerBtnsLayout);

    m_timerStatusLabel = new QLabel("Ready for your mindful writing session.", m_timerCard);
    m_timerStatusLabel->setAlignment(Qt::AlignCenter);
    m_timerStatusLabel->setStyleSheet("font-size: 12px;");
    timerLayout->addWidget(m_timerStatusLabel);

    rightColLayout->addWidget(m_timerCard);

    // 2. Writing Prompt Generator Card
    m_promptCard = new QWidget(container);
    QVBoxLayout* promptLayout = new QVBoxLayout(m_promptCard);
    promptLayout->setContentsMargins(18, 18, 18, 18);
    promptLayout->setSpacing(12);

    m_promptTitle = new QLabel("✨ Daily Writing Inspiration", m_promptCard);
    m_promptTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
    promptLayout->addWidget(m_promptTitle);

    m_promptText = new QLabel(m_promptCard);
    m_promptText->setWordWrap(true);
    m_promptText->setStyleSheet("font-size: 14px; font-style: italic; font-family: 'Georgia', serif; padding: 10px 0; line-height: 140%;");
    promptLayout->addWidget(m_promptText);

    QHBoxLayout* promptBtnsLayout = new QHBoxLayout();
    m_newPromptBtn = new QPushButton("🎲 New Prompt", m_promptCard);
    m_newPromptBtn->setCursor(Qt::PointingHandCursor);
    connect(m_newPromptBtn, &QPushButton::clicked, this, &UtilitiesView::onGenerateWritingPrompt);

    m_writeWithPromptBtn = new QPushButton("✍️ Write with this prompt", m_promptCard);
    m_writeWithPromptBtn->setCursor(Qt::PointingHandCursor);
    connect(m_writeWithPromptBtn, &QPushButton::clicked, this, [this]() {
        emit newEntryRequested();
    });

    promptBtnsLayout->addWidget(m_newPromptBtn);
    promptBtnsLayout->addWidget(m_writeWithPromptBtn);
    promptLayout->addLayout(promptBtnsLayout);

    rightColLayout->addWidget(m_promptCard);
    rightColLayout->addStretch();

    columnsLayout->addLayout(rightColLayout, 1);

    scrollArea->setWidget(container);
    rootLayout->addWidget(scrollArea, 1);
}

void UtilitiesView::applyTheme() {
    const auto& t = ThemeManager::instance().currentTheme();
    setStyleSheet(QString("background: %1;").arg(t.bgApp));

    if (m_headerLabel) m_headerLabel->setStyleSheet(QString("font-size: 26px; font-weight: bold; color: %1; font-family: 'Georgia', serif;").arg(t.textPrimary));
    if (m_subHeaderLabel) m_subHeaderLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(t.textSecondary));

    QString cardStyle = QString("background: %1; border: 1px solid %2; border-radius: 10px;").arg(t.bgCard).arg(t.border);

    if (m_calendarCard) m_calendarCard->setStyleSheet(cardStyle);
    if (m_calendarTitle) m_calendarTitle->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1;").arg(t.textPrimary));
    if (m_dateSummaryLabel) m_dateSummaryLabel->setStyleSheet(QString("font-size: 13px; font-weight: bold; color: %1;").arg(t.textPrimary));

    if (m_calendar) {
        m_calendar->setStyleSheet(QString(
            "QCalendarWidget QWidget { color: %1; background: %2; } "
            "QCalendarWidget QAbstractItemView:enabled { color: %1; background: %2; selection-background-color: %3; selection-color: #FFFFFF; } "
            "QCalendarWidget QToolButton { color: %1; font-weight: bold; } "
            "QCalendarWidget QMenu { background-color: %4; color: %1; }"
        ).arg(t.textPrimary).arg(t.bgCard).arg(t.accent).arg(t.dropdownBg));
    }

    if (m_dateEntriesList) {
        m_dateEntriesList->setStyleSheet(QString(
            "QListWidget { background: %1; border: 1px solid %2; border-radius: 6px; color: %3; } "
            "QListWidget::item { padding: 8px 10px; border-bottom: 1px solid %2; color: %3; } "
            "QListWidget::item:hover { background: %4; } "
            "QListWidget::item:selected { background: %5; color: %3; font-weight: bold; }"
        ).arg(t.bgApp).arg(t.border).arg(t.textPrimary).arg(t.dropdownHover).arg(t.selectionBg));
    }

    if (m_timerCard) m_timerCard->setStyleSheet(cardStyle);
    if (m_timerTitle) m_timerTitle->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1;").arg(t.textPrimary));
    if (m_timerDisplay) m_timerDisplay->setStyleSheet(QString("font-size: 46px; font-weight: bold; font-family: monospace; color: %1; margin: 12px 0;").arg(t.accent));
    if (m_timerStatusLabel) m_timerStatusLabel->setStyleSheet(QString("font-size: 12px; color: %1;").arg(t.textSecondary));

    if (m_timerPresetCombo) {
        m_timerPresetCombo->setStyleSheet(QString(
            "QComboBox { background: %1; border: 1px solid %2; border-radius: 6px; padding: 6px 10px; color: %3; } "
            "QComboBox QAbstractItemView { background: %4; color: %3; selection-background-color: %5; }"
        ).arg(t.bgApp).arg(t.border).arg(t.textPrimary).arg(t.dropdownBg).arg(t.selectionBg));
    }

    if (m_timerStartBtn) {
        m_timerStartBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: #FFFFFF; font-weight: bold; padding: 10px 18px; border-radius: 6px; border: none; font-size: 13px; } "
            "QPushButton:hover { background: %2; }"
        ).arg(t.accent).arg(t.accentHover));
    }

    if (m_timerResetBtn) {
        m_timerResetBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: %2; font-weight: bold; padding: 10px 14px; border-radius: 6px; border: 1px solid %3; font-size: 13px; } "
            "QPushButton:hover { background: %4; }"
        ).arg(t.bgApp).arg(t.textPrimary).arg(t.border).arg(t.dropdownHover));
    }

    if (m_promptCard) m_promptCard->setStyleSheet(cardStyle);
    if (m_promptTitle) m_promptTitle->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1;").arg(t.textPrimary));
    if (m_promptText) m_promptText->setStyleSheet(QString("font-size: 14px; font-style: italic; font-family: 'Georgia', serif; color: %1; padding: 10px 0; line-height: 140%;").arg(t.textPrimary));

    if (m_newPromptBtn) {
        m_newPromptBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: %2; font-weight: bold; padding: 8px 14px; border-radius: 6px; border: 1px solid %3; font-size: 12px; } "
            "QPushButton:hover { background: %4; }"
        ).arg(t.bgApp).arg(t.textPrimary).arg(t.border).arg(t.dropdownHover));
    }

    if (m_writeWithPromptBtn) {
        m_writeWithPromptBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: #FFFFFF; font-weight: bold; padding: 8px 14px; border-radius: 6px; border: none; font-size: 12px; } "
            "QPushButton:hover { background: %2; }"
        ).arg(t.accent).arg(t.accentHover));
    }
}

void UtilitiesView::refresh() {
    onCalendarDateSelected(m_calendar ? m_calendar->selectedDate() : QDate::currentDate());
}

void UtilitiesView::onCalendarDateSelected(const QDate& date) {
    if (!m_dateEntriesList || !m_dateSummaryLabel) return;

    m_dateSummaryLabel->setText(QString("Entries on %1:").arg(date.toString("MMMM d, yyyy")));
    m_dateEntriesList->clear();

    const auto allEntries = DatabaseManager::instance().getAllEntries(false);
    int count = 0;

    for (const auto& e : allEntries) {
        if (e.createdAt.date() == date) {
            count++;
            QString text = QString("• %1 (%2 words) %3")
                               .arg(e.title.isEmpty() ? "Untitled Entry" : e.title)
                               .arg(e.wordCount)
                               .arg(e.mood.isEmpty() ? "" : "[" + e.mood + "]");
            QListWidgetItem* item = new QListWidgetItem(text, m_dateEntriesList);
            item->setData(Qt::UserRole, e.id);
        }
    }

    if (count == 0) {
        QListWidgetItem* emptyItem = new QListWidgetItem("🌱 No entries written on this day.", m_dateEntriesList);
        emptyItem->setFlags(Qt::NoItemFlags);
    }
}

void UtilitiesView::onTimerPresetChanged(int index) {
    int minutes = m_timerPresetCombo->itemData(index).toInt();
    if (minutes <= 0) minutes = 25;
    m_totalSeconds = minutes * 60;
    m_remainingSeconds = m_totalSeconds;
    if (m_focusTimer->isActive()) {
        m_focusTimer->stop();
        m_timerRunning = false;
        m_timerStartBtn->setText("▶ Start Focus");
    }
    updateTimerDisplay();
    m_timerStatusLabel->setText("Timer set to " + QString::number(minutes) + " minutes.");
}

void UtilitiesView::onTimerToggle() {
    if (m_timerRunning) {
        m_focusTimer->stop();
        m_timerRunning = false;
        m_timerStartBtn->setText("▶ Resume Focus");
        m_timerStatusLabel->setText("Timer paused.");
    } else {
        if (m_remainingSeconds <= 0) {
            m_remainingSeconds = m_totalSeconds;
        }
        m_focusTimer->start(1000);
        m_timerRunning = true;
        m_timerStartBtn->setText("⏸ Pause Focus");
        m_timerStatusLabel->setText("Focusing... Write with full presence.");
    }
}

void UtilitiesView::onTimerReset() {
    m_focusTimer->stop();
    m_timerRunning = false;
    m_remainingSeconds = m_totalSeconds;
    m_timerStartBtn->setText("▶ Start Focus");
    m_timerStatusLabel->setText("Timer reset.");
    updateTimerDisplay();
}

void UtilitiesView::onTimerTick() {
    if (m_remainingSeconds > 0) {
        m_remainingSeconds--;
        updateTimerDisplay();
    } else {
        m_focusTimer->stop();
        m_timerRunning = false;
        m_timerStartBtn->setText("▶ Start Focus");
        m_timerStatusLabel->setText("🔔 Session finished! Well done taking time to write & reflect.");
    }
}

void UtilitiesView::updateTimerDisplay() {
    int minutes = m_remainingSeconds / 60;
    int seconds = m_remainingSeconds % 60;
    m_timerDisplay->setText(QString("%1:%2")
                               .arg(minutes, 2, 10, QChar('0'))
                               .arg(seconds, 2, 10, QChar('0')));
}

void UtilitiesView::onGenerateWritingPrompt() {
    int idx = QRandomGenerator::global()->bounded(s_writingPrompts.size());
    m_promptText->setText("“" + s_writingPrompts.at(idx) + "”");
}
