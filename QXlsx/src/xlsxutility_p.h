#ifndef XLSXUTILITY_P_H
#define XLSXUTILITY_P_H

#include <QString>

// 列号转Excel列名（如1→A，2→B，27→AA）
inline QString columnToName(int col)
{
    QString name;
    while (col > 0) {
        int remainder = (col - 1) % 26;
        name.prepend(QChar('A' + remainder));
        col = (col - 1) / 26;
    }
    return name;
}

#endif // XLSXUTILITY_H
