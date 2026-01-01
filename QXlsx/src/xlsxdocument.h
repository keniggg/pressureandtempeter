#ifndef XLSXDOCUMENT_H
#define XLSXDOCUMENT_H

#include <QString>
#include "xlsxworkbook.h"
#include "xlsxformat.h"

namespace QXlsx {

class Document
{
public:
    Document();
    ~Document();

    XlsxWorksheet *currentWorksheet() const;
    // 写入单元格（忽略格式）
    bool write(int row, int col, const QVariant &value, const XlsxFormat & = XlsxFormat());
    // 保存Excel文件
    bool saveAs(const QString &fileName);

private:
    XlsxWorkbook *m_workbook;
};

} // namespace QXlsx

#endif // XLSXDOCUMENT_H
