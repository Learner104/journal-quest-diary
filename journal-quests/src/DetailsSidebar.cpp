#include "DetailsSidebar.h"
#include "ThemeManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

DetailsSidebar::DetailsSidebar(QWidget* parent) : QWidget(parent) {
    setupUI();
    applyTheme();
}

void DetailsSidebar::applyTheme() {
    const auto& t = ThemeManager::instance().currentTheme();
    setStyleSheet(QString("background: %1; border-left: 1px solid %2; outline: none;").arg(t.bgSidebar).arg(t.border));

    if (m_headerLabel) {
        m_headerLabel->setStyleSheet(QString(
            "font-size: 15px; font-weight: bold; color: %1; font-family: 'Georgia', serif; padding-bottom: 4px;"
        ).arg(t.textPrimary));
    }

    for (QLabel* lbl : m_sectionLabels) {
        if (lbl) {
            lbl->setStyleSheet(QString(
                "font-size: 11px; font-weight: bold; color: %1; text-transform: uppercase; letter-spacing: 0.5px; margin-top: 6px;"
            ).arg(t.textSecondary));
        }
    }

    QString inputStyle = QString(
        "QComboBox, QLineEdit, QDateTimeEdit { "
        "  background: %1; "
        "  border: 1px solid %2; "
        "  border-radius: 6px; "
        "  padding: 6px 10px; "
        "  font-size: 12px; "
        "  color: %3; "
        "  outline: none; "
        "} "
        "QComboBox:focus, QLineEdit:focus, QDateTimeEdit:focus { "
        "  border: 1px solid %4; "
        "  outline: none; "
        "} "
        "QComboBox::drop-down, QDateTimeEdit::drop-down { "
        "  border: none; "
        "  width: 20px; "
        "} "
        "QComboBox QAbstractItemView, QDateTimeEdit QCalendarWidget { "
        "  background: %5; "
        "  color: %3; "
        "  selection-background-color: %6; "
        "  selection-color: %3; "
        "  border: 1px solid %2; "
        "  padding: 4px; "
        "  outline: none; "
        "}"
    ).arg(t.bgCard).arg(t.border).arg(t.textPrimary).arg(t.borderHover).arg(t.dropdownBg).arg(t.selectionBg);

    if (m_categoryCombo) m_categoryCombo->setStyleSheet(inputStyle);
    if (m_moodCombo) m_moodCombo->setStyleSheet(inputStyle);
    if (m_weatherCombo) m_weatherCombo->setStyleSheet(inputStyle);
    if (m_tagsEdit) m_tagsEdit->setStyleSheet(inputStyle);
    if (m_dateTimeEdit) m_dateTimeEdit->setStyleSheet(inputStyle);

    if (m_favCheckbox) {
        m_favCheckbox->setStyleSheet(QString(
            "QCheckBox { font-size: 12px; font-weight: bold; color: %1; margin-top: 6px; outline: none; } "
            "QCheckBox::indicator { width: 15px; height: 15px; border-radius: 4px; border: 1px solid %2; background: %3; } "
            "QCheckBox::indicator:checked { background: %4; border-color: %4; }"
        ).arg(t.textPrimary).arg(t.border).arg(t.bgCard).arg(t.accent));
    }

    if (m_dividerLine) {
        m_dividerLine->setStyleSheet(QString("background-color: %1; border: none; height: 1px;").arg(t.border));
    }

    if (m_createdLabel) {
        m_createdLabel->setStyleSheet(QString("font-size: 11px; color: %1;").arg(t.textSecondary));
    }
    if (m_updatedLabel) {
        m_updatedLabel->setStyleSheet(QString("font-size: 11px; color: %1;").arg(t.textSecondary));
    }
    if (m_docxLabel) {
        m_docxLabel->setStyleSheet(QString(
            "font-size: 10px; color: %1; background: %2; border: 1px solid %3; padding: 6px 8px; border-radius: 6px;"
        ).arg(t.textPrimary).arg(t.bgCard).arg(t.border));
    }
}

void DetailsSidebar::setupUI() {
    setFixedWidth(240);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 20, 16, 20);
    layout->setSpacing(10);

    m_headerLabel = new QLabel("Entry Details", this);
    layout->addWidget(m_headerLabel);

    auto addSectionLabel = [this, layout](const QString& title) {
        QLabel* l = new QLabel(title, this);
        m_sectionLabels.append(l);
        layout->addWidget(l);
    };

    // Category
    addSectionLabel("Collection / Category");
    m_categoryCombo = new QComboBox(this);
    m_categoryCombo->addItems({"Daily", "Reflections", "Creative", "Travel", "Ideas", "Memories", "Word Docs"});
    m_categoryCombo->setEditable(true);
    connect(m_categoryCombo, &QComboBox::currentTextChanged, this, &DetailsSidebar::onFieldModified);
    layout->addWidget(m_categoryCombo);

    // Mood
    addSectionLabel("Mood");
    m_moodCombo = new QComboBox(this);
    m_moodCombo->addItems({"🌱 Peaceful", "☀️ Joyful", "🌌 Reflective", "🔥 Focused", "🌧️ Melancholy", "✨ Inspired"});
    connect(m_moodCombo, &QComboBox::currentTextChanged, this, &DetailsSidebar::onFieldModified);
    layout->addWidget(m_moodCombo);

    // Weather (Manual, privacy first, no API)
    addSectionLabel("Weather (Offline)");
    m_weatherCombo = new QComboBox(this);
    m_weatherCombo->addItems({"☀️ Clear & Sunny", "⛅ Partly Cloudy", "🌧️ Gentle Rain", "⛈️ Thunderstorm", "❄️ Snowy", "🌙 Starry Night"});
    connect(m_weatherCombo, &QComboBox::currentTextChanged, this, &DetailsSidebar::onFieldModified);
    layout->addWidget(m_weatherCombo);

    // Tags
    addSectionLabel("Tags (Comma-separated)");
    m_tagsEdit = new QLineEdit(this);
    m_tagsEdit->setPlaceholderText("e.g. cozy, evening, project");
    connect(m_tagsEdit, &QLineEdit::textChanged, this, &DetailsSidebar::onFieldModified);
    layout->addWidget(m_tagsEdit);

    // Entry Date & Time (Editable)
    addSectionLabel("Entry Date & Time");
    m_dateTimeEdit = new QDateTimeEdit(this);
    m_dateTimeEdit->setCalendarPopup(true);
    m_dateTimeEdit->setDisplayFormat("MMM d, yyyy  h:mm AP");
    connect(m_dateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &DetailsSidebar::onFieldModified);
    layout->addWidget(m_dateTimeEdit);

    // Favorite toggle
    m_favCheckbox = new QCheckBox("★ Add to Favorites", this);
    connect(m_favCheckbox, &QCheckBox::toggled, this, &DetailsSidebar::onFieldModified);
    layout->addWidget(m_favCheckbox);

    // Divider
    m_dividerLine = new QFrame(this);
    m_dividerLine->setFrameShape(QFrame::HLine);
    m_dividerLine->setFixedHeight(1);
    layout->addWidget(m_dividerLine);

    // Timestamps and Source
    addSectionLabel("Timestamps");
    m_createdLabel = new QLabel("Created: Just now", this);
    m_createdLabel->setStyleSheet("font-size: 11px; color: #8A7B6D;");
    layout->addWidget(m_createdLabel);

    m_updatedLabel = new QLabel("Updated: Just now", this);
    m_updatedLabel->setStyleSheet("font-size: 11px; color: #8A7B6D;");
    layout->addWidget(m_updatedLabel);

    m_docxLabel = new QLabel("", this);
    m_docxLabel->setWordWrap(true);
    m_docxLabel->setStyleSheet("font-size: 10px; color: #6E5C4E; background: #E9DFD1; padding: 4px; border-radius: 4px;");
    m_docxLabel->setVisible(false);
    layout->addWidget(m_docxLabel);

    layout->addStretch();
}

void DetailsSidebar::loadEntry(const DiaryEntry& entry) {
    m_loading = true;

    int catIdx = m_categoryCombo->findText(entry.category);
    if (catIdx >= 0) m_categoryCombo->setCurrentIndex(catIdx);
    else m_categoryCombo->setEditText(entry.category);

    int moodIdx = m_moodCombo->findText(entry.mood, Qt::MatchContains);
    if (moodIdx >= 0) m_moodCombo->setCurrentIndex(moodIdx);

    int weatherIdx = m_weatherCombo->findText(entry.weather, Qt::MatchContains);
    if (weatherIdx >= 0) m_weatherCombo->setCurrentIndex(weatherIdx);

    m_tagsEdit->setText(entry.tags.join(", "));
    if (entry.createdAt.isValid()) {
        m_dateTimeEdit->setDateTime(entry.createdAt);
    } else {
        m_dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    }
    m_favCheckbox->setChecked(entry.isFavorite);

    m_createdLabel->setText("Created: " + (entry.createdAt.isValid() ? entry.createdAt.toString("MMM d, yyyy h:mm AP") : "Unsaved"));
    m_updatedLabel->setText("Updated: " + (entry.updatedAt.isValid() ? entry.updatedAt.toString("MMM d, yyyy h:mm AP") : "Unsaved"));

    if (!entry.originalDocxPath.isEmpty()) {
        m_docxLabel->setText("Linked Word Doc:\n" + entry.originalDocxPath);
        m_docxLabel->setVisible(true);
    } else {
        m_docxLabel->setVisible(false);
    }

    m_loading = false;
}

void DetailsSidebar::applyToEntry(DiaryEntry& entry) {
    entry.category = m_categoryCombo->currentText();
    entry.mood = m_moodCombo->currentText();
    entry.weather = m_weatherCombo->currentText();
    entry.tags = m_tagsEdit->text().split(',', Qt::SkipEmptyParts);
    for (QString& t : entry.tags) {
        t = t.trimmed();
    }
    if (m_dateTimeEdit) {
        entry.createdAt = m_dateTimeEdit->dateTime();
    }
    entry.isFavorite = m_favCheckbox->isChecked();
}

void DetailsSidebar::onFieldModified() {
    if (m_loading) return;
    emit detailsChanged();
}
