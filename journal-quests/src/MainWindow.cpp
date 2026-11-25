#include "MainWindow.h"
#include "DatabaseManager.h"
#include "DocxHandler.h"
#include "ThemeManager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QKeySequence>
#include <QRandomGenerator>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Journal Quest — A Personal Offline Diary");
    resize(1200, 780);
    setMinimumSize(800, 500); // Allow flexible resizing down to 800x500
    setAcceptDrops(true);

    setupUI();

    // Global Shortcuts
    new QShortcut(QKeySequence("Ctrl+N"), this, SLOT(createNewEntry()));
    new QShortcut(QKeySequence("Ctrl+F"), this, [this]() {
        // Go to All Entries with search
        m_navList->setCurrentRow(2); // "All Entries"
    });
    new QShortcut(QKeySequence("Ctrl+B"), this, SLOT(toggleSidebar()));
    new QShortcut(QKeySequence("F11"), this, [this]() {
        if (isFullScreen()) {
            showNormal();
        } else {
            showFullScreen();
        }
    });
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* rootLayout = new QHBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 1. Left Sidebar Navigation
    setupLeftNav();
    rootLayout->addWidget(m_leftSidebar);

    // 2. Center Content Stack
    m_contentStack = new QStackedWidget(this);

    m_homeScreen = new HomeScreen(this);
    m_workspace = new WritingWorkspace(this);
    m_listView = new EntryListView(this);
    m_settingsView = new SettingsView(this);
    m_utilitiesView = new UtilitiesView(this);

    m_contentStack->addWidget(m_homeScreen);   // index 0
    m_contentStack->addWidget(m_workspace);    // index 1
    m_contentStack->addWidget(m_listView);     // index 2
    m_contentStack->addWidget(m_settingsView); // index 3
    m_contentStack->addWidget(m_utilitiesView);// index 4

    rootLayout->addWidget(m_contentStack, 1);

    // 3. Right Details Sidebar
    m_detailsSidebar = new DetailsSidebar(this);
    rootLayout->addWidget(m_detailsSidebar);

    // Connections: Home Screen
    connect(m_homeScreen, &HomeScreen::openEntry, this, &MainWindow::openEntryById);
    connect(m_homeScreen, &HomeScreen::newEntryRequested, this, &MainWindow::createNewEntry);
    connect(m_homeScreen, &HomeScreen::openRandomMemory, this, &MainWindow::openRandomMemory);

    // Connections: Writing Workspace
    connect(m_workspace, &WritingWorkspace::entrySaved, this, &MainWindow::onWritingEntrySaved);
    connect(m_workspace, &WritingWorkspace::requestToggleFocus, this, &MainWindow::toggleFocusMode);
    connect(m_workspace, &WritingWorkspace::docxFileDropped, this, [this](const QString& path) {
        importDocxFile(path);
    });

    // Connections: Details Sidebar
    connect(m_detailsSidebar, &DetailsSidebar::detailsChanged, this, &MainWindow::onDetailsChanged);

    // Connections: List View
    connect(m_listView, &EntryListView::entrySelected, this, &MainWindow::openEntryById);
    connect(m_listView, &EntryListView::newEntryRequested, this, &MainWindow::createNewEntry);
    connect(m_listView, &EntryListView::restoreEntryRequested, this, &MainWindow::restoreEntry);
    connect(m_listView, &EntryListView::permanentDeleteRequested, this, &MainWindow::permanentDeleteEntry);

    // Connections: Settings View
    connect(m_settingsView, &SettingsView::backupExportRequested, this, &MainWindow::exportBackupJson);
    connect(m_settingsView, &SettingsView::backupImportRequested, this, &MainWindow::importBackupJson);
    connect(m_settingsView, &SettingsView::gamificationToggled, this, [this](bool) {
        m_homeScreen->refresh();
    });

    // Connections: Utilities View
    connect(m_utilitiesView, &UtilitiesView::entrySelected, this, &MainWindow::openEntryById);
    connect(m_utilitiesView, &UtilitiesView::newEntryRequested, this, &MainWindow::createNewEntry);
    connect(m_settingsView, &SettingsView::themeChanged, this, [this](ThemeType) {
        applyTheme();
    });

    applyTheme();

    // Start on Home Screen
    m_contentStack->setCurrentWidget(m_homeScreen);
    m_detailsSidebar->setVisible(false);
}

void MainWindow::applyTheme() {
    const auto& t = ThemeManager::instance().currentTheme();

    if (m_leftSidebar) {
        m_leftSidebar->setStyleSheet(QString("background: %1; border-right: 1px solid %2;").arg(t.bgSidebar).arg(t.border));
    }

    if (m_toggleSidebarBtn) {
        m_toggleSidebarBtn->setStyleSheet(QString(
            "QPushButton { background: transparent; border: none; font-size: 13px; color: %1; border-radius: 4px; padding: 2px; } "
            "QPushButton:hover { background: %2; color: %3; } "
            "QPushButton:focus { outline: none; }"
        ).arg(t.textSecondary).arg(t.dropdownHover).arg(t.textPrimary));
    }

    if (m_brandLabel) {
        m_brandLabel->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1; font-family: 'Georgia', serif;").arg(t.textPrimary));
    }

    if (m_newEntryBtn) {
        m_newEntryBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: #FFFFFF; font-size: 13px; font-weight: bold; padding: 10px 14px; border-radius: 8px; border: none; } "
            "QPushButton:hover { background: %2; }"
        ).arg(t.accent).arg(t.accentHover));
    }

    if (m_navList) {
        m_navList->setStyleSheet(QString(
            "QListWidget { background: transparent; border: none; font-size: 13px; color: %1; font-weight: 500; outline: none; } "
            "QListWidget::item { padding: 9px 12px; border-radius: 6px; margin-bottom: 3px; outline: none; } "
            "QListWidget::item:hover { background: %2; } "
            "QListWidget::item:selected { background: %3; color: %1; font-weight: bold; outline: none; }"
        ).arg(t.textPrimary).arg(t.border).arg(t.selectionBg));
    }

    if (m_importDocxBtn) {
        m_importDocxBtn->setStyleSheet(QString(
            "QPushButton { background: %1; border: 1px solid %2; border-radius: 8px; padding: 8px 10px; font-size: 11px; font-weight: bold; color: %3; text-align: center; } "
            "QPushButton:hover { background: %4; border-color: %5; }"
        ).arg(t.bgCard).arg(t.border).arg(t.textPrimary).arg(t.dropdownHover).arg(t.accent));
    }

    if (m_exportDocxBtn) {
        m_exportDocxBtn->setStyleSheet(QString(
            "QPushButton { background: %1; border: 1px solid %2; border-radius: 6px; padding: 6px 10px; font-size: 11px; font-weight: bold; color: %3; } "
            "QPushButton:hover { background: %4; }"
        ).arg(t.bgCard).arg(t.border).arg(t.textPrimary).arg(t.dropdownHover));
    }

    if (m_workspace) m_workspace->applyTheme();
    if (m_detailsSidebar) m_detailsSidebar->applyTheme();
    if (m_homeScreen) m_homeScreen->applyTheme();
    if (m_listView) m_listView->applyTheme();
    if (m_settingsView) m_settingsView->applyTheme();
    if (m_utilitiesView) m_utilitiesView->applyTheme();
}

void MainWindow::setupLeftNav() {
    m_leftSidebar = new QWidget(this);
    m_leftSidebar->setFixedWidth(230);
    m_leftSidebar->setStyleSheet("background: #EDE4D5; border-right: 1px solid #D9CEBF;");

    QVBoxLayout* layout = new QVBoxLayout(m_leftSidebar);
    layout->setContentsMargins(12, 18, 12, 16);
    layout->setSpacing(14);

    // App Brand Logo / Title & Toggle Collapse Button
    QHBoxLayout* topHeader = new QHBoxLayout();
    m_brandLabel = new QLabel("📖 Journal Quest", m_leftSidebar);
    m_brandLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #3A2E25; font-family: 'Georgia', serif;");

    m_toggleSidebarBtn = new QPushButton("◀", m_leftSidebar);
    m_toggleSidebarBtn->setFixedSize(26, 26);
    m_toggleSidebarBtn->setCursor(Qt::PointingHandCursor);
    m_toggleSidebarBtn->setFocusPolicy(Qt::NoFocus);
    m_toggleSidebarBtn->setToolTip("Toggle Sidebar (Ctrl+B)");
    connect(m_toggleSidebarBtn, &QPushButton::clicked, this, &MainWindow::toggleSidebar);

    topHeader->addWidget(m_brandLabel);
    topHeader->addStretch();
    topHeader->addWidget(m_toggleSidebarBtn);
    layout->addLayout(topHeader);

    // Prominent New Entry Button
    m_newEntryBtn = new QPushButton("✦ New Entry", m_leftSidebar);
    m_newEntryBtn->setCursor(Qt::PointingHandCursor);
    m_newEntryBtn->setStyleSheet("QPushButton { background: #8E5A3C; color: #FFFFFF; font-size: 13px; font-weight: bold; padding: 10px 14px; border-radius: 8px; border: none; } QPushButton:hover { background: #7A4B30; }");
    connect(m_newEntryBtn, &QPushButton::clicked, this, &MainWindow::createNewEntry);
    layout->addWidget(m_newEntryBtn);

    // Navigation Menu
    m_navList = new QListWidget(m_leftSidebar);
    m_navList->setFocusPolicy(Qt::NoFocus);
    m_navList->setStyleSheet("QListWidget { background: transparent; border: none; font-size: 13px; color: #4A3E31; font-weight: 500; outline: none; } "
                             "QListWidget::item { padding: 9px 12px; border-radius: 6px; margin-bottom: 3px; } "
                             "QListWidget::item:hover { background: #DFD3C3; } "
                             "QListWidget::item:selected { background: #D5C5B2; color: #2A1F17; font-weight: bold; }");

    auto addNavItem = [this](const QString& title, const QString& icon) {
        QListWidgetItem* item = new QListWidgetItem(icon + "  " + title, m_navList);
        item->setToolTip(title);
    };

    addNavItem("Home", "🏠");
    addNavItem("My Diary", "✍️");
    addNavItem("All Entries", "📚");
    addNavItem("Favorites", "⭐");
    addNavItem("Collections", "📁");
    addNavItem("Word Documents", "📄");
    addNavItem("Utilities", "🧰");
    addNavItem("Trash", "🗑️");
    addNavItem("Settings", "⚙️");

    connect(m_navList, &QListWidget::itemClicked, this, &MainWindow::onNavItemSelected);
    layout->addWidget(m_navList, 1);

    // Word Doc Actions at bottom of sidebar
    m_importDocxBtn = new QPushButton("📥 Import .docx", m_leftSidebar);
    m_importDocxBtn->setCursor(Qt::PointingHandCursor);
    m_importDocxBtn->setToolTip("Import docx file");
    connect(m_importDocxBtn, &QPushButton::clicked, this, [this]() {
        importDocxFile();
    });
    layout->addWidget(m_importDocxBtn);

    m_exportDocxBtn = new QPushButton("📤 Export to .docx", m_leftSidebar);
    m_exportDocxBtn->setCursor(Qt::PointingHandCursor);
    m_exportDocxBtn->setToolTip("Export current entry to docx");
    connect(m_exportDocxBtn, &QPushButton::clicked, this, &MainWindow::exportCurrentAsDocx);
    layout->addWidget(m_exportDocxBtn);

    m_navList->setCurrentRow(0);
}

void MainWindow::toggleSidebar() {
    m_sidebarCollapsed = !m_sidebarCollapsed;

    if (m_sidebarCollapsed) {
        m_leftSidebar->setFixedWidth(64);
        m_toggleSidebarBtn->setText("▶");
        m_toggleSidebarBtn->setToolTip("Expand Sidebar (Ctrl+B)");
        if (m_brandLabel) m_brandLabel->setVisible(false);
        if (m_newEntryBtn) {
            m_newEntryBtn->setText("✦");
            m_newEntryBtn->setToolTip("New Entry (Ctrl+N)");
        }
        if (m_importDocxBtn) {
            m_importDocxBtn->setText("📥");
        }
        if (m_exportDocxBtn) {
            m_exportDocxBtn->setText("📤");
        }
        for (int i = 0; i < m_navList->count(); ++i) {
            QListWidgetItem* it = m_navList->item(i);
            QString t = it->toolTip();
            // Show only icon
            if (t == "Home") it->setText("🏠");
            else if (t == "My Diary") it->setText("✍️");
            else if (t == "All Entries") it->setText("📚");
            else if (t == "Favorites") it->setText("⭐");
            else if (t == "Collections") it->setText("📁");
            else if (t == "Word Documents") it->setText("📄");
            else if (t == "Utilities") it->setText("🧰");
            else if (t == "Trash") it->setText("🗑️");
            else if (t == "Settings") it->setText("⚙️");
            it->setTextAlignment(Qt::AlignCenter);
        }
    } else {
        m_leftSidebar->setFixedWidth(230);
        m_toggleSidebarBtn->setText("◀");
        m_toggleSidebarBtn->setToolTip("Collapse Sidebar (Ctrl+B)");
        if (m_brandLabel) m_brandLabel->setVisible(true);
        if (m_newEntryBtn) {
            m_newEntryBtn->setText("✦ New Entry");
            m_newEntryBtn->setToolTip("");
        }
        if (m_importDocxBtn) {
            m_importDocxBtn->setText("📥 Import .docx");
        }
        if (m_exportDocxBtn) {
            m_exportDocxBtn->setText("📤 Export to .docx");
        }
        for (int i = 0; i < m_navList->count(); ++i) {
            QListWidgetItem* it = m_navList->item(i);
            QString t = it->toolTip();
            if (t == "Home") it->setText("🏠  Home");
            else if (t == "My Diary") it->setText("✍️  My Diary");
            else if (t == "All Entries") it->setText("📚  All Entries");
            else if (t == "Favorites") it->setText("⭐  Favorites");
            else if (t == "Collections") it->setText("📁  Collections");
            else if (t == "Word Documents") it->setText("📄  Word Documents");
            else if (t == "Utilities") it->setText("🧰  Utilities");
            else if (t == "Trash") it->setText("🗑️  Trash");
            else if (t == "Settings") it->setText("⚙️  Settings");
            it->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }
}

void MainWindow::onNavItemSelected(QListWidgetItem* item) {
    if (!item) return;
    int row = m_navList->row(item);

    switch (row) {
        case 0: // Home
            m_homeScreen->refresh();
            m_contentStack->setCurrentWidget(m_homeScreen);
            m_detailsSidebar->setVisible(false);
            break;
        case 1: // My Diary (Writing workspace)
            m_contentStack->setCurrentWidget(m_workspace);
            m_detailsSidebar->setVisible(true);
            break;
        case 2: // All Entries
            m_listView->setViewMode("All Entries", DatabaseManager::instance().getAllEntries(false), false);
            m_contentStack->setCurrentWidget(m_listView);
            m_detailsSidebar->setVisible(false);
            break;
        case 3: // Favorites
            m_listView->setViewMode("Favorite Entries", DatabaseManager::instance().getFavoriteEntries(), false);
            m_contentStack->setCurrentWidget(m_listView);
            m_detailsSidebar->setVisible(false);
            break;
        case 4: // Collections
            m_listView->setViewMode("Collections & Folders", DatabaseManager::instance().getAllEntries(false), false);
            m_contentStack->setCurrentWidget(m_listView);
            m_detailsSidebar->setVisible(false);
            break;
        case 5: // Word Documents
            m_listView->setViewMode("Word Documents (.docx)", DatabaseManager::instance().getWordDocuments(), false);
            m_contentStack->setCurrentWidget(m_listView);
            m_detailsSidebar->setVisible(false);
            break;
        case 6: // Utilities
            m_utilitiesView->refresh();
            m_contentStack->setCurrentWidget(m_utilitiesView);
            m_detailsSidebar->setVisible(false);
            break;
        case 7: // Trash
            m_listView->setViewMode("Trash (Recover or delete)", DatabaseManager::instance().getTrashEntries(), true);
            m_contentStack->setCurrentWidget(m_listView);
            m_detailsSidebar->setVisible(false);
            break;
        case 8: // Settings
            m_settingsView->refresh();
            m_contentStack->setCurrentWidget(m_settingsView);
            m_detailsSidebar->setVisible(false);
            break;
    }
}

void MainWindow::createNewEntry() {
    m_workspace->clear();
    m_currentEntryId = -1;

    DiaryEntry newEntry;
    newEntry.title = "";
    newEntry.category = "Daily";
    newEntry.mood = "Peaceful";
    newEntry.weather = "Sunny";
    newEntry.createdAt = QDateTime::currentDateTime();

    m_detailsSidebar->loadEntry(newEntry);
    m_contentStack->setCurrentWidget(m_workspace);
    m_detailsSidebar->setVisible(true);
    m_navList->setCurrentRow(1); // "My Diary"
}

void MainWindow::openEntryById(qint64 id) {
    DiaryEntry entry;
    if (DatabaseManager::instance().getEntry(id, entry)) {
        m_currentEntryId = entry.id;
        m_workspace->loadEntry(entry);
        m_detailsSidebar->loadEntry(entry);
        m_contentStack->setCurrentWidget(m_workspace);
        m_detailsSidebar->setVisible(true);
        m_navList->setCurrentRow(1);
    }
}

void MainWindow::openRandomMemory() {
    QVector<DiaryEntry> entries = DatabaseManager::instance().getAllEntries(false);
    if (entries.isEmpty()) {
        QMessageBox::information(this, "No Memories Yet", "Write your first entry to start creating memories!");
        return;
    }

    int idx = QRandomGenerator::global()->bounded(entries.size());
    openEntryById(entries[idx].id);
}

void MainWindow::onWritingEntrySaved(const DiaryEntry& entry) {
    m_currentEntryId = entry.id;
    m_detailsSidebar->loadEntry(entry);
}

void MainWindow::onDetailsChanged() {
    DiaryEntry current = m_workspace->currentEntry();
    m_detailsSidebar->applyToEntry(current);
    m_workspace->loadEntry(current);
    m_workspace->manualSave();
}

void MainWindow::toggleFocusMode() {
    bool focus = m_workspace->isFocusMode();
    m_leftSidebar->setVisible(!focus);
    m_detailsSidebar->setVisible(!focus && m_contentStack->currentWidget() == m_workspace);
}

void MainWindow::importDocxFile(const QString& specificPath) {
    QString filePath = specificPath;
    if (filePath.isEmpty()) {
        filePath = QFileDialog::getOpenFileName(this, "Import Word Document", QDir::homePath(), "Word Documents (*.docx)");
    }
    if (filePath.isEmpty()) return;

    QString html, plain, title, error;
    if (!DocxHandler::importDocx(filePath, html, plain, title, error)) {
        QMessageBox::critical(this, "Import Failed", "Could not import Word document:\n" + error);
        return;
    }

    DiaryEntry entry;
    entry.title = title;
    entry.contentHtml = html;
    entry.contentPlainText = plain;
    entry.category = "Word Docs";
    entry.originalDocxPath = filePath;
    entry.wordCount = plain.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).count();

    qint64 newId = DatabaseManager::instance().createEntry(entry);
    if (newId > 0) {
        openEntryById(newId);
        QMessageBox::information(this, "Document Imported", QString("Imported '%1' successfully!\nLinked to: %2").arg(title).arg(filePath));
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        const QList<QUrl> urls = event->mimeData()->urls();
        for (const QUrl& url : urls) {
            if (url.toLocalFile().endsWith(".docx", Qt::CaseInsensitive)) {
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void MainWindow::dropEvent(QDropEvent* event) {
    const QList<QUrl> urls = event->mimeData()->urls();
    for (const QUrl& url : urls) {
        QString localPath = url.toLocalFile();
        if (localPath.endsWith(".docx", Qt::CaseInsensitive)) {
            importDocxFile(localPath);
            event->acceptProposedAction();
            return;
        }
    }
}

void MainWindow::exportCurrentAsDocx() {
    DiaryEntry current = m_workspace->currentEntry();
    if (current.contentPlainText.isEmpty() && current.title.isEmpty()) {
        QMessageBox::warning(this, "Empty Entry", "Nothing to export yet.");
        return;
    }

    QString defaultName = current.title.isEmpty() ? "Diary Entry" : current.title;
    defaultName.replace(QRegularExpression("[^a-zA-Z0-9_ -]"), "_");

    QString savePath = QFileDialog::getSaveFileName(this, "Export to Word Document", QDir::homePath() + "/" + defaultName + ".docx", "Word Document (*.docx)");
    if (savePath.isEmpty()) return;

    QString error;
    if (DocxHandler::exportDocx(savePath, current.title, current.contentHtml, error)) {
        QMessageBox::information(this, "Export Successful", "Saved document to:\n" + savePath);
    } else {
        QMessageBox::critical(this, "Export Failed", "Failed to export .docx:\n" + error);
    }
}

void MainWindow::saveBackToOriginalDocx() {
    DiaryEntry current = m_workspace->currentEntry();
    if (current.originalDocxPath.isEmpty()) {
        exportCurrentAsDocx();
        return;
    }

    auto reply = QMessageBox::question(this, "Confirm Overwrite",
                                       QString("Are you sure you want to save changes directly to the original file?\n%1").arg(current.originalDocxPath),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    QString error;
    if (DocxHandler::exportDocx(current.originalDocxPath, current.title, current.contentHtml, error)) {
        QMessageBox::information(this, "Saved", "Original Word document updated successfully.");
    } else {
        QMessageBox::critical(this, "Save Failed", "Failed to overwrite .docx:\n" + error);
    }
}

void MainWindow::moveCurrentToTrash() {
    if (m_currentEntryId > 0) {
        DatabaseManager::instance().moveToTrash(m_currentEntryId);
        createNewEntry();
    }
}

void MainWindow::restoreEntry(qint64 id) {
    DatabaseManager::instance().restoreFromTrash(id);
    m_listView->setViewMode("Trash", DatabaseManager::instance().getTrashEntries(), true);
}

void MainWindow::permanentDeleteEntry(qint64 id) {
    auto reply = QMessageBox::warning(this, "Permanent Delete", "This will permanently delete this entry from your device. This cannot be undone.\nProceed?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        DatabaseManager::instance().permanentDeleteEntry(id);
        m_listView->setViewMode("Trash", DatabaseManager::instance().getTrashEntries(), true);
    }
}

void MainWindow::exportBackupJson() {
    QString savePath = QFileDialog::getSaveFileName(this, "Export Offline Backup", QDir::homePath() + "/journal_quest_backup.json", "JSON Backup (*.json)");
    if (savePath.isEmpty()) return;

    if (DatabaseManager::instance().exportBackup(savePath)) {
        QMessageBox::information(this, "Backup Exported", "All diary entries and progression data saved to:\n" + savePath);
    } else {
        QMessageBox::critical(this, "Export Failed", "Could not export backup file.");
    }
}

void MainWindow::importBackupJson() {
    QString filePath = QFileDialog::getOpenFileName(this, "Restore Offline Backup", QDir::homePath(), "JSON Backup (*.json)");
    if (filePath.isEmpty()) return;

    if (DatabaseManager::instance().importBackup(filePath)) {
        m_homeScreen->refresh();
        QMessageBox::information(this, "Backup Restored", "All entries and stats imported successfully.");
    } else {
        QMessageBox::critical(this, "Restore Failed", "Failed to parse backup JSON file.");
    }
}
