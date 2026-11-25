#include "WritingWorkspace.h"
#include "DatabaseManager.h"
#include "ThemeManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QTextList>
#include <QTextTable>
#include <QDateTime>
#include <QRegularExpression>
#include <QShortcut>
#include <QKeySequence>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>

WritingWorkspace::WritingWorkspace(QWidget* parent) : QWidget(parent) {
    setAcceptDrops(true);
    setupUI();

    m_autosaveTimer = new QTimer(this);
    m_autosaveTimer->setSingleShot(true);
    m_autosaveTimer->setInterval(1500); // Debounce autosave after 1.5 seconds of inactivity
    connect(m_autosaveTimer, &QTimer::timeout, this, &WritingWorkspace::performAutoSave);

    // Keyboard Shortcuts
    new QShortcut(QKeySequence("Ctrl+S"), this, SLOT(manualSave()));
    connect(new QShortcut(QKeySequence("Ctrl+Shift+F"), this), &QShortcut::activated, this, [this]() {
        setFocusMode(!m_focusMode);
        emit requestToggleFocus();
    });
}

void WritingWorkspace::setupUI() {
    applyTheme();
}

void WritingWorkspace::applyTheme() {
    const auto& t = ThemeManager::instance().currentTheme();

    if (!m_titleEdit) {
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(36, 24, 36, 16);
        mainLayout->setSpacing(12);

        // 1. Top Meta Bar
        QHBoxLayout* metaBar = new QHBoxLayout();
        m_dateLabel = new QLabel(QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy • h:mm AP"), this);
        m_saveStateLabel = new QLabel("Saved", this);
        m_focusBtn = new QPushButton("⛶ Focus Mode", this);
        m_focusBtn->setCursor(Qt::PointingHandCursor);
        connect(m_focusBtn, &QPushButton::clicked, this, [this]() {
            setFocusMode(!m_focusMode);
            emit requestToggleFocus();
        });

        m_saveBtn = new QPushButton("Save Now", this);
        m_saveBtn->setCursor(Qt::PointingHandCursor);
        connect(m_saveBtn, &QPushButton::clicked, this, &WritingWorkspace::manualSave);

        metaBar->addWidget(m_dateLabel);
        metaBar->addStretch();
        metaBar->addWidget(m_saveStateLabel);
        metaBar->addSpacing(8);
        metaBar->addWidget(m_focusBtn);
        metaBar->addSpacing(8);
        metaBar->addWidget(m_saveBtn);
        mainLayout->addLayout(metaBar);

        // 2. Toolbar
        m_toolbar = new QToolBar(this);
        m_headingCombo = new QComboBox(this);
        m_headingCombo->addItem("Normal Text");
        m_headingCombo->addItem("Heading 1 (Title)");
        m_headingCombo->addItem("Heading 2 (Subtitle)");
        m_headingCombo->addItem("Heading 3 (Section)");
        connect(m_headingCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &WritingWorkspace::onHeadingChanged);
        m_toolbar->addWidget(m_headingCombo);
        m_toolbar->addSeparator();

        auto addActionBtn = [this](const QString& text, auto slot) {
            QAction* act = m_toolbar->addAction(text);
            connect(act, &QAction::triggered, this, slot);
        };

        addActionBtn("B", &WritingWorkspace::setBold);
        addActionBtn("I", &WritingWorkspace::setItalic);
        addActionBtn("U", &WritingWorkspace::setUnderline);
        addActionBtn("S", &WritingWorkspace::setStrike);
        m_toolbar->addSeparator();
        addActionBtn("Left", &WritingWorkspace::setAlignLeft);
        addActionBtn("Center", &WritingWorkspace::setAlignCenter);
        addActionBtn("Right", &WritingWorkspace::setAlignRight);
        m_toolbar->addSeparator();
        addActionBtn("• List", &WritingWorkspace::insertBulletList);
        addActionBtn("1. List", &WritingWorkspace::insertNumberedList);
        addActionBtn("❝ Quote", &WritingWorkspace::insertQuote);
        addActionBtn("⊞ Table", &WritingWorkspace::insertTable);

        mainLayout->addWidget(m_toolbar);

        // 3. Document Title
        m_titleEdit = new QLineEdit(this);
        m_titleEdit->setPlaceholderText("Title of your thoughts...");
        connect(m_titleEdit, &QLineEdit::textChanged, this, &WritingWorkspace::onContentOrTitleChanged);
        mainLayout->addWidget(m_titleEdit);

        // 4. Rich-Text Editor Canvas
        m_textEdit = new QTextEdit(this);
        m_textEdit->setPlaceholderText("Write freely... every word nurtures your personal sanctuary.");
        connect(m_textEdit, &QTextEdit::textChanged, this, &WritingWorkspace::onContentOrTitleChanged);
        mainLayout->addWidget(m_textEdit, 1);

        // 5. Bottom Info Bar
        QHBoxLayout* bottomBar = new QHBoxLayout();
        m_counterLabel = new QLabel("0 words • 0 characters • 0 min read", this);
        bottomBar->addWidget(m_counterLabel);
        bottomBar->addStretch();
        mainLayout->addLayout(bottomBar);

        updateCounters();
    }

    // Dynamic Theming
    setStyleSheet(QString("background: %1;").arg(t.bgApp));
    m_dateLabel->setStyleSheet(QString("font-size: 13px; color: %1; font-weight: 500;").arg(t.textSecondary));
    m_counterLabel->setStyleSheet(QString("font-size: 12px; color: %1;").arg(t.textSecondary));

    m_focusBtn->setStyleSheet(QString(
        "QPushButton { border: 1px solid %1; border-radius: 6px; padding: 5px 12px; background: %2; color: %3; font-size: 12px; font-weight: bold; } "
        "QPushButton:hover { background: %4; }"
    ).arg(t.border).arg(t.bgCard).arg(t.textPrimary).arg(t.border));

    m_saveBtn->setStyleSheet(QString(
        "QPushButton { border: none; border-radius: 6px; padding: 5px 14px; background: %1; color: #FFFFFF; font-size: 12px; font-weight: bold; } "
        "QPushButton:hover { background: %2; }"
    ).arg(t.accent).arg(t.accentHover));

    m_toolbar->setStyleSheet(QString(
        "QToolBar { background: %1; border: 1px solid %2; border-radius: 8px; padding: 4px; } "
        "QToolButton { background: transparent; border: none; padding: 4px 8px; border-radius: 4px; font-weight: bold; color: %3; } "
        "QToolButton:hover { background: %4; }"
    ).arg(t.bgCard).arg(t.border).arg(t.textPrimary).arg(t.dropdownHover));

    m_headingCombo->setStyleSheet(QString(
        "QComboBox {"
        "  border: 1px solid %1;"
        "  border-radius: 6px;"
        "  padding: 4px 12px 4px 10px;"
        "  background: %2;"
        "  font-size: 13px;"
        "  font-weight: 500;"
        "  color: %3;"
        "  min-width: 140px;"
        "}"
        "QComboBox:hover {"
        "  border-color: %4;"
        "}"
        "QComboBox::drop-down {"
        "  subcontrol-origin: padding;"
        "  subcontrol-position: top right;"
        "  width: 22px;"
        "  border-left: 1px solid %1;"
        "  border-top-right-radius: 6px;"
        "  border-bottom-right-radius: 6px;"
        "  background: %5;"
        "}"
        "QComboBox QAbstractItemView {"
        "  border: 1px solid %1;"
        "  border-radius: 6px;"
        "  background-color: %2;"
        "  selection-background-color: %6;"
        "  selection-color: %3;"
        "  color: %3;"
        "  font-size: 13px;"
        "  padding: 4px;"
        "  outline: 0px;"
        "}"
        "QComboBox QAbstractItemView::item {"
        "  padding: 7px 10px;"
        "  border-radius: 4px;"
        "  min-height: 22px;"
        "}"
        "QComboBox QAbstractItemView::item:hover {"
        "  background-color: %5;"
        "}"
    ).arg(t.border).arg(t.dropdownBg).arg(t.textPrimary).arg(t.borderHover).arg(t.dropdownHover).arg(t.selectionBg));

    m_titleEdit->setStyleSheet(QString(
        "QLineEdit { border: none; font-size: 26px; font-weight: 700; color: %1; background: transparent; padding: 4px 0px; font-family: 'Georgia', serif; } "
        "QLineEdit:focus { border-bottom: 2px solid %2; }"
    ).arg(t.textPrimary).arg(t.accent));

    m_textEdit->setStyleSheet(QString(
        "QTextEdit { border: none; background: %1; color: %2; font-size: 16px; line-height: 1.6; font-family: 'Georgia', serif; selection-background-color: %3; padding: 8px 0px; }"
    ).arg(t.bgApp).arg(t.textPrimary).arg(t.selectionBg));

    setSaveState(m_isDirty ? "Unsaved changes" : "Saved", m_isDirty ? t.accent : "#5B8266");
}

void WritingWorkspace::loadEntry(const DiaryEntry& entry) {
    m_loading = true;
    m_entry = entry;

    m_titleEdit->setText(entry.title);
    if (!entry.contentHtml.isEmpty()) {
        m_textEdit->setHtml(entry.contentHtml);
    } else {
        m_textEdit->setPlainText(entry.contentPlainText);
    }

    if (entry.createdAt.isValid()) {
        m_dateLabel->setText(entry.createdAt.toString("dddd, MMMM d, yyyy • h:mm AP"));
    } else {
        m_dateLabel->setText(QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy • h:mm AP"));
    }

    m_loading = false;
    m_isDirty = false;
    setSaveState("Saved", "#5B8266");
    updateCounters();
}

DiaryEntry WritingWorkspace::currentEntry() const {
    DiaryEntry e = m_entry;
    e.title = m_titleEdit->text().trimmed();
    e.contentHtml = m_textEdit->toHtml();
    e.contentPlainText = m_textEdit->toPlainText();
    e.wordCount = m_textEdit->toPlainText().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).count();
    return e;
}

void WritingWorkspace::clear() {
    m_entry = DiaryEntry();
    m_titleEdit->clear();
    m_textEdit->clear();
    m_dateLabel->setText(QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy • h:mm AP"));
    m_isDirty = false;
    setSaveState("Saved", "#5B8266");
    updateCounters();
}

void WritingWorkspace::setFocusMode(bool enabled) {
    m_focusMode = enabled;
    m_toolbar->setVisible(!enabled);
    m_focusBtn->setText(enabled ? "Exit Focus" : "⛶ Focus Mode");
}

void WritingWorkspace::onContentOrTitleChanged() {
    if (m_loading) return;

    m_isDirty = true;
    setSaveState("Unsaved changes", "#B87333");
    updateCounters();

    // Reset autosave timer
    m_autosaveTimer->start();
}

void WritingWorkspace::performAutoSave() {
    if (!m_isDirty) return;
    setSaveState("Saving...", "#4A6FA5");

    m_entry.title = m_titleEdit->text().trimmed();
    m_entry.contentHtml = m_textEdit->toHtml();
    m_entry.contentPlainText = m_textEdit->toPlainText();
    m_entry.wordCount = m_entry.contentPlainText.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).count();

    if (m_entry.id <= 0) {
        qint64 newId = DatabaseManager::instance().createEntry(m_entry);
        if (newId > 0) {
            m_entry.id = newId;
            m_isDirty = false;
            setSaveState("Saved", "#5B8266");
            emit entrySaved(m_entry);
        } else {
            setSaveState("Save failed", "#C0392B");
        }
    } else {
        bool ok = DatabaseManager::instance().updateEntry(m_entry);
        if (ok) {
            m_isDirty = false;
            setSaveState("Saved", "#5B8266");
            emit entrySaved(m_entry);
        } else {
            setSaveState("Save failed", "#C0392B");
        }
    }
}

void WritingWorkspace::manualSave() {
    m_autosaveTimer->stop();
    performAutoSave();
}

void WritingWorkspace::updateCounters() {
    QString text = m_textEdit->toPlainText();
    int chars = text.length();
    int words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).count();
    int readingMin = (words + 199) / 200; // Average 200 WPM

    m_counterLabel->setText(QString("%1 words • %2 characters • ~%3 min read")
                            .arg(words)
                            .arg(chars)
                            .arg(readingMin == 0 ? 1 : readingMin));
}

void WritingWorkspace::setSaveState(const QString& stateText, const QString& colorHex) {
    m_saveStateLabel->setText(stateText);
    m_saveStateLabel->setStyleSheet(QString("font-size: 12px; color: %1; font-weight: bold; padding: 2px 8px; border-radius: 4px; background: #EFE9DE;")
                                    .arg(colorHex));
}

// Formatting Slots
void WritingWorkspace::setBold() {
    QTextCursor cursor = m_textEdit->textCursor();
    QTextCharFormat fmt = cursor.charFormat();
    fmt.setFontWeight(fmt.fontWeight() == QFont::Bold ? QFont::Normal : QFont::Bold);
    cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);
}

void WritingWorkspace::setItalic() {
    QTextCursor cursor = m_textEdit->textCursor();
    QTextCharFormat fmt = cursor.charFormat();
    fmt.setFontItalic(!fmt.fontItalic());
    cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);
}

void WritingWorkspace::setUnderline() {
    QTextCursor cursor = m_textEdit->textCursor();
    QTextCharFormat fmt = cursor.charFormat();
    fmt.setFontUnderline(!fmt.fontUnderline());
    cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);
}

void WritingWorkspace::setStrike() {
    QTextCursor cursor = m_textEdit->textCursor();
    QTextCharFormat fmt = cursor.charFormat();
    fmt.setFontStrikeOut(!fmt.fontStrikeOut());
    cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);
}

void WritingWorkspace::setAlignLeft() {
    m_textEdit->setAlignment(Qt::AlignLeft);
}

void WritingWorkspace::setAlignCenter() {
    m_textEdit->setAlignment(Qt::AlignCenter);
}

void WritingWorkspace::setAlignRight() {
    m_textEdit->setAlignment(Qt::AlignRight);
}

void WritingWorkspace::insertBulletList() {
    QTextCursor cursor = m_textEdit->textCursor();
    cursor.insertList(QTextListFormat::ListDisc);
}

void WritingWorkspace::insertNumberedList() {
    QTextCursor cursor = m_textEdit->textCursor();
    cursor.insertList(QTextListFormat::ListDecimal);
}

void WritingWorkspace::insertQuote() {
    QTextCursor cursor = m_textEdit->textCursor();
    cursor.beginEditBlock();
    QTextBlockFormat blockFmt = cursor.blockFormat();
    blockFmt.setLeftMargin(24);
    cursor.setBlockFormat(blockFmt);

    QTextCharFormat charFmt = cursor.charFormat();
    charFmt.setFontItalic(true);
    charFmt.setForeground(QColor("#6A5843"));
    cursor.setCharFormat(charFmt);
    cursor.endEditBlock();
}

void WritingWorkspace::insertTable() {
    QTextCursor cursor = m_textEdit->textCursor();
    QTextTableFormat tableFmt;
    tableFmt.setHeaderRowCount(1);
    tableFmt.setAlignment(Qt::AlignLeft);
    tableFmt.setBorder(1);
    tableFmt.setCellPadding(6);
    tableFmt.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    cursor.insertTable(3, 3, tableFmt);
}

void WritingWorkspace::onHeadingChanged(int index) {
    QTextCursor cursor = m_textEdit->textCursor();
    QTextCharFormat fmt;

    switch (index) {
        case 1: // Heading 1
            fmt.setFontPointSize(22);
            fmt.setFontWeight(QFont::Bold);
            break;
        case 2: // Heading 2
            fmt.setFontPointSize(18);
            fmt.setFontWeight(QFont::Bold);
            break;
        case 3: // Heading 3
            fmt.setFontPointSize(15);
            fmt.setFontWeight(QFont::DemiBold);
            break;
        default: // Normal
            fmt.setFontPointSize(13);
            fmt.setFontWeight(QFont::Normal);
            break;
    }
    cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);
}

void WritingWorkspace::dragEnterEvent(QDragEnterEvent* event) {
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

void WritingWorkspace::dropEvent(QDropEvent* event) {
    const QList<QUrl> urls = event->mimeData()->urls();
    for (const QUrl& url : urls) {
        QString localPath = url.toLocalFile();
        if (localPath.endsWith(".docx", Qt::CaseInsensitive)) {
            emit docxFileDropped(localPath);
            event->acceptProposedAction();
            return;
        }
    }
}

