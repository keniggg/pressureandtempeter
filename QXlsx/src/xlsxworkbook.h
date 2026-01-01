#ifndef XLSXWORKBOOK_H
#define XLSXWORKBOOK_H

#include <QList>
#include <QString>
#include "xlsxworksheet.h"

class XlsxWorkbook
{
public:
    XlsxWorkbook();
    ~XlsxWorkbook();

    XlsxWorksheet *addWorksheet(const QString &name = "");
    XlsxWorksheet *currentWorksheet() const { return m_currentSheet; }
    void setCurrentWorksheet(XlsxWorksheet *sheet) { m_currentSheet = sheet; }

private:
    QList<XlsxWorksheet*> m_worksheets;
    XlsxWorksheet *m_currentSheet;
};

#endif // XLSXWORKBOOK_H
