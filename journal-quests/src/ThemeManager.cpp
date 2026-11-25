#include "ThemeManager.h"
#include <QApplication>

ThemeManager::ThemeManager() {
    loadTheme(ThemeType::WarmParchment);
}

ThemeManager& ThemeManager::instance() {
    static ThemeManager s_instance;
    return s_instance;
}

void ThemeManager::loadTheme(ThemeType type) {
    m_currentType = type;

    switch (type) {
        case ThemeType::MidnightInk: // Cozy Indie Dark
            m_theme = {
                "Midnight Ink (Dark)",
                type,
                true,
                "#1C1B1A", // bgApp
                "#141312", // bgSidebar
                "#242220", // bgCard
                "#1C1B1A", // bgEditor
                "#33302C", // border
                "#A3704C", // borderHover
                "#E8E3DD", // textPrimary
                "#9C958D", // textSecondary
                "#A3704C", // accent
                "#8A5A38", // accentHover
                "#47382B", // selectionBg
                "#242220", // dropdownBg
                "#33302B"  // dropdownHover
            };
            break;

        case ThemeType::ForestFern: // Cozy Dark Green
            m_theme = {
                "Forest Fern (Earthy Dark)",
                type,
                true,
                "#161E18", // bgApp
                "#101712", // bgSidebar
                "#1D2720", // bgCard
                "#161E18", // bgEditor
                "#2C3C30", // border
                "#5B8A68", // borderHover
                "#E1EDE4", // textPrimary
                "#8DA092", // textSecondary
                "#4C7858", // accent
                "#3F644A", // accentHover
                "#2D4534", // selectionBg
                "#1D2720", // dropdownBg
                "#28372D"  // dropdownHover
            };
            break;

        case ThemeType::RoseAutumn: // Warm Rose Tinted
            m_theme = {
                "Rose Autumn (Soft Tint)",
                type,
                false,
                "#FAF5F4", // bgApp
                "#EDE3E1", // bgSidebar
                "#F3E8E6", // bgCard
                "#FAF5F4", // bgEditor
                "#DECAC7", // border
                "#B26B63", // borderHover
                "#3D2B29", // textPrimary
                "#8A7270", // textSecondary
                "#B26B63", // accent
                "#99564F", // accentHover
                "#E2BDB8", // selectionBg
                "#FFFFFF", // dropdownBg
                "#F5ECEB"  // dropdownHover
            };
            break;

        case ThemeType::SepiaVintage: // Antique Warm Sepia
            m_theme = {
                "Vintage Sepia",
                type,
                false,
                "#F4EBD9", // bgApp
                "#E8DBC3", // bgSidebar
                "#EDE1C9", // bgCard
                "#F4EBD9", // bgEditor
                "#D4C2A3", // border
                "#966F42", // borderHover
                "#3B2E1D", // textPrimary
                "#7E6D54", // textSecondary
                "#966F42", // accent
                "#7E5B32", // accentHover
                "#DECDB0", // selectionBg
                "#FAF4E8", // dropdownBg
                "#EBE0CB"  // dropdownHover
            };
            break;

        case ThemeType::WarmParchment: // Default Cozy Light
        default:
            m_theme = {
                "Warm Parchment (Light)",
                type,
                false,
                "#FAF7F2", // bgApp
                "#EDE4D5", // bgSidebar
                "#F3ECE1", // bgCard
                "#FAF7F2", // bgEditor
                "#E2D7C8", // border
                "#8E5A3C", // borderHover
                "#2E251E", // textPrimary
                "#8A7B6D", // textSecondary
                "#8E5A3C", // accent
                "#7A4B30", // accentHover
                "#D4B996", // selectionBg
                "#FFFFFF", // dropdownBg
                "#F5EFE6"  // dropdownHover
            };
            break;
    }
}

void ThemeManager::setTheme(ThemeType type) {
    loadTheme(type);
    if (qApp) {
        qApp->setStyleSheet(generateGlobalStyleSheet());
    }
}

QString ThemeManager::generateGlobalStyleSheet() const {
    const auto& t = m_theme;

    return QString(R"(
        QWidget {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
            color: %1;
            background-color: transparent;
        }
        *:focus {
            outline: none;
        }
        QMainWindow, QStackedWidget {
            background-color: %2;
        }
        QScrollBar:vertical {
            border: none;
            background: %3;
            width: 10px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: %4;
            min-height: 20px;
            border-radius: 5px;
        }
        QScrollBar::handle:vertical:hover {
            background: %5;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QToolTip {
            background-color: %6;
            color: %7;
            border: 1px solid %8;
            border-radius: 4px;
            padding: 4px;
        }
    )")
    .arg(t.textPrimary)
    .arg(t.bgApp)
    .arg(t.bgSidebar)
    .arg(t.border)
    .arg(t.borderHover)
    .arg(t.isDark ? "#2A2724" : "#3E3228")
    .arg(t.isDark ? "#E8E3DD" : "#FAF7F2")
    .arg(t.border);
}
