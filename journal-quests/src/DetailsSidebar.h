#pragma once

#include "Models.h"
#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QDateTimeEdit>

class DetailsSidebar : public QWidget {
    Q_OBJECT
public:
    explicit DetailsSidebar(QWidget* parent = nullptr);

    void loadEntry(const DiaryEntry& entry);
    void applyToEntry(DiaryEntry& entry);
    void applyTheme();

signals:
    void detailsChanged();

private slots:
    void onFieldModified();

private:
    void setupUI();

    bool m_loading = false;
    QLabel* m_headerLabel = nullptr;
    QList<QLabel*> m_sectionLabels;
    QFrame* m_dividerLine = nullptr;
    QComboBox* m_categoryCombo = nullptr;
    QComboBox* m_moodCombo = nullptr;
    QComboBox* m_weatherCombo = nullptr;
    QLineEdit* m_tagsEdit = nullptr;
    QDateTimeEdit* m_dateTimeEdit = nullptr;
    QCheckBox* m_favCheckbox = nullptr;
    QLabel* m_createdLabel = nullptr;
    QLabel* m_updatedLabel = nullptr;
    QLabel* m_docxLabel = nullptr;
};
