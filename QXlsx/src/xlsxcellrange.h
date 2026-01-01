#ifndef XLSXCELLRANGE_H
#define XLSXCELLRANGE_H

#include <QString>

class XlsxCellRange
{
public:
    XlsxCellRange() : top(-1), bottom(-1), left(-1), right(-1) {}
    XlsxCellRange(int t, int l, int b, int r) : top(t), bottom(b), left(l), right(r) {}
    explicit XlsxCellRange(const QString &range);

    bool isValid() const { return top != -1 && bottom != -1 && left != -1 && right != -1; }
    QString toString() const;

    int top, bottom, left, right;
};

#endif // XLSXCELLRANGE_H
