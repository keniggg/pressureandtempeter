QT       += core gui network widgets charts  # 添加charts模块

# ========== 修复1：TARGET改为英文，避免乱码 ==========
TARGET = PressureScanValve
TEMPLATE = app

CONFIG += c++17
# ========== 修复2：设置编码为UTF-8，兼容中文文本 ==========
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += utf8_source

# QXlsx核心路径（确保路径无中文）
INCLUDEPATH += $$PWD/QXlsx/src
DEPENDPATH += $$PWD/QXlsx/src

# ========== 修复3：确保所有源文件都添加（无遗漏） ==========
SOURCES += src/main.cpp\
           src/mainwindow.cpp \
           src/tcpclient.cpp \
           # QXlsx核心文件
           QXlsx/src/xlsxdocument.cpp \
           QXlsx/src/xlsxcell.cpp \
           QXlsx/src/xlsxworkbook.cpp \
           QXlsx/src/xlsxworksheet.cpp \
           QXlsx/src/xlsxutility.cpp

HEADERS  += src/mainwindow.h \
            src/tcpclient.h \
            # QXlsx核心头文件
            QXlsx/src/xlsxdocument.h \
            QXlsx/src/xlsxcell.h \
            QXlsx/src/xlsxcellrange.h \
            QXlsx/src/xlsxworkbook.h \
            QXlsx/src/xlsxworksheet.h \
            QXlsx/src/xlsxutility_p.h \
            QXlsx/src/xlsxformat.h

# UI文件（确保路径无中文）
FORMS    += src/mainwindow.ui

# 禁用QtQuick编译器，避免冗余错误
CONFIG -= qtquickcompiler
