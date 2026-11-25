#include <cassert>
#include <iostream>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include "DatabaseManager.h"
#include "DocxHandler.h"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    std::cout << "[TEST] Initializing SQLite Database..." << std::endl;
    QString testDbPath = "test_journal.sqlite";
    if (QFile::exists(testDbPath)) QFile::remove(testDbPath);

    bool dbOk = DatabaseManager::instance().init(testDbPath);
    assert(dbOk && "DatabaseManager::init failed");
    std::cout << "[PASS] Database initialized successfully." << std::endl;

    // Test Create Entry
    DiaryEntry entry;
    entry.title = "A Cozy Linux Evening";
    entry.contentPlainText = "Writing natively in C++ on Linux with zero latency. It feels wonderful.";
    entry.contentHtml = "<p>Writing natively in <b>C++</b> on Linux with zero latency.</p>";
    entry.category = "Reflections";
    entry.tags = {"linux", "native", "cpp"};
    entry.mood = "Peaceful";
    entry.weather = "Sunny";
    entry.wordCount = 12;

    qint64 id = DatabaseManager::instance().createEntry(entry);
    assert(id > 0 && "createEntry failed");
    std::cout << "[PASS] Created entry with ID: " << id << std::endl;

    // Test Read Entry
    DiaryEntry fetched;
    bool getOk = DatabaseManager::instance().getEntry(id, fetched);
    assert(getOk && "getEntry failed");
    assert(fetched.title == "A Cozy Linux Evening");
    assert(fetched.wordCount == 12);
    std::cout << "[PASS] Verified fetched entry: " << fetched.title.toStdString() << std::endl;

    // Test Gamification Stats
    UserStats stats = DatabaseManager::instance().getUserStats();
    assert(stats.totalEntriesWritten >= 1);
    assert(stats.totalWordsWritten >= 12);
    assert(stats.gardenLevel >= 1);
    std::cout << "[PASS] Gamification verified! XP: " << stats.totalXp << " | Garden Level: " << stats.gardenLevel << std::endl;

    // Test DOCX Export
    QString docxOut = "test_output.docx";
    QString docxErr;
    bool exportOk = DocxHandler::exportDocx(docxOut, entry.title, entry.contentHtml, docxErr);
    if (!exportOk) {
        std::cerr << "DOCX Export failed: " << docxErr.toStdString() << std::endl;
    }
    assert(exportOk && "DocxHandler::exportDocx failed");
    std::cout << "[PASS] Exported valid .docx to " << docxOut.toStdString() << std::endl;

    // Test DOCX Import
    QString inHtml, inPlain, inTitle, inErr;
    bool importOk = DocxHandler::importDocx(docxOut, inHtml, inPlain, inTitle, inErr);
    if (!importOk) {
        std::cerr << "DOCX Import failed: " << inErr.toStdString() << std::endl;
    }
    assert(importOk && "DocxHandler::importDocx failed");
    assert(!inPlain.isEmpty());
    std::cout << "[PASS] Imported .docx back! Extracted text length: " << inPlain.length() << std::endl;

    // Clean up
    QFile::remove(testDbPath);
    QFile::remove(docxOut);

    std::cout << "\n>>> ALL UNIT & INTEGRATION TESTS PASSED (100% NATIVE C++ & SQLITE & DOCX) <<<" << std::endl;
    return 0;
}
