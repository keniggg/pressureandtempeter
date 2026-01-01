/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCharts/qlineseries.h>
#include <QtCharts/qabstractbarseries.h>
#include <QtCharts/qvbarmodelmapper.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCharts/qcandlestickseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qxyseries.h>
#include <QtCharts/qxyseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qxyseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN10MainWindowE = QtMocHelpers::stringData(
    "MainWindow",
    "on_pushButton_clicked",
    "",
    "readPacket",
    "pkt",
    "slotConnected",
    "slotDisonnected",
    "on_pushButton_update_clicked",
    "handleRcvTCPPktPSI",
    "pktArray",
    "on_pushButton_config_clicked",
    "on_pushButton_scan_clicked",
    "on_pushButton_stop_clicked",
    "on_btnApplyNetwork_clicked",
    "on_btnApplyCollect_clicked",
    "on_btnApplyUnit_clicked",
    "on_btnZeroCalib_clicked",
    "on_btnSpanCalib_clicked",
    "on_btnMultiCalibStart_clicked",
    "on_btnMultiCalibCollect_clicked",
    "on_btnMultiCalibCalc_clicked",
    "on_btnMultiCalibAbort_clicked",
    "on_btnClearData_clicked",
    "on_btnSaveExcel_clicked",
    "on_btnDTSConnect_clicked",
    "on_btnDTSApplyNetwork_clicked",
    "on_btnDTSQuickConfig_clicked",
    "on_btnDTSScan_clicked",
    "on_btnDTSStop_clicked",
    "on_btnDTSClear_clicked",
    "on_btnDTSSave_clicked",
    "readPacketDTS",
    "slotDTSConnected",
    "slotDTSDisconnected",
    "readColdJunctionTemperature",
    "on_btnQuickSetIP_clicked",
    "on_btnReconnectNewIP_clicked",
    "on_btnResetDefaultIP_clicked",
    "onAutoSaveTimer",
    "onChartUpdateTimer",
    "on_btnSimultaneousStart_clicked",
    "on_btnSimultaneousStop_clicked",
    "updateSimultaneousButtonsState",
    "onNewTCPConnection",
    "onTCPClientDisconnected",
    "onTCPSendTimer",
    "on_btnDTSSingleConfig_clicked",
    "on_btnTestChannel_clicked",
    "updateTestChannelTemperature",
    "onPressureRandomTimerTimeout",
    "onMultiSectionSendTimer"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN10MainWindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      47,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  296,    2, 0x08,    1 /* Private */,
       3,    1,  297,    2, 0x08,    2 /* Private */,
       5,    0,  300,    2, 0x08,    4 /* Private */,
       6,    0,  301,    2, 0x08,    5 /* Private */,
       7,    0,  302,    2, 0x08,    6 /* Private */,
       8,    1,  303,    2, 0x08,    7 /* Private */,
      10,    0,  306,    2, 0x08,    9 /* Private */,
      11,    0,  307,    2, 0x08,   10 /* Private */,
      12,    0,  308,    2, 0x08,   11 /* Private */,
      13,    0,  309,    2, 0x08,   12 /* Private */,
      14,    0,  310,    2, 0x08,   13 /* Private */,
      15,    0,  311,    2, 0x08,   14 /* Private */,
      16,    0,  312,    2, 0x08,   15 /* Private */,
      17,    0,  313,    2, 0x08,   16 /* Private */,
      18,    0,  314,    2, 0x08,   17 /* Private */,
      19,    0,  315,    2, 0x08,   18 /* Private */,
      20,    0,  316,    2, 0x08,   19 /* Private */,
      21,    0,  317,    2, 0x08,   20 /* Private */,
      22,    0,  318,    2, 0x08,   21 /* Private */,
      23,    0,  319,    2, 0x08,   22 /* Private */,
      24,    0,  320,    2, 0x08,   23 /* Private */,
      25,    0,  321,    2, 0x08,   24 /* Private */,
      26,    0,  322,    2, 0x08,   25 /* Private */,
      27,    0,  323,    2, 0x08,   26 /* Private */,
      28,    0,  324,    2, 0x08,   27 /* Private */,
      29,    0,  325,    2, 0x08,   28 /* Private */,
      30,    0,  326,    2, 0x08,   29 /* Private */,
      31,    1,  327,    2, 0x08,   30 /* Private */,
      32,    0,  330,    2, 0x08,   32 /* Private */,
      33,    0,  331,    2, 0x08,   33 /* Private */,
      34,    0,  332,    2, 0x08,   34 /* Private */,
      35,    0,  333,    2, 0x08,   35 /* Private */,
      36,    0,  334,    2, 0x08,   36 /* Private */,
      37,    0,  335,    2, 0x08,   37 /* Private */,
      38,    0,  336,    2, 0x08,   38 /* Private */,
      39,    0,  337,    2, 0x08,   39 /* Private */,
      40,    0,  338,    2, 0x08,   40 /* Private */,
      41,    0,  339,    2, 0x08,   41 /* Private */,
      42,    0,  340,    2, 0x08,   42 /* Private */,
      43,    0,  341,    2, 0x08,   43 /* Private */,
      44,    0,  342,    2, 0x08,   44 /* Private */,
      45,    0,  343,    2, 0x08,   45 /* Private */,
      46,    0,  344,    2, 0x08,   46 /* Private */,
      47,    0,  345,    2, 0x08,   47 /* Private */,
      48,    0,  346,    2, 0x08,   48 /* Private */,
      49,    0,  347,    2, 0x08,   49 /* Private */,
      50,    0,  348,    2, 0x08,   50 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_ZN10MainWindowE.offsetsAndSizes,
    qt_meta_data_ZN10MainWindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN10MainWindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'on_pushButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readPacket'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>,
        // method 'slotConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotDisonnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButton_update_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleRcvTCPPktPSI'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>,
        // method 'on_pushButton_config_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButton_scan_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButton_stop_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnApplyNetwork_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnApplyCollect_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnApplyUnit_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnZeroCalib_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSpanCalib_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnMultiCalibStart_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnMultiCalibCollect_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnMultiCalibCalc_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnMultiCalibAbort_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnClearData_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSaveExcel_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnDTSConnect_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnDTSApplyNetwork_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnDTSQuickConfig_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnDTSScan_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnDTSStop_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnDTSClear_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnDTSSave_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readPacketDTS'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>,
        // method 'slotDTSConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotDTSDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readColdJunctionTemperature'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnQuickSetIP_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnReconnectNewIP_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnResetDefaultIP_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAutoSaveTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onChartUpdateTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSimultaneousStart_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSimultaneousStop_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateSimultaneousButtonsState'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onNewTCPConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTCPClientDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTCPSendTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnDTSSingleConfig_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnTestChannel_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateTestChannelTemperature'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPressureRandomTimerTimeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMultiSectionSendTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->on_pushButton_clicked(); break;
        case 1: _t->readPacket((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 2: _t->slotConnected(); break;
        case 3: _t->slotDisonnected(); break;
        case 4: _t->on_pushButton_update_clicked(); break;
        case 5: _t->handleRcvTCPPktPSI((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 6: _t->on_pushButton_config_clicked(); break;
        case 7: _t->on_pushButton_scan_clicked(); break;
        case 8: _t->on_pushButton_stop_clicked(); break;
        case 9: _t->on_btnApplyNetwork_clicked(); break;
        case 10: _t->on_btnApplyCollect_clicked(); break;
        case 11: _t->on_btnApplyUnit_clicked(); break;
        case 12: _t->on_btnZeroCalib_clicked(); break;
        case 13: _t->on_btnSpanCalib_clicked(); break;
        case 14: _t->on_btnMultiCalibStart_clicked(); break;
        case 15: _t->on_btnMultiCalibCollect_clicked(); break;
        case 16: _t->on_btnMultiCalibCalc_clicked(); break;
        case 17: _t->on_btnMultiCalibAbort_clicked(); break;
        case 18: _t->on_btnClearData_clicked(); break;
        case 19: _t->on_btnSaveExcel_clicked(); break;
        case 20: _t->on_btnDTSConnect_clicked(); break;
        case 21: _t->on_btnDTSApplyNetwork_clicked(); break;
        case 22: _t->on_btnDTSQuickConfig_clicked(); break;
        case 23: _t->on_btnDTSScan_clicked(); break;
        case 24: _t->on_btnDTSStop_clicked(); break;
        case 25: _t->on_btnDTSClear_clicked(); break;
        case 26: _t->on_btnDTSSave_clicked(); break;
        case 27: _t->readPacketDTS((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 28: _t->slotDTSConnected(); break;
        case 29: _t->slotDTSDisconnected(); break;
        case 30: _t->readColdJunctionTemperature(); break;
        case 31: _t->on_btnQuickSetIP_clicked(); break;
        case 32: _t->on_btnReconnectNewIP_clicked(); break;
        case 33: _t->on_btnResetDefaultIP_clicked(); break;
        case 34: _t->onAutoSaveTimer(); break;
        case 35: _t->onChartUpdateTimer(); break;
        case 36: _t->on_btnSimultaneousStart_clicked(); break;
        case 37: _t->on_btnSimultaneousStop_clicked(); break;
        case 38: _t->updateSimultaneousButtonsState(); break;
        case 39: _t->onNewTCPConnection(); break;
        case 40: _t->onTCPClientDisconnected(); break;
        case 41: _t->onTCPSendTimer(); break;
        case 42: _t->on_btnDTSSingleConfig_clicked(); break;
        case 43: _t->on_btnTestChannel_clicked(); break;
        case 44: _t->updateTestChannelTemperature(); break;
        case 45: _t->onPressureRandomTimerTimeout(); break;
        case 46: _t->onMultiSectionSendTimer(); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN10MainWindowE.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 47)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 47;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 47)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 47;
    }
    return _id;
}
QT_WARNING_POP
