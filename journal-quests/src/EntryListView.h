#pragma once

#include "Models.h"
#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

class EntryListView : public QWidget {
    Q_OBJECT
public:
    explicit EntryListView(QWidget* parent = nullptr);

    void applyTheme();
    void setViewMode(const QString& modeTitle, const QVector<DiaryEntry>& entries, bool isTrashMode = false);

signals:
    void entrySelected(qint64 entryId);
    void newEntryRequested();
    void restoreEntryRequested(qint64 entryId);
    void permanentDeleteRequested(qint64 entryId);

private slots:
    void onItemClicked(QListWidgetItem* item);
    void onSearchTextChanged(const QString& text);

private:
    void setupUI();
    void populate(const QVector<DiaryEntry>& entries);

    QLabel* m_titleLabel = nullptr;
    QLineEdit* m_searchEdit = nullptr;
    QListWidget* m_listWidget = nullptr;
    QPushButton* m_newBtn = nullptr;

    bool m_isTrashMode = false;
    QVector<DiaryEntry> m_currentEntries;
};
