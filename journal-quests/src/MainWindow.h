#pragma once

#include "Models.h"
#include "HomeScreen.h"
#include "WritingWorkspace.h"
#include "DetailsSidebar.h"
#include "EntryListView.h"
#include "SettingsView.h"
#include "UtilitiesView.h"

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onNavItemSelected(QListWidgetItem* item);
    void createNewEntry();
    void openEntryById(qint64 id);
    void openRandomMemory();
    void onWritingEntrySaved(const DiaryEntry& entry);
    void onDetailsChanged();
    void toggleFocusMode();

    // Word Doc (.docx)
    void importDocxFile(const QString& specificPath = QString());
    void exportCurrentAsDocx();
    void saveBackToOriginalDocx();

    // Trash
    void moveCurrentToTrash();
    void restoreEntry(qint64 id);
    void permanentDeleteEntry(qint64 id);

    // Backup & Restore
    void exportBackupJson();
    void importBackupJson();
    void applyTheme();
    void toggleSidebar();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void setupUI();
    void setupLeftNav();
    void refreshViews();

    // UI components
    QWidget* m_leftSidebar = nullptr;
    QPushButton* m_toggleSidebarBtn = nullptr;
    QLabel* m_brandLabel = nullptr;
    QPushButton* m_importDocxBtn = nullptr;
    QPushButton* m_exportDocxBtn = nullptr;
    QListWidget* m_navList = nullptr;
    QPushButton* m_newEntryBtn = nullptr;

    bool m_sidebarCollapsed = false;

    QStackedWidget* m_contentStack = nullptr;
    HomeScreen* m_homeScreen = nullptr;
    WritingWorkspace* m_workspace = nullptr;
    EntryListView* m_listView = nullptr;
    SettingsView* m_settingsView = nullptr;
    UtilitiesView* m_utilitiesView = nullptr;
    DetailsSidebar* m_detailsSidebar = nullptr;

    qint64 m_currentEntryId = -1;
};
