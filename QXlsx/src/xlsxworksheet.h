#ifndef XLSXWORKSHEET_H
#define XLSXWORKSHEET_H

#include <QString>
#include <QMap>
#include "xlsxcell.h"
#include "xlsxcellrange.h"

class XlsxWorksheet
{
public:
    XlsxWorksheet(const QString &name = "Sheet1") : m_name(name) {}
    ~XlsxWorksheet() { qDeleteAll(m_cells); }

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    // 写入单元格（忽略格式）
    bool write(int row, int col, const QVariant &value, const XlsxFormat & = XlsxFormat());
    XlsxCell *cellAt(int row, int col) const;
    // 获取所有单元格（核心接口）
    QMap<QPair<int, int>, XlsxCell*> cells() const { return m_cells; }

private:
    QString m_name;
    QMap<QPair<int, int>, XlsxCell*> m_cells; // row, col -> cell
};

#endif // XLSXWORKSHEET_H
