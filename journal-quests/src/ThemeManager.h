#pragma once

#include <QString>
#include <QColor>

enum class ThemeType {
    WarmParchment,  // Default Cozy Light
    MidnightInk,    // Cozy Dark
    ForestFern,     // Earthy Green Dark/Muted
    RoseAutumn,     // Warm Rose Tinted
    SepiaVintage    // Classic Antique Sepia
};

struct ThemeColors {
    QString name;
    ThemeType type;
    bool isDark;

    // Palette hex codes
    QString bgApp;          // Main background
    QString bgSidebar;      // Left navigation sidebar
    QString bgCard;         // Cards & panels
    QString bgEditor;       // Workspace editor surface
    QString border;         // Subtle dividing lines
    QString borderHover;    // Hover & accent borders
    QString textPrimary;    // Primary headings & text
    QString textSecondary;  // Timestamps, hints, subtitles
    QString accent;         // Buttons, highlights
    QString accentHover;    // Button hover
    QString selectionBg;    // Text selection background
    QString dropdownBg;     // Dropdown popups
    QString dropdownHover;  // Dropdown items hover
};

class ThemeManager {
public:
    static ThemeManager& instance();

    void setTheme(ThemeType type);
    ThemeType currentThemeType() const { return m_currentType; }
    const ThemeColors& currentTheme() const { return m_theme; }
    QString generateGlobalStyleSheet() const;

private:
    ThemeManager();
    ThemeType m_currentType = ThemeType::WarmParchment;
    ThemeColors m_theme;
    void loadTheme(ThemeType type);
};
