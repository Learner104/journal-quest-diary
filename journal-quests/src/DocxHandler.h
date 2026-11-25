#pragma once

#include <QString>

class DocxHandler {
public:
    // Imports a .docx file and converts its paragraphs, headings, bold/italic, lists, tables into clean HTML
    static bool importDocx(const QString& docxFilePath, QString& outHtml, QString& outPlainText, QString& outTitle, QString& outError);

    // Exports rich HTML (from QTextDocument or editor) back to a valid Microsoft Word .docx file
    static bool exportDocx(const QString& docxFilePath, const QString& title, const QString& htmlContent, QString& outError);

    // Safe round-trip check / helper
    static bool isDocxFile(const QString& filePath);
};
