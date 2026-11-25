#include "SettingsView.h"
#include "DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QSettings>

SettingsView::SettingsView(QWidget* parent) : QWidget(parent) {
    setupUI();
    applyTheme();
    refresh();
}

void SettingsView::setupUI() {
    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_containerWidget = new QWidget(m_scrollArea);
    QVBoxLayout* mainLayout = new QVBoxLayout(m_containerWidget);
    mainLayout->setContentsMargins(40, 32, 40, 32);
    mainLayout->setSpacing(24);

    QLabel* titleLabel = new QLabel("Settings & Personalization", m_containerWidget);
    titleLabel->setStyleSheet("font-size: 26px; font-weight: bold; font-family: 'Georgia', serif;");
    mainLayout->addWidget(titleLabel);

    // Section 1: Visual Theme Selector
    QFrame* themeFrame = new QFrame(m_containerWidget);
    themeFrame->setObjectName("themeCard");
    QVBoxLayout* lTheme = new QVBoxLayout(themeFrame);
    lTheme->setSpacing(8);

    QLabel* tLabel = new QLabel("🎨 Visual Theme & Atmosphere", themeFrame);
    tLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    lTheme->addWidget(tLabel);

    QLabel* tSub = new QLabel("Choose your sanctuary's color scheme and atmosphere. Changes are applied instantly across the entire application.", themeFrame);
    tSub->setWordWrap(true);
    tSub->setStyleSheet("font-size: 12px; margin-bottom: 6px;");
    lTheme->addWidget(tSub);

    m_themeCombo = new QComboBox(themeFrame);
    m_themeCombo->addItem("📜 Warm Parchment (Cozy Light)", static_cast<int>(ThemeType::WarmParchment));
    m_themeCombo->addItem("🌙 Midnight Ink (Cozy Dark)", static_cast<int>(ThemeType::MidnightInk));
    m_themeCombo->addItem("🌲 Forest Fern (Earthy Dark)", static_cast<int>(ThemeType::ForestFern));
    m_themeCombo->addItem("🌸 Rose Autumn (Soft Tint)", static_cast<int>(ThemeType::RoseAutumn));
    m_themeCombo->addItem("☕ Vintage Sepia", static_cast<int>(ThemeType::SepiaVintage));

    connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        ThemeType selected = static_cast<ThemeType>(m_themeCombo->itemData(index).toInt());
        ThemeManager::instance().setTheme(selected);
        emit themeChanged(selected);
    });
    lTheme->addWidget(m_themeCombo);
    mainLayout->addWidget(themeFrame);

    // Section 2: Gamification
    QFrame* sec1 = new QFrame(m_containerWidget);
    sec1->setObjectName("gamifyCard");
    QVBoxLayout* l1 = new QVBoxLayout(sec1);

    QLabel* gLabel = new QLabel("🎮 Gamification & Zen Mode", sec1);
    gLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    l1->addWidget(gLabel);

    QLabel* gSub = new QLabel("Journal Quest includes an offline leveling system and a blooming garden. You can disable it for a pure, distraction-free diary experience.", sec1);
    gSub->setWordWrap(true);
    gSub->setStyleSheet("font-size: 12px; margin-bottom: 8px;");
    l1->addWidget(gSub);

    m_gamificationCheck = new QCheckBox("Enable XP, streaks, and Garden progression", sec1);
    m_gamificationCheck->setStyleSheet("QCheckBox { font-size: 13px; font-weight: bold; }");
    l1->addWidget(m_gamificationCheck);
    mainLayout->addWidget(sec1);

    // Section 3: Save & Revert Settings Actions
    QFrame* manageFrame = new QFrame(m_containerWidget);
    manageFrame->setObjectName("manageCard");
    QVBoxLayout* lManage = new QVBoxLayout(manageFrame);
    lManage->setSpacing(10);

    QLabel* mLabel = new QLabel("💾 Save & Preferences Persistence", manageFrame);
    mLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    lManage->addWidget(mLabel);

    QLabel* mSub = new QLabel("Save your current visual theme and preferences so they are automatically restored every time you reopen the app. You can also revert to previous saved preferences.", manageFrame);
    mSub->setWordWrap(true);
    mSub->setStyleSheet("font-size: 12px; margin-bottom: 4px;");
    lManage->addWidget(mSub);

    QHBoxLayout* saveBtnsLayout = new QHBoxLayout();
    m_saveBtn = new QPushButton("💾 Save Settings", manageFrame);
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    connect(m_saveBtn, &QPushButton::clicked, this, &SettingsView::onSaveSettings);

    m_revertBtn = new QPushButton("↺ Revert / Reload Saved", manageFrame);
    m_revertBtn->setCursor(Qt::PointingHandCursor);
    connect(m_revertBtn, &QPushButton::clicked, this, &SettingsView::onRevertSettings);

    saveBtnsLayout->addWidget(m_saveBtn);
    saveBtnsLayout->addWidget(m_revertBtn);
    saveBtnsLayout->addStretch();
    lManage->addLayout(saveBtnsLayout);

    m_statusLabel = new QLabel(manageFrame);
    m_statusLabel->setStyleSheet("font-size: 12px; font-weight: bold;");
    m_statusLabel->setVisible(false);
    lManage->addWidget(m_statusLabel);

    mainLayout->addWidget(manageFrame);

    // Section 4: Privacy & Data Backup
    QFrame* sec2 = new QFrame(m_containerWidget);
    sec2->setObjectName("backupCard");
    QVBoxLayout* l2 = new QVBoxLayout(sec2);

    QLabel* bLabel = new QLabel("🔒 Local Data & Privacy", sec2);
    bLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    l2->addWidget(bLabel);

    QLabel* bSub = new QLabel("All your entries, thoughts, and documents remain 100% on this local machine. No external telemetry, no remote servers. You can export a full offline JSON backup anytime or restore previous data.", sec2);
    bSub->setWordWrap(true);
    bSub->setStyleSheet("font-size: 12px; margin-bottom: 12px;");
    l2->addWidget(bSub);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_exportBtn = new QPushButton("📦 Export Diary Data (JSON Backup)", sec2);
    m_exportBtn->setCursor(Qt::PointingHandCursor);
    connect(m_exportBtn, &QPushButton::clicked, this, &SettingsView::backupExportRequested);

    m_importBtn = new QPushButton("📥 Restore Diary Data", sec2);
    m_importBtn->setCursor(Qt::PointingHandCursor);
    connect(m_importBtn, &QPushButton::clicked, this, &SettingsView::backupImportRequested);

    btnLayout->addWidget(m_exportBtn);
    btnLayout->addWidget(m_importBtn);
    btnLayout->addStretch();
    l2->addLayout(btnLayout);
    mainLayout->addWidget(sec2);

    mainLayout->addStretch();

    m_scrollArea->setWidget(m_containerWidget);
    rootLayout->addWidget(m_scrollArea);
}

void SettingsView::applyTheme() {
    const auto& t = ThemeManager::instance().currentTheme();
    setStyleSheet(QString("background: %1; color: %2;").arg(t.bgApp).arg(t.textPrimary));
    if (m_scrollArea) {
        m_scrollArea->setStyleSheet(QString("QScrollArea { background: %1; border: none; }").arg(t.bgApp));
    }
    if (m_containerWidget) {
        m_containerWidget->setStyleSheet(QString("background: %1; color: %2;").arg(t.bgApp).arg(t.textPrimary));
    }

    QString cardStyle = QString(
        "QFrame { background: %1; border: 1px solid %2; border-radius: 10px; padding: 18px; color: %3; }"
    ).arg(t.bgCard).arg(t.border).arg(t.textPrimary);

    QList<QFrame*> cards = findChildren<QFrame*>();
    for (QFrame* f : cards) {
        if (f->objectName().contains("Card")) {
            f->setStyleSheet(cardStyle);
        }
    }

    if (m_themeCombo) {
        m_themeCombo->setStyleSheet(QString(
            "QComboBox { background: %1; border: 1px solid %2; border-radius: 6px; padding: 6px 12px; font-size: 13px; color: %3; min-width: 220px; } "
            "QComboBox QAbstractItemView { background: %1; color: %3; selection-background-color: %4; border: 1px solid %2; padding: 4px; }"
        ).arg(t.dropdownBg).arg(t.border).arg(t.textPrimary).arg(t.selectionBg));
    }

    if (m_exportBtn) {
        m_exportBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: #FFFFFF; font-weight: bold; padding: 8px 16px; border-radius: 6px; border: none; font-size: 13px; } "
            "QPushButton:hover { background: %2; }"
        ).arg(t.accent).arg(t.accentHover));
    }

    if (m_saveBtn) {
        m_saveBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: #FFFFFF; font-weight: bold; padding: 8px 18px; border-radius: 6px; border: none; font-size: 13px; } "
            "QPushButton:hover { background: %2; }"
        ).arg(t.accent).arg(t.accentHover));
    }

    if (m_revertBtn) {
        m_revertBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: %2; font-weight: bold; padding: 8px 16px; border-radius: 6px; border: 1px solid %3; font-size: 13px; } "
            "QPushButton:hover { background: %4; }"
        ).arg(t.bgCard).arg(t.textPrimary).arg(t.border).arg(t.dropdownHover));
    }

    if (m_importBtn) {
        m_importBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: %2; font-weight: bold; padding: 8px 16px; border-radius: 6px; border: 1px solid %3; font-size: 13px; } "
            "QPushButton:hover { background: %4; }"
        ).arg(t.bgCard).arg(t.textPrimary).arg(t.border).arg(t.dropdownHover));
    }
}

void SettingsView::refresh() {
    QSettings settings;
    int savedTheme = settings.value("app/theme", static_cast<int>(ThemeType::WarmParchment)).toInt();
    bool savedGamify = settings.value("app/gamification", true).toBool();

    UserStats stats = DatabaseManager::instance().getUserStats();
    stats.gamificationEnabled = savedGamify;
    m_gamificationCheck->setChecked(savedGamify);

    int idx = m_themeCombo->findData(savedTheme);
    if (idx >= 0) {
        m_themeCombo->setCurrentIndex(idx);
    }
}

void SettingsView::onSaveSettings() {
    int currentThemeIdx = m_themeCombo->currentIndex();
    int themeVal = m_themeCombo->itemData(currentThemeIdx).toInt();
    bool gamifyVal = m_gamificationCheck->isChecked();

    QSettings settings;
    settings.setValue("app/theme", themeVal);
    settings.setValue("app/gamification", gamifyVal);

    UserStats stats = DatabaseManager::instance().getUserStats();
    stats.gamificationEnabled = gamifyVal;
    DatabaseManager::instance().saveUserStats(stats);
    emit gamificationToggled(gamifyVal);

    ThemeType tType = static_cast<ThemeType>(themeVal);
    ThemeManager::instance().setTheme(tType);
    emit themeChanged(tType);

    if (m_statusLabel) {
        m_statusLabel->setText("✅ Settings saved successfully! They will persist whenever you launch the app.");
        m_statusLabel->setVisible(true);
    }
}

void SettingsView::onRevertSettings() {
    QSettings settings;
    int savedTheme = settings.value("app/theme", static_cast<int>(ThemeType::WarmParchment)).toInt();
    bool savedGamify = settings.value("app/gamification", true).toBool();

    m_gamificationCheck->setChecked(savedGamify);
    UserStats stats = DatabaseManager::instance().getUserStats();
    stats.gamificationEnabled = savedGamify;
    DatabaseManager::instance().saveUserStats(stats);
    emit gamificationToggled(savedGamify);

    int idx = m_themeCombo->findData(savedTheme);
    if (idx >= 0) {
        m_themeCombo->setCurrentIndex(idx);
    }
    ThemeType tType = static_cast<ThemeType>(savedTheme);
    ThemeManager::instance().setTheme(tType);
    emit themeChanged(tType);

    if (m_statusLabel) {
        m_statusLabel->setText("↺ Reverted to last saved settings.");
        m_statusLabel->setVisible(true);
    }
}
