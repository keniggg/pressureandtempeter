#ifndef XLSXCELL_H
#define XLSXCELL_H

#include <QVariant>
#include "xlsxformat.h"

class XlsxCell
{
public:
    XlsxCell() = default;
    // 保留格式参数但不使用（兼容接口）
    XlsxCell(const QVariant &data, const XlsxFormat & = XlsxFormat()) : m_data(data) {}

    QVariant value() const { return m_data; }
    void setValue(const QVariant &value) { m_data = value; }

    // 空实现：返回默认格式
    XlsxFormat format() const { return XlsxFormat(); }
    void setFormat(const XlsxFormat &) {}

private:
    QVariant m_data; // 仅保留单元格值
};

#endif // XLSXCELL_H
