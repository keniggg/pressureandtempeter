/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayoutMain;
    QPushButton *btnShowMultiSection;
    QHBoxLayout *horizontalLayoutTop;
    QGroupBox *groupBoxDPS;
    QVBoxLayout *verticalLayoutDPS;
    QGroupBox *groupBoxNetwork;
    QGridLayout *gridLayoutNetwork;
    QLabel *labelIP;
    QLineEdit *lineEdit_ip;
    QLabel *labelPort;
    QLineEdit *lineEdit_port;
    QPushButton *pushButton_update;
    QLabel *labelSubnet;
    QLineEdit *lineEdit_subnet;
    QLabel *labelGateway;
    QLineEdit *lineEdit_gateway;
    QPushButton *btnApplyNetwork;
    QGroupBox *groupBoxCollect;
    QGridLayout *gridLayoutCollect;
    QLabel *labelSync;
    QComboBox *comboSync;
    QLabel *labelPeriod;
    QLineEdit *lineEdit_period;
    QLabel *labelFormat;
    QComboBox *comboFormat;
    QLabel *labelTrigger;
    QComboBox *comboTrigger;
    QLabel *labelCollectNum;
    QLineEdit *lineEdit_collectNum;
    QLabel *labelUnit;
    QComboBox *comboUnit;
    QPushButton *btnApplyUnit;
    QPushButton *btnApplyCollect;
    QGroupBox *groupBoxCalib;
    QGridLayout *gridLayoutCalib;
    QPushButton *btnZeroCalib;
    QLabel *labelSpanPress;
    QLineEdit *lineEdit_spanPress;
    QPushButton *btnSpanCalib;
    QLabel *labelCalibPoints;
    QLineEdit *lineEdit_calibPoints;
    QLabel *labelCalibOrder;
    QLineEdit *lineEdit_calibOrder;
    QLabel *labelCalibAvg;
    QLineEdit *lineEdit_calibAvg;
    QPushButton *btnMultiCalibStart;
    QPushButton *btnMultiCalibCollect;
    QPushButton *btnMultiCalibCalc;
    QPushButton *btnMultiCalibAbort;
    QGroupBox *groupBoxData;
    QHBoxLayout *horizontalLayoutData;
    QPushButton *pushButton_config;
    QPushButton *pushButton_scan;
    QPushButton *pushButton_stop;
    QPushButton *btnClearData;
    QPushButton *btnSaveExcel;
    QPushButton *btnSimultaneousStart;
    QPushButton *btnSimultaneousStop;
    QLabel *labelAutoSave;
    QGroupBox *groupBoxTCPServer;
    QHBoxLayout *horizontalLayoutTCPServer;
    QLabel *labelTCPStatus;
    QLabel *labelTCPClients;
    QLabel *labelTCPFrames;
    QPushButton *btnTCPRestart;
    QGroupBox *groupBoxPressureChart;
    QVBoxLayout *verticalLayoutPressureChart;
    QWidget *pressureChartWidget;
    QPushButton *btnClearPressureChart;
    QVBoxLayout *verticalLayoutCmd;
    QHBoxLayout *horizontalLayoutCmd;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QLabel *label_2;
    QTextEdit *textEdit;
    QGroupBox *groupBoxDTS;
    QVBoxLayout *verticalLayoutDTS;
    QGroupBox *groupBoxDTSNetwork;
    QGridLayout *gridLayoutDTSNetwork;
    QLabel *labelDTSIP;
    QLineEdit *lineEdit_dts_ip;
    QLabel *labelDTSPort;
    QLineEdit *lineEdit_dts_port;
    QPushButton *btnDTSConnect;
    QLabel *labelDTSSubnet;
    QLineEdit *lineEdit_dts_subnet;
    QLabel *labelDTSGateway;
    QLineEdit *lineEdit_dts_gateway;
    QPushButton *btnDTSApplyNetwork;
    QPushButton *btnQuickSetIP;
    QPushButton *btnReconnectNewIP;
    QPushButton *btnResetDefaultIP;
    QGroupBox *groupBoxDTSConfig;
    QHBoxLayout *horizontalLayoutDTSConfig;
    QComboBox *comboDTSType;
    QLineEdit *lineEdit_dts_interval;
    QPushButton *btnDTSQuickConfig;
    QGroupBox *groupBoxDTSData;
    QHBoxLayout *horizontalLayoutDTSData;
    QPushButton *btnDTSScan;
    QPushButton *btnDTSStop;
    QPushButton *btnDTSClear;
    QPushButton *btnDTSSave;
    QLabel *labelDTSAutoSave;
    QGroupBox *groupBoxDTSDisplay;
    QVBoxLayout *verticalLayoutDTSDisplay;
    QScrollArea *scrollAreaDTS;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayoutDTSData;
    QGroupBox *groupBoxTemperatureChart;
    QVBoxLayout *verticalLayoutTemperatureChart;
    QWidget *temperatureChartWidget;
    QPushButton *btnClearTemperatureChart;
    QGroupBox *groupBoxMultiSectionPressure;
    QVBoxLayout *verticalLayoutMultiPressure;
    QSlider *sliderPressurePs;
    QLabel *labelPressurePs;
    QTabWidget *tabWidgetPressure;
    QWidget *tabPressure0;
    QVBoxLayout *verticalLayoutPressure0;
    QTableWidget *tableWidgetPressure0;
    QWidget *tabPressure01;
    QVBoxLayout *verticalLayoutPressure01;
    QTableWidget *tableWidgetPressure01;
    QWidget *tabPressure1;
    QVBoxLayout *verticalLayoutPressure1;
    QTableWidget *tableWidgetPressure1;
    QWidget *tabPressure15;
    QVBoxLayout *verticalLayoutPressure15;
    QTableWidget *tableWidgetPressure15;
    QWidget *tabPressure2;
    QVBoxLayout *verticalLayoutPressure2;
    QTableWidget *tableWidgetPressure2;
    QWidget *tabPressure3;
    QVBoxLayout *verticalLayoutPressure3;
    QTableWidget *tableWidgetPressure3;
    QWidget *tabPressure35;
    QVBoxLayout *verticalLayoutPressure35;
    QTableWidget *tableWidgetPressure35;
    QWidget *tabPressure4;
    QVBoxLayout *verticalLayoutPressure4;
    QTableWidget *tableWidgetPressure4;
    QWidget *tabPressure5;
    QVBoxLayout *verticalLayoutPressure5;
    QTableWidget *tableWidgetPressure5;
    QGroupBox *groupBoxMultiSectionTemperature;
    QVBoxLayout *verticalLayoutMultiTemperature;
    QSlider *sliderTemperatureTs;
    QLabel *labelTemperatureTs;
    QTabWidget *tabWidgetTemperature;
    QWidget *tabTemperature0;
    QVBoxLayout *verticalLayoutTemperature0;
    QTableWidget *tableWidgetTemperature0;
    QWidget *tabTemperature01;
    QVBoxLayout *verticalLayoutTemperature01;
    QTableWidget *tableWidgetTemperature01;
    QWidget *tabTemperature1;
    QVBoxLayout *verticalLayoutTemperature1;
    QTableWidget *tableWidgetTemperature1;
    QWidget *tabTemperature15;
    QVBoxLayout *verticalLayoutTemperature15;
    QTableWidget *tableWidgetTemperature15;
    QWidget *tabTemperature2;
    QVBoxLayout *verticalLayoutTemperature2;
    QTableWidget *tableWidgetTemperature2;
    QWidget *tabTemperature3;
    QVBoxLayout *verticalLayoutTemperature3;
    QTableWidget *tableWidgetTemperature3;
    QWidget *tabTemperature35;
    QVBoxLayout *verticalLayoutTemperature35;
    QTableWidget *tableWidgetTemperature35;
    QWidget *tabTemperature4;
    QVBoxLayout *verticalLayoutTemperature4;
    QTableWidget *tableWidgetTemperature4;
    QWidget *tabTemperature5;
    QVBoxLayout *verticalLayoutTemperature5;
    QTableWidget *tableWidgetTemperature5;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1600, 1228);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        verticalLayoutMain = new QVBoxLayout(centralWidget);
        verticalLayoutMain->setSpacing(6);
        verticalLayoutMain->setContentsMargins(11, 11, 11, 11);
        verticalLayoutMain->setObjectName("verticalLayoutMain");
        btnShowMultiSection = new QPushButton(centralWidget);
        btnShowMultiSection->setObjectName("btnShowMultiSection");
        btnShowMultiSection->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #FFC107; color: black; font-weight: bold; padding: 8px 16px; border-radius: 4px; }"));

        verticalLayoutMain->addWidget(btnShowMultiSection);

        horizontalLayoutTop = new QHBoxLayout();
        horizontalLayoutTop->setSpacing(6);
        horizontalLayoutTop->setObjectName("horizontalLayoutTop");
        groupBoxDPS = new QGroupBox(centralWidget);
        groupBoxDPS->setObjectName("groupBoxDPS");
        verticalLayoutDPS = new QVBoxLayout(groupBoxDPS);
        verticalLayoutDPS->setSpacing(6);
        verticalLayoutDPS->setContentsMargins(11, 11, 11, 11);
        verticalLayoutDPS->setObjectName("verticalLayoutDPS");
        groupBoxNetwork = new QGroupBox(groupBoxDPS);
        groupBoxNetwork->setObjectName("groupBoxNetwork");
        gridLayoutNetwork = new QGridLayout(groupBoxNetwork);
        gridLayoutNetwork->setSpacing(6);
        gridLayoutNetwork->setContentsMargins(11, 11, 11, 11);
        gridLayoutNetwork->setObjectName("gridLayoutNetwork");
        labelIP = new QLabel(groupBoxNetwork);
        labelIP->setObjectName("labelIP");

        gridLayoutNetwork->addWidget(labelIP, 0, 0, 1, 1);

        lineEdit_ip = new QLineEdit(groupBoxNetwork);
        lineEdit_ip->setObjectName("lineEdit_ip");

        gridLayoutNetwork->addWidget(lineEdit_ip, 0, 1, 1, 1);

        labelPort = new QLabel(groupBoxNetwork);
        labelPort->setObjectName("labelPort");

        gridLayoutNetwork->addWidget(labelPort, 0, 2, 1, 1);

        lineEdit_port = new QLineEdit(groupBoxNetwork);
        lineEdit_port->setObjectName("lineEdit_port");

        gridLayoutNetwork->addWidget(lineEdit_port, 0, 3, 1, 1);

        pushButton_update = new QPushButton(groupBoxNetwork);
        pushButton_update->setObjectName("pushButton_update");

        gridLayoutNetwork->addWidget(pushButton_update, 0, 4, 1, 1);

        labelSubnet = new QLabel(groupBoxNetwork);
        labelSubnet->setObjectName("labelSubnet");

        gridLayoutNetwork->addWidget(labelSubnet, 1, 0, 1, 1);

        lineEdit_subnet = new QLineEdit(groupBoxNetwork);
        lineEdit_subnet->setObjectName("lineEdit_subnet");

        gridLayoutNetwork->addWidget(lineEdit_subnet, 1, 1, 1, 1);

        labelGateway = new QLabel(groupBoxNetwork);
        labelGateway->setObjectName("labelGateway");

        gridLayoutNetwork->addWidget(labelGateway, 1, 2, 1, 1);

        lineEdit_gateway = new QLineEdit(groupBoxNetwork);
        lineEdit_gateway->setObjectName("lineEdit_gateway");

        gridLayoutNetwork->addWidget(lineEdit_gateway, 1, 3, 1, 1);

        btnApplyNetwork = new QPushButton(groupBoxNetwork);
        btnApplyNetwork->setObjectName("btnApplyNetwork");

        gridLayoutNetwork->addWidget(btnApplyNetwork, 1, 4, 1, 1);


        verticalLayoutDPS->addWidget(groupBoxNetwork);

        groupBoxCollect = new QGroupBox(groupBoxDPS);
        groupBoxCollect->setObjectName("groupBoxCollect");
        gridLayoutCollect = new QGridLayout(groupBoxCollect);
        gridLayoutCollect->setSpacing(6);
        gridLayoutCollect->setContentsMargins(11, 11, 11, 11);
        gridLayoutCollect->setObjectName("gridLayoutCollect");
        labelSync = new QLabel(groupBoxCollect);
        labelSync->setObjectName("labelSync");

        gridLayoutCollect->addWidget(labelSync, 0, 0, 1, 1);

        comboSync = new QComboBox(groupBoxCollect);
        comboSync->addItem(QString());
        comboSync->addItem(QString());
        comboSync->setObjectName("comboSync");

        gridLayoutCollect->addWidget(comboSync, 0, 1, 1, 1);

        labelPeriod = new QLabel(groupBoxCollect);
        labelPeriod->setObjectName("labelPeriod");

        gridLayoutCollect->addWidget(labelPeriod, 0, 2, 1, 1);

        lineEdit_period = new QLineEdit(groupBoxCollect);
        lineEdit_period->setObjectName("lineEdit_period");

        gridLayoutCollect->addWidget(lineEdit_period, 0, 3, 1, 1);

        labelFormat = new QLabel(groupBoxCollect);
        labelFormat->setObjectName("labelFormat");

        gridLayoutCollect->addWidget(labelFormat, 0, 4, 1, 1);

        comboFormat = new QComboBox(groupBoxCollect);
        comboFormat->addItem(QString());
        comboFormat->addItem(QString());
        comboFormat->setObjectName("comboFormat");

        gridLayoutCollect->addWidget(comboFormat, 0, 5, 1, 1);

        labelTrigger = new QLabel(groupBoxCollect);
        labelTrigger->setObjectName("labelTrigger");

        gridLayoutCollect->addWidget(labelTrigger, 1, 0, 1, 1);

        comboTrigger = new QComboBox(groupBoxCollect);
        comboTrigger->addItem(QString());
        comboTrigger->addItem(QString());
        comboTrigger->addItem(QString());
        comboTrigger->setObjectName("comboTrigger");

        gridLayoutCollect->addWidget(comboTrigger, 1, 1, 1, 1);

        labelCollectNum = new QLabel(groupBoxCollect);
        labelCollectNum->setObjectName("labelCollectNum");

        gridLayoutCollect->addWidget(labelCollectNum, 1, 2, 1, 1);

        lineEdit_collectNum = new QLineEdit(groupBoxCollect);
        lineEdit_collectNum->setObjectName("lineEdit_collectNum");

        gridLayoutCollect->addWidget(lineEdit_collectNum, 1, 3, 1, 1);

        labelUnit = new QLabel(groupBoxCollect);
        labelUnit->setObjectName("labelUnit");

        gridLayoutCollect->addWidget(labelUnit, 1, 4, 1, 1);

        comboUnit = new QComboBox(groupBoxCollect);
        comboUnit->addItem(QString());
        comboUnit->addItem(QString());
        comboUnit->setObjectName("comboUnit");

        gridLayoutCollect->addWidget(comboUnit, 1, 5, 1, 1);

        btnApplyUnit = new QPushButton(groupBoxCollect);
        btnApplyUnit->setObjectName("btnApplyUnit");

        gridLayoutCollect->addWidget(btnApplyUnit, 1, 6, 1, 1);

        btnApplyCollect = new QPushButton(groupBoxCollect);
        btnApplyCollect->setObjectName("btnApplyCollect");

        gridLayoutCollect->addWidget(btnApplyCollect, 2, 0, 1, 7);


        verticalLayoutDPS->addWidget(groupBoxCollect);

        groupBoxCalib = new QGroupBox(groupBoxDPS);
        groupBoxCalib->setObjectName("groupBoxCalib");
        gridLayoutCalib = new QGridLayout(groupBoxCalib);
        gridLayoutCalib->setSpacing(6);
        gridLayoutCalib->setContentsMargins(11, 11, 11, 11);
        gridLayoutCalib->setObjectName("gridLayoutCalib");
        btnZeroCalib = new QPushButton(groupBoxCalib);
        btnZeroCalib->setObjectName("btnZeroCalib");

        gridLayoutCalib->addWidget(btnZeroCalib, 0, 0, 1, 1);

        labelSpanPress = new QLabel(groupBoxCalib);
        labelSpanPress->setObjectName("labelSpanPress");

        gridLayoutCalib->addWidget(labelSpanPress, 0, 1, 1, 1);

        lineEdit_spanPress = new QLineEdit(groupBoxCalib);
        lineEdit_spanPress->setObjectName("lineEdit_spanPress");

        gridLayoutCalib->addWidget(lineEdit_spanPress, 0, 2, 1, 1);

        btnSpanCalib = new QPushButton(groupBoxCalib);
        btnSpanCalib->setObjectName("btnSpanCalib");

        gridLayoutCalib->addWidget(btnSpanCalib, 0, 3, 1, 1);

        labelCalibPoints = new QLabel(groupBoxCalib);
        labelCalibPoints->setObjectName("labelCalibPoints");

        gridLayoutCalib->addWidget(labelCalibPoints, 1, 0, 1, 1);

        lineEdit_calibPoints = new QLineEdit(groupBoxCalib);
        lineEdit_calibPoints->setObjectName("lineEdit_calibPoints");

        gridLayoutCalib->addWidget(lineEdit_calibPoints, 1, 1, 1, 1);

        labelCalibOrder = new QLabel(groupBoxCalib);
        labelCalibOrder->setObjectName("labelCalibOrder");

        gridLayoutCalib->addWidget(labelCalibOrder, 1, 2, 1, 1);

        lineEdit_calibOrder = new QLineEdit(groupBoxCalib);
        lineEdit_calibOrder->setObjectName("lineEdit_calibOrder");

        gridLayoutCalib->addWidget(lineEdit_calibOrder, 1, 3, 1, 1);

        labelCalibAvg = new QLabel(groupBoxCalib);
        labelCalibAvg->setObjectName("labelCalibAvg");

        gridLayoutCalib->addWidget(labelCalibAvg, 1, 4, 1, 1);

        lineEdit_calibAvg = new QLineEdit(groupBoxCalib);
        lineEdit_calibAvg->setObjectName("lineEdit_calibAvg");

        gridLayoutCalib->addWidget(lineEdit_calibAvg, 1, 5, 1, 1);

        btnMultiCalibStart = new QPushButton(groupBoxCalib);
        btnMultiCalibStart->setObjectName("btnMultiCalibStart");

        gridLayoutCalib->addWidget(btnMultiCalibStart, 1, 6, 1, 1);

        btnMultiCalibCollect = new QPushButton(groupBoxCalib);
        btnMultiCalibCollect->setObjectName("btnMultiCalibCollect");

        gridLayoutCalib->addWidget(btnMultiCalibCollect, 2, 0, 1, 1);

        btnMultiCalibCalc = new QPushButton(groupBoxCalib);
        btnMultiCalibCalc->setObjectName("btnMultiCalibCalc");

        gridLayoutCalib->addWidget(btnMultiCalibCalc, 2, 1, 1, 1);

        btnMultiCalibAbort = new QPushButton(groupBoxCalib);
        btnMultiCalibAbort->setObjectName("btnMultiCalibAbort");

        gridLayoutCalib->addWidget(btnMultiCalibAbort, 2, 2, 1, 1);


        verticalLayoutDPS->addWidget(groupBoxCalib);

        groupBoxData = new QGroupBox(groupBoxDPS);
        groupBoxData->setObjectName("groupBoxData");
        horizontalLayoutData = new QHBoxLayout(groupBoxData);
        horizontalLayoutData->setSpacing(6);
        horizontalLayoutData->setContentsMargins(11, 11, 11, 11);
        horizontalLayoutData->setObjectName("horizontalLayoutData");
        pushButton_config = new QPushButton(groupBoxData);
        pushButton_config->setObjectName("pushButton_config");

        horizontalLayoutData->addWidget(pushButton_config);

        pushButton_scan = new QPushButton(groupBoxData);
        pushButton_scan->setObjectName("pushButton_scan");

        horizontalLayoutData->addWidget(pushButton_scan);

        pushButton_stop = new QPushButton(groupBoxData);
        pushButton_stop->setObjectName("pushButton_stop");

        horizontalLayoutData->addWidget(pushButton_stop);

        btnClearData = new QPushButton(groupBoxData);
        btnClearData->setObjectName("btnClearData");

        horizontalLayoutData->addWidget(btnClearData);

        btnSaveExcel = new QPushButton(groupBoxData);
        btnSaveExcel->setObjectName("btnSaveExcel");

        horizontalLayoutData->addWidget(btnSaveExcel);

        btnSimultaneousStart = new QPushButton(groupBoxData);
        btnSimultaneousStart->setObjectName("btnSimultaneousStart");
        btnSimultaneousStart->setEnabled(false);
        btnSimultaneousStart->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 5px 10px; border-radius: 3px; } QPushButton:disabled { background-color: #cccccc; color: #666666; }"));

        horizontalLayoutData->addWidget(btnSimultaneousStart);

        btnSimultaneousStop = new QPushButton(groupBoxData);
        btnSimultaneousStop->setObjectName("btnSimultaneousStop");
        btnSimultaneousStop->setEnabled(false);
        btnSimultaneousStop->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 5px 10px; border-radius: 3px; } QPushButton:disabled { background-color: #cccccc; color: #666666; }"));

        horizontalLayoutData->addWidget(btnSimultaneousStop);

        labelAutoSave = new QLabel(groupBoxData);
        labelAutoSave->setObjectName("labelAutoSave");
        labelAutoSave->setStyleSheet(QString::fromUtf8("QLabel { color: red; font-weight: bold; padding: 0px 10px; }"));

        horizontalLayoutData->addWidget(labelAutoSave);


        verticalLayoutDPS->addWidget(groupBoxData);

        groupBoxTCPServer = new QGroupBox(groupBoxDPS);
        groupBoxTCPServer->setObjectName("groupBoxTCPServer");
        horizontalLayoutTCPServer = new QHBoxLayout(groupBoxTCPServer);
        horizontalLayoutTCPServer->setSpacing(6);
        horizontalLayoutTCPServer->setContentsMargins(11, 11, 11, 11);
        horizontalLayoutTCPServer->setObjectName("horizontalLayoutTCPServer");
        labelTCPStatus = new QLabel(groupBoxTCPServer);
        labelTCPStatus->setObjectName("labelTCPStatus");
        labelTCPStatus->setStyleSheet(QString::fromUtf8("QLabel { color: red; font-weight: bold; padding: 5px 10px; }"));

        horizontalLayoutTCPServer->addWidget(labelTCPStatus);

        labelTCPClients = new QLabel(groupBoxTCPServer);
        labelTCPClients->setObjectName("labelTCPClients");
        labelTCPClients->setStyleSheet(QString::fromUtf8("QLabel { color: blue; font-weight: bold; padding: 5px 10px; }"));

        horizontalLayoutTCPServer->addWidget(labelTCPClients);

        labelTCPFrames = new QLabel(groupBoxTCPServer);
        labelTCPFrames->setObjectName("labelTCPFrames");
        labelTCPFrames->setStyleSheet(QString::fromUtf8("QLabel { color: green; font-weight: bold; padding: 5px 10px; }"));

        horizontalLayoutTCPServer->addWidget(labelTCPFrames);

        btnTCPRestart = new QPushButton(groupBoxTCPServer);
        btnTCPRestart->setObjectName("btnTCPRestart");
        btnTCPRestart->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #2196F3; color: white; font-weight: bold; padding: 5px 10px; border-radius: 3px; border: 1px solid #1976D2; } QPushButton:hover { background-color: #1976D2; } QPushButton:pressed { background-color: #0D47A1; }"));

        horizontalLayoutTCPServer->addWidget(btnTCPRestart);


        verticalLayoutDPS->addWidget(groupBoxTCPServer);

        groupBoxPressureChart = new QGroupBox(groupBoxDPS);
        groupBoxPressureChart->setObjectName("groupBoxPressureChart");
        verticalLayoutPressureChart = new QVBoxLayout(groupBoxPressureChart);
        verticalLayoutPressureChart->setSpacing(6);
        verticalLayoutPressureChart->setContentsMargins(11, 11, 11, 11);
        verticalLayoutPressureChart->setObjectName("verticalLayoutPressureChart");
        pressureChartWidget = new QWidget(groupBoxPressureChart);
        pressureChartWidget->setObjectName("pressureChartWidget");
        pressureChartWidget->setMinimumSize(QSize(0, 300));

        verticalLayoutPressureChart->addWidget(pressureChartWidget);

        btnClearPressureChart = new QPushButton(groupBoxPressureChart);
        btnClearPressureChart->setObjectName("btnClearPressureChart");

        verticalLayoutPressureChart->addWidget(btnClearPressureChart);


        verticalLayoutDPS->addWidget(groupBoxPressureChart);

        verticalLayoutCmd = new QVBoxLayout();
        verticalLayoutCmd->setSpacing(6);
        verticalLayoutCmd->setObjectName("verticalLayoutCmd");
        horizontalLayoutCmd = new QHBoxLayout();
        horizontalLayoutCmd->setSpacing(6);
        horizontalLayoutCmd->setObjectName("horizontalLayoutCmd");
        lineEdit = new QLineEdit(groupBoxDPS);
        lineEdit->setObjectName("lineEdit");

        horizontalLayoutCmd->addWidget(lineEdit);

        pushButton = new QPushButton(groupBoxDPS);
        pushButton->setObjectName("pushButton");

        horizontalLayoutCmd->addWidget(pushButton);


        verticalLayoutCmd->addLayout(horizontalLayoutCmd);

        label_2 = new QLabel(groupBoxDPS);
        label_2->setObjectName("label_2");

        verticalLayoutCmd->addWidget(label_2);

        textEdit = new QTextEdit(groupBoxDPS);
        textEdit->setObjectName("textEdit");
        textEdit->setReadOnly(true);

        verticalLayoutCmd->addWidget(textEdit);


        verticalLayoutDPS->addLayout(verticalLayoutCmd);


        horizontalLayoutTop->addWidget(groupBoxDPS);

        groupBoxDTS = new QGroupBox(centralWidget);
        groupBoxDTS->setObjectName("groupBoxDTS");
        verticalLayoutDTS = new QVBoxLayout(groupBoxDTS);
        verticalLayoutDTS->setSpacing(6);
        verticalLayoutDTS->setContentsMargins(11, 11, 11, 11);
        verticalLayoutDTS->setObjectName("verticalLayoutDTS");
        groupBoxDTSNetwork = new QGroupBox(groupBoxDTS);
        groupBoxDTSNetwork->setObjectName("groupBoxDTSNetwork");
        gridLayoutDTSNetwork = new QGridLayout(groupBoxDTSNetwork);
        gridLayoutDTSNetwork->setSpacing(6);
        gridLayoutDTSNetwork->setContentsMargins(11, 11, 11, 11);
        gridLayoutDTSNetwork->setObjectName("gridLayoutDTSNetwork");
        labelDTSIP = new QLabel(groupBoxDTSNetwork);
        labelDTSIP->setObjectName("labelDTSIP");

        gridLayoutDTSNetwork->addWidget(labelDTSIP, 0, 0, 1, 1);

        lineEdit_dts_ip = new QLineEdit(groupBoxDTSNetwork);
        lineEdit_dts_ip->setObjectName("lineEdit_dts_ip");

        gridLayoutDTSNetwork->addWidget(lineEdit_dts_ip, 0, 1, 1, 1);

        labelDTSPort = new QLabel(groupBoxDTSNetwork);
        labelDTSPort->setObjectName("labelDTSPort");

        gridLayoutDTSNetwork->addWidget(labelDTSPort, 0, 2, 1, 1);

        lineEdit_dts_port = new QLineEdit(groupBoxDTSNetwork);
        lineEdit_dts_port->setObjectName("lineEdit_dts_port");

        gridLayoutDTSNetwork->addWidget(lineEdit_dts_port, 0, 3, 1, 1);

        btnDTSConnect = new QPushButton(groupBoxDTSNetwork);
        btnDTSConnect->setObjectName("btnDTSConnect");

        gridLayoutDTSNetwork->addWidget(btnDTSConnect, 0, 4, 1, 1);

        labelDTSSubnet = new QLabel(groupBoxDTSNetwork);
        labelDTSSubnet->setObjectName("labelDTSSubnet");

        gridLayoutDTSNetwork->addWidget(labelDTSSubnet, 1, 0, 1, 1);

        lineEdit_dts_subnet = new QLineEdit(groupBoxDTSNetwork);
        lineEdit_dts_subnet->setObjectName("lineEdit_dts_subnet");

        gridLayoutDTSNetwork->addWidget(lineEdit_dts_subnet, 1, 1, 1, 1);

        labelDTSGateway = new QLabel(groupBoxDTSNetwork);
        labelDTSGateway->setObjectName("labelDTSGateway");

        gridLayoutDTSNetwork->addWidget(labelDTSGateway, 1, 2, 1, 1);

        lineEdit_dts_gateway = new QLineEdit(groupBoxDTSNetwork);
        lineEdit_dts_gateway->setObjectName("lineEdit_dts_gateway");

        gridLayoutDTSNetwork->addWidget(lineEdit_dts_gateway, 1, 3, 1, 1);

        btnDTSApplyNetwork = new QPushButton(groupBoxDTSNetwork);
        btnDTSApplyNetwork->setObjectName("btnDTSApplyNetwork");

        gridLayoutDTSNetwork->addWidget(btnDTSApplyNetwork, 1, 4, 1, 1);

        btnQuickSetIP = new QPushButton(groupBoxDTSNetwork);
        btnQuickSetIP->setObjectName("btnQuickSetIP");

        gridLayoutDTSNetwork->addWidget(btnQuickSetIP, 2, 0, 1, 2);

        btnReconnectNewIP = new QPushButton(groupBoxDTSNetwork);
        btnReconnectNewIP->setObjectName("btnReconnectNewIP");

        gridLayoutDTSNetwork->addWidget(btnReconnectNewIP, 2, 2, 1, 2);

        btnResetDefaultIP = new QPushButton(groupBoxDTSNetwork);
        btnResetDefaultIP->setObjectName("btnResetDefaultIP");

        gridLayoutDTSNetwork->addWidget(btnResetDefaultIP, 2, 4, 1, 1);


        verticalLayoutDTS->addWidget(groupBoxDTSNetwork);

        groupBoxDTSConfig = new QGroupBox(groupBoxDTS);
        groupBoxDTSConfig->setObjectName("groupBoxDTSConfig");
        horizontalLayoutDTSConfig = new QHBoxLayout(groupBoxDTSConfig);
        horizontalLayoutDTSConfig->setSpacing(6);
        horizontalLayoutDTSConfig->setContentsMargins(11, 11, 11, 11);
        horizontalLayoutDTSConfig->setObjectName("horizontalLayoutDTSConfig");
        comboDTSType = new QComboBox(groupBoxDTSConfig);
        comboDTSType->addItem(QString());
        comboDTSType->addItem(QString());
        comboDTSType->addItem(QString());
        comboDTSType->addItem(QString());
        comboDTSType->addItem(QString());
        comboDTSType->addItem(QString());
        comboDTSType->addItem(QString());
        comboDTSType->addItem(QString());
        comboDTSType->addItem(QString());
        comboDTSType->addItem(QString());
        comboDTSType->addItem(QString());
        comboDTSType->addItem(QString());
        comboDTSType->setObjectName("comboDTSType");

        horizontalLayoutDTSConfig->addWidget(comboDTSType);

        lineEdit_dts_interval = new QLineEdit(groupBoxDTSConfig);
        lineEdit_dts_interval->setObjectName("lineEdit_dts_interval");

        horizontalLayoutDTSConfig->addWidget(lineEdit_dts_interval);

        btnDTSQuickConfig = new QPushButton(groupBoxDTSConfig);
        btnDTSQuickConfig->setObjectName("btnDTSQuickConfig");

        horizontalLayoutDTSConfig->addWidget(btnDTSQuickConfig);


        verticalLayoutDTS->addWidget(groupBoxDTSConfig);

        groupBoxDTSData = new QGroupBox(groupBoxDTS);
        groupBoxDTSData->setObjectName("groupBoxDTSData");
        horizontalLayoutDTSData = new QHBoxLayout(groupBoxDTSData);
        horizontalLayoutDTSData->setSpacing(6);
        horizontalLayoutDTSData->setContentsMargins(11, 11, 11, 11);
        horizontalLayoutDTSData->setObjectName("horizontalLayoutDTSData");
        btnDTSScan = new QPushButton(groupBoxDTSData);
        btnDTSScan->setObjectName("btnDTSScan");

        horizontalLayoutDTSData->addWidget(btnDTSScan);

        btnDTSStop = new QPushButton(groupBoxDTSData);
        btnDTSStop->setObjectName("btnDTSStop");

        horizontalLayoutDTSData->addWidget(btnDTSStop);

        btnDTSClear = new QPushButton(groupBoxDTSData);
        btnDTSClear->setObjectName("btnDTSClear");

        horizontalLayoutDTSData->addWidget(btnDTSClear);

        btnDTSSave = new QPushButton(groupBoxDTSData);
        btnDTSSave->setObjectName("btnDTSSave");

        horizontalLayoutDTSData->addWidget(btnDTSSave);

        labelDTSAutoSave = new QLabel(groupBoxDTSData);
        labelDTSAutoSave->setObjectName("labelDTSAutoSave");
        labelDTSAutoSave->setStyleSheet(QString::fromUtf8("QLabel { color: red; font-weight: bold; padding: 0px 10px; }"));

        horizontalLayoutDTSData->addWidget(labelDTSAutoSave);


        verticalLayoutDTS->addWidget(groupBoxDTSData);

        groupBoxDTSDisplay = new QGroupBox(groupBoxDTS);
        groupBoxDTSDisplay->setObjectName("groupBoxDTSDisplay");
        verticalLayoutDTSDisplay = new QVBoxLayout(groupBoxDTSDisplay);
        verticalLayoutDTSDisplay->setSpacing(6);
        verticalLayoutDTSDisplay->setContentsMargins(11, 11, 11, 11);
        verticalLayoutDTSDisplay->setObjectName("verticalLayoutDTSDisplay");
        scrollAreaDTS = new QScrollArea(groupBoxDTSDisplay);
        scrollAreaDTS->setObjectName("scrollAreaDTS");
        scrollAreaDTS->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 761, 255));
        gridLayoutDTSData = new QGridLayout(scrollAreaWidgetContents);
        gridLayoutDTSData->setSpacing(6);
        gridLayoutDTSData->setContentsMargins(11, 11, 11, 11);
        gridLayoutDTSData->setObjectName("gridLayoutDTSData");
        scrollAreaDTS->setWidget(scrollAreaWidgetContents);

        verticalLayoutDTSDisplay->addWidget(scrollAreaDTS);


        verticalLayoutDTS->addWidget(groupBoxDTSDisplay);

        groupBoxTemperatureChart = new QGroupBox(groupBoxDTS);
        groupBoxTemperatureChart->setObjectName("groupBoxTemperatureChart");
        verticalLayoutTemperatureChart = new QVBoxLayout(groupBoxTemperatureChart);
        verticalLayoutTemperatureChart->setSpacing(6);
        verticalLayoutTemperatureChart->setContentsMargins(11, 11, 11, 11);
        verticalLayoutTemperatureChart->setObjectName("verticalLayoutTemperatureChart");
        temperatureChartWidget = new QWidget(groupBoxTemperatureChart);
        temperatureChartWidget->setObjectName("temperatureChartWidget");
        temperatureChartWidget->setMinimumSize(QSize(0, 300));

        verticalLayoutTemperatureChart->addWidget(temperatureChartWidget);

        btnClearTemperatureChart = new QPushButton(groupBoxTemperatureChart);
        btnClearTemperatureChart->setObjectName("btnClearTemperatureChart");

        verticalLayoutTemperatureChart->addWidget(btnClearTemperatureChart);


        verticalLayoutDTS->addWidget(groupBoxTemperatureChart);


        horizontalLayoutTop->addWidget(groupBoxDTS);


        verticalLayoutMain->addLayout(horizontalLayoutTop);

        groupBoxMultiSectionPressure = new QGroupBox(centralWidget);
        groupBoxMultiSectionPressure->setObjectName("groupBoxMultiSectionPressure");
        verticalLayoutMultiPressure = new QVBoxLayout(groupBoxMultiSectionPressure);
        verticalLayoutMultiPressure->setSpacing(6);
        verticalLayoutMultiPressure->setContentsMargins(11, 11, 11, 11);
        verticalLayoutMultiPressure->setObjectName("verticalLayoutMultiPressure");
        sliderPressurePs = new QSlider(groupBoxMultiSectionPressure);
        sliderPressurePs->setObjectName("sliderPressurePs");
        sliderPressurePs->setMinimum(101);
        sliderPressurePs->setMaximum(4000);
        sliderPressurePs->setValue(101);
        sliderPressurePs->setOrientation(Qt::Orientation::Horizontal);
        sliderPressurePs->setTickPosition(QSlider::TickPosition::TicksBelow);
        sliderPressurePs->setTickInterval(100);

        verticalLayoutMultiPressure->addWidget(sliderPressurePs);

        labelPressurePs = new QLabel(groupBoxMultiSectionPressure);
        labelPressurePs->setObjectName("labelPressurePs");
        labelPressurePs->setStyleSheet(QString::fromUtf8("QLabel { font-weight: bold; }"));

        verticalLayoutMultiPressure->addWidget(labelPressurePs);

        tabWidgetPressure = new QTabWidget(groupBoxMultiSectionPressure);
        tabWidgetPressure->setObjectName("tabWidgetPressure");
        tabPressure0 = new QWidget();
        tabPressure0->setObjectName("tabPressure0");
        verticalLayoutPressure0 = new QVBoxLayout(tabPressure0);
        verticalLayoutPressure0->setSpacing(6);
        verticalLayoutPressure0->setContentsMargins(11, 11, 11, 11);
        verticalLayoutPressure0->setObjectName("verticalLayoutPressure0");
        tableWidgetPressure0 = new QTableWidget(tabPressure0);
        if (tableWidgetPressure0->columnCount() < 16)
            tableWidgetPressure0->setColumnCount(16);
        if (tableWidgetPressure0->rowCount() < 12)
            tableWidgetPressure0->setRowCount(12);
        tableWidgetPressure0->setObjectName("tableWidgetPressure0");
        tableWidgetPressure0->setRowCount(12);
        tableWidgetPressure0->setColumnCount(16);

        verticalLayoutPressure0->addWidget(tableWidgetPressure0);

        tabWidgetPressure->addTab(tabPressure0, QString());
        tabPressure01 = new QWidget();
        tabPressure01->setObjectName("tabPressure01");
        verticalLayoutPressure01 = new QVBoxLayout(tabPressure01);
        verticalLayoutPressure01->setSpacing(6);
        verticalLayoutPressure01->setContentsMargins(11, 11, 11, 11);
        verticalLayoutPressure01->setObjectName("verticalLayoutPressure01");
        tableWidgetPressure01 = new QTableWidget(tabPressure01);
        if (tableWidgetPressure01->columnCount() < 8)
            tableWidgetPressure01->setColumnCount(8);
        if (tableWidgetPressure01->rowCount() < 3)
            tableWidgetPressure01->setRowCount(3);
        tableWidgetPressure01->setObjectName("tableWidgetPressure01");
        tableWidgetPressure01->setRowCount(3);
        tableWidgetPressure01->setColumnCount(8);

        verticalLayoutPressure01->addWidget(tableWidgetPressure01);

        tabWidgetPressure->addTab(tabPressure01, QString());
        tabPressure1 = new QWidget();
        tabPressure1->setObjectName("tabPressure1");
        verticalLayoutPressure1 = new QVBoxLayout(tabPressure1);
        verticalLayoutPressure1->setSpacing(6);
        verticalLayoutPressure1->setContentsMargins(11, 11, 11, 11);
        verticalLayoutPressure1->setObjectName("verticalLayoutPressure1");
        tableWidgetPressure1 = new QTableWidget(tabPressure1);
        if (tableWidgetPressure1->columnCount() < 8)
            tableWidgetPressure1->setColumnCount(8);
        if (tableWidgetPressure1->rowCount() < 3)
            tableWidgetPressure1->setRowCount(3);
        tableWidgetPressure1->setObjectName("tableWidgetPressure1");
        tableWidgetPressure1->setRowCount(3);
        tableWidgetPressure1->setColumnCount(8);

        verticalLayoutPressure1->addWidget(tableWidgetPressure1);

        tabWidgetPressure->addTab(tabPressure1, QString());
        tabPressure15 = new QWidget();
        tabPressure15->setObjectName("tabPressure15");
        verticalLayoutPressure15 = new QVBoxLayout(tabPressure15);
        verticalLayoutPressure15->setSpacing(6);
        verticalLayoutPressure15->setContentsMargins(11, 11, 11, 11);
        verticalLayoutPressure15->setObjectName("verticalLayoutPressure15");
        tableWidgetPressure15 = new QTableWidget(tabPressure15);
        if (tableWidgetPressure15->columnCount() < 8)
            tableWidgetPressure15->setColumnCount(8);
        if (tableWidgetPressure15->rowCount() < 3)
            tableWidgetPressure15->setRowCount(3);
        tableWidgetPressure15->setObjectName("tableWidgetPressure15");
        tableWidgetPressure15->setRowCount(3);
        tableWidgetPressure15->setColumnCount(8);

        verticalLayoutPressure15->addWidget(tableWidgetPressure15);

        tabWidgetPressure->addTab(tabPressure15, QString());
        tabPressure2 = new QWidget();
        tabPressure2->setObjectName("tabPressure2");
        verticalLayoutPressure2 = new QVBoxLayout(tabPressure2);
        verticalLayoutPressure2->setSpacing(6);
        verticalLayoutPressure2->setContentsMargins(11, 11, 11, 11);
        verticalLayoutPressure2->setObjectName("verticalLayoutPressure2");
        tableWidgetPressure2 = new QTableWidget(tabPressure2);
        if (tableWidgetPressure2->columnCount() < 8)
            tableWidgetPressure2->setColumnCount(8);
        if (tableWidgetPressure2->rowCount() < 3)
            tableWidgetPressure2->setRowCount(3);
        tableWidgetPressure2->setObjectName("tableWidgetPressure2");
        tableWidgetPressure2->setRowCount(3);
        tableWidgetPressure2->setColumnCount(8);

        verticalLayoutPressure2->addWidget(tableWidgetPressure2);

        tabWidgetPressure->addTab(tabPressure2, QString());
        tabPressure3 = new QWidget();
        tabPressure3->setObjectName("tabPressure3");
        verticalLayoutPressure3 = new QVBoxLayout(tabPressure3);
        verticalLayoutPressure3->setSpacing(6);
        verticalLayoutPressure3->setContentsMargins(11, 11, 11, 11);
        verticalLayoutPressure3->setObjectName("verticalLayoutPressure3");
        tableWidgetPressure3 = new QTableWidget(tabPressure3);
        if (tableWidgetPressure3->columnCount() < 8)
            tableWidgetPressure3->setColumnCount(8);
        if (tableWidgetPressure3->rowCount() < 3)
            tableWidgetPressure3->setRowCount(3);
        tableWidgetPressure3->setObjectName("tableWidgetPressure3");
        tableWidgetPressure3->setRowCount(3);
        tableWidgetPressure3->setColumnCount(8);

        verticalLayoutPressure3->addWidget(tableWidgetPressure3);

        tabWidgetPressure->addTab(tabPressure3, QString());
        tabPressure35 = new QWidget();
        tabPressure35->setObjectName("tabPressure35");
        verticalLayoutPressure35 = new QVBoxLayout(tabPressure35);
        verticalLayoutPressure35->setSpacing(6);
        verticalLayoutPressure35->setContentsMargins(11, 11, 11, 11);
        verticalLayoutPressure35->setObjectName("verticalLayoutPressure35");
        tableWidgetPressure35 = new QTableWidget(tabPressure35);
        if (tableWidgetPressure35->columnCount() < 8)
            tableWidgetPressure35->setColumnCount(8);
        if (tableWidgetPressure35->rowCount() < 3)
            tableWidgetPressure35->setRowCount(3);
        tableWidgetPressure35->setObjectName("tableWidgetPressure35");
        tableWidgetPressure35->setRowCount(3);
        tableWidgetPressure35->setColumnCount(8);

        verticalLayoutPressure35->addWidget(tableWidgetPressure35);

        tabWidgetPressure->addTab(tabPressure35, QString());
        tabPressure4 = new QWidget();
        tabPressure4->setObjectName("tabPressure4");
        verticalLayoutPressure4 = new QVBoxLayout(tabPressure4);
        verticalLayoutPressure4->setSpacing(6);
        verticalLayoutPressure4->setContentsMargins(11, 11, 11, 11);
        verticalLayoutPressure4->setObjectName("verticalLayoutPressure4");
        tableWidgetPressure4 = new QTableWidget(tabPressure4);
        if (tableWidgetPressure4->columnCount() < 8)
            tableWidgetPressure4->setColumnCount(8);
        if (tableWidgetPressure4->rowCount() < 3)
            tableWidgetPressure4->setRowCount(3);
        tableWidgetPressure4->setObjectName("tableWidgetPressure4");
        tableWidgetPressure4->setRowCount(3);
        tableWidgetPressure4->setColumnCount(8);

        verticalLayoutPressure4->addWidget(tableWidgetPressure4);

        tabWidgetPressure->addTab(tabPressure4, QString());
        tabPressure5 = new QWidget();
        tabPressure5->setObjectName("tabPressure5");
        verticalLayoutPressure5 = new QVBoxLayout(tabPressure5);
        verticalLayoutPressure5->setSpacing(6);
        verticalLayoutPressure5->setContentsMargins(11, 11, 11, 11);
        verticalLayoutPressure5->setObjectName("verticalLayoutPressure5");
        tableWidgetPressure5 = new QTableWidget(tabPressure5);
        if (tableWidgetPressure5->columnCount() < 8)
            tableWidgetPressure5->setColumnCount(8);
        if (tableWidgetPressure5->rowCount() < 3)
            tableWidgetPressure5->setRowCount(3);
        tableWidgetPressure5->setObjectName("tableWidgetPressure5");
        tableWidgetPressure5->setRowCount(3);
        tableWidgetPressure5->setColumnCount(8);

        verticalLayoutPressure5->addWidget(tableWidgetPressure5);

        tabWidgetPressure->addTab(tabPressure5, QString());

        verticalLayoutMultiPressure->addWidget(tabWidgetPressure);


        verticalLayoutMain->addWidget(groupBoxMultiSectionPressure);

        groupBoxMultiSectionTemperature = new QGroupBox(centralWidget);
        groupBoxMultiSectionTemperature->setObjectName("groupBoxMultiSectionTemperature");
        verticalLayoutMultiTemperature = new QVBoxLayout(groupBoxMultiSectionTemperature);
        verticalLayoutMultiTemperature->setSpacing(6);
        verticalLayoutMultiTemperature->setContentsMargins(11, 11, 11, 11);
        verticalLayoutMultiTemperature->setObjectName("verticalLayoutMultiTemperature");
        sliderTemperatureTs = new QSlider(groupBoxMultiSectionTemperature);
        sliderTemperatureTs->setObjectName("sliderTemperatureTs");
        sliderTemperatureTs->setMinimum(20);
        sliderTemperatureTs->setMaximum(1100);
        sliderTemperatureTs->setValue(20);
        sliderTemperatureTs->setOrientation(Qt::Orientation::Horizontal);
        sliderTemperatureTs->setTickPosition(QSlider::TickPosition::TicksBelow);
        sliderTemperatureTs->setTickInterval(50);

        verticalLayoutMultiTemperature->addWidget(sliderTemperatureTs);

        labelTemperatureTs = new QLabel(groupBoxMultiSectionTemperature);
        labelTemperatureTs->setObjectName("labelTemperatureTs");
        labelTemperatureTs->setStyleSheet(QString::fromUtf8("QLabel { font-weight: bold; }"));

        verticalLayoutMultiTemperature->addWidget(labelTemperatureTs);

        tabWidgetTemperature = new QTabWidget(groupBoxMultiSectionTemperature);
        tabWidgetTemperature->setObjectName("tabWidgetTemperature");
        tabTemperature0 = new QWidget();
        tabTemperature0->setObjectName("tabTemperature0");
        verticalLayoutTemperature0 = new QVBoxLayout(tabTemperature0);
        verticalLayoutTemperature0->setSpacing(6);
        verticalLayoutTemperature0->setContentsMargins(11, 11, 11, 11);
        verticalLayoutTemperature0->setObjectName("verticalLayoutTemperature0");
        tableWidgetTemperature0 = new QTableWidget(tabTemperature0);
        if (tableWidgetTemperature0->columnCount() < 16)
            tableWidgetTemperature0->setColumnCount(16);
        if (tableWidgetTemperature0->rowCount() < 12)
            tableWidgetTemperature0->setRowCount(12);
        tableWidgetTemperature0->setObjectName("tableWidgetTemperature0");
        tableWidgetTemperature0->setRowCount(12);
        tableWidgetTemperature0->setColumnCount(16);

        verticalLayoutTemperature0->addWidget(tableWidgetTemperature0);

        tabWidgetTemperature->addTab(tabTemperature0, QString());
        tabTemperature01 = new QWidget();
        tabTemperature01->setObjectName("tabTemperature01");
        verticalLayoutTemperature01 = new QVBoxLayout(tabTemperature01);
        verticalLayoutTemperature01->setSpacing(6);
        verticalLayoutTemperature01->setContentsMargins(11, 11, 11, 11);
        verticalLayoutTemperature01->setObjectName("verticalLayoutTemperature01");
        tableWidgetTemperature01 = new QTableWidget(tabTemperature01);
        if (tableWidgetTemperature01->columnCount() < 8)
            tableWidgetTemperature01->setColumnCount(8);
        if (tableWidgetTemperature01->rowCount() < 3)
            tableWidgetTemperature01->setRowCount(3);
        tableWidgetTemperature01->setObjectName("tableWidgetTemperature01");
        tableWidgetTemperature01->setRowCount(3);
        tableWidgetTemperature01->setColumnCount(8);

        verticalLayoutTemperature01->addWidget(tableWidgetTemperature01);

        tabWidgetTemperature->addTab(tabTemperature01, QString());
        tabTemperature1 = new QWidget();
        tabTemperature1->setObjectName("tabTemperature1");
        verticalLayoutTemperature1 = new QVBoxLayout(tabTemperature1);
        verticalLayoutTemperature1->setSpacing(6);
        verticalLayoutTemperature1->setContentsMargins(11, 11, 11, 11);
        verticalLayoutTemperature1->setObjectName("verticalLayoutTemperature1");
        tableWidgetTemperature1 = new QTableWidget(tabTemperature1);
        if (tableWidgetTemperature1->columnCount() < 8)
            tableWidgetTemperature1->setColumnCount(8);
        if (tableWidgetTemperature1->rowCount() < 3)
            tableWidgetTemperature1->setRowCount(3);
        tableWidgetTemperature1->setObjectName("tableWidgetTemperature1");
        tableWidgetTemperature1->setRowCount(3);
        tableWidgetTemperature1->setColumnCount(8);

        verticalLayoutTemperature1->addWidget(tableWidgetTemperature1);

        tabWidgetTemperature->addTab(tabTemperature1, QString());
        tabTemperature15 = new QWidget();
        tabTemperature15->setObjectName("tabTemperature15");
        verticalLayoutTemperature15 = new QVBoxLayout(tabTemperature15);
        verticalLayoutTemperature15->setSpacing(6);
        verticalLayoutTemperature15->setContentsMargins(11, 11, 11, 11);
        verticalLayoutTemperature15->setObjectName("verticalLayoutTemperature15");
        tableWidgetTemperature15 = new QTableWidget(tabTemperature15);
        if (tableWidgetTemperature15->columnCount() < 8)
            tableWidgetTemperature15->setColumnCount(8);
        if (tableWidgetTemperature15->rowCount() < 3)
            tableWidgetTemperature15->setRowCount(3);
        tableWidgetTemperature15->setObjectName("tableWidgetTemperature15");
        tableWidgetTemperature15->setRowCount(3);
        tableWidgetTemperature15->setColumnCount(8);

        verticalLayoutTemperature15->addWidget(tableWidgetTemperature15);

        tabWidgetTemperature->addTab(tabTemperature15, QString());
        tabTemperature2 = new QWidget();
        tabTemperature2->setObjectName("tabTemperature2");
        verticalLayoutTemperature2 = new QVBoxLayout(tabTemperature2);
        verticalLayoutTemperature2->setSpacing(6);
        verticalLayoutTemperature2->setContentsMargins(11, 11, 11, 11);
        verticalLayoutTemperature2->setObjectName("verticalLayoutTemperature2");
        tableWidgetTemperature2 = new QTableWidget(tabTemperature2);
        if (tableWidgetTemperature2->columnCount() < 8)
            tableWidgetTemperature2->setColumnCount(8);
        if (tableWidgetTemperature2->rowCount() < 3)
            tableWidgetTemperature2->setRowCount(3);
        tableWidgetTemperature2->setObjectName("tableWidgetTemperature2");
        tableWidgetTemperature2->setRowCount(3);
        tableWidgetTemperature2->setColumnCount(8);

        verticalLayoutTemperature2->addWidget(tableWidgetTemperature2);

        tabWidgetTemperature->addTab(tabTemperature2, QString());
        tabTemperature3 = new QWidget();
        tabTemperature3->setObjectName("tabTemperature3");
        verticalLayoutTemperature3 = new QVBoxLayout(tabTemperature3);
        verticalLayoutTemperature3->setSpacing(6);
        verticalLayoutTemperature3->setContentsMargins(11, 11, 11, 11);
        verticalLayoutTemperature3->setObjectName("verticalLayoutTemperature3");
        tableWidgetTemperature3 = new QTableWidget(tabTemperature3);
        if (tableWidgetTemperature3->columnCount() < 8)
            tableWidgetTemperature3->setColumnCount(8);
        if (tableWidgetTemperature3->rowCount() < 3)
            tableWidgetTemperature3->setRowCount(3);
        tableWidgetTemperature3->setObjectName("tableWidgetTemperature3");
        tableWidgetTemperature3->setRowCount(3);
        tableWidgetTemperature3->setColumnCount(8);

        verticalLayoutTemperature3->addWidget(tableWidgetTemperature3);

        tabWidgetTemperature->addTab(tabTemperature3, QString());
        tabTemperature35 = new QWidget();
        tabTemperature35->setObjectName("tabTemperature35");
        verticalLayoutTemperature35 = new QVBoxLayout(tabTemperature35);
        verticalLayoutTemperature35->setSpacing(6);
        verticalLayoutTemperature35->setContentsMargins(11, 11, 11, 11);
        verticalLayoutTemperature35->setObjectName("verticalLayoutTemperature35");
        tableWidgetTemperature35 = new QTableWidget(tabTemperature35);
        if (tableWidgetTemperature35->columnCount() < 8)
            tableWidgetTemperature35->setColumnCount(8);
        if (tableWidgetTemperature35->rowCount() < 3)
            tableWidgetTemperature35->setRowCount(3);
        tableWidgetTemperature35->setObjectName("tableWidgetTemperature35");
        tableWidgetTemperature35->setRowCount(3);
        tableWidgetTemperature35->setColumnCount(8);

        verticalLayoutTemperature35->addWidget(tableWidgetTemperature35);

        tabWidgetTemperature->addTab(tabTemperature35, QString());
        tabTemperature4 = new QWidget();
        tabTemperature4->setObjectName("tabTemperature4");
        verticalLayoutTemperature4 = new QVBoxLayout(tabTemperature4);
        verticalLayoutTemperature4->setSpacing(6);
        verticalLayoutTemperature4->setContentsMargins(11, 11, 11, 11);
        verticalLayoutTemperature4->setObjectName("verticalLayoutTemperature4");
        tableWidgetTemperature4 = new QTableWidget(tabTemperature4);
        if (tableWidgetTemperature4->columnCount() < 8)
            tableWidgetTemperature4->setColumnCount(8);
        if (tableWidgetTemperature4->rowCount() < 3)
            tableWidgetTemperature4->setRowCount(3);
        tableWidgetTemperature4->setObjectName("tableWidgetTemperature4");
        tableWidgetTemperature4->setRowCount(3);
        tableWidgetTemperature4->setColumnCount(8);

        verticalLayoutTemperature4->addWidget(tableWidgetTemperature4);

        tabWidgetTemperature->addTab(tabTemperature4, QString());
        tabTemperature5 = new QWidget();
        tabTemperature5->setObjectName("tabTemperature5");
        verticalLayoutTemperature5 = new QVBoxLayout(tabTemperature5);
        verticalLayoutTemperature5->setSpacing(6);
        verticalLayoutTemperature5->setContentsMargins(11, 11, 11, 11);
        verticalLayoutTemperature5->setObjectName("verticalLayoutTemperature5");
        tableWidgetTemperature5 = new QTableWidget(tabTemperature5);
        if (tableWidgetTemperature5->columnCount() < 8)
            tableWidgetTemperature5->setColumnCount(8);
        if (tableWidgetTemperature5->rowCount() < 3)
            tableWidgetTemperature5->setRowCount(3);
        tableWidgetTemperature5->setObjectName("tableWidgetTemperature5");
        tableWidgetTemperature5->setRowCount(3);
        tableWidgetTemperature5->setColumnCount(8);

        verticalLayoutTemperature5->addWidget(tableWidgetTemperature5);

        tabWidgetTemperature->addTab(tabTemperature5, QString());

        verticalLayoutMultiTemperature->addWidget(tabWidgetTemperature);


        verticalLayoutMain->addWidget(groupBoxMultiSectionTemperature);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\345\217\214\350\256\276\345\244\207\351\207\207\351\233\206\347\263\273\347\273\237", nullptr));
        btnShowMultiSection->setText(QCoreApplication::translate("MainWindow", "\346\265\213\350\257\225\351\200\232\351\201\223\346\230\276\347\244\272", nullptr));
        groupBoxDPS->setTitle(QCoreApplication::translate("MainWindow", "DPS-16P2 \345\216\213\345\212\233\346\211\253\346\217\217\351\230\200", nullptr));
        groupBoxNetwork->setTitle(QCoreApplication::translate("MainWindow", "\347\275\221\347\273\234\351\205\215\347\275\256\357\274\210\350\257\264\346\230\216\344\271\2464.2.18\357\274\211", nullptr));
        labelIP->setText(QCoreApplication::translate("MainWindow", "\350\256\276\345\244\207IP\357\274\232", nullptr));
        lineEdit_ip->setText(QCoreApplication::translate("MainWindow", "192.168.100.105", nullptr));
        labelPort->setText(QCoreApplication::translate("MainWindow", "\347\253\257\345\217\243\357\274\232", nullptr));
        lineEdit_port->setText(QCoreApplication::translate("MainWindow", "9000", nullptr));
        pushButton_update->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245", nullptr));
        labelSubnet->setText(QCoreApplication::translate("MainWindow", "\345\255\220\347\275\221\346\216\251\347\240\201\357\274\232", nullptr));
        lineEdit_subnet->setText(QCoreApplication::translate("MainWindow", "255.255.255.0", nullptr));
        labelGateway->setText(QCoreApplication::translate("MainWindow", "\347\275\221\345\205\263\357\274\232", nullptr));
        lineEdit_gateway->setText(QCoreApplication::translate("MainWindow", "192.168.100.1", nullptr));
        btnApplyNetwork->setText(QCoreApplication::translate("MainWindow", "\345\272\224\347\224\250\347\275\221\347\273\234\351\205\215\347\275\256", nullptr));
        groupBoxCollect->setTitle(QCoreApplication::translate("MainWindow", "\351\207\207\351\233\206\351\205\215\347\275\256\357\274\210\350\257\264\346\230\216\344\271\2464.2.8\357\274\211", nullptr));
        labelSync->setText(QCoreApplication::translate("MainWindow", "\345\220\214\346\255\245\346\226\271\345\274\217\357\274\232", nullptr));
        comboSync->setItemText(0, QCoreApplication::translate("MainWindow", "\347\241\254\344\273\266\350\247\246\345\217\221(0)", nullptr));
        comboSync->setItemText(1, QCoreApplication::translate("MainWindow", "\345\206\205\351\203\250\346\227\266\351\222\237(1)", nullptr));

        labelPeriod->setText(QCoreApplication::translate("MainWindow", "\351\207\207\351\233\206\345\221\250\346\234\237(ms)\357\274\232", nullptr));
        lineEdit_period->setText(QCoreApplication::translate("MainWindow", "20", nullptr));
        labelFormat->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\346\240\274\345\274\217\357\274\232", nullptr));
        comboFormat->setItemText(0, QCoreApplication::translate("MainWindow", "\345\244\247\345\244\264\346\240\274\345\274\217(7)", nullptr));
        comboFormat->setItemText(1, QCoreApplication::translate("MainWindow", "\345\260\217\345\244\264\346\240\274\345\274\217(8)", nullptr));

        labelTrigger->setText(QCoreApplication::translate("MainWindow", "\350\247\246\345\217\221\346\226\271\345\274\217\357\274\232", nullptr));
        comboTrigger->setItemText(0, QCoreApplication::translate("MainWindow", "\344\270\212\345\215\207\346\262\277", nullptr));
        comboTrigger->setItemText(1, QCoreApplication::translate("MainWindow", "\344\270\213\351\231\215\346\262\277", nullptr));
        comboTrigger->setItemText(2, QCoreApplication::translate("MainWindow", "\345\217\214\346\262\277", nullptr));

        labelCollectNum->setText(QCoreApplication::translate("MainWindow", "\351\207\207\351\233\206\346\254\241\346\225\260(0=\350\277\236\347\273\255)\357\274\232", nullptr));
        lineEdit_collectNum->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        labelUnit->setText(QCoreApplication::translate("MainWindow", "\345\216\213\345\212\233\345\215\225\344\275\215\357\274\232", nullptr));
        comboUnit->setItemText(0, QCoreApplication::translate("MainWindow", "psi", nullptr));
        comboUnit->setItemText(1, QCoreApplication::translate("MainWindow", "kPa", nullptr));

        btnApplyUnit->setText(QCoreApplication::translate("MainWindow", "\345\272\224\347\224\250\345\215\225\344\275\215", nullptr));
        btnApplyCollect->setText(QCoreApplication::translate("MainWindow", "\345\272\224\347\224\250\351\207\207\351\233\206\351\205\215\347\275\256", nullptr));
        groupBoxCalib->setTitle(QCoreApplication::translate("MainWindow", "\346\240\241\345\207\206\351\205\215\347\275\256\357\274\210\350\257\264\346\230\216\344\271\2465\357\274\211", nullptr));
        btnZeroCalib->setText(QCoreApplication::translate("MainWindow", "\351\233\266\347\202\271\346\240\241\345\207\206\357\274\210h\345\221\275\344\273\244\357\274\211", nullptr));
        labelSpanPress->setText(QCoreApplication::translate("MainWindow", "\346\273\241\351\207\217\347\250\213\345\216\213\345\212\233(psi)\357\274\232", nullptr));
        lineEdit_spanPress->setText(QCoreApplication::translate("MainWindow", "100", nullptr));
        btnSpanCalib->setText(QCoreApplication::translate("MainWindow", "\346\273\241\351\207\217\347\250\213\346\240\241\345\207\206\357\274\210Z\345\221\275\344\273\244\357\274\211", nullptr));
        labelCalibPoints->setText(QCoreApplication::translate("MainWindow", "\346\240\241\345\207\206\347\202\271\346\225\260\357\274\232", nullptr));
        lineEdit_calibPoints->setText(QCoreApplication::translate("MainWindow", "5", nullptr));
        labelCalibOrder->setText(QCoreApplication::translate("MainWindow", "\346\213\237\345\220\210\351\230\266\346\225\260\357\274\232", nullptr));
        lineEdit_calibOrder->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        labelCalibAvg->setText(QCoreApplication::translate("MainWindow", "\345\271\263\345\235\207\346\254\241\346\225\260\357\274\232", nullptr));
        lineEdit_calibAvg->setText(QCoreApplication::translate("MainWindow", "500", nullptr));
        btnMultiCalibStart->setText(QCoreApplication::translate("MainWindow", "\345\220\257\345\212\250\345\244\232\347\202\271\346\240\241\345\207\206\357\274\210C 00\357\274\211", nullptr));
        btnMultiCalibCollect->setText(QCoreApplication::translate("MainWindow", "\351\207\207\351\233\206\346\240\241\345\207\206\347\202\271\357\274\210C 01\357\274\211", nullptr));
        btnMultiCalibCalc->setText(QCoreApplication::translate("MainWindow", "\350\256\241\347\256\227\346\240\241\345\207\206\347\263\273\346\225\260\357\274\210C 02\357\274\211", nullptr));
        btnMultiCalibAbort->setText(QCoreApplication::translate("MainWindow", "\347\273\210\346\255\242\346\240\241\345\207\206\357\274\210C 03\357\274\211", nullptr));
        groupBoxData->setTitle(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\346\223\215\344\275\234", nullptr));
        pushButton_config->setText(QCoreApplication::translate("MainWindow", "\345\277\253\351\200\237\351\205\215\347\275\256", nullptr));
        pushButton_scan->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\351\207\207\351\233\206\357\274\210c 01\357\274\211", nullptr));
        pushButton_stop->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242\351\207\207\351\233\206\357\274\210c 02\357\274\211", nullptr));
        btnClearData->setText(QCoreApplication::translate("MainWindow", "\346\270\205\351\231\244\346\225\260\346\215\256", nullptr));
        btnSaveExcel->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230Excel", nullptr));
        btnSimultaneousStart->setText(QCoreApplication::translate("MainWindow", "\345\220\214\346\227\266\351\207\207\351\233\206", nullptr));
        btnSimultaneousStop->setText(QCoreApplication::translate("MainWindow", "\345\220\214\346\227\266\347\273\223\346\235\237", nullptr));
        labelAutoSave->setText(QCoreApplication::translate("MainWindow", "\350\207\252\345\212\250\344\277\235\345\255\230: \345\205\263\351\227\255", nullptr));
        groupBoxTCPServer->setTitle(QCoreApplication::translate("MainWindow", "TCP\346\225\260\346\215\256\345\217\221\351\200\201\346\234\215\345\212\241\345\231\250", nullptr));
        labelTCPStatus->setText(QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241\345\231\250\347\212\266\346\200\201: \345\201\234\346\255\242", nullptr));
        labelTCPClients->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\345\256\242\346\210\267\347\253\257: 0", nullptr));
        labelTCPFrames->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\345\270\247\346\225\260: 0", nullptr));
        btnTCPRestart->setText(QCoreApplication::translate("MainWindow", "\351\207\215\345\220\257\346\234\215\345\212\241\345\231\250", nullptr));
        groupBoxPressureChart->setTitle(QCoreApplication::translate("MainWindow", "16\351\200\232\351\201\223\345\216\213\345\212\233\345\256\236\346\227\266\346\233\262\347\272\277", nullptr));
        btnClearPressureChart->setText(QCoreApplication::translate("MainWindow", "\346\270\205\351\231\244\345\216\213\345\212\233\345\233\276\350\241\250", nullptr));
        lineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\350\207\252\345\256\232\344\271\211\345\221\275\344\273\244\357\274\210\345\246\202c 00 1 FFFF 1 1000 8 0\357\274\211", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\345\221\275\344\273\244", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\350\256\276\345\244\207\345\223\215\345\272\224\344\270\216\351\207\207\351\233\206\346\225\260\346\215\256\357\274\232", nullptr));
        groupBoxDTS->setTitle(QCoreApplication::translate("MainWindow", "DTS-48F2 \347\203\255\347\224\265\345\201\266\351\207\207\351\233\206\344\273\252", nullptr));
        groupBoxDTSNetwork->setTitle(QCoreApplication::translate("MainWindow", "\347\275\221\347\273\234\351\205\215\347\275\256\357\274\210\350\257\264\346\230\216\344\271\2465.1\357\274\211", nullptr));
        labelDTSIP->setText(QCoreApplication::translate("MainWindow", "\350\256\276\345\244\207IP\357\274\232", nullptr));
        lineEdit_dts_ip->setText(QCoreApplication::translate("MainWindow", "192.168.100.24", nullptr));
        labelDTSPort->setText(QCoreApplication::translate("MainWindow", "\347\253\257\345\217\243\357\274\232", nullptr));
        lineEdit_dts_port->setText(QCoreApplication::translate("MainWindow", "9000", nullptr));
        btnDTSConnect->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245", nullptr));
        labelDTSSubnet->setText(QCoreApplication::translate("MainWindow", "\345\255\220\347\275\221\346\216\251\347\240\201\357\274\232", nullptr));
        lineEdit_dts_subnet->setText(QCoreApplication::translate("MainWindow", "255.255.255.0", nullptr));
        labelDTSGateway->setText(QCoreApplication::translate("MainWindow", "\347\275\221\345\205\263\357\274\232", nullptr));
        lineEdit_dts_gateway->setText(QCoreApplication::translate("MainWindow", "192.168.100.1", nullptr));
        btnDTSApplyNetwork->setText(QCoreApplication::translate("MainWindow", "\345\272\224\347\224\250\347\275\221\347\273\234\351\205\215\347\275\256", nullptr));
        btnQuickSetIP->setText(QCoreApplication::translate("MainWindow", "\345\277\253\351\200\237\350\256\276\347\275\256\344\270\272192.168.100.24", nullptr));
        btnReconnectNewIP->setText(QCoreApplication::translate("MainWindow", "\351\207\215\346\226\260\350\277\236\346\216\245\346\226\260IP", nullptr));
        btnResetDefaultIP->setText(QCoreApplication::translate("MainWindow", "\345\244\215\344\275\215\344\270\272\351\273\230\350\256\244IP", nullptr));
        groupBoxDTSConfig->setTitle(QCoreApplication::translate("MainWindow", "\345\277\253\351\200\237\351\205\215\347\275\256", nullptr));
        comboDTSType->setItemText(0, QCoreApplication::translate("MainWindow", "K\345\236\213\347\203\255\347\224\265\345\201\266", nullptr));
        comboDTSType->setItemText(1, QCoreApplication::translate("MainWindow", "J\345\236\213\347\203\255\347\224\265\345\201\266", nullptr));
        comboDTSType->setItemText(2, QCoreApplication::translate("MainWindow", "T\345\236\213\347\203\255\347\224\265\345\201\266", nullptr));
        comboDTSType->setItemText(3, QCoreApplication::translate("MainWindow", "E\345\236\213\347\203\255\347\224\265\345\201\266", nullptr));
        comboDTSType->setItemText(4, QCoreApplication::translate("MainWindow", "N\345\236\213\347\203\255\347\224\265\345\201\266", nullptr));
        comboDTSType->setItemText(5, QCoreApplication::translate("MainWindow", "R\345\236\213\347\203\255\347\224\265\345\201\266", nullptr));
        comboDTSType->setItemText(6, QCoreApplication::translate("MainWindow", "S\345\236\213\347\203\255\347\224\265\345\201\266", nullptr));
        comboDTSType->setItemText(7, QCoreApplication::translate("MainWindow", "B\345\236\213\347\203\255\347\224\265\345\201\266", nullptr));
        comboDTSType->setItemText(8, QCoreApplication::translate("MainWindow", "\302\26110V\347\224\265\345\216\213", nullptr));
        comboDTSType->setItemText(9, QCoreApplication::translate("MainWindow", "\302\2611V\347\224\265\345\216\213", nullptr));
        comboDTSType->setItemText(10, QCoreApplication::translate("MainWindow", "\302\2610.1V\347\224\265\345\216\213", nullptr));
        comboDTSType->setItemText(11, QCoreApplication::translate("MainWindow", "\302\2610.01V\347\224\265\345\216\213", nullptr));

        lineEdit_dts_interval->setPlaceholderText(QCoreApplication::translate("MainWindow", "\351\207\207\346\240\267\351\227\264\351\232\224(ms)\357\274\214\351\273\230\350\256\24420", nullptr));
        btnDTSQuickConfig->setText(QCoreApplication::translate("MainWindow", "\345\277\253\351\200\237\351\205\215\347\275\256", nullptr));
        groupBoxDTSData->setTitle(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\346\223\215\344\275\234", nullptr));
        btnDTSScan->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\351\207\207\351\233\206", nullptr));
        btnDTSStop->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242\351\207\207\351\233\206", nullptr));
        btnDTSClear->setText(QCoreApplication::translate("MainWindow", "\346\270\205\351\231\244\346\225\260\346\215\256", nullptr));
        btnDTSSave->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\346\225\260\346\215\256", nullptr));
        labelDTSAutoSave->setText(QCoreApplication::translate("MainWindow", "\350\207\252\345\212\250\344\277\235\345\255\230: \345\205\263\351\227\255", nullptr));
        groupBoxDTSDisplay->setTitle(QCoreApplication::translate("MainWindow", "48\351\200\232\351\201\223\346\270\251\345\272\246/\347\224\265\345\216\213\346\225\260\346\215\256", nullptr));
        groupBoxTemperatureChart->setTitle(QCoreApplication::translate("MainWindow", "48\351\200\232\351\201\223\346\270\251\345\272\246/\347\224\265\345\216\213\345\256\236\346\227\266\346\233\262\347\272\277", nullptr));
        btnClearTemperatureChart->setText(QCoreApplication::translate("MainWindow", "\346\270\205\351\231\244\346\270\251\345\272\246\345\233\276\350\241\250", nullptr));
        groupBoxMultiSectionPressure->setTitle(QCoreApplication::translate("MainWindow", "\345\244\232\346\210\252\351\235\242\345\216\213\345\212\233\346\225\260\346\215\256\346\230\276\347\244\272\357\274\210384\351\200\232\351\201\223\357\274\211", nullptr));
        labelPressurePs->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215Ps\345\200\274\357\274\2320.101 MPa", nullptr));
        tabWidgetPressure->setTabText(tabWidgetPressure->indexOf(tabPressure0), QCoreApplication::translate("MainWindow", "0\346\210\252\351\235\242\357\274\210192\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetPressure->setTabText(tabWidgetPressure->indexOf(tabPressure01), QCoreApplication::translate("MainWindow", "0.1\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetPressure->setTabText(tabWidgetPressure->indexOf(tabPressure1), QCoreApplication::translate("MainWindow", "1\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetPressure->setTabText(tabWidgetPressure->indexOf(tabPressure15), QCoreApplication::translate("MainWindow", "1.5\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetPressure->setTabText(tabWidgetPressure->indexOf(tabPressure2), QCoreApplication::translate("MainWindow", "2\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetPressure->setTabText(tabWidgetPressure->indexOf(tabPressure3), QCoreApplication::translate("MainWindow", "3\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetPressure->setTabText(tabWidgetPressure->indexOf(tabPressure35), QCoreApplication::translate("MainWindow", "3.5\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetPressure->setTabText(tabWidgetPressure->indexOf(tabPressure4), QCoreApplication::translate("MainWindow", "4\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetPressure->setTabText(tabWidgetPressure->indexOf(tabPressure5), QCoreApplication::translate("MainWindow", "5\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        groupBoxMultiSectionTemperature->setTitle(QCoreApplication::translate("MainWindow", "\345\244\232\346\210\252\351\235\242\346\270\251\345\272\246\346\225\260\346\215\256\346\230\276\347\244\272\357\274\210384\351\200\232\351\201\223\357\274\211", nullptr));
        labelTemperatureTs->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215Ts\345\200\274\357\274\23220 \342\204\203", nullptr));
        tabWidgetTemperature->setTabText(tabWidgetTemperature->indexOf(tabTemperature0), QCoreApplication::translate("MainWindow", "0\346\210\252\351\235\242\357\274\210192\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetTemperature->setTabText(tabWidgetTemperature->indexOf(tabTemperature01), QCoreApplication::translate("MainWindow", "0.1\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetTemperature->setTabText(tabWidgetTemperature->indexOf(tabTemperature1), QCoreApplication::translate("MainWindow", "1\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetTemperature->setTabText(tabWidgetTemperature->indexOf(tabTemperature15), QCoreApplication::translate("MainWindow", "1.5\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetTemperature->setTabText(tabWidgetTemperature->indexOf(tabTemperature2), QCoreApplication::translate("MainWindow", "2\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetTemperature->setTabText(tabWidgetTemperature->indexOf(tabTemperature3), QCoreApplication::translate("MainWindow", "3\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetTemperature->setTabText(tabWidgetTemperature->indexOf(tabTemperature35), QCoreApplication::translate("MainWindow", "3.5\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetTemperature->setTabText(tabWidgetTemperature->indexOf(tabTemperature4), QCoreApplication::translate("MainWindow", "4\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
        tabWidgetTemperature->setTabText(tabWidgetTemperature->indexOf(tabTemperature5), QCoreApplication::translate("MainWindow", "5\346\210\252\351\235\242\357\274\21024\351\200\232\351\201\223\357\274\211", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
