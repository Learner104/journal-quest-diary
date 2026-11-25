#include "DocxHandler.h"
#include <QProcess>
#include <QTemporaryDir>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QRegularExpression>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextCursor>
#include <QDebug>

bool DocxHandler::isDocxFile(const QString& filePath) {
    return filePath.endsWith(".docx", Qt::CaseInsensitive);
}

bool DocxHandler::importDocx(const QString& docxFilePath, QString& outHtml, QString& outPlainText, QString& outTitle, QString& outError) {
    QFileInfo fi(docxFilePath);
    if (!fi.exists()) {
        outError = "File does not exist: " + docxFilePath;
        return false;
    }

    outTitle = fi.baseName();

    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        outError = "Could not create temporary extraction directory.";
        return false;
    }

    // Extract word/document.xml using standard unzip
    QProcess unzipProc;
    QStringList args;
    args << "-q" << "-o" << docxFilePath << "word/document.xml" << "-d" << tempDir.path();
    unzipProc.start("unzip", args);
    if (!unzipProc.waitForFinished(10000) || unzipProc.exitCode() != 0) {
        outError = "Failed to extract .docx archive: " + unzipProc.readAllStandardError();
        return false;
    }

    QString docXmlPath = tempDir.path() + "/word/document.xml";
    QFile docFile(docXmlPath);
    if (!docFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        outError = "Could not open word/document.xml inside .docx.";
        return false;
    }

    QXmlStreamReader xml(&docFile);
    QString htmlBuilder;
    QString plainTextBuilder;

    bool inParagraph = false;
    bool inRun = false;
    bool inBold = false;
    bool inItalic = false;
    bool inUnderline = false;
    bool inStrike = false;
    QString headingLevel = ""; // e.g. "h1", "h2"
    bool inTable = false;
    bool inRow = false;
    bool inCell = false;

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString name = xml.name().toString();

            if (name == "tbl") {
                inTable = true;
                htmlBuilder += "<table border=\"1\" style=\"border-collapse:collapse; margin: 10px 0; width: 100%;\">\n";
            } else if (name == "tr") {
                inRow = true;
                htmlBuilder += "<tr>\n";
            } else if (name == "tc") {
                inCell = true;
                htmlBuilder += "<td style=\"padding: 6px; border: 1px solid #c4b5a5;\">";
            } else if (name == "p") {
                inParagraph = true;
                headingLevel = "";
            } else if (name == "pStyle") {
                QString val = xml.attributes().value("w:val").toString().toLower();
                if (val.contains("heading 1") || val == "heading1" || val == "title") {
                    headingLevel = "h1";
                } else if (val.contains("heading 2") || val == "heading2") {
                    headingLevel = "h2";
                } else if (val.contains("heading 3") || val == "heading3") {
                    headingLevel = "h3";
                }
            } else if (name == "r") {
                inRun = true;
                inBold = false;
                inItalic = false;
                inUnderline = false;
                inStrike = false;
            } else if (name == "b") {
                inBold = true;
            } else if (name == "i") {
                inItalic = true;
            } else if (name == "u") {
                inUnderline = true;
            } else if (name == "strike") {
                inStrike = true;
            } else if (name == "t") {
                QString text = xml.readElementText();
                if (!text.isEmpty()) {
                    QString escaped = text.toHtmlEscaped();
                    if (inBold) escaped = "<b>" + escaped + "</b>";
                    if (inItalic) escaped = "<i>" + escaped + "</i>";
                    if (inUnderline) escaped = "<u>" + escaped + "</u>";
                    if (inStrike) escaped = "<s>" + escaped + "</s>";

                    htmlBuilder += escaped;
                    plainTextBuilder += text;
                }
            }
        } else if (token == QXmlStreamReader::EndElement) {
            QString name = xml.name().toString();

            if (name == "p") {
                inParagraph = false;
                if (!headingLevel.isEmpty()) {
                    // Wrap the block in heading
                    htmlBuilder = "<" + headingLevel + ">" + htmlBuilder + "</" + headingLevel + ">\n";
                } else if (!inCell) {
                    htmlBuilder += "<br/>\n";
                }
                plainTextBuilder += "\n";
            } else if (name == "tc") {
                inCell = false;
                htmlBuilder += "</td>\n";
            } else if (name == "tr") {
                inRow = false;
                htmlBuilder += "</tr>\n";
            } else if (name == "tbl") {
                inTable = false;
                htmlBuilder += "</table>\n";
            }
        }
    }

    docFile.close();

    if (xml.hasError()) {
        outError = "XML parsing error: " + xml.errorString();
        return false;
    }

    outHtml = htmlBuilder;
    outPlainText = plainTextBuilder.trimmed();
    return true;
}

bool DocxHandler::exportDocx(const QString& docxFilePath, const QString& title, const QString& htmlContent, QString& outError) {
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        outError = "Could not create temporary packaging directory.";
        return false;
    }

    QString baseDir = tempDir.path();
    QDir().mkpath(baseDir + "/_rels");
    QDir().mkpath(baseDir + "/word/_rels");

    // 1. [Content_Types].xml
    {
        QFile file(baseDir + "/[Content_Types].xml");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            outError = "Failed to write [Content_Types].xml";
            return false;
        }
        file.write("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
                   "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">\r\n"
                   "  <Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>\r\n"
                   "  <Default Extension=\"xml\" ContentType=\"application/xml\"/>\r\n"
                   "  <Override PartName=\"/word/document.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>\r\n"
                   "</Types>\r\n");
        file.close();
    }

    // 2. _rels/.rels
    {
        QFile file(baseDir + "/_rels/.rels");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            outError = "Failed to write _rels/.rels";
            return false;
        }
        file.write("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
                   "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">\r\n"
                   "  <Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"word/document.xml\"/>\r\n"
                   "</Relationships>\r\n");
        file.close();
    }

    // 3. word/document.xml
    {
        QFile file(baseDir + "/word/document.xml");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            outError = "Failed to write word/document.xml";
            return false;
        }

        QTextDocument doc;
        doc.setHtml(htmlContent);

        QString xmlContent;
        xmlContent += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n";
        xmlContent += "<w:document xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">\r\n";
        xmlContent += "  <w:body>\r\n";

        // Title Paragraph
        if (!title.isEmpty()) {
            xmlContent += "    <w:p>\r\n";
            xmlContent += "      <w:pPr>\r\n";
            xmlContent += "        <w:pStyle w:val=\"Heading1\"/>\r\n";
            xmlContent += "      </w:pPr>\r\n";
            xmlContent += "      <w:r>\r\n";
            xmlContent += "        <w:rPr><w:b/><w:sz w:val=\"36\"/></w:rPr>\r\n";
            xmlContent += "        <w:t>" + title.toHtmlEscaped() + "</w:t>\r\n";
            xmlContent += "      </w:r>\r\n";
            xmlContent += "    </w:p>\r\n";
        }

        // Iterate paragraphs from document
        QTextBlock block = doc.begin();
        while (block.isValid()) {
            QString text = block.text();
            xmlContent += "    <w:p>\r\n";
            xmlContent += "      <w:r>\r\n";
            xmlContent += "        <w:t xml:space=\"preserve\">" + text.toHtmlEscaped() + "</w:t>\r\n";
            xmlContent += "      </w:r>\r\n";
            xmlContent += "    </w:p>\r\n";
            block = block.next();
        }

        xmlContent += "    <w:sectPr>\r\n";
        xmlContent += "      <w:pgSz w:w=\"11906\" w:h=\"16838\"/>\r\n"; // Standard A4
        xmlContent += "      <w:pgMar w:top=\"1440\" w:right=\"1440\" w:bottom=\"1440\" w:left=\"1440\"/>\r\n";
        xmlContent += "    </w:sectPr>\r\n";
        xmlContent += "  </w:body>\r\n";
        xmlContent += "</w:document>\r\n";

        file.write(xmlContent.toUtf8());
        file.close();
    }

    // Zip into .docx
    QFileInfo targetFi(docxFilePath);
    QString absoluteDocxPath = targetFi.absoluteFilePath();
    if (QFile::exists(absoluteDocxPath)) {
        QFile::remove(absoluteDocxPath);
    }

    QProcess zipProc;
    zipProc.setWorkingDirectory(baseDir);
    QStringList args;
    args << "-r" << "-q" << absoluteDocxPath << "[Content_Types].xml" << "_rels" << "word";
    zipProc.start("zip", args);
    if (!zipProc.waitForFinished(10000) || zipProc.exitCode() != 0) {
        outError = "Failed to bundle files into .docx: " + zipProc.readAllStandardError();
        return false;
    }

    return true;
}
