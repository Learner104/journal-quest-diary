#pragma once

#include "Models.h"
#include "ThemeManager.h"
#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>

class SettingsView : public QWidget {
    Q_OBJECT
public:
    explicit SettingsView(QWidget* parent = nullptr);

    void refresh();
    void applyTheme();

private slots:
    void onSaveSettings();
    void onRevertSettings();

signals:
    void backupExportRequested();
    void backupImportRequested();
    void gamificationToggled(bool enabled);
    void themeChanged(ThemeType type);

private:
    void setupUI();

    QScrollArea* m_scrollArea = nullptr;
    QWidget* m_containerWidget = nullptr;
    QComboBox* m_themeCombo = nullptr;
    QCheckBox* m_gamificationCheck = nullptr;
    QPushButton* m_saveBtn = nullptr;
    QPushButton* m_revertBtn = nullptr;
    QPushButton* m_exportBtn = nullptr;
    QPushButton* m_importBtn = nullptr;
    QLabel* m_statusLabel = nullptr;
};
