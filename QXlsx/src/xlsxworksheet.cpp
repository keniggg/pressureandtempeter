#include "xlsxworksheet.h"

bool XlsxWorksheet::write(int row, int col, const QVariant &value, const XlsxFormat &)
{
    QPair<int, int> key(row, col);
    // 释放原有单元格
    if (m_cells.contains(key))
        delete m_cells[key];
    // 写入新值
    m_cells[key] = new XlsxCell(value);
    return true;
}

XlsxCell *XlsxWorksheet::cellAt(int row, int col) const
{
    return m_cells.value(QPair<int, int>(row, col), nullptr);
}
