#include <QApplication>
#include <QIcon>
#include <QFontDatabase>
#include <QSettings>
#include "MainWindow.h"
#include "DatabaseManager.h"
#include "ThemeManager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("Journal Quest");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("IndieDiary");

    // Initialize local SQLite database
    DatabaseManager::instance().init();

    // Load saved settings if present
    QSettings settings;
    int savedTheme = settings.value("app/theme", static_cast<int>(ThemeType::WarmParchment)).toInt();
    bool savedGamify = settings.value("app/gamification", true).toBool();

    UserStats stats = DatabaseManager::instance().getUserStats();
    stats.gamificationEnabled = savedGamify;
    DatabaseManager::instance().saveUserStats(stats);

    ThemeManager::instance().setTheme(static_cast<ThemeType>(savedTheme));

    MainWindow window;
    window.show();

    return app.exec();
}
