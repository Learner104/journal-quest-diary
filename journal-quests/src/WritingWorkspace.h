#pragma once

#include "Models.h"
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include <QTimer>
#include <QComboBox>

class WritingWorkspace : public QWidget {
    Q_OBJECT
public:
    explicit WritingWorkspace(QWidget* parent = nullptr);

    void loadEntry(const DiaryEntry& entry);
    DiaryEntry currentEntry() const;
    void clear();

    void applyTheme();
    void setFocusMode(bool enabled);
    bool isFocusMode() const { return m_focusMode; }

signals:
    void entrySaved(const DiaryEntry& entry);
    void requestToggleFocus();
    void docxFileDropped(const QString& filePath);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

public slots:
    void manualSave();

private slots:
    void onContentOrTitleChanged();
    void performAutoSave();

    // Toolbar formatting
    void setBold();
    void setItalic();
    void setUnderline();
    void setStrike();
    void setAlignLeft();
    void setAlignCenter();
    void setAlignRight();
    void insertBulletList();
    void insertNumberedList();
    void insertQuote();
    void insertTable();
    void onHeadingChanged(int index);

private:
    void setupUI();
    void updateCounters();
    void setSaveState(const QString& stateText, const QString& colorHex);

    DiaryEntry m_entry;
    bool m_loading = false;
    bool m_isDirty = false;
    bool m_focusMode = false;

    QLineEdit* m_titleEdit = nullptr;
    QTextEdit* m_textEdit = nullptr;
    QLabel* m_dateLabel = nullptr;
    QLabel* m_saveStateLabel = nullptr;
    QLabel* m_counterLabel = nullptr; // words, chars, reading time

    QToolBar* m_toolbar = nullptr;
    QComboBox* m_headingCombo = nullptr;
    QPushButton* m_focusBtn = nullptr;
    QPushButton* m_saveBtn = nullptr;

    QTimer* m_autosaveTimer = nullptr;
};
