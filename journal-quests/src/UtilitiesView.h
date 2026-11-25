#pragma once

#include "Models.h"
#include <QWidget>
#include <QCalendarWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTimer>
#include <QSpinBox>
#include <QComboBox>

class UtilitiesView : public QWidget {
    Q_OBJECT
public:
    explicit UtilitiesView(QWidget* parent = nullptr);

    void applyTheme();
    void refresh();

signals:
    void entrySelected(qint64 entryId);
    void newEntryRequested();

private slots:
    void onCalendarDateSelected(const QDate& date);
    void onTimerToggle();
    void onTimerReset();
    void onTimerTick();
    void onTimerPresetChanged(int index);
    void onGenerateWritingPrompt();

private:
    void setupUI();
    void updateTimerDisplay();

    // UI elements
    QLabel* m_headerLabel = nullptr;
    QLabel* m_subHeaderLabel = nullptr;

    // Calendar Card
    QWidget* m_calendarCard = nullptr;
    QLabel* m_calendarTitle = nullptr;
    QCalendarWidget* m_calendar = nullptr;
    QLabel* m_dateSummaryLabel = nullptr;
    QListWidget* m_dateEntriesList = nullptr;

    // Pomodoro / Focus Timer Card
    QWidget* m_timerCard = nullptr;
    QLabel* m_timerTitle = nullptr;
    QLabel* m_timerDisplay = nullptr;
    QComboBox* m_timerPresetCombo = nullptr;
    QPushButton* m_timerStartBtn = nullptr;
    QPushButton* m_timerResetBtn = nullptr;
    QLabel* m_timerStatusLabel = nullptr;
    QTimer* m_focusTimer = nullptr;
    int m_remainingSeconds = 25 * 60;
    int m_totalSeconds = 25 * 60;
    bool m_timerRunning = false;

    // Quick Inspiration / Prompt Generator Card
    QWidget* m_promptCard = nullptr;
    QLabel* m_promptTitle = nullptr;
    QLabel* m_promptText = nullptr;
    QPushButton* m_newPromptBtn = nullptr;
    QPushButton* m_writeWithPromptBtn = nullptr;
};
