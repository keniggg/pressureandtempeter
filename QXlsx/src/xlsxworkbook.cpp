#include "xlsxworkbook.h"

XlsxWorkbook::XlsxWorkbook() : m_currentSheet(nullptr)
{
    // 默认添加第一个工作表
    m_currentSheet = addWorksheet("Sheet1");
}

XlsxWorkbook::~XlsxWorkbook()
{
    qDeleteAll(m_worksheets);
}

XlsxWorksheet *XlsxWorkbook::addWorksheet(const QString &name)
{
    QString sheetName = name.isEmpty() ? QString("Sheet%1").arg(m_worksheets.size() + 1) : name;
    XlsxWorksheet *sheet = new XlsxWorksheet(sheetName);
    m_worksheets.append(sheet);
    return sheet;
}
