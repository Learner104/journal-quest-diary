#include "EntryListView.h"
#include "ThemeManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QContextMenuEvent>

EntryListView::EntryListView(QWidget* parent) : QWidget(parent) {
    setupUI();
    applyTheme();
}

void EntryListView::applyTheme() {
    const auto& t = ThemeManager::instance().currentTheme();
    setStyleSheet(QString("background: %1;").arg(t.bgApp));

    if (m_titleLabel) {
        m_titleLabel->setStyleSheet(QString("font-size: 24px; font-weight: bold; color: %1; font-family: 'Georgia', serif;").arg(t.textPrimary));
    }

    if (m_newBtn) {
        m_newBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: #FFFFFF; font-weight: bold; padding: 8px 16px; border-radius: 6px; border: none; font-size: 13px; } "
            "QPushButton:hover { background: %2; }"
        ).arg(t.accent).arg(t.accentHover));
    }

    if (m_searchEdit) {
        m_searchEdit->setStyleSheet(QString(
            "QLineEdit { background: %1; border: 1px solid %2; border-radius: 8px; padding: 8px 14px; font-size: 13px; color: %3; } "
            "QLineEdit:focus { border-color: %4; }"
        ).arg(t.bgCard).arg(t.border).arg(t.textPrimary).arg(t.accent));
    }

    if (m_listWidget) {
        m_listWidget->setStyleSheet(QString(
            "QListWidget { background: %1; border: 1px solid %2; border-radius: 8px; padding: 8px; color: %3; } "
            "QListWidget::item { padding: 12px 14px; border-bottom: 1px solid %2; border-radius: 6px; margin-bottom: 2px; color: %3; } "
            "QListWidget::item:hover { background: %4; } "
            "QListWidget::item:selected { background: %5; color: %3; }"
        ).arg(t.bgCard).arg(t.border).arg(t.textPrimary).arg(t.dropdownHover).arg(t.selectionBg));
    }
}

void EntryListView::setupUI() {
    setStyleSheet("background: #FAF7F2;");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(32, 28, 32, 28);
    mainLayout->setSpacing(16);

    // Header Bar
    QHBoxLayout* header = new QHBoxLayout();
    m_titleLabel = new QLabel("All Entries", this);
    m_titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #3A2E25; font-family: 'Georgia', serif;");

    m_newBtn = new QPushButton("+ New Entry", this);
    m_newBtn->setCursor(Qt::PointingHandCursor);
    m_newBtn->setStyleSheet("QPushButton { background: #8E5A3C; color: #FFFFFF; font-weight: bold; padding: 8px 16px; border-radius: 6px; border: none; font-size: 13px; } QPushButton:hover { background: #7A4B30; }");
    connect(m_newBtn, &QPushButton::clicked, this, &EntryListView::newEntryRequested);

    header->addWidget(m_titleLabel);
    header->addStretch();
    header->addWidget(m_newBtn);
    mainLayout->addLayout(header);

    // Search bar
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("🔍 Search by title, thoughts, tags, or categories...");
    m_searchEdit->setStyleSheet("QLineEdit { background: #FFFFFF; border: 1px solid #D5C7B2; border-radius: 8px; padding: 8px 14px; font-size: 13px; color: #3E3228; } QLineEdit:focus { border-color: #8E5A3C; }");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &EntryListView::onSearchTextChanged);
    mainLayout->addWidget(m_searchEdit);

    // List View
    m_listWidget = new QListWidget(this);
    m_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_listWidget->setStyleSheet("QListWidget { background: #FFFFFF; border: 1px solid #E4DACB; border-radius: 8px; padding: 8px; } "
                                "QListWidget::item { padding: 12px 14px; border-bottom: 1px solid #F0E8DC; border-radius: 6px; margin-bottom: 2px; } "
                                "QListWidget::item:hover { background: #FAF5ED; } "
                                "QListWidget::item:selected { background: #EFE4D3; color: #2E251E; }");
    connect(m_listWidget, &QListWidget::itemClicked, this, &EntryListView::onItemClicked);
    connect(m_listWidget, &QListWidget::customContextMenuRequested, this, [this](const QPoint& pos) {
        QListWidgetItem* item = m_listWidget->itemAt(pos);
        if (!item) return;
        qint64 id = item->data(Qt::UserRole).toLongLong();
        if (id <= 0) return;

        QMenu menu(this);
        if (m_isTrashMode) {
            QAction* restoreAct = menu.addAction("♻️ Restore Entry");
            QAction* deleteAct = menu.addAction("❌ Delete Permanently");
            QAction* selected = menu.exec(m_listWidget->mapToGlobal(pos));
            if (selected == restoreAct) emit restoreEntryRequested(id);
            else if (selected == deleteAct) emit permanentDeleteRequested(id);
        } else {
            QAction* openAct = menu.addAction("📖 Open Entry");
            QAction* selected = menu.exec(m_listWidget->mapToGlobal(pos));
            if (selected == openAct) emit entrySelected(id);
        }
    });

    mainLayout->addWidget(m_listWidget, 1);
}

void EntryListView::setViewMode(const QString& modeTitle, const QVector<DiaryEntry>& entries, bool isTrashMode) {
    m_titleLabel->setText(modeTitle);
    m_isTrashMode = isTrashMode;
    m_currentEntries = entries;
    m_searchEdit->clear();
    m_newBtn->setVisible(!isTrashMode);

    populate(m_currentEntries);
}

void EntryListView::populate(const QVector<DiaryEntry>& entries) {
    m_listWidget->clear();

    for (const auto& e : entries) {
        QString snippet = e.contentPlainText.left(120).replace("\n", " ");
        if (e.contentPlainText.length() > 120) snippet += "...";

        QString fav = e.isFavorite ? " ★" : "";
        QString wordTag = e.originalDocxPath.isEmpty() ? "" : " [DOCX]";

        QString itemText = QString("%1%2%3  •  %4\n%5\n%6  |  %7  |  %8 words")
                               .arg(e.title.isEmpty() ? "Untitled Entry" : e.title)
                               .arg(fav)
                               .arg(wordTag)
                               .arg(e.createdAt.toString("MMM d, yyyy"))
                               .arg(snippet.isEmpty() ? "<i>(No text preview)</i>" : snippet)
                               .arg(e.mood)
                               .arg(e.category)
                               .arg(e.wordCount);

        QListWidgetItem* item = new QListWidgetItem(itemText, m_listWidget);
        item->setData(Qt::UserRole, e.id);
    }

    if (entries.isEmpty()) {
        QListWidgetItem* emptyItem = new QListWidgetItem(m_isTrashMode ? "🗑️ Trash is completely empty." : "🌱 No entries found.", m_listWidget);
        emptyItem->setFlags(Qt::NoItemFlags);
    }
}

void EntryListView::onItemClicked(QListWidgetItem* item) {
    if (!item) return;
    qint64 id = item->data(Qt::UserRole).toLongLong();
    if (id > 0 && !m_isTrashMode) {
        emit entrySelected(id);
    }
}

void EntryListView::onSearchTextChanged(const QString& text) {
    if (text.trimmed().isEmpty()) {
        populate(m_currentEntries);
        return;
    }

    QVector<DiaryEntry> filtered;
    for (const auto& e : m_currentEntries) {
        if (e.title.contains(text, Qt::CaseInsensitive) ||
            e.contentPlainText.contains(text, Qt::CaseInsensitive) ||
            e.category.contains(text, Qt::CaseInsensitive) ||
            e.tags.join(" ").contains(text, Qt::CaseInsensitive)) {
            filtered.append(e);
        }
    }
    populate(filtered);
}
