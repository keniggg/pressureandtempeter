#include "xlsxdocument.h"
#include <QFile>
#include <QXmlStreamWriter>
#include <QDebug>
#include <QDateTime>
#include <algorithm>
#include "xlsxutility_p.h"
#include <QFileInfo>  // 添加这行

using namespace QXlsx;

Document::Document()
{
    m_workbook = new XlsxWorkbook();
}

Document::~Document()
{
    delete m_workbook;
}

XlsxWorksheet *Document::currentWorksheet() const
{
    return m_workbook->currentWorksheet();
}

bool Document::write(int row, int col, const QVariant &value, const XlsxFormat &)
{
    if (!m_workbook->currentWorksheet())
        return false;
    return m_workbook->currentWorksheet()->write(row, col, value);
}

bool Document::saveAs(const QString &fileName)
{
    // 确保生成Excel 2003 XML格式的文件，扩展名应为.xls
    QString actualFileName = fileName;

    // 检查文件扩展名，如果不是.xls，则改为.xls
    if (!actualFileName.endsWith(".xls", Qt::CaseInsensitive) &&
        !actualFileName.endsWith(".xml", Qt::CaseInsensitive)) {
        // 移除现有的扩展名，添加.xls
        QFileInfo fileInfo(actualFileName);
        actualFileName = fileInfo.path() + "/" + fileInfo.completeBaseName() + ".xls";
    }

    QFile file(actualFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing:" << actualFileName;
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);

    writer.writeStartDocument("1.0", true);

    writer.writeStartElement("Workbook");
    writer.writeAttribute("xmlns", "urn:schemas-microsoft-com:office:spreadsheet");
    writer.writeAttribute("xmlns:o", "urn:schemas-microsoft-com:office:office");
    writer.writeAttribute("xmlns:x", "urn:schemas-microsoft-com:office:excel");
    writer.writeAttribute("xmlns:ss", "urn:schemas-microsoft-com:office:spreadsheet");
    writer.writeAttribute("xmlns:html", "http://www.w3.org/TR/REC-html40");

    writer.writeStartElement("DocumentProperties");
    writer.writeAttribute("xmlns", "urn:schemas-microsoft-com:office:office");
    writer.writeTextElement("Created", QDateTime::currentDateTime().toString("yyyy-MM-ddTHH:mm:ssZ"));
    writer.writeEndElement(); // DocumentProperties

    writer.writeStartElement("Styles");

    writer.writeStartElement("Style");
    writer.writeAttribute("ss:ID", "Default");
    writer.writeAttribute("ss:Name", "Normal");
    writer.writeEmptyElement("Alignment");
    writer.writeAttribute("ss:Vertical", "Bottom");
    writer.writeEmptyElement("Borders");
    writer.writeStartElement("Font");
    writer.writeAttribute("ss:FontName", "宋体");
    writer.writeAttribute("x:CharSet", "134");
    writer.writeAttribute("ss:Size", "11");
    writer.writeAttribute("ss:Color", "#000000");
    writer.writeEndElement(); // Font
    writer.writeEmptyElement("Interior");
    writer.writeEmptyElement("NumberFormat");
    writer.writeEmptyElement("Protection");
    writer.writeEndElement(); // Style

    writer.writeStartElement("Style");
    writer.writeAttribute("ss:ID", "Header");
    writer.writeStartElement("Font");
    writer.writeAttribute("ss:FontName", "宋体");
    writer.writeAttribute("x:CharSet", "134");
    writer.writeAttribute("ss:Size", "11");
    writer.writeAttribute("ss:Color", "#000000");
    writer.writeAttribute("ss:Bold", "1");
    writer.writeEndElement(); // Font
    writer.writeEndElement(); // Style

    writer.writeEndElement(); // Styles

    // 开始工作表
    QString sheetName = currentWorksheet() ? currentWorksheet()->name() : "Sheet1";
    // 清理工作表名称中的非法字符
    sheetName = sheetName.replace(":", "_").replace("\\", "_").replace("/", "_")
                    .replace("?", "_").replace("*", "_").replace("[", "_")
                    .replace("]", "_");
    if (sheetName.length() > 31) sheetName = sheetName.left(31);

    writer.writeStartElement("Worksheet");
    writer.writeAttribute("ss:Name", sheetName);

    writer.writeStartElement("Table");

    // 获取单元格数据
    QMap<QPair<int, int>, XlsxCell*> cells;
    if (currentWorksheet()) {
        cells = currentWorksheet()->cells();
    }

    if (!cells.isEmpty()) {
        // 找到最大行和列
        int maxRow = 0;
        int maxCol = 0;

        for (auto it = cells.constBegin(); it != cells.constEnd(); ++it) {
            int row = it.key().first;
            int col = it.key().second;
            if (row > maxRow) maxRow = row;
            if (col > maxCol) maxCol = col;
        }

        // 写入行数据
        for (int rowNum = 1; rowNum <= maxRow; rowNum++) {
            writer.writeStartElement("Row");

            for (int colNum = 1; colNum <= maxCol; colNum++) {
                QPair<int, int> key(rowNum, colNum);
                if (cells.contains(key)) {
                    XlsxCell *cell = cells.value(key);
                    if (cell) {
                        QVariant value = cell->value();
                        writer.writeStartElement("Cell");

                        // 如果是第一行，设置为表头样式
                        if (rowNum == 1) {
                            writer.writeAttribute("ss:StyleID", "Header");
                        }

                        writer.writeStartElement("Data");

                        // 判断数据类型
                        QMetaType::Type type = static_cast<QMetaType::Type>(value.typeId());

                        if (type == QMetaType::Int || type == QMetaType::UInt ||
                            type == QMetaType::LongLong || type == QMetaType::ULongLong ||
                            type == QMetaType::Double || type == QMetaType::Float) {
                            writer.writeAttribute("ss:Type", "Number");
                            // 对于数字，直接写入
                            if (type == QMetaType::Double || type == QMetaType::Float) {
                                writer.writeCharacters(QString::number(value.toDouble(), 'f', 6));
                            } else {
                                writer.writeCharacters(value.toString());
                            }
                        } else {
                            writer.writeAttribute("ss:Type", "String");
                            // 对于字符串，需要转义XML特殊字符
                            QString text = value.toString();
                            text.replace("&", "&amp;")
                                .replace("<", "&lt;")
                                .replace(">", "&gt;")
                                .replace("\"", "&quot;")
                                .replace("'", "&apos;");
                            writer.writeCharacters(text);
                        }

                        writer.writeEndElement(); // Data
                        writer.writeEndElement(); // Cell
                    } else {
                        // 空单元格
                        writer.writeEmptyElement("Cell");
                    }
                } else {
                    // 空单元格
                    if (colNum == 1) {
                        // 至少写一个空单元格，确保行存在
                        writer.writeEmptyElement("Cell");
                    }
                }
            }

            writer.writeEndElement(); // Row
        }
    }

    writer.writeEndElement(); // Table

    writer.writeStartElement("WorksheetOptions");
    writer.writeAttribute("xmlns", "urn:schemas-microsoft-com:office:excel");

    writer.writeStartElement("PageSetup");
    writer.writeEmptyElement("Header");
    writer.writeAttribute("x:Margin", "0.3");
    writer.writeEmptyElement("Footer");
    writer.writeAttribute("x:Margin", "0.3");
    writer.writeEmptyElement("PageMargins");
    writer.writeAttribute("x:Bottom", "0.75");
    writer.writeAttribute("x:Left", "0.7");
    writer.writeAttribute("x:Right", "0.7");
    writer.writeAttribute("x:Top", "0.75");
    writer.writeEndElement(); // PageSetup

    writer.writeStartElement("Print");
    writer.writeEmptyElement("ValidPrinterInfo");
    writer.writeTextElement("PaperSizeIndex", "9");
    writer.writeTextElement("HorizontalResolution", "600");
    writer.writeTextElement("VerticalResolution", "600");
    writer.writeEndElement(); // Print

    writer.writeEmptyElement("Selected");
    writer.writeTextElement("ProtectObjects", "False");
    writer.writeTextElement("ProtectScenarios", "False");
    writer.writeEndElement(); // WorksheetOptions

    writer.writeEndElement(); // Worksheet
    writer.writeEndElement(); // Workbook

    writer.writeEndDocument();

    file.close();

    qDebug() << "File saved as:" << actualFileName;

    return true;
}
