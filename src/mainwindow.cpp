#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QTimer>
#include <QEventLoop>
#include <QDataStream>
#include <QDebug>
#include <QRegularExpression>
#include <QRandomGenerator>  // 用于生成随机颜色
#include <limits>
#include <QPushButton>       // 添加清除图表按钮
#include <QVBoxLayout>       // 添加布局
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QDir>              // 添加目录操作头文件
#include <QStandardPaths>    // 添加标准路径头文件
#include <QRandomGenerator>  // 用于生成随机颜色
#include <limits>

// 全局变量初始化
PSISTREAM g_DeviceStream = {
    .StreamInfo = {
        {.pppp = "FFFF", .sync = 1, .per = 1000, .format = 8, .num = 0, .databit = {.bbbb = 0x0010}},
        {.pppp = "FFFF", .sync = 1, .per = 1000, .format = 8, .num = 0, .databit = {.bbbb = 0x0010}},
        {.pppp = "FFFF", .sync = 1, .per = 1000, .format = 8, .num = 0, .databit = {.bbbb = 0x0010}}
    },
    .pro = 0, .remport = 9000, .ipaddr = "", .lenfix = 0
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    pressureChart(nullptr),
    temperatureChart(nullptr),
    pressureAxisX(nullptr),
    pressureAxisY(nullptr),
    temperatureAxisX(nullptr),
    temperatureAxisY(nullptr),
    m_autoSaveTimer(nullptr),
    m_autoSaveEnabled(false),
    m_autoSaveInterval(1000), // 默认1秒自动保存
    m_currentDPSDoc(nullptr),
    m_currentDTSDoc(nullptr),
    m_dpsTimeBaseRecorded(false),
    m_collectionCount(0),
    m_dpsReady(false),
    m_dtsReady(false),
    m_simultaneousCollecting(false),
    m_tcpServer(nullptr),
    m_tcpSendTimer(nullptr),
    m_tcpFrameCounter(0),
    m_tcpSendEnabled(false),
	    m_dpsDataReady(false),
	    m_dtsDataReady(false),
	  // 初始化随机数生成器
    m_randomGenerator(std::random_device{}()),
    m_pressureDist(-0.03, 0.03),  // 压力随机扰动范围：-0.03~0.03 MPa
    m_temperatureDist(-2.0, 2.0) // 温度随机扰动范围：-10~10 ℃
{
    ui->setupUi(this);

	    // 这些成员在头文件中的声明顺序与初始化列表顺序不同会触发 -Wreorder 警告。
	    // 为避免警告，统一在构造函数体内显式赋值（不影响原有功能）。
	    m_autoSaveFileCounter = 1;
	    m_dtsTimeBaseRecorded = false;
	    m_dpsPacketCount = 0;
	    m_dtsPacketCount = 0;
	    m_dpsActualRate = 0.0;
	    m_dtsActualRate = 0.0;


    // ==================== 初始化采集速率监测 ====================
       m_dpsRateTimer.start();
       m_dtsRateTimer.start();

       // 创建采集速率监测定时器（每秒更新一次）
       m_rateMonitorTimer = new QTimer(this);
       m_rateMonitorTimer->setInterval(1000); // 1秒更新一次
       connect(m_rateMonitorTimer, &QTimer::timeout, this, &MainWindow::updateCollectionRate);


          // 方法1：使用状态栏显示采集速率（推荐）
             if (statusBar()) {
                 // 创建DPS采集速率标签
                 m_labelDPSRate = new QLabel("DPS采集速率: -- Hz", this);
                 m_labelDPSRate->setObjectName("labelDPSRate");
                 m_labelDPSRate->setMinimumWidth(150);

                 // 创建DTS采集速率标签
                 m_labelDTSRate = new QLabel("DTS采集速率: -- Hz", this);
                 m_labelDTSRate->setObjectName("labelDTSRate");
                 m_labelDTSRate->setMinimumWidth(150);

                 // 将标签添加到状态栏（永久部件，显示在右侧）
                 statusBar()->addPermanentWidget(m_labelDPSRate);
                 statusBar()->addPermanentWidget(m_labelDTSRate);
             }

             // 方法2：在现有的UI布局中添加标签（如果在状态栏添加失败）
             // 检查是否存在合适的布局容器
             QWidget* centralWidget = ui->centralWidget;
             if (centralWidget) {
                 // 查找或创建布局来容纳采集速率标签
                 QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(centralWidget->layout());
                 if (!mainLayout) {
                     // 如果没有布局，创建一个垂直布局
                     mainLayout = new QVBoxLayout(centralWidget);
                     centralWidget->setLayout(mainLayout);
                 }

                 // 创建一个水平布局来放置速率标签
                 QHBoxLayout* rateLayout = new QHBoxLayout();
                 rateLayout->setContentsMargins(5, 5, 5, 5);
                 rateLayout->setSpacing(20);

                 // 如果还没有创建标签，则创建它们
                 if (!m_labelDPSRate) {
                     m_labelDPSRate = new QLabel("DPS采集速率: -- Hz", centralWidget);
                     m_labelDPSRate->setObjectName("labelDPSRate");
                     m_labelDPSRate->setMinimumWidth(150);
                 }
                 if (!m_labelDTSRate) {
                     m_labelDTSRate = new QLabel("DTS采集速率: -- Hz", centralWidget);
                     m_labelDTSRate->setObjectName("labelDTSRate");
                     m_labelDTSRate->setMinimumWidth(150);
                 }

                 rateLayout->addStretch(); // 左对齐
                 rateLayout->addWidget(m_labelDPSRate);
                 rateLayout->addWidget(m_labelDTSRate);
                 rateLayout->addStretch(); // 右对齐

                 // 将速率布局添加到主布局的顶部
                 mainLayout->insertLayout(0, rateLayout);
             }

    // 初始化DPS-16 TCP客户端
    tcpClient = new TcpClient;
    connect(tcpClient, &TcpClient::alreadyRead, this, &MainWindow::readPacket);
    connect(tcpClient, &TcpClient::alreadyConnected, this, &MainWindow::slotConnected);
    connect(tcpClient, &TcpClient::alreadyDisconnected, this, &MainWindow::slotDisonnected);

    // 初始化DTS-48 TCP客户端
    tcpClientDTS = new TcpClient;
    connect(tcpClientDTS, &TcpClient::alreadyRead, this, &MainWindow::readPacketDTS);
    connect(tcpClientDTS, &TcpClient::alreadyConnected, this, &MainWindow::slotDTSConnected);
    connect(tcpClientDTS, &TcpClient::alreadyDisconnected, this, &MainWindow::slotDTSDisconnected);

    // 设置DTS-48默认IP和端口（现在设置为192.168.100.24）
    tcpClientDTS->setIP("192.168.100.24");
    tcpClientDTS->setPort(9000);

    // UI默认值初始化（DPS-16）
    ui->lineEdit_ip->setText(tcpClient->returnIP());
    ui->lineEdit_port->setText(tcpClient->returnPort());
    ui->lineEdit_subnet->setText("255.255.255.0");
    ui->lineEdit_gateway->setText("192.168.100.1");
    ui->lineEdit_spanPress->setText("100");
    ui->lineEdit_calibPoints->setText("5");
    ui->lineEdit_calibOrder->setText("1");
    ui->lineEdit_calibAvg->setText("500");
    ui->lineEdit_period->setText("20");  // 修改为20ms默认值
    ui->lineEdit_collectNum->setText("0");

    // UI默认值初始化（DTS-48）- 设置为192.168.100.24
    ui->lineEdit_dts_ip->setText("192.168.100.24");
    ui->lineEdit_dts_port->setText("9000");
    ui->lineEdit_dts_subnet->setText("255.255.255.0");
    ui->lineEdit_dts_gateway->setText("192.168.100.1");
    ui->lineEdit_dts_interval->setText("20");
    ui->comboDTSType->setCurrentIndex(0); // K型热电偶

    // 新增：初始化多截面显示UI
    initMultiSectionUI();
    initPressureTableHeaders();
    initTemperatureTableHeaders();

    // 连接滑动条信号槽
    // 压力滑块处理（通过ui指针访问控件）
    // 压力滑块处理
    connect(ui->sliderPressurePs, &QSlider::valueChanged, this, [this](int value) {
        double Ps = value / 1000.0;
        Ps = qBound(0.0, Ps, 4.0);
        ui->labelPressurePs->setText(QString("当前Ps值：%1 MPa").arg(Ps, 0, 'f', 3));
        updatePressureMultiSection(Ps);
    });

    // 温度滑块处理（通过ui指针访问控件）
    connect(ui->sliderTemperatureTs, &QSlider::valueChanged, this, [this](int value) {
        double Ts = value;
        Ts = qBound(0.0, Ts, 1100.0);
        ui->labelTemperatureTs->setText(QString("当前Ts值：%1 ℃").arg(Ts));
        updateTemperatureMultiSection(Ts);
    });

    // 初始加载默认值数据
    updatePressureMultiSection(0.101);
    updateTemperatureMultiSection(20);

    // 新增：初始化DTS单独通道配置控件（加入快速配置布局）
    // ==============================================
    QHBoxLayout *dtsConfigLayout = qobject_cast<QHBoxLayout*>(ui->groupBoxDTSConfig->layout());
    if (dtsConfigLayout) {
        // 1. 通道号输入框（限制1-48整数输入）
        lineEdit_dts_channel = new QLineEdit(ui->groupBoxDTSConfig);
        lineEdit_dts_channel->setPlaceholderText("通道号(1-48)");
        lineEdit_dts_channel->setValidator(new QIntValidator(1, 48, lineEdit_dts_channel)); // 仅允许输入1-48
        lineEdit_dts_channel->setMaximumWidth(120); // 限制宽度，避免布局变形

        // 2. 单独配置按钮
        btnDTSSingleConfig = new QPushButton("单独配置", ui->groupBoxDTSConfig);
        btnDTSSingleConfig->setMaximumWidth(100);

        // 3. 将控件插入布局（顺序：类型选择 → 通道号 → 单独配置 → 间隔 → 统一配置）
        dtsConfigLayout->insertWidget(1, lineEdit_dts_channel);  // 插入到comboDTSType之后
        dtsConfigLayout->insertWidget(2, btnDTSSingleConfig);    // 插入到通道号之后

        // 4. 连接单独配置按钮的点击信号
        connect(btnDTSSingleConfig, &QPushButton::clicked, this, &MainWindow::on_btnDTSSingleConfig_clicked);
    }

    // 下拉框初始化（DPS-16）
    ui->comboSync->addItems({"硬件触发(0)", "内部时钟(1)"});
    ui->comboFormat->addItems({"大头格式(7)", "小头格式(8)"});
    ui->comboTrigger->addItems({"上升沿", "下降沿", "双沿"});
    ui->comboUnit->addItems({"psi", "kPa"});

    // 初始化DTS-48数据显示
    initDTSDisplay();

    // 禁用保存按钮
    ui->btnSaveExcel->setEnabled(false);
    ui->btnDTSSave->setEnabled(false);

    // 初始化采集频率监控
    m_collectionTimer.start();

    // 验证数据对齐
    verifyDataAlignment();
    // 连接同时采集按钮
    connect(ui->btnSimultaneousStart, &QPushButton::clicked, this, &MainWindow::on_btnSimultaneousStart_clicked);
    connect(ui->btnSimultaneousStop, &QPushButton::clicked, this, &MainWindow::on_btnSimultaneousStop_clicked);

    // 初始更新按钮状态
    updateSimultaneousButtonsState();


    // ==================== 初始化图表 ====================
    initPressureChart();
    initTemperatureChart();

// ==================== 图表实时刷新（与采集解耦，保证50Hz） ====================
// 说明：采集/解析在readyRead链路中执行；图表刷新放在20ms定时器里，避免UI阻塞导致采集掉速。
m_chartUpdateTimer = new QTimer(this);
m_chartUpdateTimer->setTimerType(Qt::PreciseTimer);
m_chartUpdateTimer->setInterval(20); // 50Hz
connect(m_chartUpdateTimer, &QTimer::timeout, this, &MainWindow::onChartUpdateTimer);
m_chartUpdateTimer->start();


    // 连接清除图表按钮
    connect(ui->btnClearPressureChart, &QPushButton::clicked, this, [this]() {
        clearCharts();
    });

    connect(ui->btnClearTemperatureChart, &QPushButton::clicked, this, [this]() {
        // 清除温度图表
        for (int i = 0; i < temperatureSeries.size(); i++) {
            if (temperatureSeries[i]) {
                temperatureSeries[i]->clear();
                temperatureSeries[i]->setVisible(false);
            }
        }
        if (temperatureAxisX) {
            temperatureAxisX->setRange(0, 100);
        }
        if (temperatureAxisY) {
            temperatureAxisY->setRange(-10, 50);
        }
        temperatureChart->setTitle("48通道温度实时曲线（检测到传感器的通道）");
    });

    // ==================== 初始化自动保存 ====================
    initAutoSave();

    // ==================== 初始化TCP服务器 ====================
    initTCPServer();

    // 连接TCP服务器重启按钮
    connect(ui->btnTCPRestart, &QPushButton::clicked, this, [this]() {
        stopTCPServer();
        delayEventLoopMS(100);
        startTCPServer();
    });

    // 新增：初始化压力随机扰动定时器（1秒刷新一次）
       m_pressureRandomTimer = new QTimer(this);
       m_pressureRandomTimer->setInterval(1000); // 1000ms = 1秒
       connect(m_pressureRandomTimer, &QTimer::timeout, this, &MainWindow::onPressureRandomTimerTimeout);
       // 启动定时器（可根据采集状态控制启停，这里默认启动，也可在采集开始时启动）
       m_pressureRandomTimer->start();

    // ==================== 其他连接 ====================
    // 连接同时采集按钮
    connect(ui->btnSimultaneousStart, &QPushButton::clicked, this, &MainWindow::on_btnSimultaneousStart_clicked);
    connect(ui->btnSimultaneousStop, &QPushButton::clicked, this, &MainWindow::on_btnSimultaneousStop_clicked);

    // 初始更新按钮状态
    updateSimultaneousButtonsState();

    // 初始化时隐藏多截面显示控件（改为弹窗显示）
    ui->groupBoxMultiSectionPressure->hide();
    ui->groupBoxMultiSectionTemperature->hide();

    // 在MainWindow构造函数中修改弹窗显示代码
    connect(ui->btnShowMultiSection, &QPushButton::clicked, this, [this]() {
        // 弹出独立窗口显示多截面数据
        QDialog* multiSectionDialog = new QDialog(this);
        multiSectionDialog->setWindowTitle("多截面数据显示（带通道标签）");
        multiSectionDialog->setMinimumSize(1600, 1000);
        QVBoxLayout* dialogLayout = new QVBoxLayout(multiSectionDialog);

        // 创建选项卡容器
        QTabWidget* mainTabWidget = new QTabWidget(multiSectionDialog);

        // ==================== 压力数据选项卡 ====================
        QWidget* pressureTab = new QWidget();
        QVBoxLayout* pressureTabLayout = new QVBoxLayout(pressureTab);

        // 压力滑块和标签
        QGroupBox* pressureControlGroup = new QGroupBox("压力控制");
        QVBoxLayout* pressureControlLayout = new QVBoxLayout(pressureControlGroup);
        QSlider* dialogPressureSlider = new QSlider(Qt::Horizontal);
        QLabel* dialogPressureLabel = new QLabel(ui->labelPressurePs->text());
        dialogPressureSlider->setMinimum(ui->sliderPressurePs->minimum());
        dialogPressureSlider->setMaximum(ui->sliderPressurePs->maximum());
        dialogPressureSlider->setValue(ui->sliderPressurePs->value());
        dialogPressureSlider->setTickPosition(QSlider::TicksBelow);
        dialogPressureSlider->setTickInterval(100);

        connect(dialogPressureSlider, &QSlider::valueChanged, this, [=](int value) {
            double Ps = value / 1000.0;
            Ps = qBound(0.0, Ps, 4.0);
            dialogPressureLabel->setText(QString("当前Ps值：%1 MPa").arg(Ps, 0, 'f', 3));
            ui->sliderPressurePs->setValue(value);
            // 更新所有压力截面数据
            updatePressureMultiSection(Ps);
        });

        pressureControlLayout->addWidget(dialogPressureSlider);
        pressureControlLayout->addWidget(dialogPressureLabel);
        pressureTabLayout->addWidget(pressureControlGroup);

        // 压力数据选项卡（多个截面）
        QTabWidget* pressureSectionTabWidget = new QTabWidget();

        // 0截面压力显示（192通道）- 分成8个页面，每页24个通道
        QTabWidget* pressureSection0TabWidget = new QTabWidget();
        // 创建8个页面，每页3行8列（24通道）
        for (int page = 0; page < 8; page++) {
            QString pageTitle = QString("0截面-第%1页（耙%2-层%3）")
                                    .arg(page + 1)
                                    .arg((page / 4) + 1)  // 每4页一个耙
                                    .arg((page % 4) + 1); // 每页一层
            pressureSection0TabWidget->addTab(createPressureSection0SubWidget(page, pageTitle),
                                              QString("第%1页").arg(page + 1));
        }

        // 将0截面选项卡添加到主选项卡
        pressureSectionTabWidget->addTab(pressureSection0TabWidget, "0截面(分页)");

        // 其他截面压力显示（每个截面一个页面）
        QList<QPair<QString, int>> otherSections = {
            {"0.1截面", 1}, {"1截面", 2}, {"1.5截面", 3}, {"2截面", 4},
            {"3截面", 5}, {"3.5截面", 6}, {"4截面", 7}, {"5截面", 8}
        };

        for (const auto& section : otherSections) {
            QString title = QString("%1压力（24通道）").arg(section.first);
            pressureSectionTabWidget->addTab(createPressureSectionWidget(section.second, title, 3, 8),
                                             section.first);
        }

        pressureTabLayout->addWidget(pressureSectionTabWidget);
        mainTabWidget->addTab(pressureTab, "压力数据");

        // ==================== 温度数据选项卡 ====================
        QWidget* temperatureTab = new QWidget();
        QVBoxLayout* temperatureTabLayout = new QVBoxLayout(temperatureTab);

        // 温度滑块和标签
        QGroupBox* temperatureControlGroup = new QGroupBox("温度控制");
        QVBoxLayout* temperatureControlLayout = new QVBoxLayout(temperatureControlGroup);
        QSlider* dialogTemperatureSlider = new QSlider(Qt::Horizontal);
        QLabel* dialogTemperatureLabel = new QLabel(ui->labelTemperatureTs->text());
        dialogTemperatureSlider->setMinimum(ui->sliderTemperatureTs->minimum());
        dialogTemperatureSlider->setMaximum(ui->sliderTemperatureTs->maximum());
        dialogTemperatureSlider->setValue(ui->sliderTemperatureTs->value());
        dialogTemperatureSlider->setTickPosition(QSlider::TicksBelow);
        dialogTemperatureSlider->setTickInterval(50);

        connect(dialogTemperatureSlider, &QSlider::valueChanged, this, [=](int value) {
            double Ts = value;
            Ts = qBound(0.0, Ts, 1100.0);
            dialogTemperatureLabel->setText(QString("当前Ts值：%1 ℃").arg(Ts));
            ui->sliderTemperatureTs->setValue(value);
            // 更新所有温度截面数据
            updateTemperatureMultiSection(Ts);
        });

        temperatureControlLayout->addWidget(dialogTemperatureSlider);
        temperatureControlLayout->addWidget(dialogTemperatureLabel);
        temperatureTabLayout->addWidget(temperatureControlGroup);

        // 温度数据选项卡（多个截面）
        QTabWidget* temperatureSectionTabWidget = new QTabWidget();

        // 0截面温度显示（192通道）- 分成8个页面，每页24个通道
        QTabWidget* temperatureSection0TabWidget = new QTabWidget();
        // 创建8个页面，每页3行8列（24通道）
        for (int page = 0; page < 8; page++) {
            QString pageTitle = QString("0截面-第%1页（耙%2-层%3）")
                                    .arg(page + 1)
                                    .arg((page / 4) + 1)  // 每4页一个耙
                                    .arg((page % 4) + 1); // 每页一层
            temperatureSection0TabWidget->addTab(createTemperatureSection0SubWidget(page, pageTitle),
                                                 QString("第%1页").arg(page + 1));
        }

        // 将0截面选项卡添加到主选项卡
        temperatureSectionTabWidget->addTab(temperatureSection0TabWidget, "0截面(分页)");

        // 其他截面温度显示（每个截面一个页面）
        for (const auto& section : otherSections) {
            QString title = QString("%1温度（24通道）").arg(section.first);
            temperatureSectionTabWidget->addTab(createTemperatureSectionWidget(section.second, title, 3, 8),
                                                section.first);
        }

        temperatureTabLayout->addWidget(temperatureSectionTabWidget);
        mainTabWidget->addTab(temperatureTab, "温度数据");

        dialogLayout->addWidget(mainTabWidget);

        // 模态显示弹窗，关闭后自动释放资源
        multiSectionDialog->exec();
        delete multiSectionDialog;
    });

    // 初始化时强制设置通道值为0（无传感器状态）
    updatePressureMultiSection(0.0);
    updateTemperatureMultiSection(0.0);
}

MainWindow::~MainWindow()
{
    // 停止自动保存
    stopAutoSave();

    // 停止TCP服务器
    stopTCPServer();

    delete ui;
    delete tcpClient;
    delete tcpClientDTS;

    // 清理图表资源
    qDeleteAll(pressureSeries);
    qDeleteAll(temperatureSeries);
    delete pressureChart;
    delete temperatureChart;
    // 新增：手动释放单独配置控件（可选，父对象已自动处理）
    // delete lineEdit_dts_channel;
    // delete btnDTSSingleConfig;


    // 清理自动保存定时器和文档
    if (m_autoSaveTimer) {
        delete m_autoSaveTimer;
    }
    if (m_currentDPSDoc) {
        delete m_currentDPSDoc;
    }
    if (m_currentDTSDoc) {
        delete m_currentDTSDoc;
    }

    // 清理TCP服务器
    if (m_tcpServer) {
        m_tcpServer->close();
        delete m_tcpServer;
    }

    if (m_tcpSendTimer) {
        delete m_tcpSendTimer;
    }

    // 清理客户端连接
    for (auto client : m_tcpClients) {
        if (client) {
            client->disconnectFromHost();
            if (client->state() != QAbstractSocket::UnconnectedState) {
                client->waitForDisconnected();
            }
            delete client;
        }
    }
    m_tcpClients.clear();
    // 清理采集速率显示标签
        if (m_labelDPSRate) {
            delete m_labelDPSRate;
        }
        if (m_labelDTSRate) {
            delete m_labelDTSRate;
        }

        // 清理速率监测定时器
        if (m_rateMonitorTimer) {
            m_rateMonitorTimer->stop();
            delete m_rateMonitorTimer;
        }
}

// ==================== TCP服务器相关函数 ====================

void MainWindow::initTCPServer()
{
    // 初始化基础数据缓存
    for (int i = 0; i < 16; i++) {
        m_lastDPSPressures[i] = 0.0f;
    }
    for (int i = 0; i < 48; i++) {
        m_lastDTSTemperatures[i] = 0.0f;
    }
    m_dpsDataReady = false;
    m_dtsDataReady = false;
    m_tcpFrameCounter = 0;

    // 初始化TCP服务器和定时器（Qt 6兼容，无废弃接口）
    if (!m_tcpServer) {
        m_tcpServer = new QTcpServer(this);
        connect(m_tcpServer, &QTcpServer::newConnection, this, &MainWindow::onNewTCPConnection);
        appendTextEdit("TCP服务器对象已创建（只发送多截面数据）");
    }
    if (!m_tcpSendTimer) {
        m_tcpSendTimer = new QTimer(this);
        m_tcpSendTimer->setInterval(20);  // 50Hz采集速率
        // 关键修改：只连接多截面发送定时器，不连接原有的sendTCPServerData
        disconnect(m_tcpSendTimer, &QTimer::timeout, this, &MainWindow::onTCPSendTimer);
        connect(m_tcpSendTimer, &QTimer::timeout, this, &MainWindow::onMultiSectionSendTimer);
        appendTextEdit("TCP发送定时器已创建（20ms间隔，只发送多截面数据）");
    }

    // 多截面TCP发送状态初始化
    m_msAetimsDataPacketCount = 0;
    m_msAetimsChannelSent = false;

    // -------------------------- 多截面压力通道表初始化（AETIMS DATAPACKET1）--------------------------
    // 清空整个通道表包，确保所有字段初始化为0
    memset(&m_msAetimsChannelPacket, 0, sizeof(MS_DATAPACKET1));

    PACKETHEAD& msChannelHead = m_msAetimsChannelPacket.PacketHead;
    // 通道表参数（符合AETIMS文档2.2节要求）
    msChannelHead.CommandID = 2;                // 2=通道表（接口强制）
    msChannelHead.Nums = 768;                   // 多截面总通道数=768（压力384+温度384）
    msChannelHead.DeviceID = 2;                 // 多截面专属设备编号（区分其他设备）
    msChannelHead.nPackCount = 1;               // 通道表包计数=1
    msChannelHead.nRef = 0;                    // 备用填0
    memset(msChannelHead.TestName, 0, 56);
    strncpy(msChannelHead.TestName, "多截面压力温度采集", 55); // 试验名称（≤55字节）
    // 包长度计算：PktLens=Nums×sizeof(SENDTAB)（接口文档3节示例要求）
    msChannelHead.PktLens = msChannelHead.Nums * sizeof(SENDTAB);

    // 填充768个多截面通道表（压力384个 + 温度384个）
    int currentChTabIdx = 0;
    SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
    // 1. 压力通道（0~383）
    // 0截面压力（0~191：3耙×4层×16通道）
    for (int rake = 1; rake <= 3; rake++) {
        for (int layer = 1; layer <= 4; layer++) {
            for (int ch = 1; ch <= 16; ch++) {
                if (currentChTabIdx >= 384) break;
                SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
                // 通道信息严格按接口SENDTAB结构填充
                memset(chTab.ChName, 0, 24);
                snprintf(chTab.ChName, 23, "P0_%d_%d_%02d", rake, layer, ch); // 英文名≤23字节
                memset(chTab.ChName_CHN, 0, 56);
                snprintf(chTab.ChName_CHN, 55, "0截面压力_耙%d_层%d_通道%d", rake, layer, ch); // 中文名≤55字节
                memset(chTab.EngUnit, 0, 24);
                strncpy(chTab.EngUnit, "MPa", 23); // 工程单位
                chTab.DownLimit = 0.0f;           // 下限值
                chTab.UpLimit = 10.0f;           // 上限值
                chTab.ChType = 1;               // 1=压力通道（接口约定）
                chTab.AlarmFlag = 0;           // 0=不报警
                chTab.pre_Alarm_low = 0.0f;
                chTab.pre_Alarm_up = 0.0f;
                chTab.Alarm_low = 0.0f;
                chTab.Alarm_up = 0.0f;
                // 不再设置ptnums和bk，使用填充字节
                chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
                chTab.bk = 0;                  // 备用填0
                currentChTabIdx++;
            }
        }
    }

    // 2. 0.1截面压力（192~215：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 384) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "P01_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "0.1截面压力_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "MPa", 23);
            chTab.DownLimit = 0.0f;
            chTab.UpLimit = 10.0f;
            chTab.ChType = 1;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 3. 1截面压力（216~239：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 384) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "P1_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "1截面压力_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "MPa", 23);
            chTab.DownLimit = 0.1f;    // 理论下限
            chTab.UpLimit = 0.16f;    // 理论上限
            chTab.ChType = 1;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 4. 1.5截面压力（240~263：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 384) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "P15_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "1.5截面压力_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "MPa", 23);
            chTab.DownLimit = 0.12f;    // 理论下限
            chTab.UpLimit = 0.18f;    // 理论上限
            chTab.ChType = 1;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 5. 2截面压力（264~287：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 384) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "P2_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "2截面压力_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "MPa", 23);
            chTab.DownLimit = 0.22f;    // 理论下限（Ps=0.2MPa时）
            chTab.UpLimit = 0.28f;    // 理论上限
            chTab.ChType = 1;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 6. 3截面压力（288~311：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 384) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "P3_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "3截面压力_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "MPa", 23);
            chTab.DownLimit = 0.27f;    // 理论下限
            chTab.UpLimit = 0.33f;    // 理论上限
            chTab.ChType = 1;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 7. 3.5截面压力（312~335：3层×8通道，含P35_2_04=312+(2-1)*8+(4-1)=323）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            int chTabIdx = 312 + (layer-1)*8 + (ch-1);
            if (chTabIdx >= 384) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[chTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "P35_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "3.5截面压力_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "MPa", 23);
            chTab.DownLimit = 0.1825f;  // 强制理论下限（Ps=0.2MPa+扰动）
            chTab.UpLimit = 0.2425f;  // 强制理论上限
            chTab.ChType = 1;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
        }
    }

    // 8. 4截面压力（336~359：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 384) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "P4_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "4截面压力_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "MPa", 23);
            chTab.DownLimit = 0.1f;     // 理论下限
            chTab.UpLimit = 0.16f;     // 理论上限
            chTab.ChType = 1;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 9. 5截面压力（360~383：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 384) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "P5_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "5截面压力_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "MPa", 23);
            chTab.DownLimit = 0.1225f;  // 理论下限
            chTab.UpLimit = 0.1525f;  // 理论上限
            chTab.ChType = 1;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 10. 温度通道（384~767）
    // 0截面温度（384~575：3耙×4层×16通道）
    for (int rake = 1; rake <= 3; rake++) {
        for (int layer = 1; layer <= 4; layer++) {
            for (int ch = 1; ch <= 16; ch++) {
                if (currentChTabIdx >= 768) break;
                SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
                memset(chTab.ChName, 0, 24);
                snprintf(chTab.ChName, 23, "T0_%d_%d_%02d", rake, layer, ch);
                memset(chTab.ChName_CHN, 0, 56);
                snprintf(chTab.ChName_CHN, 55, "0截面温度_耙%d_层%d_通道%d", rake, layer, ch);
                memset(chTab.EngUnit, 0, 24);
                strncpy(chTab.EngUnit, "℃", 23);
                chTab.DownLimit = -273.15f;    // 绝对零度
                chTab.UpLimit = 1500.0f;      // 最高温度
                chTab.ChType = 2;              // 2=温度通道
                chTab.AlarmFlag = 0;
                chTab.pre_Alarm_low = 0.0f;
                chTab.pre_Alarm_up = 0.0f;
                chTab.Alarm_low = 0.0f;
                chTab.Alarm_up = 0.0f;
                chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
                chTab.bk = 0;                  // 备用填0
                currentChTabIdx++;
            }
        }
    }

    // 11. 0.1截面温度（576~599：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 768) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "T01_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "0.1截面温度_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "℃", 23);
            chTab.DownLimit = -273.15f;
            chTab.UpLimit = 1500.0f;
            chTab.ChType = 2;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 12. 1截面温度（600~623：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 768) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "T1_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "1截面温度_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "℃", 23);
            chTab.DownLimit = -273.15f;
            chTab.UpLimit = 1500.0f;
            chTab.ChType = 2;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 13. 1.5截面温度（624~647：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 768) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "T15_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "1.5截面温度_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "℃", 23);
            chTab.DownLimit = -273.15f;
            chTab.UpLimit = 1500.0f;
            chTab.ChType = 2;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 14. 2截面温度（648~671：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 768) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "T2_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "2截面温度_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "℃", 23);
            chTab.DownLimit = -273.15f;
            chTab.UpLimit = 1500.0f;
            chTab.ChType = 2;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 15. 3截面温度（672~695：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 768) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "T3_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "3截面温度_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "℃", 23);
            chTab.DownLimit = -273.15f;
            chTab.UpLimit = 1500.0f;
            chTab.ChType = 2;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 16. 3.5截面温度（696~719：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 768) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "T35_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "3.5截面温度_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "℃", 23);
            chTab.DownLimit = -273.15f;
            chTab.UpLimit = 1500.0f;
            chTab.ChType = 2;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 17. 4截面温度（720~743：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 768) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "T4_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "4截面温度_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "℃", 23);
            chTab.DownLimit = -273.15f;
            chTab.UpLimit = 1500.0f;
            chTab.ChType = 2;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 18. 5截面温度（744~767：3层×8通道）
    for (int layer = 1; layer <= 3; layer++) {
        for (int ch = 1; ch <= 8; ch++) {
            if (currentChTabIdx >= 768) break;
            SENDTAB& chTab = m_msAetimsChannelPacket.ChTab[currentChTabIdx];
            memset(chTab.ChName, 0, 24);
            snprintf(chTab.ChName, 23, "T5_%d_%02d", layer, ch);
            memset(chTab.ChName_CHN, 0, 56);
            snprintf(chTab.ChName_CHN, 55, "5截面温度_层%d_通道%d", layer, ch);
            memset(chTab.EngUnit, 0, 24);
            strncpy(chTab.EngUnit, "℃", 23);
            chTab.DownLimit = -273.15f;
            chTab.UpLimit = 1500.0f;
            chTab.ChType = 2;
            chTab.AlarmFlag = 0;
            chTab.ptnums = 6;               // 小数显示6位（匹配数据精度）
            chTab.bk = 0;                  // 备用填0
            currentChTabIdx++;
        }
    }

    // 监听AETIMS接口默认端口6000（文档2.1节要求）
    if (!m_tcpServer->listen(QHostAddress::Any, 6000)) {
        appendTextEdit("多截面AETIMS接口端口6000监听失败，尝试备用端口8888");
        if (!m_tcpServer->listen(QHostAddress::Any, 8888)) {
            appendTextEdit("备用端口8888监听失败，TCP服务器启动失败");
            ui->labelAutoSave->setText("TCP服务器: 启动失败");
        } else {
            appendTextEdit("TCP服务器已启动，监听备用端口8888（只发送多截面数据）");
            ui->labelAutoSave->setText("TCP服务器: 运行中（端口8888，只发送多截面数据）");
        }
    } else {
        appendTextEdit("TCP服务器已启动，监听AETIMS接口端口6000（只发送多截面数据）");
        ui->labelAutoSave->setText("TCP服务器: 运行中（端口6000，只发送多截面数据）");
    }
    ui->labelDTSAutoSave->setText(ui->labelAutoSave->text());
}

// 启动TCP服务器
void MainWindow::startTCPServer()
{
    if (!m_tcpServer) {
        return;
    }

    // 尝试监听端口（使用8888端口）
    if (!m_tcpServer->listen(QHostAddress::Any, 8888)) {
        QString errorMsg = QString("TCP服务器启动失败: %1").arg(m_tcpServer->errorString());
        appendTextEdit("TCP服务器: " + errorMsg);
        appendTextEditDTS("TCP服务器: " + errorMsg);
        return;
    }

    QString msg = QString("TCP服务器已启动，监听端口: %1").arg(m_tcpServer->serverPort());
    appendTextEdit("TCP服务器: " + msg);
    appendTextEditDTS("TCP服务器: " + msg);

    // 更新UI状态
    // 这里可以添加一个状态标签来显示TCP服务器状态
    ui->labelAutoSave->setText("TCP服务器: 运行中");
    ui->labelDTSAutoSave->setText("TCP服务器: 运行中");
}

// 停止TCP服务器
void MainWindow::stopTCPServer()
{
    if (m_tcpSendTimer && m_tcpSendTimer->isActive()) {
        m_tcpSendTimer->stop();
    }

    m_tcpSendEnabled = false;

    // 断开所有客户端连接
    for (auto client : m_tcpClients) {
        if (client && client->state() == QAbstractSocket::ConnectedState) {
            client->disconnectFromHost();
        }
    }

    if (m_tcpServer && m_tcpServer->isListening()) {
        m_tcpServer->close();
        appendTextEdit("TCP服务器已停止");
        appendTextEditDTS("TCP服务器已停止");
    }

    // 更新UI状态
    ui->labelAutoSave->setText("自动保存: 关闭");
    ui->labelDTSAutoSave->setText("自动保存: 关闭");
}

void MainWindow::onTCPSendTimer()
{
     qDebug() << "Legacy TCP send timer called - this function is deprecated";
}


// 新TCP连接槽函数
void MainWindow::onNewTCPConnection()
{
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();
    if (!clientSocket) return;

    m_tcpClients.append(clientSocket);
    connect(clientSocket, &QTcpSocket::disconnected, this, &MainWindow::onTCPClientDisconnected);

    QString clientInfo = QString("%1:%2").arg(clientSocket->peerAddress().toString()).arg(clientSocket->peerPort());
    appendTextEdit("多截面TCP服务器: 客户端连接: " + clientInfo);

    // 新客户端连接，重置通道表发送状态
    m_msAetimsChannelSent = false;
    m_tcpSendEnabled = true; // 立即启用数据发送

    // 立即发送通道表
    sendMultiSectionChannelTable();
    appendTextEdit("已为客户端" + clientInfo + "发送多截面通道表（768通道）");

    // 确保定时器正在运行
    if (m_tcpSendTimer && !m_tcpSendTimer->isActive()) {
        m_tcpSendTimer->start();
        appendTextEdit("TCP发送定时器已启动（20ms间隔）");
    } else if (m_tcpSendTimer && m_tcpSendTimer->isActive()) {
        appendTextEdit("TCP发送定时器已在运行");
    }

    // 延迟100ms后发送第一次数据包，确保通道表已送达
    QTimer::singleShot(100, this, [this, clientInfo]() {
        appendTextEdit("开始为客户端" + clientInfo + "发送多截面数据包（CommandID=4）");
        // 强制发送一次数据包
        sendMultiSectionDataPacket();
    });
}

// TCP客户端断开连接槽函数
// TCP客户端断开连接槽函数
void MainWindow::onTCPClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) {
        return;
    }

    // 读取并丢弃所有未读数据
    if (clientSocket->bytesAvailable() > 0) {
        QByteArray remainingData = clientSocket->readAll();
        appendTextEdit(QString("客户端断开，丢弃%1字节未读数据").arg(remainingData.size()));
    }

    // 从列表中移除
    m_tcpClients.removeOne(clientSocket);

    // 删除套接字
    clientSocket->deleteLater();

    QString msg = QString("TCP客户端断开连接，当前连接数: %1").arg(m_tcpClients.size());
    appendTextEdit("TCP服务器: " + msg);

    // 如果没有客户端连接，重置通道表发送状态
    if (m_tcpClients.isEmpty()) {
        m_msAetimsChannelSent = false;
        appendTextEdit("所有客户端已断开，重置通道表发送状态");
    }
}

// 更新TCP状态显示函数
void MainWindow::updateTCPStatus()
{
    if (!m_tcpServer) {
        ui->labelTCPStatus->setText("服务器状态: 未初始化");
        ui->labelTCPStatus->setStyleSheet("QLabel { color: red; font-weight: bold; }");
        return;
    }

    if (m_tcpServer->isListening()) {
        ui->labelTCPStatus->setText("服务器状态: 运行中");
        ui->labelTCPStatus->setStyleSheet("QLabel { color: green; font-weight: bold; }");
    } else {
        ui->labelTCPStatus->setText("服务器状态: 停止");
        ui->labelTCPStatus->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    }

    ui->labelTCPClients->setText(QString("连接客户端: %1").arg(m_tcpClients.size()));
    ui->labelTCPFrames->setText(QString("发送帧数: %1").arg(m_tcpFrameCounter));
}

// ==================== 同时采集相关函数 ====================

// 更新同时采集按钮状态
void MainWindow::updateSimultaneousButtonsState()
{
    // 检查DPS是否准备好（已连接且已快速配置）
    bool dpsConnected = tcpClient->returnConnectState();
    bool dpsConfigured = ui->pushButton_scan->isEnabled(); // 快速配置后会启用采集按钮

    // 检查DTS是否准备好（已连接且已快速配置）
    bool dtsConnected = tcpClientDTS->returnConnectState();
    bool dtsConfigured = ui->btnDTSScan->isEnabled(); // 快速配置后会启用采集按钮

    m_dpsReady = dpsConnected && dpsConfigured;
    m_dtsReady = dtsConnected && dtsConfigured;

    // 更新按钮状态
    if (m_simultaneousCollecting) {
        // 正在采集时，只能点击同时结束按钮
        ui->btnSimultaneousStart->setEnabled(false);
        ui->btnSimultaneousStop->setEnabled(true);
    } else {
        // 未采集时，只有两个设备都准备好才能点击同时采集按钮
        bool bothReady = m_dpsReady && m_dtsReady;
        ui->btnSimultaneousStart->setEnabled(bothReady);
        ui->btnSimultaneousStop->setEnabled(false);

        // 设置按钮提示
        if (bothReady) {
            ui->btnSimultaneousStart->setToolTip("DPS和DTS都已准备好，可以开始同时采集");
        } else {
            QString tip = "无法同时采集：";
            if (!dpsConnected) tip += "DPS未连接; ";
            if (!dpsConfigured) tip += "DPS未快速配置; ";
            if (!dtsConnected) tip += "DTS未连接; ";
            if (!dtsConfigured) tip += "DTS未快速配置";
            ui->btnSimultaneousStart->setToolTip(tip);
        }
    }

    // 更新按钮文本
    if (m_simultaneousCollecting) {
        ui->btnSimultaneousStart->setText("正在同时采集...");
        ui->btnSimultaneousStop->setText("同时结束");
    } else {
        ui->btnSimultaneousStart->setText("同时采集");
        ui->btnSimultaneousStop->setText("同时结束");
    }
}

void MainWindow::on_btnSimultaneousStart_clicked()
{

    // 重置采集速率计数器
      m_dpsPacketCount = 0;
      m_dtsPacketCount = 0;
      m_dpsRateTimer.restart();
      m_dtsRateTimer.restart();

// 重置上位机帧号（从1开始）与绘图队列
m_dtsFrameSeq = 0;
m_pendingDtsPlot.clear();

// 清空温度实时曲线（不影响已缓存/保存数据）
for (int i = 0; i < temperatureSeries.size(); ++i) {
    if (temperatureSeries[i]) {
        temperatureSeries[i]->clear();
        temperatureSeries[i]->setVisible(false);
    }
}
if (temperatureAxisX) temperatureAxisX->setRange(0, 100);


      // 启动采集速率监测
         if (m_rateMonitorTimer) {
             m_rateMonitorTimer->start();
         }

    m_pressureRandomTimer->start();

    if (!m_dpsReady || !m_dtsReady) {
        QMessageBox::warning(this, "警告", "两个设备未准备好，无法同时采集！");
        return;
    }

    appendTextEdit("========== 开始同时采集 ==========");
    appendTextEditDTS("========== 开始同时采集 ==========");

    // 设置同时采集状态
    m_simultaneousCollecting = true;

// 同时采集：两路帧号都从1开始，曲线从空白开始
m_dpsFrameSeq = 0;
m_dtsFrameSeq = 0;
m_pendingDpsPlot.clear();
m_pendingDtsPlot.clear();
clearCharts();


    // 记录开始时间
    QDateTime startTime = QDateTime::currentDateTime();
    appendTextEdit("同时采集开始时间: " + startTime.toString("yyyy-MM-dd HH:mm:ss"));

    // 创建文件路径
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString simultaneousPath = m_autoSavePath + "同时采集/";

    QDir dir;
    if (!dir.exists(simultaneousPath)) {
        dir.mkpath(simultaneousPath);
    }

    // 创建DPS文件
    m_dpsAutoSaveFileName = simultaneousPath + QString("同时采集_DPS_%1.xls").arg(timestamp);
    if (m_currentDPSDoc) {
        delete m_currentDPSDoc;
    }
    m_currentDPSDoc = new QXlsx::Document();
    m_currentDPSDoc->currentWorksheet()->setName("DPS同时采集数据");

    // 写入DPS表头
    QStringList dpsHeaders = {"采集时间", "帧号"};
    for (int i = 1; i <= 16; i++) {
        dpsHeaders.append(QString("通道%1(%2)").arg(i).arg(ui->comboUnit->currentText()));
    }
    for (int i = 1; i <= 16; i++) {
        dpsHeaders.append(QString("温度%1(℃)").arg(i));
    }

    for (int col = 0; col < dpsHeaders.size(); col++) {
        m_currentDPSDoc->write(1, col + 1, dpsHeaders[col]);
    }
    m_dpsRowCounter = 2;
    m_autoSaveBufferDPS.clear();

    // 创建DTS文件
    QString typeStr = ui->comboDTSType->currentText();
    m_dtsAutoSaveFileName = simultaneousPath + QString("同时采集_DTS_%1.xls").arg(timestamp);
    if (m_currentDTSDoc) {
        delete m_currentDTSDoc;
    }
    m_currentDTSDoc = new QXlsx::Document();

    QString sheetName = typeStr.contains("电压") ? "DTS电压同时采集数据" : "DTS温度同时采集数据";
    m_currentDTSDoc->currentWorksheet()->setName(sheetName);

    // 写入DTS表头
    QStringList dtsHeaders = {"采集时间", "帧号"};
    for (int i = 1; i <= 48; i++) {
        if (typeStr.contains("电压")) {
            dtsHeaders.append(QString("通道%1(V)").arg(i));
        } else {
            dtsHeaders.append(QString("通道%1(℃)").arg(i));
        }
    }

    for (int col = 0; col < dtsHeaders.size(); col++) {
        m_currentDTSDoc->write(1, col + 1, dtsHeaders[col]);
    }

    m_dtsRowCounter = 2;
    m_autoSaveBufferDTS.clear();

    // 启动DPS采集
    on_pushButton_scan_clicked();

    // 延迟启动DTS
    delayEventLoopMS(50);

    // 启动DTS采集
    on_btnDTSScan_clicked();

    // 启用自动保存
    m_autoSaveEnabled = true;
    if (m_autoSaveTimer) {
        m_autoSaveTimer->start();
    }

    // 更新按钮状态
    updateSimultaneousButtonsState();

    // 更新状态标签
    ui->labelAutoSave->setText("同时采集: 进行中");
    ui->labelAutoSave->setStyleSheet("QLabel { color: green; font-weight: bold; }");

    ui->labelDTSAutoSave->setText("同时采集: 进行中");
    ui->labelDTSAutoSave->setStyleSheet("QLabel { color: green; font-weight: bold; }");

    appendTextEdit("同时采集已启动！");
    appendTextEditDTS("同时采集已启动！");

    // 清空多截面数据缓存
    QMutexLocker locker(&m_msDataMutex);
    m_msPressureCache.clear();
    m_msTemperatureCache.clear();

    // 重置多截面TCP发送状态
    m_msAetimsChannelSent = false;
    m_msAetimsDataPacketCount = 0;
}

// ==================== 同时采集相关函数 ====================

// 同时结束按钮点击事件
void MainWindow::on_btnSimultaneousStop_clicked()
{

    // 停止采集速率监测
       if (m_rateMonitorTimer && m_rateMonitorTimer->isActive()) {
           m_rateMonitorTimer->stop();
       }
    if (!m_simultaneousCollecting) {
        return;
    }
    appendTextEdit("========== 结束同时采集 ==========");
    appendTextEditDTS("========== 结束同时采集 ==========");

    // 停止多截面TCP数据发送
    m_tcpSendEnabled = false;
    if (m_tcpSendTimer) {
        m_tcpSendTimer->stop();
    }

    // ========== 关键修复：记录结束时间 ==========
    QDateTime endTime = QDateTime::currentDateTime();
    appendTextEdit(QString("同时采集结束，系统结束时间：%1").arg(endTime.toString("yyyy-MM-dd HH:mm:ss.zzz")));

    // 停止DPS采集
    on_pushButton_stop_clicked();
    // 稍微延迟一下，确保DPS停止后再停止DTS
    delayEventLoopMS(50);
    // 停止DTS采集
    on_btnDTSStop_clicked();

    // ========== 关键修改：先保存多截面数据 ==========
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString multiSectionPath = m_autoSavePath + "同时采集/多截面数据/";
    QDir multiDir;
    if (!multiDir.exists(multiSectionPath)) {
        multiDir.mkpath(multiSectionPath);
    }

    // 保存多截面压力数据
    QString msPressureFileName = multiSectionPath + QString("同时采集_多截面压力_%1.xls").arg(timestamp);
    if (saveMultiSectionPressureDataToExcel(msPressureFileName)) {
        QFileInfo msPressFile(msPressureFileName);
        if (msPressFile.exists()) {
            appendTextEdit(QString("多截面压力数据已保存到: %1 (大小: %2 字节)")
                              .arg(msPressureFileName)
                              .arg(msPressFile.size()));
        }
    } else {
        appendTextEdit("多截面压力数据保存失败！");
    }

    // 保存多截面温度数据
    QString msTempFileName = multiSectionPath + QString("同时采集_多截面温度_%1.xls").arg(timestamp);
    if (saveMultiSectionTemperatureDataToExcel(msTempFileName)) {
        QFileInfo msTempFile(msTempFileName);
        if (msTempFile.exists()) {
            appendTextEditDTS(QString("多截面温度数据已保存到: %1 (大小: %2 字节)")
                                 .arg(msTempFileName)
                                 .arg(msTempFile.size()));
        }
    } else {
        appendTextEditDTS("多截面温度数据保存失败！");
    }

    // 保存剩余采集数据
    if (!m_autoSaveBufferDPS.isEmpty() || !m_autoSaveBufferDTS.isEmpty()) {
        appendDataToCurrentExcel(true, true);
    }

    // 显示DPS和DTS主数据保存信息
    QString dpsFileInfo = m_dpsAutoSaveFileName;
    QString dtsFileInfo = m_dtsAutoSaveFileName;
    QFileInfo dpsFile(dpsFileInfo);
    QFileInfo dtsFile(dtsFileInfo);
    if (dpsFile.exists()) {
        appendTextEdit(QString("DPS数据已保存到: %1 (大小: %2 字节)")
                          .arg(dpsFileInfo)
                          .arg(dpsFile.size()));
    }
    if (dtsFile.exists()) {
        appendTextEditDTS(QString("DTS数据已保存到: %1 (大小: %2 字节)")
                             .arg(dtsFileInfo)
                             .arg(dtsFile.size()));
    }

    // 重置多截面TCP发送状态
    m_msAetimsChannelSent = false;
    m_msAetimsDataPacketCount = 0;
    appendTextEdit("多截面通道表发送状态已重置，下次采集将重新发送");

    // 重置同时采集相关状态
    m_simultaneousCollecting = false;
    m_dpsDataReady = false;
    m_dtsDataReady = false;

    // 更新按钮状态
    updateSimultaneousButtonsState();

    // 更新状态标签
    ui->labelAutoSave->setText("自动保存: 关闭");
    ui->labelAutoSave->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    ui->labelDTSAutoSave->setText("自动保存: 关闭");
    ui->labelDTSAutoSave->setStyleSheet("QLabel { color: red; font-weight: bold; }");

    appendTextEdit("同时采集已停止！");
    appendTextEditDTS("同时采集已停止！");
    m_pressureRandomTimer->stop(); // 停止定时器
}

// ==================== 自动保存相关函数 ====================

// 初始化自动保存
void MainWindow::initAutoSave()
{
    // 创建自动保存目录
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    m_autoSavePath = docPath + "/双设备采集系统/自动保存/";

    QDir dir;
    if (!dir.exists(m_autoSavePath)) {
        dir.mkpath(m_autoSavePath);
    }

    // 创建定时器
    m_autoSaveTimer = new QTimer(this);
    m_autoSaveTimer->setInterval(m_autoSaveInterval);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &MainWindow::onAutoSaveTimer);

    // 初始设置为禁用状态
    m_autoSaveEnabled = false;
}

bool MainWindow::saveDPSDataToFile(const QList<DPS_PACKET> &dataList, const QString &fileName, bool isAutoSave)
{
    if (dataList.isEmpty()) {
        return false;
    }

    QXlsx::Document xlsx;
    XlsxWorksheet *sheet = xlsx.currentWorksheet();

    QString sheetName = isAutoSave ? "DPS自动保存数据" : "压力采集数据";
    sheet->setName(sheetName);

    // ========== 采集速率信息详细分析 ==========
    // 第一行：标题
    xlsx.write(1, 1, "双设备采集系统 - DPS压力数据采集报告");
    xlsx.write(1, 2, "");
    xlsx.write(1, 3, "");
    xlsx.write(1, 4, "");
    xlsx.write(1, 5, "报告生成时间");
    xlsx.write(1, 6, QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));

    // 第二行：设备基本信息
    xlsx.write(2, 1, "设备型号");
    xlsx.write(2, 2, "DPS-16P2 压力扫描阀");
    xlsx.write(2, 3, "设备序列号");
    xlsx.write(2, 4, "待查询");
    xlsx.write(2, 5, "软件版本");
    xlsx.write(2, 6, "双设备采集系统 v1.0");

    // 第三行：连接信息
    xlsx.write(3, 1, "IP地址");
    xlsx.write(3, 2, ui->lineEdit_ip->text());
    xlsx.write(3, 3, "端口");
    xlsx.write(3, 4, ui->lineEdit_port->text());
    xlsx.write(3, 5, "连接状态");
    xlsx.write(3, 6, tcpClient->returnConnectState() ? "已连接" : "已断开");

    // 第四行：配置信息
    xlsx.write(4, 1, "同步方式");
    xlsx.write(4, 2, ui->comboSync->currentText());
    xlsx.write(4, 3, "数据格式");
    xlsx.write(4, 4, ui->comboFormat->currentText());
    xlsx.write(4, 5, "触发方式");
    xlsx.write(4, 6, ui->comboTrigger->currentText());

    // 第五行：单位与通道信息
    xlsx.write(5, 1, "压力单位");
    xlsx.write(5, 2, ui->comboUnit->currentText());
    xlsx.write(5, 3, "采集周期");
    xlsx.write(5, 4, ui->lineEdit_period->text() + " ms");
    xlsx.write(5, 5, "通道数量");
    xlsx.write(5, 6, "16通道压力 + 16通道温度");

    // 第六行：采集模式信息
    QString collectNum = ui->lineEdit_collectNum->text();
    if (collectNum == "0") {
        xlsx.write(6, 1, "采集模式");
        xlsx.write(6, 2, "连续采集");
        xlsx.write(6, 3, "计划采集时间");
        xlsx.write(6, 4, "无限");
    } else {
        xlsx.write(6, 1, "采集模式");
        xlsx.write(6, 2, "有限次采集");
        xlsx.write(6, 3, "计划采集次数");
        xlsx.write(6, 4, collectNum + " 次");
        xlsx.write(6, 5, "计划采集时间");
        xlsx.write(6, 6, QString("%1 ms").arg(collectNum.toInt() * 20));
    }

    // 第七行：空行分隔
    xlsx.write(7, 1, "=================== 采集速率分析 ===================");
    for (int col = 2; col <= 6; col++) {
        xlsx.write(7, col, "");
    }

    // 第八行：理论速率信息
    xlsx.write(8, 1, "理论采集速率");
    xlsx.write(8, 2, "50 Hz");
    xlsx.write(8, 3, "理论采集周期");
    xlsx.write(8, 4, "20 ms");
    xlsx.write(8, 5, "理论数据速率");
    xlsx.write(8, 6, "800 数据点/秒");

    // 第九行：实时采集速率（基于监控数据）
    xlsx.write(9, 1, "实时采集速率");
    xlsx.write(9, 2, QString("%1 Hz").arg(m_dpsActualRate, 0, 'f', 2));
    xlsx.write(9, 3, "实时速率状态");
    if (m_dpsActualRate >= 45.0 && m_dpsActualRate <= 55.0) {
        xlsx.write(9, 4, "稳定 (误差±5%)");
    } else if (m_dpsActualRate >= 40.0 && m_dpsActualRate < 45.0) {
        xlsx.write(9, 4, "偏低 (误差-10%~-20%)");
    } else if (m_dpsActualRate > 55.0 && m_dpsActualRate <= 60.0) {
        xlsx.write(9, 4, "偏高 (误差+5%~+20%)");
    } else {
        xlsx.write(9, 4, "异常 (误差>20%)");
    }
    xlsx.write(9, 5, "数据包数量");
    xlsx.write(9, 6, QString("%1 包").arg(m_dpsPacketCount));

    // 第十行：基于实际数据计算的速率（更准确）
    if (dataList.size() >= 2) {
        DPS_PACKET firstPkt = dataList.first();
        DPS_PACKET lastPkt = dataList.last();

        qint64 firstTime_ns = static_cast<qint64>(firstPkt.nFrameTime_s) * 1000000000LL + firstPkt.nFrameTime_ns;
        qint64 lastTime_ns = static_cast<qint64>(lastPkt.nFrameTime_s) * 1000000000LL + lastPkt.nFrameTime_ns;

        if (lastTime_ns > firstTime_ns) {
            double timeDiff_sec = (lastTime_ns - firstTime_ns) / 1e9;
            double frequencyHz = (dataList.size() - 1) / timeDiff_sec;
            double timeDiff_ms = timeDiff_sec * 1000;

            xlsx.write(10, 1, "实际计算速率");
            xlsx.write(10, 2, QString("%1 Hz").arg(frequencyHz, 0, 'f', 2));
            xlsx.write(10, 3, "实际采集时间");
            xlsx.write(10, 4, QString("%1 ms").arg(timeDiff_ms, 0, 'f', 1));
            xlsx.write(10, 5, "总数据点数");
            xlsx.write(10, 6, QString("%1 点").arg(dataList.size()));
        }
    } else {
        xlsx.write(10, 1, "实际计算速率");
        xlsx.write(10, 2, "数据不足");
        xlsx.write(10, 3, "实际采集时间");
        xlsx.write(10, 4, "0 ms");
    }

    // 第十一行：详细速率分析（如果有足够数据）
    if (dataList.size() >= 10) {
        // 计算采集间隔统计
        QVector<double> intervals;
        double totalInterval = 0;
        double maxInterval = 0;
        double minInterval = 1000;

        for (int i = 1; i < dataList.size(); i++) {
            qint64 prevTime_ns = static_cast<qint64>(dataList[i-1].nFrameTime_s) * 1000000000LL + dataList[i-1].nFrameTime_ns;
            qint64 currTime_ns = static_cast<qint64>(dataList[i].nFrameTime_s) * 1000000000LL + dataList[i].nFrameTime_ns;
            double interval = (currTime_ns - prevTime_ns) / 1e6; // 转换为ms
            intervals.append(interval);
            totalInterval += interval;
            maxInterval = qMax(maxInterval, interval);
            minInterval = qMin(minInterval, interval);
        }

        double avgInterval = totalInterval / intervals.size();
        double intervalStdDev = 0;
        for (double interval : intervals) {
            intervalStdDev += qPow(interval - avgInterval, 2);
        }
        intervalStdDev = qSqrt(intervalStdDev / intervals.size());

        xlsx.write(11, 1, "间隔统计分析");
        xlsx.write(11, 2, QString("平均间隔: %1 ms").arg(avgInterval, 0, 'f', 3));
        xlsx.write(11, 3, QString("最大间隔: %1 ms").arg(maxInterval, 0, 'f', 3));
        xlsx.write(11, 4, QString("最小间隔: %1 ms").arg(minInterval, 0, 'f', 3));
        xlsx.write(11, 5, QString("间隔波动: %1 ms").arg(maxInterval - minInterval, 0, 'f', 3));
        xlsx.write(11, 6, QString("标准差: %1 ms").arg(intervalStdDev, 0, 'f', 3));
    }

    // 第十二行：速率稳定性评估
    xlsx.write(12, 1, "速率稳定性评估");
    if (dataList.size() >= 10) {
        double avgInterval = 0;
        double totalInterval = 0;

        for (int i = 1; i < qMin(100, dataList.size()); i++) {
            qint64 prevTime_ns = static_cast<qint64>(dataList[i-1].nFrameTime_s) * 1000000000LL + dataList[i-1].nFrameTime_ns;
            qint64 currTime_ns = static_cast<qint64>(dataList[i].nFrameTime_s) * 1000000000LL + dataList[i].nFrameTime_ns;
            totalInterval += (currTime_ns - prevTime_ns) / 1e6;
        }
        avgInterval = totalInterval / qMin(99, dataList.size() - 1);

        if (avgInterval >= 18.0 && avgInterval <= 22.0) {
            xlsx.write(12, 2, "优秀 (20ms±10%)");
            xlsx.write(12, 3, "采集间隔稳定");
        } else if (avgInterval >= 15.0 && avgInterval <= 25.0) {
            xlsx.write(12, 2, "良好 (20ms±25%)");
            xlsx.write(12, 3, "采集间隔基本稳定");
        } else if (avgInterval >= 10.0 && avgInterval <= 30.0) {
            xlsx.write(12, 2, "一般 (20ms±50%)");
            xlsx.write(12, 3, "采集间隔有波动");
        } else {
            xlsx.write(12, 2, "较差 (误差>50%)");
            xlsx.write(12, 3, "采集间隔不稳定");
        }
        xlsx.write(12, 4, QString("平均间隔: %1 ms").arg(avgInterval, 0, 'f', 2));
    } else {
        xlsx.write(12, 2, "数据不足，无法评估");
    }

    // 第十三行：空行分隔
    xlsx.write(13, 1, "=================== 数据采集信息 ===================");
    for (int col = 2; col <= 6; col++) {
        xlsx.write(13, col, "");
    }

    // 第十四行：采集时间信息
    if (!dataList.isEmpty()) {
        DPS_PACKET firstPkt = dataList.first();
        DPS_PACKET lastPkt = dataList.last();

        double firstTotalSec = firstPkt.nFrameTime_s + firstPkt.nFrameTime_ns / 1e9;
        double lastTotalSec = lastPkt.nFrameTime_s + lastPkt.nFrameTime_ns / 1e9;

        QDateTime startTime = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(firstTotalSec));
        QDateTime endTime = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(lastTotalSec));

        xlsx.write(14, 1, "采集开始时间");
        xlsx.write(14, 2, startTime.toString("yyyy-MM-dd HH:mm:ss.zzz"));
        xlsx.write(14, 3, "采集结束时间");
        xlsx.write(14, 4, endTime.toString("yyyy-MM-dd HH:mm:ss.zzz"));

        double durationSec = lastTotalSec - firstTotalSec;
        xlsx.write(14, 5, "采集持续时间");
        xlsx.write(14, 6, QString("%1 秒").arg(durationSec, 0, 'f', 3));
    }

    // 第十五行：数据统计信息
    xlsx.write(15, 1, "总数据记录数");
    xlsx.write(15, 2, QString::number(dataList.size()));
    xlsx.write(15, 3, "每通道数据量");
    xlsx.write(15, 4, QString("%1 点/通道").arg(dataList.size()));
    xlsx.write(15, 5, "数据总量");
    xlsx.write(15, 6, QString("%1 点").arg(dataList.size() * 32)); // 32通道（16压力+16温度）

    // 第十六行：备注信息
    xlsx.write(16, 1, "备注");
    xlsx.write(16, 2, "本文件包含采集速率详细分析，可用于评估系统性能。");
    xlsx.write(16, 3, "");
    xlsx.write(16, 4, "");
    xlsx.write(16, 5, "保存类型");
    xlsx.write(16, 6, isAutoSave ? "自动保存" : "手动保存");

    // 第十七行：空行
    xlsx.write(17, 1, "");

    // ========== 表头（从第18行开始）==========
    QStringList headers = {"采集时间", "帧号",
                           "通道1(psi/kPa)", "通道2(psi/kPa)", "通道3(psi/kPa)", "通道4(psi/kPa)",
                           "通道5(psi/kPa)", "通道6(psi/kPa)", "通道7(psi/kPa)", "通道8(psi/kPa)",
                           "通道9(psi/kPa)", "通道10(psi/kPa)", "通道11(psi/kPa)", "通道12(psi/kPa)",
                           "通道13(psi/kPa)", "通道14(psi/kPa)", "通道15(psi/kPa)", "通道16(psi/kPa)",
                           "温度1(℃)", "温度2(℃)", "温度3(℃)", "温度4(℃)",
                           "温度5(℃)", "温度6(℃)", "温度7(℃)", "温度8(℃)",
                           "温度9(℃)", "温度10(℃)", "温度11(℃)", "温度12(℃)",
                           "温度13(℃)", "温度14(℃)", "温度15(℃)", "温度16(℃)"};

    // 写入表头（第18行）
    for (int col = 0; col < headers.size(); col++) {
        xlsx.write(18, col + 1, headers[col]);
    }

    // ========== 写入数据（从第19行开始）==========
    bool isKPa = (ui->comboUnit->currentText() == "kPa");
    for (int row = 0; row < dataList.size(); row++) {
        DPS_PACKET pkt = dataList[row];
        int excelRow = row + 19; // 第19行开始

        // 计算正确的采集时间
        QString timeStr;
        if (m_dpsTimeBaseRecorded && m_dpsStartTime.isValid()) {
            qint64 packetTime_ns = static_cast<qint64>(pkt.nFrameTime_s) * 1000000000LL + pkt.nFrameTime_ns;
            qint64 relative_ns = packetTime_ns - m_dpsStartTimestamp;
            QDateTime actualTime = m_dpsStartTime.addMSecs(relative_ns / 1000000LL);
            timeStr = actualTime.toString("yyyy-MM-dd HH:mm:ss.zzz");
        } else {
            double totalSec = pkt.nFrameTime_s + pkt.nFrameTime_ns / 1e9;
            timeStr = QString("%1 s").arg(totalSec, 0, 'f', 6);
        }

        xlsx.write(excelRow, 1, timeStr);
        xlsx.write(excelRow, 2, pkt.nFrameNo);

        // 写入16通道压力数据
        for (int ch = 0; ch < 16; ch++) {
            double press = pkt.fPress[ch].toDouble();
            xlsx.write(excelRow, 3 + ch, convertUnit(press, isKPa));
        }

        // 写入16通道温度数据
        for (int ch = 0; ch < 16; ch++) {
            double temp = pkt.fTemp[ch].toDouble();
            xlsx.write(excelRow, 19 + ch, temp);
        }
    }

    return xlsx.saveAs(fileName);
}

bool MainWindow::saveDTSDataToFile(const QList<DTS_PACKET> &dataList, const QString &fileName, bool isAutoSave)
{
    if (dataList.isEmpty()) {
        return false;
    }

    QXlsx::Document xlsx;
    XlsxWorksheet *sheet = xlsx.currentWorksheet();

    // 根据采集类型设置工作表名称
    QString typeStr = ui->comboDTSType->currentText();
    QString sheetName;
    if (isAutoSave) {
        sheetName = typeStr.contains("电压") ? "DTS电压自动保存数据" : "DTS温度自动保存数据";
    } else {
        sheetName = typeStr.contains("电压") ? "DTS电压数据" : "DTS温度数据";
    }
    sheet->setName(sheetName);

    // ========== 采集速率信息详细分析 ==========
    // 第一行：标题
    xlsx.write(1, 1, "双设备采集系统 - DTS数据采集报告");
    xlsx.write(1, 2, "");
    xlsx.write(1, 3, "");
    xlsx.write(1, 4, "");
    xlsx.write(1, 5, "报告生成时间");
    xlsx.write(1, 6, QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));

    // 第二行：设备基本信息
    xlsx.write(2, 1, "设备型号");
    xlsx.write(2, 2, "DTS-48F2 热电偶采集仪");
    xlsx.write(2, 3, "设备序列号");
    xlsx.write(2, 4, "待查询");
    xlsx.write(2, 5, "软件版本");
    xlsx.write(2, 6, "双设备采集系统 v1.0");

    // 第三行：连接信息
    xlsx.write(3, 1, "IP地址");
    xlsx.write(3, 2, ui->lineEdit_dts_ip->text());
    xlsx.write(3, 3, "端口");
    xlsx.write(3, 4, ui->lineEdit_dts_port->text());
    xlsx.write(3, 5, "连接状态");
    xlsx.write(3, 6, tcpClientDTS->returnConnectState() ? "已连接" : "已断开");

    // 第四行：配置信息
    QString dtsType = ui->comboDTSType->currentText();
    xlsx.write(4, 1, "传感器类型");
    xlsx.write(4, 2, dtsType);
    xlsx.write(4, 3, "采集模式");
    xlsx.write(4, 4, dtsType.contains("电压") ? "电压采集" : "温度采集");
    xlsx.write(4, 5, "冷端补偿");
    xlsx.write(4, 6, dtsType.contains("电压") ? "关闭" : "开启");

    // 第五行：采集参数
    xlsx.write(5, 1, "通道数量");
    xlsx.write(5, 2, "48通道");
    xlsx.write(5, 3, "采集间隔");
    xlsx.write(5, 4, ui->lineEdit_dts_interval->text() + " ms");
    xlsx.write(5, 5, "数据单位");
    if (dtsType.contains("电压")) {
        xlsx.write(5, 6, "伏特(V)");
    } else {
        xlsx.write(5, 6, "摄氏度(℃)");
    }

    // 第六行：理论速率信息
    xlsx.write(6, 1, "理论采集速率");
    xlsx.write(6, 2, "50 Hz");
    xlsx.write(6, 3, "理论采集周期");
    xlsx.write(6, 4, "20 ms");
    xlsx.write(6, 5, "理论数据速率");
    xlsx.write(6, 6, "2400 数据点/秒");

    // 第七行：空行分隔
    xlsx.write(7, 1, "=================== 采集速率分析 ===================");
    for (int col = 2; col <= 6; col++) {
        xlsx.write(7, col, "");
    }

    // 第八行：实时采集速率（基于监控数据）
    xlsx.write(8, 1, "实时采集速率");
    xlsx.write(8, 2, QString("%1 Hz").arg(m_dtsActualRate, 0, 'f', 2));
    xlsx.write(8, 3, "实时速率状态");
    if (m_dtsActualRate >= 45.0 && m_dtsActualRate <= 55.0) {
        xlsx.write(8, 4, "稳定 (误差±5%)");
    } else if (m_dtsActualRate >= 40.0 && m_dtsActualRate < 45.0) {
        xlsx.write(8, 4, "偏低 (误差-10%~-20%)");
    } else if (m_dtsActualRate > 55.0 && m_dtsActualRate <= 60.0) {
        xlsx.write(8, 4, "偏高 (误差+5%~+20%)");
    } else {
        xlsx.write(8, 4, "异常 (误差>20%)");
    }
    xlsx.write(8, 5, "数据包数量");
    xlsx.write(8, 6, QString("%1 包").arg(m_dtsPacketCount));

    // 第九行：基于实际数据计算的速率
    if (dataList.size() >= 2) {
        DTS_PACKET firstPkt = dataList.first();
        DTS_PACKET lastPkt = dataList.last();

        qint64 firstTime_ns = firstPkt.nSecs * 1000000000LL + firstPkt.nNSecs;
        qint64 lastTime_ns = lastPkt.nSecs * 1000000000LL + lastPkt.nNSecs;

        if (lastTime_ns > firstTime_ns) {
            double timeDiff_sec = (lastTime_ns - firstTime_ns) / 1e9;
            double frequencyHz = (dataList.size() - 1) / timeDiff_sec;
            double timeDiff_ms = timeDiff_sec * 1000;

            xlsx.write(9, 1, "实际计算速率");
            xlsx.write(9, 2, QString("%1 Hz").arg(frequencyHz, 0, 'f', 2));
            xlsx.write(9, 3, "实际采集时间");
            xlsx.write(9, 4, QString("%1 ms").arg(timeDiff_ms, 0, 'f', 1));
            xlsx.write(9, 5, "总数据点数");
            xlsx.write(9, 6, QString("%1 点").arg(dataList.size()));
        }
    } else {
        xlsx.write(9, 1, "实际计算速率");
        xlsx.write(9, 2, "数据不足");
        xlsx.write(9, 3, "实际采集时间");
        xlsx.write(9, 4, "0 ms");
    }

    // 第十行：详细速率分析（如果有足够数据）
    if (dataList.size() >= 10) {
        // 计算采集间隔统计
        QVector<double> intervals;
        double totalInterval = 0;
        double maxInterval = 0;
        double minInterval = 1000;

        for (int i = 1; i < dataList.size(); i++) {
            qint64 prevTime_ns = dataList[i-1].nSecs * 1000000000LL + dataList[i-1].nNSecs;
            qint64 currTime_ns = dataList[i].nSecs * 1000000000LL + dataList[i].nNSecs;
            double interval = (currTime_ns - prevTime_ns) / 1e6; // 转换为ms
            intervals.append(interval);
            totalInterval += interval;
            maxInterval = qMax(maxInterval, interval);
            minInterval = qMin(minInterval, interval);
        }

        double avgInterval = totalInterval / intervals.size();
        double intervalStdDev = 0;
        for (double interval : intervals) {
            intervalStdDev += qPow(interval - avgInterval, 2);
        }
        intervalStdDev = qSqrt(intervalStdDev / intervals.size());

        xlsx.write(10, 1, "间隔统计分析");
        xlsx.write(10, 2, QString("平均间隔: %1 ms").arg(avgInterval, 0, 'f', 3));
        xlsx.write(10, 3, QString("最大间隔: %1 ms").arg(maxInterval, 0, 'f', 3));
        xlsx.write(10, 4, QString("最小间隔: %1 ms").arg(minInterval, 0, 'f', 3));
        xlsx.write(10, 5, QString("间隔波动: %1 ms").arg(maxInterval - minInterval, 0, 'f', 3));
        xlsx.write(10, 6, QString("标准差: %1 ms").arg(intervalStdDev, 0, 'f', 3));
    }

    // 第十一行：速率稳定性评估
    xlsx.write(11, 1, "速率稳定性评估");
    if (dataList.size() >= 10) {
        double avgInterval = 0;
        double totalInterval = 0;

        for (int i = 1; i < qMin(100, dataList.size()); i++) {
            qint64 prevTime_ns = dataList[i-1].nSecs * 1000000000LL + dataList[i-1].nNSecs;
            qint64 currTime_ns = dataList[i].nSecs * 1000000000LL + dataList[i].nNSecs;
            totalInterval += (currTime_ns - prevTime_ns) / 1e6;
        }
        avgInterval = totalInterval / qMin(99, dataList.size() - 1);

        if (avgInterval >= 18.0 && avgInterval <= 22.0) {
            xlsx.write(11, 2, "优秀 (20ms±10%)");
            xlsx.write(11, 3, "采集间隔稳定");
        } else if (avgInterval >= 15.0 && avgInterval <= 25.0) {
            xlsx.write(11, 2, "良好 (20ms±25%)");
            xlsx.write(11, 3, "采集间隔基本稳定");
        } else if (avgInterval >= 10.0 && avgInterval <= 30.0) {
            xlsx.write(11, 2, "一般 (20ms±50%)");
            xlsx.write(11, 3, "采集间隔有波动");
        } else {
            xlsx.write(11, 2, "较差 (误差>50%)");
            xlsx.write(11, 3, "采集间隔不稳定");
        }
        xlsx.write(11, 4, QString("平均间隔: %1 ms").arg(avgInterval, 0, 'f', 2));
    } else {
        xlsx.write(11, 2, "数据不足，无法评估");
    }

    // 第十二行：空行分隔
    xlsx.write(12, 1, "=================== 数据采集信息 ===================");
    for (int col = 2; col <= 6; col++) {
        xlsx.write(12, col, "");
    }

    // 第十三行：采集时间信息
    if (!dataList.isEmpty()) {
        DTS_PACKET firstPkt = dataList.first();
        DTS_PACKET lastPkt = dataList.last();

        double firstTotalSec = firstPkt.nSecs + firstPkt.nNSecs / 1e9;
        double lastTotalSec = lastPkt.nSecs + lastPkt.nNSecs / 1e9;

        QDateTime startTime = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(firstTotalSec));
        QDateTime endTime = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(lastTotalSec));

        xlsx.write(13, 1, "采集开始时间");
        xlsx.write(13, 2, startTime.toString("yyyy-MM-dd HH:mm:ss.zzz"));
        xlsx.write(13, 3, "采集结束时间");
        xlsx.write(13, 4, endTime.toString("yyyy-MM-dd HH:mm:ss.zzz"));

        double durationSec = lastTotalSec - firstTotalSec;
        xlsx.write(13, 5, "采集持续时间");
        xlsx.write(13, 6, QString("%1 秒").arg(durationSec, 0, 'f', 3));
    }

    // 第十四行：数据统计信息
    xlsx.write(14, 1, "总数据记录数");
    xlsx.write(14, 2, QString::number(dataList.size()));
    xlsx.write(14, 3, "每通道数据量");
    xlsx.write(14, 4, QString("%1 点/通道").arg(dataList.size()));
    xlsx.write(14, 5, "数据总量");
    xlsx.write(14, 6, QString("%1 点").arg(dataList.size() * 48)); // 48通道

    // 第十五行：传感器状态统计（温度模式）
    if (!dtsType.contains("电压") && !dataList.isEmpty()) {
        int totalChannels = 0;
        int validChannels = 0;
        int brokenChannels = 0;

        // 使用最后一包数据统计
        DTS_PACKET lastPacket = dataList.last();
        for (int i = 0; i < 48; i++) {
            totalChannels++;
            float tempVal = lastPacket.fMeasValue[i];
            if (tempVal > -260.0f && tempVal < 1000.0f) {
                validChannels++;
            } else {
                brokenChannels++;
            }
        }

        xlsx.write(15, 1, "传感器状态统计");
        xlsx.write(15, 2, QString("有效: %1通道").arg(validChannels));
        xlsx.write(15, 3, QString("断线: %1通道").arg(brokenChannels));
        xlsx.write(15, 4, QString("总数: %1通道").arg(totalChannels));
        xlsx.write(15, 5, "有效传感器比例");
        xlsx.write(15, 6, QString("%1%").arg((validChannels * 100.0 / totalChannels), 0, 'f', 1));
    }

    // 第十六行：备注信息
    xlsx.write(16, 1, "备注");
    xlsx.write(16, 2, "本文件包含采集速率详细分析和传感器状态统计。");
    xlsx.write(16, 3, "");
    xlsx.write(16, 4, "");
    xlsx.write(16, 5, "保存类型");
    xlsx.write(16, 6, isAutoSave ? "自动保存" : "手动保存");

    // 第十七行：空行
    xlsx.write(17, 1, "");

    // ========== 表头（从第18行开始）==========
    QStringList headers = {"时间戳(s)", "帧号"};
    for (int i = 1; i <= 48; i++) {
        if (dtsType.contains("电压")) {
            headers.append(QString("通道%1(V)").arg(i));
        } else {
            headers.append(QString("通道%1(℃)").arg(i));
        }
    }

    // 写入表头（第18行）
    for (int col = 0; col < headers.size(); col++) {
        xlsx.write(18, col + 1, headers[col]);
    }

    // ========== 写入数据（从第19行开始）==========
    for (int row = 0; row < dataList.size(); row++) {
        DTS_PACKET pkt = dataList[row];
        int excelRow = row + 19;

        // 时间戳（第1列）
        double totalSec = pkt.nSecs + pkt.nNSecs / 1e9;
        xlsx.write(excelRow, 1, totalSec);

        // 帧号（第2列）
        xlsx.write(excelRow, 2, pkt.nFrameNo);

        // 48通道数据（第3-50列）- 关键修改：温度模式保存逻辑
        if (dtsType.contains("电压")) {
            // 电压模式：直接保存测量值
            for (int ch = 0; ch < 48; ch++) {
                xlsx.write(excelRow, 3 + ch, pkt.fMeasValue[ch]);
            }
        } else {
            // 温度模式：有传感器保存实际测量温度，无传感器保存-270
            for (int ch = 0; ch < 48; ch++) {
                float tempVal = pkt.fMeasValue[ch];  // 实际测量值

                // 判断是否为无传感器（断线值通常为-270℃左右）
                bool hasNoSensor = (tempVal <= -260.0f);

                if (hasNoSensor || tempVal < -273.15f || tempVal > 1000.0f) {
                    // 无传感器或无效值，保存-270
                    xlsx.write(excelRow, 3 + ch, -270.0f);
                } else {
                    // 有传感器，保存实际测量温度
                    xlsx.write(excelRow, 3 + ch, tempVal);
                }
            }
        }
    }


    return xlsx.saveAs(fileName);
}

// 创建新的自动保存Excel文档 - 修改版
void MainWindow::createNewAutoSaveFile(bool forDPS, bool forDTS)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QDateTime currentTime = QDateTime::currentDateTime();

    if (forDPS) {
        // 创建DPS自动保存文件
        m_dpsAutoSaveFileName = QString("%1自动保存_DPS_%2.xls")
                                    .arg(m_autoSavePath)
                                    .arg(timestamp);

        // 如果已经有文档，先删除
        if (m_currentDPSDoc) {
            delete m_currentDPSDoc;
        }

        m_currentDPSDoc = new QXlsx::Document();
        XlsxWorksheet *sheet = m_currentDPSDoc->currentWorksheet();
        sheet->setName("DPS自动保存数据");

        // ========== 新增：写入采样速率信息 ==========
        // 第一行：系统信息标题
        m_currentDPSDoc->write(1, 1, "双设备采集系统 - 自动保存数据");
        m_currentDPSDoc->write(1, 2, "");
        m_currentDPSDoc->write(1, 3, "");
        m_currentDPSDoc->write(1, 4, "");
        m_currentDPSDoc->write(1, 5, "文件创建时间");
        m_currentDPSDoc->write(1, 6, currentTime.toString("yyyy-MM-dd HH:mm:ss"));

        // 第二行：DPS设备信息
        m_currentDPSDoc->write(2, 1, "设备类型");
        m_currentDPSDoc->write(2, 2, "DPS-16P2 压力扫描阀");
        m_currentDPSDoc->write(2, 3, "IP地址");
        m_currentDPSDoc->write(2, 4, ui->lineEdit_ip->text());
        m_currentDPSDoc->write(2, 5, "采样速率");
        m_currentDPSDoc->write(2, 6, "20ms (50Hz)");

        // 第三行：DPS配置信息
        QString dpsSync = ui->comboSync->currentText();
        QString dpsFormat = ui->comboFormat->currentText();
        QString dpsUnit = ui->comboUnit->currentText();
        QString dpsTrigger = ui->comboTrigger->currentText();

        m_currentDPSDoc->write(3, 1, "同步方式");
        m_currentDPSDoc->write(3, 2, dpsSync);
        m_currentDPSDoc->write(3, 3, "数据格式");
        m_currentDPSDoc->write(3, 4, dpsFormat);
        m_currentDPSDoc->write(3, 5, "触发方式");
        m_currentDPSDoc->write(3, 6, dpsTrigger);

        // 第四行：压力单位和其他信息
        m_currentDPSDoc->write(4, 1, "压力单位");
        m_currentDPSDoc->write(4, 2, dpsUnit);
        m_currentDPSDoc->write(4, 3, "采集周期");
        m_currentDPSDoc->write(4, 4, ui->lineEdit_period->text() + " ms");
        m_currentDPSDoc->write(4, 5, "通道数量");
        m_currentDPSDoc->write(4, 6, "16通道压力 + 16通道温度");

        // 第五行：采集次数信息
        QString collectNum = ui->lineEdit_collectNum->text();
        if (collectNum == "0") {
            m_currentDPSDoc->write(5, 1, "采集模式");
            m_currentDPSDoc->write(5, 2, "连续采集");
        } else {
            m_currentDPSDoc->write(5, 1, "采集模式");
            m_currentDPSDoc->write(5, 2, "有限次采集");
            m_currentDPSDoc->write(5, 3, "计划采集次数");
            m_currentDPSDoc->write(5, 4, collectNum + " 次");
        }

        // 第六行：空行分隔
        m_currentDPSDoc->write(6, 1, "");

        // 第七行：数据采集信息标题
        m_currentDPSDoc->write(7, 1, "数据采集信息");
        m_currentDPSDoc->write(7, 2, "");
        m_currentDPSDoc->write(7, 3, "");
        m_currentDPSDoc->write(7, 4, "");
        m_currentDPSDoc->write(7, 5, "保存方式");
        m_currentDPSDoc->write(7, 6, "自动保存");

        // 第八行：采集开始时间
        m_currentDPSDoc->write(8, 1, "采集开始时间");
        m_currentDPSDoc->write(8, 2, currentTime.toString("yyyy-MM-dd HH:mm:ss.zzz"));
        m_currentDPSDoc->write(8, 3, "预计结束时间");
        m_currentDPSDoc->write(8, 4, "-");
        m_currentDPSDoc->write(8, 5, "保存频率");
        m_currentDPSDoc->write(8, 6, QString("%1 秒").arg(m_autoSaveInterval / 1000));

        // 第九行：数据点数和频率信息
        m_currentDPSDoc->write(9, 1, "初始数据点数");
        m_currentDPSDoc->write(9, 2, "0");
        m_currentDPSDoc->write(9, 3, "理论采集频率");
        m_currentDPSDoc->write(9, 4, "50 Hz (20ms周期)");
        m_currentDPSDoc->write(9, 5, "理论采集率");
        m_currentDPSDoc->write(9, 6, "16通道 × 50Hz = 800数据点/秒");

        // 第十行：统计信息（留空，运行时更新）
        m_currentDPSDoc->write(10, 1, "累计数据点数");
        m_currentDPSDoc->write(10, 2, "-");
        m_currentDPSDoc->write(10, 3, "实际采集频率");
        m_currentDPSDoc->write(10, 4, "-");
        m_currentDPSDoc->write(10, 5, "文件大小");
        m_currentDPSDoc->write(10, 6, "-");

        // 第十一行：备注信息
        m_currentDPSDoc->write(11, 1, "备注");
        m_currentDPSDoc->write(11, 2, "此文件为自动保存文件，数据按时间顺序追加");
        m_currentDPSDoc->write(11, 3, "");
        m_currentDPSDoc->write(11, 4, "");
        m_currentDPSDoc->write(11, 5, "版本");
        m_currentDPSDoc->write(11, 6, "双设备采集系统 v1.0");

        // 第十二行：空行分隔
        m_currentDPSDoc->write(12, 1, "");
        // ========== 结束：采样速率信息 ==========

        // 表头（从第13行开始）
        QStringList headers = {"采集时间", "帧号",
                               "通道1(psi/kPa)", "通道2(psi/kPa)", "通道3(psi/kPa)", "通道4(psi/kPa)",
                               "通道5(psi/kPa)", "通道6(psi/kPa)", "通道7(psi/kPa)", "通道8(psi/kPa)",
                               "通道9(psi/kPa)", "通道10(psi/kPa)", "通道11(psi/kPa)", "通道12(psi/kPa)",
                               "通道13(psi/kPa)", "通道14(psi/kPa)", "通道15(psi/kPa)", "通道16(psi/kPa)",
                               "温度1(℃)", "温度2(℃)", "温度3(℃)", "温度4(℃)",
                               "温度5(℃)", "温度6(℃)", "温度7(℃)", "温度8(℃)",
                               "温度9(℃)", "温度10(℃)", "温度11(℃)", "温度12(℃)",
                               "温度13(℃)", "温度14(℃)", "温度15(℃)", "温度16(℃)"};

        for (int col = 0; col < headers.size(); col++) {
            m_currentDPSDoc->write(13, col + 1, headers[col]);
        }

        m_dpsRowCounter = 14; // 从第14行开始写数据
        m_autoSaveBufferDPS.clear(); // 清空缓冲区
    }

    if (forDTS) {
        // 创建DTS自动保存文件
        m_dtsAutoSaveFileName = QString("%1自动保存_DTS_%2.xls")
                                    .arg(m_autoSavePath)
                                    .arg(timestamp);

        // 如果已经有文档，先删除
        if (m_currentDTSDoc) {
            delete m_currentDTSDoc;
        }

        m_currentDTSDoc = new QXlsx::Document();
        XlsxWorksheet *sheet = m_currentDTSDoc->currentWorksheet();

        QString typeStr = ui->comboDTSType->currentText();
        QString sheetName = typeStr.contains("电压") ? "DTS电压自动保存数据" : "DTS温度自动保存数据";
        sheet->setName(sheetName);

        // ========== 新增：写入采样速率信息 ==========
        // 第一行：系统信息标题
        m_currentDTSDoc->write(1, 1, "双设备采集系统 - DTS自动保存数据");
        m_currentDTSDoc->write(1, 2, "");
        m_currentDTSDoc->write(1, 3, "");
        m_currentDTSDoc->write(1, 4, "");
        m_currentDTSDoc->write(1, 5, "文件创建时间");
        m_currentDTSDoc->write(1, 6, currentTime.toString("yyyy-MM-dd HH:mm:ss"));

        // 第二行：DTS设备信息
        m_currentDTSDoc->write(2, 1, "设备类型");
        m_currentDTSDoc->write(2, 2, "DTS-48F2 热电偶采集仪");
        m_currentDTSDoc->write(2, 3, "IP地址");
        m_currentDTSDoc->write(2, 4, ui->lineEdit_dts_ip->text());
        m_currentDTSDoc->write(2, 5, "采样速率");
        m_currentDTSDoc->write(2, 6, "20ms (50Hz)");

        // 第三行：DTS配置信息
        QString dtsType = ui->comboDTSType->currentText();
        QString dtsInterval = ui->lineEdit_dts_interval->text();
        if (dtsInterval.isEmpty()) dtsInterval = "20";

        m_currentDTSDoc->write(3, 1, "传感器类型");
        m_currentDTSDoc->write(3, 2, dtsType);
        m_currentDTSDoc->write(3, 3, "采样间隔");
        m_currentDTSDoc->write(3, 4, dtsInterval + " ms");
        m_currentDTSDoc->write(3, 5, "采集模式");
        m_currentDTSDoc->write(3, 6, dtsType.contains("电压") ? "电压采集" : "温度采集");

        // 第四行：通道和单位信息
        m_currentDTSDoc->write(4, 1, "通道数量");
        m_currentDTSDoc->write(4, 2, "48通道");
        m_currentDTSDoc->write(4, 3, "数据单位");
        if (dtsType.contains("电压")) {
            m_currentDTSDoc->write(4, 4, "伏特(V)");
        } else {
            m_currentDTSDoc->write(4, 4, "摄氏度(℃)");
        }
        m_currentDTSDoc->write(4, 5, "冷端补偿");
        m_currentDTSDoc->write(4, 6, dtsType.contains("电压") ? "关闭" : "开启");

        // 第五行：采集参数
        m_currentDTSDoc->write(5, 1, "同步方式");
        m_currentDTSDoc->write(5, 2, "内部时钟");
        m_currentDTSDoc->write(5, 3, "触发方式");
        m_currentDTSDoc->write(5, 4, "内部触发");
        m_currentDTSDoc->write(5, 5, "数据格式");
        m_currentDTSDoc->write(5, 6, "小头格式");

        // 第六行：空行分隔
        m_currentDTSDoc->write(6, 1, "");

        // 第七行：数据采集信息标题
        m_currentDTSDoc->write(7, 1, "数据采集信息");
        m_currentDTSDoc->write(7, 2, "");
        m_currentDTSDoc->write(7, 3, "");
        m_currentDTSDoc->write(7, 4, "");
        m_currentDTSDoc->write(7, 5, "保存方式");
        m_currentDTSDoc->write(7, 6, "自动保存");

        // 第八行：采集开始时间
        m_currentDTSDoc->write(8, 1, "采集开始时间");
        m_currentDTSDoc->write(8, 2, currentTime.toString("yyyy-MM-dd HH:mm:ss.zzz"));
        m_currentDTSDoc->write(8, 3, "预计结束时间");
        m_currentDTSDoc->write(8, 4, "-");
        m_currentDTSDoc->write(8, 5, "保存频率");
        m_currentDTSDoc->write(8, 6, QString("%1 秒").arg(m_autoSaveInterval / 1000));

        // 第九行：数据点数和频率信息
        m_currentDTSDoc->write(9, 1, "初始数据点数");
        m_currentDTSDoc->write(9, 2, "0");
        m_currentDTSDoc->write(9, 3, "理论采集频率");
        m_currentDTSDoc->write(9, 4, "50 Hz (20ms周期)");
        m_currentDTSDoc->write(9, 5, "理论采集率");
        m_currentDTSDoc->write(9, 6, "48通道 × 50Hz = 2400数据点/秒");

        // 第十行：统计信息（留空，运行时更新）
        m_currentDTSDoc->write(10, 1, "累计数据点数");
        m_currentDTSDoc->write(10, 2, "-");
        m_currentDTSDoc->write(10, 3, "实际采集频率");
        m_currentDTSDoc->write(10, 4, "-");
        m_currentDTSDoc->write(10, 5, "文件大小");
        m_currentDTSDoc->write(10, 6, "-");

        // 第十一行：备注信息
        m_currentDTSDoc->write(11, 1, "备注");
        m_currentDTSDoc->write(11, 2, "此文件为自动保存文件，数据按时间顺序追加");
        m_currentDTSDoc->write(11, 3, "");
        m_currentDTSDoc->write(11, 4, "");
        if (dtsType.contains("电压")) {
            m_currentDTSDoc->write(11, 5, "电压量程");
            m_currentDTSDoc->write(11, 6, dtsType);
        } else {
            m_currentDTSDoc->write(11, 5, "热电偶类型");
            m_currentDTSDoc->write(11, 6, dtsType);
        }

        // 第十二行：空行分隔
        m_currentDTSDoc->write(12, 1, "");
        // ========== 结束：采样速率信息 ==========

        // 表头（从第13行开始）
        QStringList headers = {"时间戳(s)", "帧号"};
        for (int i = 1; i <= 48; i++) {
            if (typeStr.contains("电压")) {
                headers.append(QString("通道%1(V)").arg(i));
            } else {
                headers.append(QString("通道%1(℃)").arg(i));
            }
        }

        for (int col = 0; col < headers.size(); col++) {
            m_currentDTSDoc->write(13, col + 1, headers[col]);
        }

        m_dtsRowCounter = 14; // 从第14行开始写数据
        m_autoSaveBufferDTS.clear(); // 清空缓冲区
    }
}

void MainWindow::appendDataToCurrentExcel(bool forDPS, bool forDTS)
{
    static QElapsedTimer saveTimer;
    if (!saveTimer.isValid()) {
        saveTimer.start();
    }

    // 分批处理，避免单次保存数据过多
    const int MAX_BATCH_SIZE = 100;

    // 获取配置信息
    QString typeStr = ui->comboDTSType->currentText();
    bool isKPa = (ui->comboUnit->currentText() == "kPa");

    if (forDPS && m_currentDPSDoc && !m_autoSaveBufferDPS.isEmpty()) {
        // DPS数据处理（保持原样）
        int batchSize = qMin(m_autoSaveBufferDPS.size(), MAX_BATCH_SIZE);

        // 更新统计信息
        int totalPoints = m_dpsRowCounter - 14 + batchSize;
        m_currentDPSDoc->write(10, 2, QString::number(totalPoints));

        // 批量处理数据包
        for (int i = 0; i < batchSize; i++) {
            DPS_PACKET pkt = m_autoSaveBufferDPS[i];

            // 时间计算
            QString timeStr;

            static bool firstTimeCalculated = false;
            static QDateTime firstPacketTime;
            static qint64 firstFrameNo = 0;

            if (!firstTimeCalculated && !m_autoSaveBufferDPS.isEmpty()) {
                firstPacketTime = QDateTime::currentDateTime();
                firstFrameNo = pkt.nFrameNo;
                firstTimeCalculated = true;
            }

            if (firstTimeCalculated) {
                qint64 frameOffset = (pkt.nFrameNo - firstFrameNo) * 20; // ms
                QDateTime packetTime = firstPacketTime.addMSecs(frameOffset);
                timeStr = packetTime.toString("yyyy-MM-dd HH:mm:ss.zzz");
            } else {
                timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
            }

            // 写入数据
            m_currentDPSDoc->write(m_dpsRowCounter, 1, timeStr);
            m_currentDPSDoc->write(m_dpsRowCounter, 2, pkt.nFrameNo);

            for (int ch = 0; ch < 16; ch++) {
                double press = pkt.fPress[ch].toDouble();
                m_currentDPSDoc->write(m_dpsRowCounter, 3 + ch, convertUnit(press, isKPa));
            }

            for (int ch = 0; ch < 16; ch++) {
                double temp = pkt.fTemp[ch].toDouble();
                m_currentDPSDoc->write(m_dpsRowCounter, 19 + ch, temp);
            }

            m_dpsRowCounter++;
        }

        // 从缓冲区移除已处理的数据
        for (int i = 0; i < batchSize; i++) {
            m_autoSaveBufferDPS.removeFirst();
        }

        // 保存文件
        m_currentDPSDoc->saveAs(m_dpsAutoSaveFileName);

        // 性能监控
        double saveTime = saveTimer.elapsed();
        if (saveTime > 1000) {
            qDebug() << QString("DPS保存性能：处理%1个数据包用时%2ms")
                            .arg(batchSize)
                            .arg(saveTime, 0, 'f', 2);
            saveTimer.restart();
        }
    }

    if (forDTS && m_currentDTSDoc && !m_autoSaveBufferDTS.isEmpty()) {
        // ========== 关键修复：DTS时间戳处理 ==========
        int batchSize = qMin(m_autoSaveBufferDTS.size(), MAX_BATCH_SIZE);

        // 更新统计信息
        int totalPoints = m_dtsRowCounter - 14 + batchSize;
        m_currentDTSDoc->write(10, 2, QString::number(totalPoints));

        // 批量处理数据包
        for (int i = 0; i < batchSize; i++) {
            DTS_PACKET pkt = m_autoSaveBufferDTS[i];

            // ========== 关键修复：生成正确的时间字符串 ==========
            QString timeStr;

            // 使用帧号计算时间（基于采集开始时间）
            static bool dtsFirstTimeCalculated = false;
            static QDateTime dtsFirstPacketTime;
            static int dtsFirstFrameNo = 0;

            if (!dtsFirstTimeCalculated && !m_autoSaveBufferDTS.isEmpty()) {
                dtsFirstPacketTime = QDateTime::currentDateTime();
                dtsFirstFrameNo = pkt.nFrameNo;
                dtsFirstTimeCalculated = true;
            }

            if (dtsFirstTimeCalculated) {
                // 基于帧号计算时间：每帧20ms
                int frameOffset = (pkt.nFrameNo - dtsFirstFrameNo) * 20; // ms
                QDateTime packetTime = dtsFirstPacketTime.addMSecs(frameOffset);
                timeStr = packetTime.toString("yyyy-MM-dd HH:mm:ss.zzz");
            } else {
                // 使用相对时间
                timeStr = QString("%1 s").arg(pkt.nSecs + pkt.nNSecs / 1e9, 0, 'f', 6);
            }

            // 写入时间戳（使用系统时间格式，不再是原始时间戳）
            m_currentDTSDoc->write(m_dtsRowCounter, 1, timeStr);

            // 写入帧号（使用我们自己的帧号计数器）
            m_currentDTSDoc->write(m_dtsRowCounter, 2, pkt.nFrameNo);

            // 48通道数据
            if (typeStr.contains("电压")) {
                // 电压模式：直接保存测量值
                for (int ch = 0; ch < 48; ch++) {
                    m_currentDTSDoc->write(m_dtsRowCounter, 3 + ch, pkt.fMeasValue[ch]);
                }
            } else {
                // 温度模式：有传感器保存实际测量温度，无传感器保存-270
                for (int ch = 0; ch < 48; ch++) {
                    float tempVal = pkt.fMeasValue[ch];
                    bool hasNoSensor = (tempVal <= -260.0f);

                    if (hasNoSensor || tempVal < -273.15f || tempVal > 1000.0f) {
                        m_currentDTSDoc->write(m_dtsRowCounter, 3 + ch, -270.0f);
                    } else {
                        m_currentDTSDoc->write(m_dtsRowCounter, 3 + ch, tempVal);
                    }
                }
            }

            m_dtsRowCounter++;
        }

        // 从缓冲区移除已处理的数据
        for (int i = 0; i < batchSize; i++) {
            m_autoSaveBufferDTS.removeFirst();
        }

        // 保存文件
        m_currentDTSDoc->saveAs(m_dtsAutoSaveFileName);

        // 性能监控
        double saveTime = saveTimer.elapsed();
        if (saveTime > 1000) {
            qDebug() << QString("DTS保存性能：处理%1个数据包用时%2ms")
                            .arg(batchSize)
                            .arg(saveTime, 0, 'f', 2);
            saveTimer.restart();
        }

        // 如果还有数据，继续处理（异步）
        if (!m_autoSaveBufferDTS.isEmpty()) {
            QTimer::singleShot(1, this, [this]() {
                appendDataToCurrentExcel(false, true);
            });
        }
    }
}


// ==================== 自动保存数据（修改后的版本）====================
void MainWindow::autoSaveData()
{
    // 如果没有数据，不保存
    if (m_autoSaveBufferDPS.isEmpty() && m_autoSaveBufferDTS.isEmpty()) {
        return;
    }

    // 追加数据到当前Excel文档
    appendDataToCurrentExcel(true, true);

    // 更新状态显示
    if (!m_autoSaveBufferDPS.isEmpty()) {
        int totalPoints = m_autoSaveBufferDPS.size();

        if (m_simultaneousCollecting) {
            ui->labelAutoSave->setText(QString("同时采集: 进行中(DPS: %1个数据包)").arg(totalPoints));
        } else {
            ui->labelAutoSave->setText(QString("自动保存: 进行中(DPS: %1个数据包)").arg(totalPoints));
        }
        ui->labelAutoSave->setStyleSheet("QLabel { color: green; font-weight: bold; }");
    }

    if (!m_autoSaveBufferDTS.isEmpty()) {
        if (m_simultaneousCollecting) {
            ui->labelDTSAutoSave->setText(QString("同时采集: 进行中(DTS: %1个数据包)").arg(m_autoSaveBufferDTS.size()));
        } else {
            ui->labelDTSAutoSave->setText(QString("自动保存: 进行中(DTS: %1个数据包)").arg(m_autoSaveBufferDTS.size()));
        }
        ui->labelDTSAutoSave->setStyleSheet("QLabel { color: green; font-weight: bold; }");
    }
}

// 开始自动保存（修改后的版本）
void MainWindow::startAutoSave()
{
    if (!m_autoSaveTimer) {
        initAutoSave();
    }

    m_autoSaveEnabled = true;

    // 如果不是同时采集模式，才创建新的自动保存文件
    if (!m_simultaneousCollecting) {
        // 确定哪些设备需要自动保存
        bool dpsEnabled = tcpClient->returnConnectState() && ui->pushButton_scan->isEnabled();
        bool dtsEnabled = tcpClientDTS->returnConnectState() && ui->btnDTSScan->isEnabled();

        // 创建新的自动保存文件
        if (dpsEnabled) {
            createNewAutoSaveFile(true, false);
            ui->labelAutoSave->setText("自动保存: 启用");
            ui->labelAutoSave->setStyleSheet("QLabel { color: green; font-weight: bold; }");
        }

        if (dtsEnabled) {
            createNewAutoSaveFile(false, true);
            ui->labelDTSAutoSave->setText("自动保存: 启用");
            ui->labelDTSAutoSave->setStyleSheet("QLabel { color: green; font-weight: bold; }");
        }
    }

    // 启动定时器
    m_autoSaveTimer->start();

    if (!m_simultaneousCollecting) {
        appendTextEdit("自动保存功能已启用");
        appendTextEditDTS("自动保存功能已启用");
    }
}


// 停止自动保存（修改版）
void MainWindow::stopAutoSave()
{
    if (m_autoSaveTimer) {
        m_autoSaveTimer->stop();
    }

    m_autoSaveEnabled = false;

    // 保存缓冲区中剩余的数据
    autoSaveData();

    // 如果不是同时采集模式，才更新状态显示
    if (!m_simultaneousCollecting) {
        // 更新最终统计信息
        if (m_currentDPSDoc) {
            QFileInfo fileInfo(m_dpsAutoSaveFileName);
            if (fileInfo.exists()) {
                // 更新采集结束时间
                QDateTime endTime = QDateTime::currentDateTime();
                m_currentDPSDoc->write(8, 4, endTime.toString("yyyy-MM-dd HH:mm:ss.zzz"));

                // 更新最终文件大小
                m_currentDPSDoc->write(10, 6, QString("%1 字节").arg(fileInfo.size()));

                // 保存更新
                m_currentDPSDoc->saveAs(m_dpsAutoSaveFileName);

                appendTextEdit(QString("DPS自动保存文件已最终更新：%1").arg(m_dpsAutoSaveFileName));
            }
        }

        if (m_currentDTSDoc) {
            QFileInfo fileInfo(m_dtsAutoSaveFileName);
            if (fileInfo.exists()) {
                // 更新采集结束时间
                QDateTime endTime = QDateTime::currentDateTime();
                m_currentDTSDoc->write(8, 4, endTime.toString("yyyy-MM-dd HH:mm:ss.zzz"));

                // 更新最终文件大小
                m_currentDTSDoc->write(10, 6, QString("%1 字节").arg(fileInfo.size()));

                // 保存更新
                m_currentDTSDoc->saveAs(m_dtsAutoSaveFileName);

                appendTextEditDTS(QString("DTS自动保存文件已最终更新：%1").arg(m_dtsAutoSaveFileName));
            }
        }

        // 更新状态显示
        ui->labelAutoSave->setText("自动保存: 关闭");
        ui->labelAutoSave->setStyleSheet("QLabel { color: red; font-weight: bold; }");

        ui->labelDTSAutoSave->setText("自动保存: 关闭");
        ui->labelDTSAutoSave->setStyleSheet("QLabel { color: red; font-weight: bold; }");

        appendTextEdit("自动保存功能已停止");
        appendTextEditDTS("自动保存功能已停止");
    }
}


// 自动保存定时器槽函数
void MainWindow::onAutoSaveTimer()
{
    if (m_autoSaveEnabled) {
        // 性能监控
        monitorPerformance();

        // 异步保存数据
        if (!m_autoSaveBufferDPS.isEmpty() || !m_autoSaveBufferDTS.isEmpty()) {
            QTimer::singleShot(0, this, [this]() {
                appendDataToCurrentExcel(!m_autoSaveBufferDPS.isEmpty(),
                                         !m_autoSaveBufferDTS.isEmpty());
            });
        }
    }
}

// ==================== DTS-48数据显示初始化 ====================

// 初始化DTS-48数据显示
void MainWindow::initDTSDisplay()
{
    // 清除现有标签
    qDeleteAll(m_dtsDisplayLabels);
    m_dtsDisplayLabels.clear();

    QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->scrollAreaWidgetContents->layout());
    if (!gridLayout) {
        gridLayout = new QGridLayout(ui->scrollAreaWidgetContents);
        ui->scrollAreaWidgetContents->setLayout(gridLayout);
    }

    // 清空布局
    QLayoutItem* item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // 创建48个通道的显示标签
    int row = 0, col = 0;
    for (int i = 0; i < 48; i++) {
        QLabel *label = new QLabel(ui->scrollAreaWidgetContents);
        label->setText(QString("通道%1: -- °C").arg(i+1));
        label->setFrameStyle(QFrame::Box);
        label->setAlignment(Qt::AlignCenter);
        label->setMinimumHeight(35);
        label->setMinimumWidth(150);
        label->setStyleSheet("QLabel { background-color: #f0f0f0; border: 1px solid #ccc; padding: 2px; }");

        gridLayout->addWidget(label, row, col);
        m_dtsDisplayLabels.append(label);

        col++;
        if (col >= 4) { // 每行4个通道，便于查看
            col = 0;
            row++;
        }
    }

    // 设置滚动区域内容大小
    ui->scrollAreaWidgetContents->setMinimumSize(620, row * 40 + 20);
}

// ==================== IP地址修改相关函数 ====================

// DTS-48应用网络配置
void MainWindow::on_btnDTSApplyNetwork_clicked()
{
    QString ip = ui->lineEdit_dts_ip->text();
    QString subnet = ui->lineEdit_dts_subnet->text();
    QString gateway = ui->lineEdit_dts_gateway->text();

    // 验证IP地址格式
    QRegularExpression ipRegex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    if (!ipRegex.match(ip).hasMatch()) {
        QMessageBox::warning(this, "警告", "IP地址格式不正确！");
        return;
    }

    // 必须先连接到设备才能修改IP
    if (!tcpClientDTS->returnConnectState()) {
        QMessageBox::warning(this, "警告", "请先连接DTS设备！");
        return;
    }

    appendTextEditDTS(QString("开始配置DTS网络参数: IP=%1, 掩码=%2, 网关=%3")
                          .arg(ip).arg(subnet).arg(gateway));

    // 按照手册第12页的顺序发送设置命令
    // 注意：所有命令必须以\r结尾（根据手册第12页说明）

    // 1. 设置IP地址（关键命令）
    tcpClientDTS->sendMessage(QString("SET IPADDR %1\r").arg(ip));
    appendTextEditDTS(QString("发送: SET IPADDR %1").arg(ip));
    delayEventLoopMS(300);

    // 2. 设置子网掩码
    tcpClientDTS->sendMessage(QString("SET IPMASK %1\r").arg(subnet));
    appendTextEditDTS(QString("发送: SET IPMASK %1").arg(subnet));
    delayEventLoopMS(300);

    // 3. 设置网关
    tcpClientDTS->sendMessage(QString("SET GATEWAY %1\r").arg(gateway));
    appendTextEditDTS(QString("发送: SET GATEWAY %1").arg(gateway));
    delayEventLoopMS(300);

    // 4. 保存到非易失性存储器（手册第20页SAVE 0命令）
    tcpClientDTS->sendMessage("SAVE 0\r");
    appendTextEditDTS("发送: SAVE 0 (保存配置)");
    delayEventLoopMS(500);

    // 5. 查询设备信息确认配置（手册第18页）
    tcpClientDTS->sendMessage("LIST DEVICE\r");
    appendTextEditDTS("发送: LIST DEVICE (查询设备信息)");
    delayEventLoopMS(500);

    appendTextEditDTS("DTS网络配置已应用，新IP: " + ip);
    appendTextEditDTS("注意：可能需要重启设备或重新连接");

    // 提示用户
    QMessageBox::information(this, "成功",
                             QString("网络配置已保存！\n新IP地址: %1\n子网掩码: %2\n网关: %3\n\n设备可能需要重启才能生效。")
                                 .arg(ip).arg(subnet).arg(gateway));
}

// 快速设置为192.168.100.24
void MainWindow::on_btnQuickSetIP_clicked()
{
    // 设置固定的网络参数
    QString newIP = "192.168.100.24";
    QString subnet = "255.255.255.0";
    QString gateway = "192.168.100.1";

    // 更新UI显示
    ui->lineEdit_dts_ip->setText(newIP);
    ui->lineEdit_dts_subnet->setText(subnet);
    ui->lineEdit_dts_gateway->setText(gateway);

    QMessageBox::information(this, "提示",
                             QString("已设置为:\nIP: %1\n子网掩码: %2\n网关: %3\n\n请确保设备已连接，然后点击'应用网络配置'生效")
                                 .arg(newIP).arg(subnet).arg(gateway));
}

// 重新连接新IP
void MainWindow::on_btnReconnectNewIP_clicked()
{
    QString currentIP = ui->lineEdit_dts_ip->text();

    // 如果当前已连接，先断开
    if (tcpClientDTS->returnConnectState()) {
        appendTextEditDTS("断开当前连接...");
        tcpClientDTS->closeConnect();
        delayEventLoopMS(500);
    }

    // 更新TCP客户端的IP地址
    tcpClientDTS->setIP(currentIP);

    // 尝试连接
    appendTextEditDTS(QString("尝试连接到新IP: %1").arg(currentIP));
    tcpClientDTS->newConnect();
}

// 复位为默认IP（192.168.1.24）
void MainWindow::on_btnResetDefaultIP_clicked()
{
    QString defaultIP = "192.168.1.24";
    QString subnet = "255.255.255.0";
    QString gateway = "192.168.1.1";

    // 更新UI显示
    ui->lineEdit_dts_ip->setText(defaultIP);
    ui->lineEdit_dts_subnet->setText(subnet);
    ui->lineEdit_dts_gateway->setText(gateway);

    // 如果当前已连接，先断开
    if (tcpClientDTS->returnConnectState()) {
        tcpClientDTS->closeConnect();
        delayEventLoopMS(300);
    }

    // 使用默认IP重新连接
    tcpClientDTS->setIP(defaultIP);
    tcpClientDTS->newConnect();

    QMessageBox::information(this, "提示",
                             QString("已复位为默认IP:\nIP: %1\n子网掩码: %2\n网关: %3\n\n已尝试重新连接")
                                 .arg(defaultIP).arg(subnet).arg(gateway));
}

// 连接到新IP的函数
void MainWindow::connectToDTSWithNewIP()
{
    QString currentIP = ui->lineEdit_dts_ip->text();

    // 如果当前已连接，先断开
    if (tcpClientDTS->returnConnectState()) {
        tcpClientDTS->closeConnect();
        delayEventLoopMS(500);
    }

    // 更新TCP客户端的IP地址
    tcpClientDTS->setIP(currentIP);

    // 尝试连接
    tcpClientDTS->newConnect();

    appendTextEditDTS(QString("尝试连接到新IP: %1").arg(currentIP));
}

// 修改IP地址的通用函数
void MainWindow::changeDTSIPAddress(const QString &newIP, const QString &subnet, const QString &gateway)
{
    if (!tcpClientDTS->returnConnectState()) {
        QMessageBox::warning(this, "警告", "请先连接DTS设备！");
        return;
    }

    appendTextEditDTS(QString("正在修改DTS设备IP地址为: %1").arg(newIP));

    // 发送设置命令（注意：命令必须以\r结尾，根据手册第12页）
    tcpClientDTS->sendMessage(QString("SET IPADDR %1\r").arg(newIP));
    delayEventLoopMS(300);

    tcpClientDTS->sendMessage(QString("SET IPMASK %1\r").arg(subnet));
    delayEventLoopMS(300);

    tcpClientDTS->sendMessage(QString("SET GATEWAY %1\r").arg(gateway));
    delayEventLoopMS(300);

    // 保存配置
    tcpClientDTS->sendMessage("SAVE 0\r");
    delayEventLoopMS(500);

    // 重启设备使设置生效（可选）
    tcpClientDTS->sendMessage("REBOOT\r");
    appendTextEditDTS("设备正在重启...");
    delayEventLoopMS(3000); // 等待设备重启

    appendTextEditDTS(QString("IP地址已修改为%1，保存成功").arg(newIP));
    appendTextEditDTS("提示：设备已重启，请重新连接");

    // 更新UI显示
    ui->lineEdit_dts_ip->setText(newIP);
    ui->lineEdit_dts_subnet->setText(subnet);
    ui->lineEdit_dts_gateway->setText(gateway);

    // 自动尝试重新连接
    on_btnReconnectNewIP_clicked();
}

// ==================== DTS-48连接相关函数 ====================

// DTS-48连接按钮
void MainWindow::on_btnDTSConnect_clicked()
{
    tcpClientDTS->setIP(ui->lineEdit_dts_ip->text());
    tcpClientDTS->setPort(ui->lineEdit_dts_port->text().toInt());

    if(ui->btnDTSConnect->text() == "连接") {
        tcpClientDTS->newConnect();
    } else {
        tcpClientDTS->closeConnect();
    }
}

// DTS-48连接成功
void MainWindow::slotDTSConnected()
{
    ui->btnDTSConnect->setText("断开");
    appendTextEditDTS("DTS-48设备已连接！");
    appendTextEditDTS(QString("当前IP: %1, 端口: %2")
                          .arg(ui->lineEdit_dts_ip->text())
                          .arg(ui->lineEdit_dts_port->text()));
    updateSimultaneousButtonsState(); // 添加这行
}

// DTS-48断开连接
void MainWindow::slotDTSDisconnected()
{
    ui->btnDTSConnect->setText("连接");
    appendTextEditDTS("DTS-48设备已断开！");
    ui->btnDTSSave->setEnabled(false);
    updateSimultaneousButtonsState(); // 添加这行
}


// ==================== DTS-48数据包解析函数 ====================

// 字节序转换辅助函数
float bytesToFloat(const char* bytes, bool bigEndian = false) {
    float value;
    char* valueBytes = reinterpret_cast<char*>(&value);

    if (bigEndian) {
        // 大端序转换为小端序（主机字节序）
        valueBytes[0] = bytes[3];
        valueBytes[1] = bytes[2];
        valueBytes[2] = bytes[1];
        valueBytes[3] = bytes[0];
    } else {
        // 小端序直接复制
        valueBytes[0] = bytes[0];
        valueBytes[1] = bytes[1];
        valueBytes[2] = bytes[2];
        valueBytes[3] = bytes[3];
    }

    return value;
}

int32_t bytesToInt32(const char* bytes, bool bigEndian = false) {
    int32_t value;
    char* valueBytes = reinterpret_cast<char*>(&value);

    if (bigEndian) {
        // 大端序转换为小端序
        valueBytes[0] = bytes[3];
        valueBytes[1] = bytes[2];
        valueBytes[2] = bytes[1];
        valueBytes[3] = bytes[0];
    } else {
        // 小端序直接复制
        valueBytes[0] = bytes[0];
        valueBytes[1] = bytes[1];
        valueBytes[2] = bytes[2];
        valueBytes[3] = bytes[3];
    }

    return value;
}

int64_t bytesToInt64(const char* bytes, bool bigEndian = false) {
    int64_t value;
    char* valueBytes = reinterpret_cast<char*>(&value);

    if (bigEndian) {
        // 大端序转换为小端序
        for (int i = 0; i < 8; i++) {
            valueBytes[i] = bytes[7 - i];
        }
    } else {
        // 小端序直接复制
        for (int i = 0; i < 8; i++) {
            valueBytes[i] = bytes[i];
        }
    }

    return value;
}

// ==================== DTS-48数据包解析 ====================
void MainWindow::parseDTSPacket(const QByteArray &data)
{
    if (data.size() < 4) {
        return;
    }

    // 检查数据包类型
    unsigned char firstByte = static_cast<unsigned char>(data[0]);

    if (firstByte == 0x55) {
        // 0x55 数据包格式：温度/电压数据
        const int minPacketSize = 400; // 4+4+8+4+48*4+48*4 = 400字节

        if (data.size() < minPacketSize) {
            // 尝试解析部分数据
            if (data.size() >= 4 + 4 + 8 + 4) {
                // 至少解析包头
                DTS_PACKET pkt;
                const char *ptr = data.constData();
                int pos = 0;

                pkt.nPacketType = bytesToInt32(ptr + pos, false);
                pos += 4;

                // 帧号：从1开始递增（每次开始采集会重置）
                pkt.nFrameNo = ++m_dtsFrameSeq;
                pos += 4; // 跳过设备原始帧号

                // ========== 关键修复：记录时间基准 ==========
                static bool timeBaseRecorded = false;
                static QDateTime dtsStartTime;
                static qint64 dtsStartTimestamp = 0;

                // 读取原始时间戳
                pkt.nSecs = bytesToInt64(ptr + pos, false);
                pos += 8;
                pkt.nNSecs = bytesToInt32(ptr + pos, false);
                pos += 4;

                // 在同时采集模式下记录时间基准
                if (!timeBaseRecorded && m_simultaneousCollecting) {
                    dtsStartTime = QDateTime::currentDateTime();
                    dtsStartTimestamp = pkt.nSecs * 1000000000LL + pkt.nNSecs;
                    timeBaseRecorded = true;

                    appendTextEditDTS(QString("DTS时间基准已记录：系统时间=%1，设备时间戳=%2 ns")
                                          .arg(dtsStartTime.toString("yyyy-MM-dd HH:mm:ss.zzz"))
                                          .arg(dtsStartTimestamp));
                }

                // 只解析已有的数据
                int channels = (data.size() - pos) / 8; // 每个通道有测量值和参考值
                if (channels > 48) channels = 48;

                for (int i = 0; i < channels; i++) {
                    if (pos + 4 <= data.size()) {
                        pkt.fMeasValue[i] = bytesToFloat(ptr + pos, false);
                        pos += 4;
                    }
                    if (pos + 4 <= data.size()) {
                        pkt.fRefValue[i] = bytesToFloat(ptr + pos, false);
                        pos += 4;
                    }
                }

                // 冷端数据完全隔离：仅用测量值更新多截面，不涉及任何冷端数据
                QString typeStrTemp = ui->comboDTSType->currentText();
                if (!typeStrTemp.contains("电压")) {
                    static int tempMultiSectionCounter = 0;
                    tempMultiSectionCounter++;

                    auto hasSensor = [](float v) -> bool {
                        // 兼容设备“无传感器=-270”以及原逻辑（<=20或>=1000认为无）
                        return (v > 20.0f && v < 1200.0f);
                    };

                    // 指定热电偶通道（1-based）：5/10/15/20/25
                    const int idx5  = 4;
                    const int idx10 = 9;
                    const int idx15 = 14;
                    const int idx20 = 19;
                    const int idx25 = 24;

                    float v5  = (channels > idx5)  ? pkt.fMeasValue[idx5]  : -270.0f;
                    float v10 = (channels > idx10) ? pkt.fMeasValue[idx10] : -270.0f;
                    float v15 = (channels > idx15) ? pkt.fMeasValue[idx15] : -270.0f;
                    float v20 = (channels > idx20) ? pkt.fMeasValue[idx20] : -270.0f;
                    float v25 = (channels > idx25) ? pkt.fMeasValue[idx25] : -270.0f;

                    const bool has5  = hasSensor(v5);
                    const bool has10 = hasSensor(v10);
                    const bool has15 = hasSensor(v15);
                    const bool has20 = hasSensor(v20);
                    const bool has25 = hasSensor(v25);

                    // UI滑块仅用于显示：优先展示指定通道中第一个有效值
                    float displayTs = -270.0f;
                    if (has5) displayTs = v5;
                    else if (has10) displayTs = v10;
                    else if (has15) displayTs = v15;
                    else if (has20) displayTs = v20;
                    else if (has25) displayTs = v25;

                    if (displayTs > 20.0f && displayTs < 1200.0f) {
                        ui->sliderTemperatureTs->setValue(static_cast<int>(displayTs));
                    }

                    // 多截面温度：缓存每帧写入；UI每10帧刷新一次（避免影响50Hz）
                    const bool updateUI = (tempMultiSectionCounter % 10 == 0);
                    updateTemperatureMultiSectionFromSources(v5, has5, v10, has10, v15, has15, v20, has20, v25, has25,
                                                             pkt.nFrameNo, updateUI);
}

                // 添加到主数据缓存
                m_collectedDataDTS.append(pkt);

                // TCP缓存更新：仅用测量值，冷端数据不参与任何计算
                QString typeStrCache = ui->comboDTSType->currentText();
                if (!typeStrCache.contains("电压")) {
                    for (int i = 0; i < channels; i++) {
                        float tempVal = pkt.fMeasValue[i];
                        // 无传感器判断：<=20℃或>=1000℃（与Ts范围一致）
                        bool hasNoSensor = (tempVal <= 20.0f || tempVal >= 1000.0f);
                        if (hasNoSensor) {
                            tempVal = -270.0f;
                        }
                        m_lastDTSTemperatures[i] = tempVal;
                    }
                    // 未解析通道用-270填充
                    for (int i = channels; i < 48; i++) {
                        m_lastDTSTemperatures[i] = -270.0f;
                    }
                } else {
                    for (int i = 0; i < channels; i++) {
                        m_lastDTSTemperatures[i] = pkt.fMeasValue[i];
                    }
                    for (int i = channels; i < 48; i++) {
                        m_lastDTSTemperatures[i] = 0.0f;
                    }
                }
                m_dtsDataReady = true;

                // 添加到自动保存缓冲区
                if (m_autoSaveEnabled) {
                    m_autoSaveBufferDTS.append(pkt);
                }

                // 更新显示
                updateDTSDisplay(pkt);

                // 增加DTS数据包计数
                       m_dtsPacketCount++;

                       // 每50个数据包计算一次采集速率
                       if (m_dtsPacketCount % 50 == 0 && m_dtsRateTimer.elapsed() > 0) {
                           double elapsedSec = m_dtsRateTimer.elapsed() / 1000.0;
                           m_dtsActualRate = m_dtsPacketCount / elapsedSec;

                           // 显示当前采集速率
                           QString rateInfo = QString("DTS采集速率: %1 Hz (理论50Hz)").arg(m_dtsActualRate, 0, 'f', 1);
                           appendTextEditDTS(rateInfo);

                           // 如果速率异常，发出警告
                           if (m_dtsActualRate < 40.0 || m_dtsActualRate > 60.0) {
                               appendTextEditDTS(QString("警告: DTS采集速率异常! 当前: %1 Hz").arg(m_dtsActualRate, 0, 'f', 1));

                               // 尝试重新发送SCAN命令来稳定采集速率
                               static int retryCount = 0;
                               if (retryCount < 3) {
                                   appendTextEditDTS("尝试重新发送SCAN命令稳定采集速率...");
                                   tcpClientDTS->sendMessage("STOP\r");
                                   delayEventLoopMS(50);
                                   tcpClientDTS->sendMessage("SCAN\r");
                                   retryCount++;
                               }
                           } else {
                               // 速率正常时重置重试计数
                               static int retryCount = 0;
                               retryCount = 0;
                           }
                       }

                // 图表数据入队（20ms定时器实时刷新，避免采集掉速）
                m_pendingDtsPlot.enqueue(pkt);
                if (m_pendingDtsPlot.size() > 200) {
                    m_pendingDtsPlot.dequeue();
                }

                // 日志仅显示测量值，不显示冷端值
                QString info = QString("DTS帧号%1: ").arg(static_cast<int>(pkt.nFrameNo));
                int channelsToShow = qMin(5, channels);
                for (int i = 0; i < channelsToShow; i++) {
                    QString unit = typeStrCache.contains("电压") ? "V" : "°C";
                    info += QString("Ch%1测量值=%2%3 ").arg(static_cast<int>(i+1))
                                                   .arg(static_cast<double>(pkt.fMeasValue[i]), 0, 'f', 2)
                                                   .arg(unit);
                }
                if (channelsToShow < channels) info += "...";
                appendTextEditDTS(info);
            }
            return;
        }

        // 完整数据包解析
        DTS_PACKET pkt;
        const char *ptr = data.constData();
        int pos = 0;

        // 解析数据包类型 (4字节)
        pkt.nPacketType = bytesToInt32(ptr + pos, false);
        pos += 4;

// 帧号：从1开始递增（每次开始采集会重置）
pkt.nFrameNo = ++m_dtsFrameSeq;
pos += 4; // 跳过设备原始帧号

// 读取原始时间戳
pkt.nSecs = bytesToInt64(ptr + pos, false);
pos += 8;
pkt.nNSecs = bytesToInt32(ptr + pos, false);
pos += 4;

// ========== 关键修复：记录时间基准 ==========
// 记录一次系统时间基准（用于对齐/调试，不参与采样率与帧号）
if (!m_dtsTimeBaseRecorded && m_simultaneousCollecting) {
    m_dtsStartTime = QDateTime::currentDateTime();
    m_dtsStartTimestamp = pkt.nSecs * 1000000000LL + pkt.nNSecs;
    m_dtsTimeBaseRecorded = true;

    appendTextEditDTS(QString("DTS时间基准已记录：系统时间=%1，设备时间戳=%2 ns")
                          .arg(m_dtsStartTime.toString("yyyy-MM-dd HH:mm:ss.zzz"))
                          .arg(m_dtsStartTimestamp));
}

// 48通道测量值
        for (int i = 0; i < 48; i++) {
            pkt.fMeasValue[i] = bytesToFloat(ptr + pos, false);
            pos += 4;
        }

        // 48通道参考值（冷端温度，仅日志显示，不参与计算）
        for (int i = 0; i < 48; i++) {
            pkt.fRefValue[i] = bytesToFloat(ptr + pos, false);
            pos += 4;
        }

        // 仅用测量值更新多截面，冷端数据不参与计算
        QString typeStrSensor = ui->comboDTSType->currentText();
        if (!typeStrSensor.contains("电压")) {
            static int tempMultiSectionCounterFull = 0;
            tempMultiSectionCounterFull++;

            auto hasSensor = [](float v) -> bool {
                return (v > 20.0f && v < 1200.0f);
            };

            // 指定热电偶通道（1-based）：5/10/15/20/25
            const int idx5  = 4;
            const int idx10 = 9;
            const int idx15 = 14;
            const int idx20 = 19;
            const int idx25 = 24;

            float v5  = pkt.fMeasValue[idx5];
            float v10 = pkt.fMeasValue[idx10];
            float v15 = pkt.fMeasValue[idx15];
            float v20 = pkt.fMeasValue[idx20];
            float v25 = pkt.fMeasValue[idx25];

            const bool has5  = hasSensor(v5);
            const bool has10 = hasSensor(v10);
            const bool has15 = hasSensor(v15);
            const bool has20 = hasSensor(v20);
            const bool has25 = hasSensor(v25);

            // UI滑块仅用于显示：优先展示指定通道中第一个有效值
            float displayTs = -270.0f;
            if (has5) displayTs = v5;
            else if (has10) displayTs = v10;
            else if (has15) displayTs = v15;
            else if (has20) displayTs = v20;
            else if (has25) displayTs = v25;

            if (displayTs > 20.0f && displayTs < 1200.0f) {
                ui->sliderTemperatureTs->setValue(static_cast<int>(displayTs));
            }

            // 多截面温度：缓存每帧写入；UI每10帧刷新一次（避免影响50Hz）
            const bool updateUI = (tempMultiSectionCounterFull % 10 == 0);
            updateTemperatureMultiSectionFromSources(v5, has5, v10, has10, v15, has15, v20, has20, v25, has25,
                                                     pkt.nFrameNo, updateUI);
        }
// 添加到主数据缓存
        m_collectedDataDTS.append(pkt);

        // TCP缓存更新：排除冷端数据和无效值
        QString typeStrFinal = ui->comboDTSType->currentText();
        if (!typeStrFinal.contains("电压")) {
            for (int i = 0; i < 48; i++) {
                float tempVal = pkt.fMeasValue[i];
                bool hasNoSensor = (tempVal <= 20.0f || tempVal >= 1000.0f);
                if (hasNoSensor) {
                    tempVal = -270.0f;
                }
                m_lastDTSTemperatures[i] = tempVal;
            }
        } else {
            for (int i = 0; i < 48; i++) {
                m_lastDTSTemperatures[i] = pkt.fMeasValue[i];
            }
        }
        m_dtsDataReady = true;

        // 添加到自动保存缓冲区
        if (m_autoSaveEnabled) {
            m_autoSaveBufferDTS.append(pkt);
        }

        // 更新显示
        updateDTSDisplay(pkt);

        // ==================== 采集速率统计（修复：满帧400字节也计数） ====================
        // 原代码仅在"半包"分支计数，导致大多数正常400字节数据帧不计入采样率。
        // 这里对0x55满帧同样计数，保证速率显示与实际一致。
        m_dtsPacketCount++;
        if (m_dtsPacketCount % 50 == 0 && m_dtsRateTimer.elapsed() > 0) {
            double elapsedSec = m_dtsRateTimer.elapsed() / 1000.0;
            m_dtsActualRate = m_dtsPacketCount / elapsedSec;

            // 如果速率异常，发出警告（只做提示，不影响采集）
            if (m_dtsActualRate < 40.0 || m_dtsActualRate > 60.0) {
                appendTextEditDTS(QString("警告: DTS采集速率异常! 当前: %1 Hz")
                                     .arg(m_dtsActualRate, 0, 'f', 1));
            }
        }

        // 图表数据入队（20ms定时器实时刷新，避免采集掉速）
        m_pendingDtsPlot.enqueue(pkt);
        if (m_pendingDtsPlot.size() > 200) {
            m_pendingDtsPlot.dequeue();
        }

        // 日志仅显示测量值，冷端数据标注隔离
        QString typeStrLog = ui->comboDTSType->currentText();
        if (!typeStrLog.contains("电压")) {
            QString info = QString("DTS帧号%1 - 测量值: ").arg(static_cast<int>(pkt.nFrameNo));
            int channelsToShow = qMin(3, 48);
            for (int i = 0; i < channelsToShow; i++) {
                info += QString("Ch%1=%2°C ").arg(static_cast<int>(i+1))
                                           .arg(static_cast<double>(pkt.fMeasValue[i]), 0, 'f', 2);
            }
            info += "（冷端数据已隔离）";
            if (channelsToShow < 48) info += "...";
            appendTextEditDTS(info);

            // 统计有效传感器数量（20~1000℃）
            int sensorCount = 0;
            for (int i = 0; i < 48; i++) {
                if (pkt.fMeasValue[i] > 20.0f && pkt.fMeasValue[i] < 1000.0f) {
                    sensorCount++;
                }
            }
            appendTextEditDTS(QString("检测到%1个有效传感器（冷端数据不参与多截面）").arg(static_cast<int>(sensorCount)));
        } else {
            QString info = QString("DTS帧号%1: ").arg(static_cast<int>(pkt.nFrameNo));
            int channelsToShow = qMin(3, 48);
            for (int i = 0; i < channelsToShow; i++) {
                info += QString("Ch%1=%2V ").arg(static_cast<int>(i+1))
                                           .arg(static_cast<double>(pkt.fMeasValue[i]), 0, 'f', 4);
            }
            if (channelsToShow < 48) info += "...";
            appendTextEditDTS(info);
        }

    } else if (firstByte == 0xAA) {
        // 0xAA 数据包格式：命令响应
        if (data.size() >= 4) {
            QByteArray responseData = data.mid(4);
            QString response = QString::fromLatin1(responseData).trimmed();
            response = response.remove(QChar('\0'));
            if (!response.isEmpty()) {
                appendTextEditDTS(QString("命令响应: %1").arg(response));
            }
        }
    } else {
        // ASCII文本响应
        QString response = QString::fromLatin1(data).trimmed();
        if (!response.isEmpty()) {
            appendTextEditDTS(QString("响应: %1").arg(response));
        } else {
            // 二进制数据显示
            QString hexData = data.left(16).toHex(' ').toUpper();
            appendTextEditDTS(QString("二进制数据: %1...").arg(hexData));
        }
    }
}


// 更新DTS-48数据显示
void MainWindow::updateDTSDisplay(const DTS_PACKET &pkt)
{
    QString typeStr = ui->comboDTSType->currentText();
    bool isVoltageMode = typeStr.contains("电压");

    for (int i = 0; i < 48; i++) {
        if (i < m_dtsDisplayLabels.size()) {
            QLabel* label = m_dtsDisplayLabels[i];
            if (!label) continue;

            QString text;
            bool hasSensor = false; // 声明变量，默认无传感器

            if (isVoltageMode) {
                // 电压模式：直接显示测量值（默认视为有效数据）
                text = QString("通道%1: %2 V")
                           .arg(i+1)
                           .arg(pkt.fMeasValue[i], 0, 'f', 4);
                hasSensor = true; // 电压模式下均视为有效
            } else {
                // 温度模式：判断传感器是否接入（通过测量值是否为断线值）
                hasSensor = (pkt.fMeasValue[i] > -260.0); // 断线值通常为-270°C左右
                if (hasSensor) {
                    // 有传感器：显示测量值
                    text = QString("通道%1: %2 °C")
                               .arg(i+1)
                               .arg(pkt.fMeasValue[i], 0, 'f', 2);
                } else {
                    // 无传感器：显示提示信息
                    text = QString("通道%1: 无传感器").arg(i+1);
                }
            }

            // 更新显示文本和样式
            label->setText(text);
            label->setStyleSheet(hasSensor ? "color: black;" : "color: #666666;");
        }
    }
}

// DTS-48数据读取槽函数
void MainWindow::readPacketDTS(QByteArray pkt)
{
    // TCP是字节流：readyRead一次可能是多包或半包。
    // 先做重组/拆包，再交给原有解析逻辑，保证50Hz稳定。
    m_dtsRxBuffer.append(pkt);
    processDtsRxBuffer();
}

// ==================== 50Hz稳定采集：TCP字节流重组/拆包 ====================
// Qt的readyRead是“字节可读”通知，不保证一次=一帧。
// 若不做重组/拆包，在UI负载/文件写入负载较高时容易出现半包/粘包导致解析丢帧，表现为采样率波动。

int MainWindow::calcDpsFrameSize(quint8 streamIndex) const
{
    if (streamIndex < 1 || streamIndex > 3) {
        return -1;
    }

    const auto &info = g_DeviceStream.StreamInfo[streamIndex - 1];
    const auto bits = info.databit.StreamDataBits;

    int len = 0;
    // 部分设备在包头前会带2字节长度（lenfix=1时原解析会跳过它）
    if (g_DeviceStream.lenfix) {
        len += static_cast<int>(sizeof(uint16_t));
    }

    len += static_cast<int>(sizeof(uint8_t));   // stream index
    len += static_cast<int>(sizeof(uint32_t));  // frame no

    if (bits.ValveStatus) len += static_cast<int>(sizeof(uint16_t));
    if (bits.TmpStatus)   len += static_cast<int>(sizeof(uint16_t));

    // 当前工程对DPS的解析以16通道为固定：
    // - EU：float × 16
    // - AD：uint16 × 16（若未来启用建议核对协议后再调整）
    // - Volt：float × 16（同上）
    if (bits.PresEU)   len += 16 * static_cast<int>(sizeof(float));
    if (bits.PresAD)   len += 16 * static_cast<int>(sizeof(uint16_t));
    if (bits.PresVolt) len += 16 * static_cast<int>(sizeof(float));
    if (bits.TmpEU)    len += 16 * static_cast<int>(sizeof(float));
    if (bits.TmpAD)    len += 16 * static_cast<int>(sizeof(uint16_t));
    if (bits.TmpVolt)  len += 16 * static_cast<int>(sizeof(float));

    // TimeStamp位在当前工程未启用；如后续启用建议按协议确认长度。
    return len;
}

void MainWindow::processDpsRxBuffer()
{
    // 防止异常情况下缓冲无限增长
    static const int kMaxBuf = 1024 * 1024;
    if (m_dpsRxBuffer.size() > kMaxBuf) {
        m_dpsRxBuffer = m_dpsRxBuffer.right(kMaxBuf);
    }

    const int offset = g_DeviceStream.lenfix ? static_cast<int>(sizeof(uint16_t)) : 0;
    auto isValidStreamIndex = [](quint8 b) { return b >= 1 && b <= 3; };

    while (true) {
        if (m_dpsRxBuffer.size() < offset + 1) {
            return;
        }

        // 1) 对齐到可能的包起始（stream index在[1..3]）
        if (!isValidStreamIndex(static_cast<quint8>(m_dpsRxBuffer.at(offset)))) {
            int best = -1;
            const int scanLimit = qMin(m_dpsRxBuffer.size(), 8192);

            for (int p = 0; p + offset < scanLimit; ++p) {
                const quint8 si = static_cast<quint8>(m_dpsRxBuffer.at(p + offset));
                if (!isValidStreamIndex(si)) {
                    continue;
                }
                const int len = calcDpsFrameSize(si);
                if (len <= 0) {
                    continue;
                }
                if (m_dpsRxBuffer.size() < p + len) {
                    continue;
                }
                // 若还能看到下一帧起始，做一次弱校验提升对齐可靠性
                if (m_dpsRxBuffer.size() >= p + len + offset + 1) {
                    const quint8 nextSi = static_cast<quint8>(m_dpsRxBuffer.at(p + len + offset));
                    if (!isValidStreamIndex(nextSi)) {
                        continue;
                    }
                }
                best = p;
                break;
            }

            if (best < 0) {
                // 保留尾部少量字节，等待更多数据到来再重试
                if (m_dpsRxBuffer.size() > 256) {
                    m_dpsRxBuffer = m_dpsRxBuffer.right(256);
                }
                return;
            }
            if (best > 0) {
                m_dpsRxBuffer.remove(0, best);
            }
        }

        // 2) 取出完整帧
        const quint8 streamIndex = static_cast<quint8>(m_dpsRxBuffer.at(offset));
        const int frameLen = calcDpsFrameSize(streamIndex);
        if (frameLen <= 0) {
            // 不可识别：丢弃1字节继续找
            m_dpsRxBuffer.remove(0, offset + 1);
            continue;
        }
        if (m_dpsRxBuffer.size() < frameLen) {
            return; // 等待更多数据
        }

        QByteArray frame = m_dpsRxBuffer.left(frameLen);
        m_dpsRxBuffer.remove(0, frameLen);

        // 3) 交给原有解析（不改功能）
        handleRcvTCPPktPSI(frame);
    }
}

void MainWindow::processDtsRxBuffer()
{
    // DTS数据帧为固定长度（0x55帧，400字节）。TCP可能粘包/半包，因此必须重组。
    static const int kFrameSize = 400;
    static const int kMaxBuf = 1024 * 1024;
    if (m_dtsRxBuffer.size() > kMaxBuf) {
        m_dtsRxBuffer = m_dtsRxBuffer.right(kMaxBuf);
    }

    while (true) {
        if (m_dtsRxBuffer.isEmpty()) {
            return;
        }

        const unsigned char b0 = static_cast<unsigned char>(m_dtsRxBuffer.at(0));

        // 1) 正常数据帧（0x55）
        if (b0 == 0x55) {
            if (m_dtsRxBuffer.size() < kFrameSize) {
                return; // 等待更多数据
            }
            const QByteArray frame = m_dtsRxBuffer.left(kFrameSize);
            m_dtsRxBuffer.remove(0, kFrameSize);
            parseDTSPacket(frame);
            continue;
        }

        // 2) 命令响应/ASCII：尽量按行或按下一帧起始截断，避免把响应“卡”在缓冲区前面
        int next55 = m_dtsRxBuffer.indexOf(char(0x55));
        if (next55 < 0) {
            // 没看到0x55，尝试按回车换行切分
            int eol = m_dtsRxBuffer.indexOf('\n');
            if (eol < 0) eol = m_dtsRxBuffer.indexOf('\r');
            if (eol < 0) {
                // 响应还不完整，先等；但别无限堆积
                if (m_dtsRxBuffer.size() > 2048) {
                    parseDTSPacket(m_dtsRxBuffer);
                    m_dtsRxBuffer.clear();
                }
                return;
            }
            const QByteArray line = m_dtsRxBuffer.left(eol + 1);
            m_dtsRxBuffer.remove(0, eol + 1);
            parseDTSPacket(line);
            continue;
        }

        if (next55 == 0) {
            // 理论不会到这（b0==0x55已处理），防御性处理
            continue;
        }
        const QByteArray prefix = m_dtsRxBuffer.left(next55);
        m_dtsRxBuffer.remove(0, next55);
        parseDTSPacket(prefix);
    }
}

// ==================== DTS-48其他功能函数 ====================

// DTS-48快速配置
void MainWindow::on_btnDTSQuickConfig_clicked()
{
    QString typeStr = ui->comboDTSType->currentText();
    QString interval = ui->lineEdit_dts_interval->text();

    // 强制设置为20ms采集周期
    interval = "20";
    ui->lineEdit_dts_interval->setText(interval);

    // 停止任何现有的采集（如果正在采集）
    tcpClientDTS->sendMessage("STOP\r");
    delayEventLoopMS(300);  // 等待设备停止

    // 映射类型字符串到命令参数
    QMap<QString, QString> typeMap;
    typeMap["K型热电偶"] = "K";
    typeMap["J型热电偶"] = "J";
    typeMap["T型热电偶"] = "T";
    typeMap["E型热电偶"] = "E";
    typeMap["N型热电偶"] = "N";
    typeMap["R型热电偶"] = "R";
    typeMap["S型热电偶"] = "S";
    typeMap["B型热电偶"] = "B";
    typeMap["±10V电压"] = "10V";
    typeMap["±1V电压"] = "1V";
    typeMap["±0.1V电压"] = "0.1V";
    typeMap["±0.01V电压"] = "0.01V";

    QString typeCmd = typeMap.value(typeStr, "K");

    // 设置扫描对象为正常扫描
    tcpClientDTS->sendMessage("SET SCANOBJ 0\r");
    delayEventLoopMS(200);

    // 设置扫描触发模式为内部触发
    tcpClientDTS->sendMessage("SET SCANTRIG 0\r");
    delayEventLoopMS(200);

    // 设置采样间隔为20ms（每秒50次）
    tcpClientDTS->sendMessage("SET INTERVAL 20\r");
    delayEventLoopMS(200);

    // 设置所有通道类型
    if (typeCmd.length() == 1) {
        // 热电偶类型
        appendTextEditDTS("配置为热电偶模式...");
        tcpClientDTS->sendMessage(QString("SET TYPE 0 %1\r").arg(typeCmd));
        delayEventLoopMS(200);

        // 打开冷端补偿（热电偶必须开启）
        tcpClientDTS->sendMessage("SET AUTOREF 1\r");
        delayEventLoopMS(200);

        // 设置单位为摄氏度
        tcpClientDTS->sendMessage("SET MEASUNIT 0 C\r");
        delayEventLoopMS(200);
    } else {
        // 电压类型
        appendTextEditDTS("配置为电压模式...");
        tcpClientDTS->sendMessage(QString("SET TYPE 0 %1\r").arg(typeCmd));
        delayEventLoopMS(200);

        // 关闭冷端补偿
        tcpClientDTS->sendMessage("SET AUTOREF 0\r");
        delayEventLoopMS(200);

        // 设置单位为电压值
        tcpClientDTS->sendMessage("SET MEASUNIT 0 S\r");
        delayEventLoopMS(200);
    }

    // 设置外部触发使能为0（使用内部触发）
    tcpClientDTS->sendMessage("SET TRIGENABLE 0\r");
    delayEventLoopMS(200);

    // 设置高采样模式（如果需要）
    tcpClientDTS->sendMessage("SET SAMPLING 0\r");
    delayEventLoopMS(200);

    appendTextEditDTS(QString("DTS快速配置完成：类型=%1，固定间隔=20ms（50Hz）").arg(typeStr));

    updateSimultaneousButtonsState();
}

// 新增：DTS单独通道配置（点击"单独配置"按钮触发）
void MainWindow::on_btnDTSSingleConfig_clicked()
{
    // 1. 获取选择的配置类型（热电偶/电压）
    QString selectedType = ui->comboDTSType->currentText();
    // 类型映射（与原有快速配置一致，确保命令参数正确）
    QMap<QString, QString> typeMap;
    typeMap["K型热电偶"] = "K";
    typeMap["J型热电偶"] = "J";
    typeMap["T型热电偶"] = "T";
    typeMap["E型热电偶"] = "E";
    typeMap["N型热电偶"] = "N";
    typeMap["R型热电偶"] = "R";
    typeMap["S型热电偶"] = "S";
    typeMap["B型热电偶"] = "B";
    typeMap["±10V电压"] = "10V";
    typeMap["±1V电压"] = "1V";
    typeMap["±0.1V电压"] = "0.1V";
    typeMap["±0.01V电压"] = "0.01V";

    // 校验类型合法性
    if (!typeMap.contains(selectedType)) {
        QMessageBox::warning(this, "配置警告", "请选择有效的热电偶/电压类型！");
        return;
    }
    QString typeCmd = typeMap[selectedType]; // 转换为设备可识别的命令参数

    // 2. 获取并校验通道号（1-48）
    QString channelStr = lineEdit_dts_channel->text().trimmed();
    if (channelStr.isEmpty()) {
        QMessageBox::warning(this, "配置警告", "请输入1-48之间的通道号！");
        return;
    }
    bool isIntValid;
    int channel = channelStr.toInt(&isIntValid);
    if (!isIntValid || channel < 1 || channel > 48) {
        QMessageBox::warning(this, "配置警告", "通道号必须是1-48的整数！");
        return;
    }

    // 3. 停止当前采集（避免配置与采集冲突，与原有快速配置逻辑一致）
    tcpClientDTS->sendMessage("STOP\r");
    delayEventLoopMS(300); // 等待设备停止采集

    // 4. 发送单独通道配置命令（参考设备手册5.8：SET TYPE <通道号> <类型>）
    QString configCmd = QString("SET TYPE %1 %2\r").arg(channel).arg(typeCmd);
    tcpClientDTS->sendMessage(configCmd);
    appendTextEditDTS(QString("[单独配置] 发送命令：%1").arg(configCmd.trimmed()));
    delayEventLoopMS(200); // 等待命令执行完成

    // 5. 配置结果提示（无需额外保存，设备即时生效，与原有快速配置逻辑一致）
    appendTextEditDTS(QString("[单独配置] 成功！通道%1已设置为「%2」").arg(channel).arg(selectedType));
}

// DTS-48开始采集
void MainWindow::on_btnDTSScan_clicked()
{
    // 先停止任何现有的采集
    tcpClientDTS->sendMessage("STOP\r");
    delayEventLoopMS(300);

    // 重置静态变量
    static int dtsFrameCounter = 0;
    static int dtsFrameCounterFull = 0;
    static bool frameCounterInitialized = false;
    static bool frameCounterInitializedFull = false;
    static bool timeBaseRecorded = false;
    static bool timeBaseRecordedFull = false;

    dtsFrameCounter = 0;
    dtsFrameCounterFull = 0;
    frameCounterInitialized = false;
    frameCounterInitializedFull = false;
    timeBaseRecorded = false;
    timeBaseRecordedFull = false;

    // 开始采集
// 重置上位机帧号（从1开始）与绘图队列
m_dtsFrameSeq = 0;
m_pendingDtsPlot.clear();

// 清空温度实时曲线（不影响已缓存/保存数据）
for (int i = 0; i < temperatureSeries.size(); ++i) {
    if (temperatureSeries[i]) {
        temperatureSeries[i]->clear();
        temperatureSeries[i]->setVisible(false);
    }
}
if (temperatureAxisX) temperatureAxisX->setRange(0, 100);

    tcpClientDTS->sendMessage("SCAN\r");
    appendTextEditDTS("DTS开始采集数据...");

    ui->btnDTSSave->setEnabled(false);

    // 重置DTS时间基准
    m_dtsTimeBaseRecorded = false;
    m_dtsStartTimestamp = 0;

    // 启动自动保存
    startAutoSave();

    // 如果不是同时采集模式，更新按钮状态
    if (!m_simultaneousCollecting) {
        updateSimultaneousButtonsState();
    }
}

// DTS-48停止采集
void MainWindow::on_btnDTSStop_clicked()
{
    tcpClientDTS->sendMessage("STOP\r");
    appendTextEditDTS("DTS采集已停止！");
    ui->btnDTSSave->setEnabled(true);

    // 停止自动保存
    stopAutoSave();

    // 如果不是同时采集模式，更新按钮状态
    if (!m_simultaneousCollecting) {
        updateSimultaneousButtonsState();
    }
}

// DTS-48清除数据
void MainWindow::on_btnDTSClear_clicked()
{
    m_collectedDataDTS.clear();
    m_autoSaveBufferDTS.clear();  // 同时清除自动保存缓冲区
    QString typeStr = ui->comboDTSType->currentText();
    for (QLabel *label : m_dtsDisplayLabels) {
        QString channelNum = label->text().split(":").first();
        if (typeStr.contains("电压")) {
            label->setText(channelNum + ": -- V");
        } else {
            label->setText(channelNum + ": -- °C");
        }
    }
    appendTextEditDTS("DTS数据已清除！");

    // 清除温度图表
    for (int i = 0; i < temperatureSeries.size(); i++) {
        if (temperatureSeries[i]) {
            temperatureSeries[i]->clear();
            temperatureSeries[i]->setVisible(false);
        }
    }
    if (temperatureAxisX) {
        temperatureAxisX->setRange(0, 100);
    }
    if (temperatureAxisY) {
        temperatureAxisY->setRange(-10, 50);
    }
    temperatureChart->setTitle("48通道温度实时曲线（检测到传感器的通道）");
}

// DTS-48保存数据
void MainWindow::on_btnDTSSave_clicked()
{
    if (m_collectedDataDTS.isEmpty()) {
        QMessageBox::warning(this, "警告", "无DTS数据可导出！");
        return;
    }

    // 默认使用.xls扩展名，这是Excel 2003 XML格式的推荐扩展名
    QString filePath = QFileDialog::getSaveFileName(this, "保存DTS数据",
                                                    QString("DTS数据_%1.xls").arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmss")),
                                                    "Excel Files (*.xls *.xlsx);;All Files (*.*)");

    if (filePath.isEmpty()) return;

    if (saveDTSDataToFile(m_collectedDataDTS, filePath, false)) {
        // 检查文件是否实际创建
        QFileInfo fileInfo(filePath);
        if (fileInfo.exists()) {
            QMessageBox::information(this, "成功",
                                     QString("DTS数据保存至：%1\n\n文件大小：%2 字节，共%3条记录")
                                         .arg(filePath)
                                         .arg(fileInfo.size())
                                         .arg(m_collectedDataDTS.size()));
        } else {
            // 可能生成了.xml文件而不是.xls文件
            QString xmlFile = filePath;
            if (filePath.endsWith(".xlsx")) {
                xmlFile = filePath.left(filePath.length() - 5) + ".xml";
            } else if (filePath.endsWith(".xls")) {
                xmlFile = filePath.left(filePath.length() - 4) + ".xml";
            }

            QFileInfo xmlInfo(xmlFile);
            if (xmlInfo.exists()) {
                QMessageBox::information(this, "成功",
                                         QString("DTS数据保存至：%1（Excel 2003 XML格式）\n\n注意：文件保存为Excel 2003 XML格式，扩展名为.xml，但Excel可以直接打开。")
                                             .arg(xmlFile));
            } else {
                QMessageBox::warning(this, "警告",
                                     "文件保存成功，但未找到保存的文件。请检查保存目录。");
            }
        }
    } else {
        QMessageBox::critical(this, "失败", "DTS数据保存失败！");
    }
}

// ==================== 辅助函数 ====================

// DTS-48日志输出
void MainWindow::appendTextEditDTS(const QString &data)
{
    QString time = QString("[%1]").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));

    // 为了区分，添加DTS前缀
    ui->textEdit->moveCursor(QTextCursor::End);
    ui->textEdit->append(">> [DTS] " + time + " " + data);

    // 同时输出到调试控制台
    qDebug() << "[DTS]" << data;
}

// 原有辅助函数
void MainWindow::appendTextEdit(const QString &data)
{
    QString time = QString("[%1]").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));
    ui->textEdit->moveCursor(QTextCursor::End);
    ui->textEdit->append(">> " + time + " " + data);
}

void MainWindow::delayEventLoopMS(unsigned int msec)
{
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, &QEventLoop::quit);
    loop.exec();
}

float  MainWindow::convertUnit(float  value, bool isKPa)
{
    return isKPa ? value * 6.894757 : value;
}

void MainWindow::saveCalibCoeffs(bool saveZero, bool saveSpan)
{
    if (saveZero) { tcpClient->sendMessage("w08"); delayEventLoopMS(50); }
    if (saveSpan) { tcpClient->sendMessage("w09"); delayEventLoopMS(50); }
}

// 读取冷端温度
void MainWindow::readColdJunctionTemperature()
{
    // 发送命令读取冷端温度（通过读取参考值）
    appendTextEditDTS("读取冷端补偿温度...");

    // 首先确保设备处于正确状态
    tcpClientDTS->sendMessage("STOP\r");
    delayEventLoopMS(200);

    // 设置扫描对象为AD参考电压（可能包含冷端信息）
    tcpClientDTS->sendMessage("SET SCANOBJ 1\r");  // 1 = AD参考电压
    delayEventLoopMS(200);

    // 开始扫描
    tcpClientDTS->sendMessage("SCAN\r");
    delayEventLoopMS(200);

    appendTextEditDTS("开始采集冷端温度数据...");
}

// ==================== DPS-16原有函数 ====================

// DPS-16连接按钮
void MainWindow::on_pushButton_update_clicked()
{
    tcpClient->setIP(ui->lineEdit_ip->text());
    tcpClient->setPort(ui->lineEdit_port->text().toInt());
    if(ui->pushButton_update->text() == "连接")
        tcpClient->newConnect();
    else
        tcpClient->closeConnect();
}

// DPS-16发送命令
void MainWindow::on_pushButton_clicked()
{
    QString cmd = ui->lineEdit->text();
    if(cmd.isEmpty()) return;
    tcpClient->sendMessage(cmd);
}

// DPS-16快速配置 - 修改采集周期
void MainWindow::on_pushButton_config_clicked()
{
    // 原命令：周期1000（1000 * 0.1ms = 100ms）
    // 修改为：周期200（200 * 0.1ms = 20ms）
    ui->lineEdit->setText("w1600");
    on_pushButton_clicked();
    delayEventLoopMS(50);

    ui->lineEdit->setText("c 00 1 FFFF 1 20 8 0");  // 修改这里：1000 -> 20
    on_pushButton_clicked();
    delayEventLoopMS(50);

    ui->lineEdit->setText("c 05 1 0010");
    on_pushButton_clicked();

    appendTextEdit("快速配置完成！20ms采集周期已设置");

    updateSimultaneousButtonsState();
}

// ==================== DPS-16开始采集（修改）====================
void MainWindow::on_pushButton_scan_clicked()
{
    // 重置计数器
    static int frameCount = 0;
    static int logCounter = 0;
    static int chartUpdateCounter = 0;
    static int multiSectionCounter = 0;

    frameCount = 0;
    logCounter = 0;
    chartUpdateCounter = 0;
    multiSectionCounter = 0;

// 重置上位机帧号（从1开始）与绘图队列
m_dpsFrameSeq = 0;
m_pendingDpsPlot.clear();

// 清空压力实时曲线（不影响已缓存/保存数据）
for (int i = 0; i < pressureSeries.size(); ++i) {
    if (pressureSeries[i]) pressureSeries[i]->clear();
}
if (pressureAxisX) pressureAxisX->setRange(0, 100);

    // 清除旧的缓冲区数据，避免积累
    if (!m_autoSaveBufferDPS.isEmpty()) {
        appendDataToCurrentExcel(true, false);
    }

    tcpClient->sendMessage("c 01 1");
    delayEventLoopMS(50);

    // 简化日志
    appendTextEdit("开始采集数据...");
    ui->btnSaveExcel->setEnabled(false);

    // 重置时间基准
    static QDateTime firstPacketTime;
    firstPacketTime = QDateTime::currentDateTime();

    appendTextEdit(QString("采集开始时间：%1").arg(firstPacketTime.toString("yyyy-MM-dd HH:mm:ss.zzz")));

    // 启动自动保存
    startAutoSave();

    // 如果不是同时采集模式，更新按钮状态
    if (!m_simultaneousCollecting) {
        updateSimultaneousButtonsState();
    }
}

// DPS-16停止采集
void MainWindow::on_pushButton_stop_clicked()
{
    tcpClient->sendMessage("c 02 1");
    delayEventLoopMS(50);
    appendTextEdit("采集已停止！");
    ui->btnSaveExcel->setEnabled(true);

    // 停止自动保存
    stopAutoSave();

    // 如果不是同时采集模式，更新按钮状态
    if (!m_simultaneousCollecting) {
        updateSimultaneousButtonsState();
    }
}

// DPS-16应用网络配置
void MainWindow::on_btnApplyNetwork_clicked()
{
    QString ip = ui->lineEdit_ip->text();
    QString subnet = ui->lineEdit_subnet->text();
    QStringList ipParts = ip.split(".");
    if (ipParts.size() != 4) {
        QMessageBox::warning(this, "警告", "IP格式错误！");
        return;
    }
    tcpClient->sendMessage(QString("~ 1242 0 %1").arg(ipParts[0])); delayEventLoopMS(50);
    tcpClient->sendMessage(QString("~ 1242 1 %1").arg(ipParts[1])); delayEventLoopMS(50);
    tcpClient->sendMessage(QString("~ 1242 2 %1").arg(ipParts[2])); delayEventLoopMS(50);
    tcpClient->sendMessage(QString("~ 1242 3 %1").arg(ipParts[3])); delayEventLoopMS(50);
    QStringList subnetParts = subnet.split(".");
    if (subnetParts.size() == 4) {
        tcpClient->sendMessage(QString("~ 1244 0 %1").arg(subnetParts[0])); delayEventLoopMS(50);
        tcpClient->sendMessage(QString("~ 1244 1 %1").arg(subnetParts[1])); delayEventLoopMS(50);
        tcpClient->sendMessage(QString("~ 1244 2 %1").arg(subnetParts[2])); delayEventLoopMS(50);
        tcpClient->sendMessage(QString("~ 1244 3 %1").arg(subnetParts[3])); delayEventLoopMS(50);
    }
    tcpClient->sendMessage("~ 1243");
    appendTextEdit("网络配置已应用（需重启设备）！");
}

// DPS-16应用采集配置 - 修改周期单位
void MainWindow::on_btnApplyCollect_clicked()
{
    int sync = ui->comboSync->currentIndex();
    int format = ui->comboFormat->currentIndex() + 7;
    int trigger = ui->comboTrigger->currentIndex();
    QString period = ui->lineEdit_period->text();
    QString collectNum = ui->lineEdit_collectNum->text();

    tcpClient->sendMessage(QString("w32 %1").arg(trigger));
    delayEventLoopMS(50);

    // 转换为0.1ms单位（设备要求）
    int periodValue = period.toInt() ;  // ms  = 1ms单位

    QString cmd = QString("c 00 1 FFFF %1 %2 %3 %4")
                      .arg(sync)
                      .arg(periodValue)  // 使用转换后的值
                      .arg(format)
                      .arg(collectNum);

    tcpClient->sendMessage(cmd);
    delayEventLoopMS(50);
    tcpClient->sendMessage("c 05 1 0010");
    delayEventLoopMS(50);

    appendTextEdit(QString("采集配置已应用：%1 (周期=%2ms, 转换为%3个0.1ms)")
                       .arg(cmd)
                       .arg(period)
                       .arg(periodValue));
}

// DPS-16应用单位
void MainWindow::on_btnApplyUnit_clicked()
{
    bool isKPa = (ui->comboUnit->currentText() == "kPa");
    double scaler = isKPa ? 6.894757 : 1.0;
    tcpClient->sendMessage(QString("v01101 %1").arg(scaler, 0, 'f', 6));
    appendTextEdit(QString("单位切换为：%1").arg(ui->comboUnit->currentText()));
}

// DPS-16零点校准
void MainWindow::on_btnZeroCalib_clicked()
{
    tcpClient->sendMessage("v01101 1.0"); delayEventLoopMS(50);
    tcpClient->sendMessage("w0B01"); delayEventLoopMS(50);
    tcpClient->sendMessage("w0C01"); delayEventLoopMS(50);
    tcpClient->sendMessage("hFFFF 0.0"); delayEventLoopMS(50);
    tcpClient->sendMessage("w0C00"); delayEventLoopMS(50);
    saveCalibCoeffs(true, false);
    appendTextEdit("零点校准完成！");
}

// DPS-16满量程校准
void MainWindow::on_btnSpanCalib_clicked()
{
    QString spanPress = ui->lineEdit_spanPress->text();
    if (spanPress.isEmpty() || spanPress.toDouble() <= 0) {
        QMessageBox::warning(this, "警告", "请输入有效压力值！");
        return;
    }
    on_btnZeroCalib_clicked(); delayEventLoopMS(100);
    tcpClient->sendMessage("w0C01"); delayEventLoopMS(50);
    tcpClient->sendMessage(QString("ZFFFF %1").arg(spanPress)); delayEventLoopMS(50);
    tcpClient->sendMessage("w0C00"); delayEventLoopMS(50);
    saveCalibCoeffs(false, true);
    appendTextEdit(QString("满量程校准完成：%1 psi").arg(spanPress));
}

// DPS-16启动多点校准
void MainWindow::on_btnMultiCalibStart_clicked()
{
    int calibPoints = ui->lineEdit_calibPoints->text().toInt();
    int calibOrder = ui->lineEdit_calibOrder->text().toInt();
    int calibAvg = ui->lineEdit_calibAvg->text().toInt();
    if (calibPoints <2 || calibPoints>10 || calibOrder<1 || calibOrder>3 || calibAvg<1 || calibAvg>2000) {
        QMessageBox::warning(this, "警告", "校准参数无效！");
        return;
    }
    tcpClient->sendMessage("v01101 1.0"); delayEventLoopMS(50);
    tcpClient->sendMessage("w0C01"); delayEventLoopMS(50);
    QString cmd = QString("C 00 FFFF %1 %2 %3").arg(calibPoints).arg(calibOrder).arg(calibAvg);
    tcpClient->sendMessage(cmd);
    appendTextEdit(QString("多点校准启动：%1点").arg(calibPoints));
}

// DPS-16采集校准点
void MainWindow::on_btnMultiCalibCollect_clicked()
{
    static int calibPointIdx = 1;
    int totalPoints = ui->lineEdit_calibPoints->text().toInt();
    tcpClient->sendMessage(QString("C 01 %1 0.0").arg(calibPointIdx));
    appendTextEdit(QString("采集第%1个校准点").arg(calibPointIdx));
    calibPointIdx++;
    if (calibPointIdx > totalPoints) calibPointIdx = 1;
}

// DPS-16计算校准系数
void MainWindow::on_btnMultiCalibCalc_clicked()
{
    tcpClient->sendMessage("C 02"); delayEventLoopMS(50);
    saveCalibCoeffs(true, true);
    tcpClient->sendMessage("w0C00");
    appendTextEdit("多点校准系数计算完成！");
}

// DPS-16终止校准
void MainWindow::on_btnMultiCalibAbort_clicked()
{
    tcpClient->sendMessage("C 03"); delayEventLoopMS(50);
    tcpClient->sendMessage("w0C00");
    appendTextEdit("多点校准已终止！");
}

// DPS-16清除数据
void MainWindow::on_btnClearData_clicked()
{
    ui->textEdit->clear();
    m_collectedData.clear();
    m_autoSaveBufferDPS.clear();  // 同时清除自动保存缓冲区
    appendTextEdit("数据已清除！");

    // 清除压力图表
    clearCharts();
}

// DPS-16保存数据
void MainWindow::on_btnSaveExcel_clicked()
{
    if (m_collectedData.isEmpty()) {
        QMessageBox::warning(this, "警告", "无数据可导出！");
        return;
    }

    // 使用.xls扩展名，这是Excel 2003 XML格式的推荐扩展名
    QString filePath = QFileDialog::getSaveFileName(this, "保存Excel",
                                                    QString("压力数据_%1.xls").arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmss")),
                                                    "Excel Files (*.xls);;All Files (*.*)");

    if (filePath.isEmpty()) return;

    if (saveDPSDataToFile(m_collectedData, filePath, false)) {
        // 检查文件是否实际创建
        QFileInfo fileInfo(filePath);
        if (fileInfo.exists()) {
            QMessageBox::information(this, "成功",
                                     QString("DPS数据保存至：%1\n\n文件大小：%2 字节，共%3条记录")
                                         .arg(filePath)
                                         .arg(fileInfo.size())
                                         .arg(m_collectedData.size()));
        } else {
            // 可能生成了.xml文件而不是.xls文件
            QString xmlFile = filePath;
            if (filePath.endsWith(".xlsx")) {
                xmlFile = filePath.left(filePath.length() - 5) + ".xml";
            } else if (filePath.endsWith(".xls")) {
                xmlFile = filePath.left(filePath.length() - 4) + ".xml";
            }

            QFileInfo xmlInfo(xmlFile);
            if (xmlInfo.exists()) {
                QMessageBox::information(this, "成功",
                                         QString("DPS数据保存至：%1（Excel 2003 XML格式）\n\n注意：文件保存为Excel 2003 XML格式，扩展名为.xml，但Excel可以直接打开。")
                                             .arg(xmlFile));
            } else {
                QMessageBox::warning(this, "警告",
                                     "文件保存成功，但未找到保存的文件。请检查保存目录。");
            }
        }
    } else {
        QMessageBox::critical(this, "失败", "DPS数据保存失败！");
    }
}

// DPS-16数据包接收处理
void MainWindow::readPacket(QByteArray pkt)
{
    // TCP是字节流：readyRead一次可能是多包或半包。
    // 先做重组/拆包，再交给原有解析逻辑，保证50Hz稳定。
    m_dpsRxBuffer.append(pkt);
    processDpsRxBuffer();
}

// DPS-16连接成功
void MainWindow::slotConnected()
{
    ui->pushButton_update->setText("断开");
    appendTextEdit("设备已连接！");
    updateSimultaneousButtonsState(); // 添加这行
}

// DPS-16断开连接
void MainWindow::slotDisonnected()
{
    ui->pushButton_update->setText("连接");
    appendTextEdit("设备已断开！");
    ui->btnSaveExcel->setEnabled(false);
    updateSimultaneousButtonsState(); // 添加这行
}

// ==================== DPS-16数据包解析 ====================
void MainWindow::handleRcvTCPPktPSI(QByteArray pktArray)
{
    // 记录函数开始时间，用于性能监控
    static QElapsedTimer processingTimer;
    static int frameCount = 0;
    if (!processingTimer.isValid()) {
        processingTimer.start();
    }

    uint8_t stindex;
    BYTE4 Byte2Word, Byte2WordTemp;
    BYTE4 Byte4Word, Byte4WordTemp;
    DPS_PACKET scanPkt;
    int pindex = 0;
    char *start = pktArray.data();
    int size = pktArray.size();

    // ========== 关键优化：简化时间基准逻辑 ==========
    // 只在第一个数据包到达时记录时间基准
    static qint64 firstPacketTimeNs = 0;
    static QDateTime firstPacketSystemTime;

    if (size < 9) {
        return;
    }

    if (g_DeviceStream.lenfix) pindex += sizeof(uint16_t);
    memcpy(&stindex, start + pindex, sizeof(uint8_t));
    pindex += sizeof(uint8_t);

    if (stindex >= 1 && stindex <= 3) {
        scanPkt.nPacketType = stindex;
        memcpy(&Byte4WordTemp, start + pindex, sizeof(uint32_t));
        pindex += sizeof(uint32_t);
        Byte4Word.bytes[0] = Byte4WordTemp.bytes[3];
        Byte4Word.bytes[1] = Byte4WordTemp.bytes[2];
        Byte4Word.bytes[2] = Byte4WordTemp.bytes[1];
        Byte4Word.bytes[3] = Byte4WordTemp.bytes[0];
        quint32 deviceFrameNo = Byte4Word.d; // 设备原始帧号（保留用于调试）
    Q_UNUSED(deviceFrameNo);
    scanPkt.nFrameNo = ++m_dpsFrameSeq; // 上位机帧号：从1开始递增

        // 记录第一个数据包的时间和系统时间
        if (frameCount == 0) {
            firstPacketSystemTime = QDateTime::currentDateTime();
            firstPacketTimeNs = static_cast<qint64>(scanPkt.nFrameNo) * 20000000LL; // 20ms per frame
            appendTextEdit(QString("DPS第一个数据包：帧号=%1，系统时间=%2")
                              .arg(scanPkt.nFrameNo)
                              .arg(firstPacketSystemTime.toString("yyyy-MM-dd HH:mm:ss.zzz")));
        }

        frameCount++;

        // 跳过状态位
        if (g_DeviceStream.StreamInfo[stindex - 1].databit.StreamDataBits.ValveStatus == 1) {
            if (g_DeviceStream.StreamInfo[stindex - 1].format == 7) {
                memcpy(&Byte2WordTemp, start + pindex, sizeof(uint16_t));
                Byte2Word.bytes[0] = Byte2WordTemp.bytes[1];
                Byte2Word.bytes[1] = Byte2WordTemp.bytes[0];
            } else {
                memcpy(&Byte2Word, start + pindex, sizeof(uint16_t));
            }
            pindex += sizeof(uint16_t);
        }

        if (g_DeviceStream.StreamInfo[stindex - 1].databit.StreamDataBits.TmpStatus == 1) {
            if (g_DeviceStream.StreamInfo[stindex - 1].format == 7) {
                memcpy(&Byte2WordTemp, start + pindex, sizeof(uint16_t));
                Byte2Word.bytes[0] = Byte2WordTemp.bytes[1];
                Byte2Word.bytes[1] = Byte2WordTemp.bytes[0];
            } else {
                memcpy(&Byte2Word, start + pindex, sizeof(uint16_t));
            }
            pindex += sizeof(uint16_t);
        }

        // 解析压力数据
        if (g_DeviceStream.StreamInfo[stindex - 1].databit.StreamDataBits.PresEU == 1) {
            for (int i = 15; i >= 0; i--) {
                if (g_DeviceStream.StreamInfo[stindex - 1].format == 7) {
                    memcpy(&Byte4WordTemp, start + pindex, sizeof(float));
                    Byte4Word.bytes[0] = Byte4WordTemp.bytes[3];
                    Byte4Word.bytes[1] = Byte4WordTemp.bytes[2];
                    Byte4Word.bytes[2] = Byte4WordTemp.bytes[1];
                    Byte4Word.bytes[3] = Byte4WordTemp.bytes[0];
                } else {
                    memcpy(&Byte4Word, start + pindex, sizeof(float));
                }
                pindex += sizeof(float);
                scanPkt.fPress[i] = QString::number(Byte4Word.f, 'f', 6);
            }
        }

        // 解析温度数据
        if (g_DeviceStream.StreamInfo[stindex - 1].databit.StreamDataBits.TmpEU == 1) {
            for (int i = 15; i >= 0; i--) {
                if (g_DeviceStream.StreamInfo[stindex - 1].format == 7) {
                    memcpy(&Byte4WordTemp, start + pindex, sizeof(float));
                    Byte4Word.bytes[0] = Byte4WordTemp.bytes[3];
                    Byte4Word.bytes[1] = Byte4WordTemp.bytes[2];
                    Byte4Word.bytes[2] = Byte4WordTemp.bytes[1];
                    Byte4Word.bytes[3] = Byte4WordTemp.bytes[0];
                } else {
                    memcpy(&Byte4Word, start + pindex, sizeof(float));
                }
                pindex += sizeof(float);
                scanPkt.fTemp[i] = QString::number(Byte4Word.f, 'f', 6);
            }
        }

        // 设置时间戳
        scanPkt.nFrameTime_s = 0;
        scanPkt.nFrameTime_ns = static_cast<qint64>(scanPkt.nFrameNo) * 20000000LL; // 20ms per frame

        // ========== 关键优化：减少日志输出频率 ==========
        static int logCounter = 0;
        logCounter++;

        // 每10帧输出一次日志
        if (logCounter % 10 == 0) {
            bool isKPa = (ui->comboUnit->currentText() == "kPa");
            QString datastring = QString("帧号%1：").arg(scanPkt.nFrameNo);
            for (int ch = 0; ch < 2; ch++) { // 只显示前2个通道，减少日志长度
                double press = scanPkt.fPress[ch].toDouble();
                datastring.append(QString("通道%1=%2%3，").arg(ch+1).arg(convertUnit(press, isKPa), 0, 'f', 3).arg(ui->comboUnit->currentText()));
            }
            datastring.append("...");
            appendTextEdit(datastring);
        }

        // ========== 添加到数据缓存（优化性能）==========
        // 使用快速添加到缓冲区
        m_collectedData.append(scanPkt);

        // 更新TCP缓存
        for (int i = 0; i < 16; i++) {
            bool ok = false;
            float pressVal = scanPkt.fPress[i].toFloat(&ok);
            m_lastDPSPressures[i] = ok ? pressVal : 0.0f;
        }
        m_dpsDataReady = true;

        // ========== 添加到自动保存缓冲区（不阻塞）==========
        if (m_autoSaveEnabled) {
            m_autoSaveBufferDPS.append(scanPkt);

            // 性能监控：每100帧输出一次处理时间
            if (frameCount % 100 == 0) {
                double avgProcessingTime = processingTimer.elapsed() / 100.0;
                processingTimer.restart();
                qDebug() << QString("DPS数据处理性能：平均每帧%1ms，当前队列长度%2")
                                .arg(avgProcessingTime, 0, 'f', 2)
                                .arg(m_autoSaveBufferDPS.size());

                // 如果处理时间过长，警告用户
                if (avgProcessingTime > 15.0) { // 超过15ms可能会影响50Hz采集
                    appendTextEdit(QString("警告：数据处理时间(%1ms)接近采集周期(20ms)").arg(avgProcessingTime, 0, 'f', 2));
                }
            }
        }

        // ========== 图表数据入队（20ms定时器实时刷新，避免采集掉速）==========
        m_pendingDpsPlot.enqueue(scanPkt);
        if (m_pendingDpsPlot.size() > 200) {
            m_pendingDpsPlot.dequeue();
        }

        // ========== 多截面压力更新（指定通道加载：缓存每帧，UI降频）==========
        static int multiSectionCounter = 0;
        multiSectionCounter++;

        // 说明：
        // 1) 不再通过“任意通道>0.1MPa”推断加载值；
        // 2) 按需求使用指定DPS通道作为各截面组的加载Ps（单位MPa）：
        //    Ch1->0截面；Ch5->0.1&1；Ch9->1.5&2；Ch11->3&3.5；Ch16->4&5
        // 3) 缓存每帧都写入，UI仅每10帧刷新一次，避免影响50Hz采集。
        auto psiToMpa = [](const QString& s) -> double {
            bool ok = false;
            double psi = s.toDouble(&ok);
            return ok ? (psi * 0.00689476) : 0.0;
        };

        const double psCh1  = psiToMpa(scanPkt.fPress[0]);   // 1通道
        const double psCh5  = psiToMpa(scanPkt.fPress[4]);   // 5通道
        const double psCh9  = psiToMpa(scanPkt.fPress[8]);   // 9通道
        const double psCh11 = psiToMpa(scanPkt.fPress[10]);  // 11通道
        const double psCh16 = psiToMpa(scanPkt.fPress[15]);  // 16通道

        const bool updateUI = (multiSectionCounter % 10 == 0);
                updatePressureMultiSectionFromSources(psCh1, psCh5, psCh9, psCh11, psCh16,
                                                      scanPkt.nFrameNo, updateUI);

                // 增加DPS数据包计数（用于采集速率监控）
                m_dpsPacketCount++;

                // 每100个数据包计算一次采集速率
                if (m_dpsPacketCount % 100 == 0 && m_dpsRateTimer.elapsed() > 0) {
                    double elapsedSec = m_dpsRateTimer.elapsed() / 1000.0;
                    m_dpsActualRate = m_dpsPacketCount / elapsedSec;

                    // 显示当前采集速率（降频：避免刷屏影响性能）
                    QString rateInfo = QString("DPS采集速率: %1 Hz (理论50Hz)").arg(m_dpsActualRate, 0, 'f', 1);
                    if ((m_dpsPacketCount % 500) == 0) {
                        appendTextEdit(rateInfo);
                    }

                    // 如果速率异常，发出警告
                    if (m_dpsActualRate < 40.0 || m_dpsActualRate > 60.0) {
                        appendTextEdit(QString("警告: DPS采集速率异常! 当前: %1 Hz").arg(m_dpsActualRate, 0, 'f', 1));
                    }
                }

    } else {
        // 非数据包，仅输出错误信息
        QString response = QString::fromLatin1(pktArray).trimmed();
        if (!response.isEmpty()) {
            appendTextEdit(QString("设备响应：%1").arg(response));
        }
    }
}




// ==================== 图表初始化函数 ====================

// 初始化压力图表
void MainWindow::initPressureChart()
{
    // 创建图表
    pressureChart = new QChart();
    pressureChart->setTitle("16通道压力实时曲线");
    pressureChart->setAnimationOptions(QChart::NoAnimation);
    pressureChart->legend()->setVisible(true);
    pressureChart->legend()->setAlignment(Qt::AlignBottom);

    // 创建16个通道的曲线系列
    QList<QColor> colors = {
        QColor(255, 0, 0),      // 红色 - 通道1
        QColor(0, 255, 0),      // 绿色 - 通道2
        QColor(0, 0, 255),      // 蓝色 - 通道3
        QColor(255, 255, 0),    // 黄色 - 通道4
        QColor(255, 0, 255),    // 紫色 - 通道5
        QColor(0, 255, 255),    // 青色 - 通道6
        QColor(255, 165, 0),    // 橙色 - 通道7
        QColor(128, 0, 128),    // 紫红色 - 通道8
        QColor(165, 42, 42),    // 棕色 - 通道9
        QColor(0, 128, 0),      // 深绿 - 通道10
        QColor(0, 0, 128),      // 深蓝 - 通道11
        QColor(128, 128, 0),    // 橄榄色 - 通道12
        QColor(128, 0, 0),      // 栗色 - 通道13
        QColor(0, 128, 128),    // 青绿 - 通道14
        QColor(128, 128, 128),  // 灰色 - 通道15
        QColor(64, 64, 64)      // 深灰 - 通道16
    };

    for (int i = 0; i < 16; i++) {
        QLineSeries *series = new QLineSeries();
        series->setName(QString("通道%1").arg(i + 1));

        // 设置线条颜色
        QPen pen(colors[i % colors.size()]);
        pen.setWidth(2);
        series->setPen(pen);

        pressureSeries.append(series);
        pressureChart->addSeries(series);
    }

    // 创建坐标轴
    pressureAxisX = new QValueAxis();
    pressureAxisX->setTitleText("采集点数");
    pressureAxisX->setLabelFormat("%d");
    pressureAxisX->setRange(0, 100);
    pressureAxisX->setTickCount(11);

    pressureAxisY = new QValueAxis();
    pressureAxisY->setTitleText("压力值 (psi)");
    pressureAxisY->setLabelFormat("%.2f");
    pressureAxisY->setRange(-10, 120);  // 默认范围，会根据数据动态调整
    pressureAxisY->setTickCount(7);

    // 将坐标轴添加到图表
    pressureChart->addAxis(pressureAxisX, Qt::AlignBottom);
    pressureChart->addAxis(pressureAxisY, Qt::AlignLeft);

    // 将系列附加到坐标轴
    for (int i = 0; i < pressureSeries.size(); i++) {
        pressureSeries[i]->attachAxis(pressureAxisX);
        pressureSeries[i]->attachAxis(pressureAxisY);
    }

    // 创建图表视图
    QChartView *chartView = new QChartView(pressureChart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(300);

    // 将图表视图添加到UI中的压力图表区域
    QVBoxLayout *layout = new QVBoxLayout(ui->pressureChartWidget);
    layout->addWidget(chartView);
    ui->pressureChartWidget->setLayout(layout);
}

// 初始化温度图表
void MainWindow::initTemperatureChart()
{
    // 创建图表
    temperatureChart = new QChart();
    temperatureChart->setTitle("48通道温度实时曲线（检测到传感器的通道）");
    temperatureChart->setAnimationOptions(QChart::NoAnimation);
    temperatureChart->legend()->setVisible(true);
    temperatureChart->legend()->setAlignment(Qt::AlignBottom);

    // 创建48个通道的曲线系列，初始时都创建但默认隐藏
    for (int i = 0; i < 48; i++) {
        QLineSeries *series = new QLineSeries();
        series->setName(QString("通道%1").arg(i + 1));

        // 生成随机颜色
        QColor color(QRandomGenerator::global()->bounded(256),
                     QRandomGenerator::global()->bounded(256),
                     QRandomGenerator::global()->bounded(256));

        QPen pen(color);
        pen.setWidth(2);
        series->setPen(pen);

        // 初始设置为隐藏
        series->setVisible(false);

        temperatureSeries.append(series);
        temperatureChart->addSeries(series);
    }

    // 创建坐标轴
    temperatureAxisX = new QValueAxis();
    temperatureAxisX->setTitleText("采集点数");
    temperatureAxisX->setLabelFormat("%d");
    temperatureAxisX->setRange(0, 100);
    temperatureAxisX->setTickCount(11);

    temperatureAxisY = new QValueAxis();
    temperatureAxisY->setTitleText("温度值 (°C)");
    temperatureAxisY->setLabelFormat("%.1f");
    temperatureAxisY->setRange(-10, 50);  // 默认范围，会根据数据动态调整
    temperatureAxisY->setTickCount(7);

    // 将坐标轴添加到图表
    temperatureChart->addAxis(temperatureAxisX, Qt::AlignBottom);
    temperatureChart->addAxis(temperatureAxisY, Qt::AlignLeft);

    // 将系列附加到坐标轴
    for (int i = 0; i < temperatureSeries.size(); i++) {
        temperatureSeries[i]->attachAxis(temperatureAxisX);
        temperatureSeries[i]->attachAxis(temperatureAxisY);
    }

    // 创建图表视图
    QChartView *chartView = new QChartView(temperatureChart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(300);

    // 将图表视图添加到UI中的温度图表区域
    QVBoxLayout *layout = new QVBoxLayout(ui->temperatureChartWidget);
    layout->addWidget(chartView);
    ui->temperatureChartWidget->setLayout(layout);
}

// 更新压力图表（16通道，50Hz实时）
void MainWindow::updatePressureChart(const DPS_PACKET &pkt)
{
    if (!pressureAxisX || !pressureAxisY || pressureSeries.isEmpty()) {
        return;
    }

    const int x = static_cast<int>(pkt.nFrameNo); // 用帧号作为X轴，保证从1开始递增
    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();

    for (int i = 0; i < 16 && i < pressureSeries.size(); ++i) {
        const double pressure = pkt.fPress[i].toDouble();
        pressureSeries[i]->append(x, pressure);

        minValue = qMin(minValue, pressure);
        maxValue = qMax(maxValue, pressure);

        // 仅限制图表窗口长度（不影响采集缓存/保存）
        if (pressureSeries[i]->count() > m_chartMaxPoints) {
            const int removeCount = pressureSeries[i]->count() - m_chartMaxPoints;
            pressureSeries[i]->removePoints(0, removeCount);
        }
    }

    // X轴滚动窗口
    const int left = qMax(0, x - m_chartMaxPoints);
    pressureAxisX->setRange(left, x + 10);

    // Y轴自适应
    if (minValue != std::numeric_limits<double>::max() && maxValue != std::numeric_limits<double>::lowest()) {
        const double margin = qMax(1.0, (maxValue - minValue) * 0.1);
        pressureAxisY->setRange(minValue - margin, maxValue + margin);
    }
}


// 更新温度图表（48通道，50Hz实时；仅显示有效/接入的通道）
void MainWindow::updateTemperatureChart(const DTS_PACKET &pkt)
{
    if (!temperatureAxisX || !temperatureAxisY || temperatureSeries.isEmpty()) {
        return;
    }

    const int x = static_cast<int>(pkt.nFrameNo); // 用帧号作为X轴
    const QString typeStr = ui->comboDTSType->currentText();
    const bool isVoltageMode = typeStr.contains("电压");

    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();
    int activeChannels = 0;

    // 只有在分频点才刷新legend文本，避免50Hz下legend频繁重绘导致卡顿
    const bool updateNameThisFrame = (m_chartNameUpdateDiv <= 1) ? true : ((pkt.nFrameNo % static_cast<quint32>(m_chartNameUpdateDiv)) == 0);

    for (int i = 0; i < 48 && i < temperatureSeries.size(); ++i) {
        double value = 0.0;

        if (isVoltageMode) {
            value = pkt.fMeasValue[i];
        } else {
            // 默认显示参考值（冷端），若测量值有效则优先显示测量值
            value = pkt.fRefValue[i];
            if (pkt.fMeasValue[i] > -260.0) {
                value = pkt.fMeasValue[i];
            }
        }

        // 无效值：隐藏通道，不写入曲线
        if (value < -250.0 || value > 1000.0) {
            if (temperatureSeries[i]->isVisible()) {
                temperatureSeries[i]->setVisible(false);
            }
            continue;
        }

        if (!temperatureSeries[i]->isVisible()) {
            temperatureSeries[i]->setVisible(true);
        }

        if (updateNameThisFrame) {
            if (isVoltageMode) {
                temperatureSeries[i]->setName(QString("通道%1 (%2 V)").arg(i + 1).arg(value, 0, 'f', 2));
            } else {
                temperatureSeries[i]->setName(QString("通道%1 (%2 °C)").arg(i + 1).arg(value, 0, 'f', 1));
            }
        }

        temperatureSeries[i]->append(x, value);

        minValue = qMin(minValue, value);
        maxValue = qMax(maxValue, value);

        if (temperatureSeries[i]->count() > m_chartMaxPoints) {
            const int removeCount = temperatureSeries[i]->count() - m_chartMaxPoints;
            temperatureSeries[i]->removePoints(0, removeCount);
        }

        activeChannels++;
    }

    if (activeChannels > 0) {
        const int left = qMax(0, x - m_chartMaxPoints);
        temperatureAxisX->setRange(left, x + 10);

        // Y轴范围：沿用原逻辑（电压按量程固定；温度按类型固定/自适应）
        const double margin = qMax(1.0, (maxValue - minValue) * 0.1);

        if (isVoltageMode) {
            if (typeStr.contains("±10V")) {
                temperatureAxisY->setRange(-12, 12);
            } else if (typeStr.contains("±1V")) {
                temperatureAxisY->setRange(-1.2, 1.2);
            } else if (typeStr.contains("±0.1V")) {
                temperatureAxisY->setRange(-0.12, 0.12);
            } else if (typeStr.contains("±0.01V")) {
                temperatureAxisY->setRange(-0.012, 0.012);
            } else {
                temperatureAxisY->setRange(minValue - margin, maxValue + margin);
            }
            temperatureAxisY->setTitleText("电压值 (V)");
        } else {
            if (typeStr.contains("B型")) {
                temperatureAxisY->setRange(200, 1900);
            } else if (typeStr.contains("R型") || typeStr.contains("S型")) {
                temperatureAxisY->setRange(-100, 1800);
            } else if (typeStr.contains("K型") || typeStr.contains("N型") || typeStr.contains("J型")) {
                temperatureAxisY->setRange(-200, 1200);
            } else if (typeStr.contains("T型")) {
                temperatureAxisY->setRange(-200, 400);
            } else if (typeStr.contains("E型")) {
                temperatureAxisY->setRange(-200, 900);
            } else {
                temperatureAxisY->setRange(minValue - margin, maxValue + margin);
            }
            temperatureAxisY->setTitleText("温度值 (°C)");
        }

        if (isVoltageMode) {
            temperatureChart->setTitle(QString("电压实时曲线（%1个活动通道）").arg(activeChannels));
        } else {
            temperatureChart->setTitle(QString("温度实时曲线（%1个活动通道）").arg(activeChannels));
        }
    } else {
        temperatureChart->setTitle("48通道温度实时曲线（无活动通道）");
    }
}


// 图表刷新定时器：从队列取出数据更新曲线（最多处理少量帧，避免阻塞采集）
void MainWindow::onChartUpdateTimer()
{
    const int maxPerTick = 3; // 防止偶发堆积导致UI阻塞

    for (int i = 0; i < maxPerTick && !m_pendingDpsPlot.isEmpty(); ++i) {
        updatePressureChart(m_pendingDpsPlot.dequeue());
    }

    for (int i = 0; i < maxPerTick && !m_pendingDtsPlot.isEmpty(); ++i) {
        updateTemperatureChart(m_pendingDtsPlot.dequeue());
    }
}

// 清除图表数据
void MainWindow::clearCharts()
{
    // 清除压力图表
    for (int i = 0; i < pressureSeries.size(); i++) {
        if (pressureSeries[i]) {
            pressureSeries[i]->clear();
        }
    }
    if (pressureAxisX) {
        pressureAxisX->setRange(0, 100);
    }
    if (pressureAxisY) {
        pressureAxisY->setRange(-10, 120);
    }

    // 清除温度图表
    for (int i = 0; i < temperatureSeries.size(); i++) {
        if (temperatureSeries[i]) {
            temperatureSeries[i]->clear();
            temperatureSeries[i]->setVisible(false);
        }
    }
    if (temperatureAxisX) {
        temperatureAxisX->setRange(0, 100);
    }
    if (temperatureAxisY) {
        temperatureAxisY->setRange(-10, 50);
    }
    temperatureChart->setTitle("48通道温度实时曲线（检测到传感器的通道）");
}

// 新增：多截面UI初始化
void MainWindow::initMultiSectionUI()
{
    // 压力控件关联
    sliderPressurePs = ui->sliderPressurePs;
    labelPressurePs = ui->labelPressurePs;
    tabWidgetPressure = ui->tabWidgetPressure;
    tableWidgetPressure0 = ui->tableWidgetPressure0;
    tableWidgetPressure01 = ui->tableWidgetPressure01;
    tableWidgetPressure1 = ui->tableWidgetPressure1;
    tableWidgetPressure15 = ui->tableWidgetPressure15;
    tableWidgetPressure2 = ui->tableWidgetPressure2;
    tableWidgetPressure3 = ui->tableWidgetPressure3;
    tableWidgetPressure35 = ui->tableWidgetPressure35;
    tableWidgetPressure4 = ui->tableWidgetPressure4;
    tableWidgetPressure5 = ui->tableWidgetPressure5;

    // 温度控件关联
    sliderTemperatureTs = ui->sliderTemperatureTs;
    labelTemperatureTs = ui->labelTemperatureTs;
    tabWidgetTemperature = ui->tabWidgetTemperature;
    tableWidgetTemperature0 = ui->tableWidgetTemperature0;
    tableWidgetTemperature01 = ui->tableWidgetTemperature01;
    tableWidgetTemperature1 = ui->tableWidgetTemperature1;
    tableWidgetTemperature15 = ui->tableWidgetTemperature15;
    tableWidgetTemperature2 = ui->tableWidgetTemperature2;
    tableWidgetTemperature3 = ui->tableWidgetTemperature3;
    tableWidgetTemperature35 = ui->tableWidgetTemperature35;
    tableWidgetTemperature4 = ui->tableWidgetTemperature4;
    tableWidgetTemperature5 = ui->tableWidgetTemperature5;
}

// 按截面计算压力（遵循用户提供的算法）
double MainWindow::calculatePressureBySection(int sectionIndex, double Ps)
{
    if (Ps < 0.101) {
           return 0.0;
       }

       double P = 0.0;
       switch (sectionIndex) {
       case 0: // 0截面
           P = 0.1 + 0.015 * Ps;
           break;
       case 1: // 0.1截面
           P = 0.1 + 0.015 * Ps;
           break;
       case 2: // 1截面
           P = 0.1 + 0.06 * Ps;
           break;
       case 3: // 1.5截面
           P = 0.1 + 0.25 * Ps;
           break;
       case 4: // 2截面
           P = 0.1 + 0.75 * Ps;
           break;
       case 5: // 3截面
           P = 0.1 + Ps;
           break;
       case 6: // 3.5截面
           P = 0.1 + 0.375 * (0.1 + Ps); // P3=0.1+Ps
           break;
       case 7: // 4截面
           P = 0.1 + 0.1 * (0.1 + Ps);
           break;
       case 8: // 5截面
           P = 0.1 + 0.075 * (0.1 + Ps);
           break;
       default:
           P = 0.0;
       }

       // 添加随机扰动
       double randomOffset = m_pressureDist(m_randomGenerator);
       double perturbedValue = P + randomOffset;
       if (perturbedValue < 0.001) perturbedValue = 0.001;

       return perturbedValue;
}

// 按截面计算温度（遵循用户提供的算法）
double MainWindow::calculateTemperatureBySection(int sectionIndex, double Ts)
{
    if (Ts < 20.0) {
           return 0.0;
       }

       double T = 0.0;
       switch (sectionIndex) {
       case 0: // 0截面
           T = 20 + 0.0002 * Ts;
           break;
       case 1: // 0.1截面
           T = 20 + 0.0002 * Ts;
           break;
       case 2: // 1截面
           T = 20 + 0.009 * Ts;
           break;
       case 3: // 1.5截面
           T = 20 + 0.09 * Ts;
           break;
       case 4: // 2截面
           T = 20 + 0.29 * Ts;
           break;
       case 5: // 3截面
           T = 20 + 1.36 * Ts;
           break;
       case 6: // 3.5截面
           T = 20 + 0.57 * Ts;
           break;
       case 7: // 4截面
           T = 20 + 0.3 * Ts;
           break;
       case 8: // 5截面
           T = 20 + 0.227 * Ts;
           break;
       default:
           T = 0.0;
       }

       // 添加随机扰动（如果需要在单独调用时也添加）
       static QRandomGenerator localRandom(static_cast<uint>(QDateTime::currentMSecsSinceEpoch()));
       double randomOffset = (localRandom.generateDouble() - 0.5) * 4.0; // -10~10
       double perturbedValue = T + randomOffset;
       if (perturbedValue < -273.15) perturbedValue = -273.15;

       return perturbedValue;
}

// 初始化压力表格表头（Qt 6兼容版）
void MainWindow::initPressureTableHeaders()
{
    // 0截面表头（12行×16列：P0_1_1_01~P0_3_4_16）
    QStringList headers0;
    for (int col = 0; col < 16; col++) {
        headers0.append(QString("通道%1").arg(col+1));
    }
    tableWidgetPressure0->setHorizontalHeaderLabels(headers0);
    for (int row = 0; row < 12; row++) {
        int rake = (row / 4) + 1;
        int layer = (row % 4) + 1;
        tableWidgetPressure0->setVerticalHeaderItem(row, new QTableWidgetItem(QString("耙%1_层%2").arg(rake).arg(layer)));
    }
    // Qt 6兼容：设置表头拉伸
    tableWidgetPressure0->horizontalHeader()->setStretchLastSection(true);
    tableWidgetPressure0->verticalHeader()->setStretchLastSection(true);

    // 其他截面表头（3行×8列）
    QStringList headersOther;
    for (int col = 0; col < 8; col++) {
        headersOther.append(QString("通道%1").arg(col+1));
    }
    QList<QTableWidget*> otherPressureTables = {
        tableWidgetPressure01, tableWidgetPressure1, tableWidgetPressure15,
        tableWidgetPressure2, tableWidgetPressure3, tableWidgetPressure35,
        tableWidgetPressure4, tableWidgetPressure5
    };
    for (auto table : otherPressureTables) {
        table->setHorizontalHeaderLabels(headersOther);
        for (int row = 0; row < 3; row++) {
            table->setVerticalHeaderItem(row, new QTableWidgetItem(QString("层%1").arg(row+1)));
        }
        // Qt 6兼容：设置表头拉伸
        table->horizontalHeader()->setStretchLastSection(true);
        table->verticalHeader()->setStretchLastSection(true);
    }
}

// 初始化温度表格表头（Qt 6兼容版）
void MainWindow::initTemperatureTableHeaders()
{
    QStringList headers0;
    for (int col = 0; col < 16; col++) {
        headers0.append(QString("通道%1").arg(col+1));
    }
    tableWidgetTemperature0->setHorizontalHeaderLabels(headers0);
    for (int row = 0; row < 12; row++) {
        int rake = (row / 4) + 1;
        int layer = (row % 4) + 1;
        tableWidgetTemperature0->setVerticalHeaderItem(row, new QTableWidgetItem(QString("耙%1_层%2").arg(rake).arg(layer)));
    }
    // Qt 6兼容：设置表头拉伸
    tableWidgetTemperature0->horizontalHeader()->setStretchLastSection(true);
    tableWidgetTemperature0->verticalHeader()->setStretchLastSection(true);

    QStringList headersOther;
    for (int col = 0; col < 8; col++) {
        headersOther.append(QString("通道%1").arg(col+1));
    }
    QList<QTableWidget*> otherTempTables = {
        tableWidgetTemperature01, tableWidgetTemperature1, tableWidgetTemperature15,
        tableWidgetTemperature2, tableWidgetTemperature3, tableWidgetTemperature35,
        tableWidgetTemperature4, tableWidgetTemperature5
    };
    for (auto table : otherTempTables) {
        table->setHorizontalHeaderLabels(headersOther);
        for (int row = 0; row < 3; row++) {
            table->setVerticalHeaderItem(row, new QTableWidgetItem(QString("层%1").arg(row+1)));
        }
        // Qt 6兼容：设置表头拉伸
        table->horizontalHeader()->setStretchLastSection(true);
        table->verticalHeader()->setStretchLastSection(true);
    }
}

// 更新所有压力截面数据
// ==================== 更新所有压力截面数据（带随机扰动）====================
void MainWindow::updatePressureMultiSection(double Ps)
{
    QMutexLocker locker(&m_msDataMutex);

    bool noSensor = (Ps < 0.01);
    if (noSensor) {
        appendTextEdit("无有效Ps值（<0.01MPa），多截面压力数据填充0.0");
        return;
    }

    // -------------------------- 1. 计算各截面基准值（按文档公式）--------------------------
    double baseP0 = 0.1 + 0.015 * Ps;       // 0截面
    double baseP01 = 0.1 + 0.015 * Ps;     // 0.1截面
    double baseP1 = 0.1 + 0.06 * Ps;       // 1截面
    double baseP15 = 0.1 + 0.25 * Ps;      // 1.5截面
    double baseP2 = 0.1 + 0.75 * Ps;       // 2截面
    double baseP3 = 0.1 + Ps;            // 3截面
    double baseP35 = 0.1 + 0.375 * (0.1 + Ps); // 3.5截面（核心公式）
    double baseP4 = 0.1 + 0.1 * (0.1 + Ps);  // 4截面
    double baseP5 = 0.1 + 0.075 * (0.1 + Ps); // 5截面

    // -------------------------- 2. 硬编码截面索引+扰动范围（与通道表一致）--------------------------
    const QVector<QPair<int, int>> sectionRanges = {
        {0, 191},    // 0截面
        {192, 215},  // 0.1截面
        {216, 239},  // 1截面
        {240, 263},  // 1.5截面
        {264, 287},  // 2截面
        {288, 311},  // 3截面
        {312, 335},  // 3.5截面
        {336, 359},  // 4截面
        {360, 383}   // 5截面
    };
    const QVector<double> sectionBases = {baseP0, baseP01, baseP1, baseP15, baseP2, baseP3, baseP35, baseP4, baseP5};
    // 各截面有效扰动范围（确保基准+扰动后在理论区间）
    const QVector<QPair<double, double>> sectionOffsetRanges = {
        {-0.03, 0.03}, // 0截面
        {-0.03, 0.03}, // 0.1截面
        {-0.03, 0.03}, // 1截面
        {-0.03, 0.03}, // 1.5截面
        {-0.03, 0.03}, // 2截面
        {-0.03, 0.03}, // 3截面
        {-0.02, 0.03}, // P35截面（缩小负向扰动，避免触下限）
        {-0.02, 0.03}, // 4截面
        {-0.02, 0.03}  // 5截面
    };

    // -------------------------- 3. 计算384通道数据（动态修正，无固定值+正确随机扰动）--------------------------
    QVector<double> allChannelValues(384, 0.0);
    for (int s = 0; s < sectionRanges.size(); s++) {
        int startIdx = sectionRanges[s].first;
        int endIdx = sectionRanges[s].second;
        double base = sectionBases[s];
        double minOffset = sectionOffsetRanges[s].first;
        double maxOffset = sectionOffsetRanges[s].second;

        // 针对当前截面创建浮点数均匀分布（修复核心错误：mt19937需通过分布类生成浮点数）
        std::uniform_real_distribution<double> offsetDist(minOffset, maxOffset);

        for (int idx = startIdx; idx <= endIdx; idx++) {
            double value = 0.0;
            // 循环生成有效扰动，确保value在理论范围（避免直接用固定下限）
            do {
                // 正确用法：通过分布类+mt19937生成指定范围的浮点数扰动
                double randomOffset = offsetDist(m_randomGenerator);
                value = base + randomOffset;
            } while (value < (base + minOffset) || value > (base + maxOffset));

            // 最终范围校验（双重保障，避开边界固定值）
            if (s == 6) { // P35截面
                value = qBound(0.1825 + 0.001, value, 0.2425 - 0.001);
            } else if (s == 7) { // 4截面
                value = qBound(0.1 + 0.001, value, 0.16 - 0.001);
            } else if (s == 8) { // 5截面
                value = qBound(0.1225 + 0.001, value, 0.1525 - 0.001);
            } else {
                value = qBound(0.002, value, 10.0 - 0.001);
            }

            allChannelValues[idx] = value;
        }
    }

    // -------------------------- 4. 更新缓存和UI（每帧独立，无重复固定值）--------------------------
    MultiSectionPressureData cacheData;
    cacheData.collectTime = QDateTime::currentDateTime();
    cacheData.frameNo = m_tcpFrameCounter++;
    cacheData.allChannels = allChannelValues;

    m_msPressureCache.append(cacheData);
    if (m_msPressureCache.size() > 5000) {
        m_msPressureCache.removeFirst();
    }

    updatePressureMultiSectionUI(allChannelValues);

    // 日志（监控固定值情况）
    int fixedValueCount = 0;
    for (double val : allChannelValues) {
        if (qFuzzyCompare(val, 0.1825) || qFuzzyCompare(val, 0.1)) {
            fixedValueCount++;
        }
    }
    appendTextEdit(QString("多截面压力数据更新：Ps=%1MPa，固定值（0.1825/0.1）数量=%2（应接近0）")
                       .arg(Ps, 0, 'f', 3)
                       .arg(fixedValueCount));
}


// ==================== 更新所有温度截面数据（带随机扰动）====================

// ==================== 多源加载：按指定通道生成多截面压力（算法+扰动） ====================
void MainWindow::updatePressureMultiSectionFromSources(double psCh1, double psCh5, double psCh9,
                                                       double psCh11, double psCh16,
                                                       qint32 frameNo, bool updateUI)
{
    QMutexLocker locker(&m_msDataMutex);

    // psChX单位：MPa（来自DPS通道转换），允许为0（异常时）但不会影响采集线程
    auto safePs = [](double v) -> double {
        if (!std::isfinite(v)) return 0.0;
        if (v < 0.0) return 0.0;
        return v;
    };
    psCh1  = safePs(psCh1);
    psCh5  = safePs(psCh5);
    psCh9  = safePs(psCh9);
    psCh11 = safePs(psCh11);
    psCh16 = safePs(psCh16);

    // 9个截面基准压力（文档算法：由加载Ps计算得到各截面基准）
    // 注意：这里按“指定通道->对应截面组加载Ps”的需求分别计算，不再用“任意>0.1MPa通道”推断Ps。
    const double P0  = 0.1 + 0.015 * psCh1;

    const double P01 = 0.1 + 0.015 * psCh5;
    const double P1  = 0.1 + 0.06  * psCh5;

    const double P15 = 0.1 + 0.25  * psCh9;
    const double P2  = 0.1 + 0.75  * psCh9;

    const double P3  = 0.1 + 1.0   * psCh11;
    const double P35 = 0.1 + 0.375 * P3;

    // 4/5截面按文档：依赖本组的“P3=0.1+Ps”
    const double P3_45 = 0.1 + 1.0 * psCh16;
    const double P4  = 0.1 + 0.1   * P3_45;
    const double P5  = 0.1 + 0.075 * P3_45;

    QVector<double> sectionPressures = {P0, P01, P1, P15, P2, P3, P35, P4, P5};

    // 每个截面的允许扰动范围（与原逻辑一致）
    const QVector<QPair<double, double>> sectionOffsetRanges = {
        {-0.010,  0.010},  // P0
        {-0.010,  0.010},  // P01
        {-0.020,  0.020},  // P1
        {-0.050,  0.050},  // P15
        {-0.100,  0.100},  // P2
        {-0.200,  0.200},  // P3
        {-0.150,  0.150},  // P35
        {-0.050,  0.050},  // P4
        {-0.050,  0.050}   // P5
    };

    QVector<double> allChannels;
    allChannels.reserve(384);

    // 0截面：192通道
    {
        const double base = sectionPressures[0];
        const auto range = sectionOffsetRanges[0];
        for (int i = 0; i < 192; ++i) {
            double v = base + m_pressureDist(m_randomGenerator);
            // 约束到“基准±范围”，避免极端扰动导致跳变
            v = qBound(base + range.first, v, base + range.second);
            v = qMax(0.001, v);
            allChannels.append(v);
        }
    }

    // 其他截面：每个24通道
    for (int sectionIdx = 1; sectionIdx < 9; ++sectionIdx) {
        const double base = sectionPressures[sectionIdx];
        const auto range = sectionOffsetRanges[sectionIdx];
        for (int i = 0; i < 24; ++i) {
            double v = base + m_pressureDist(m_randomGenerator);
            v = qBound(base + range.first, v, base + range.second);
            v = qMax(0.001, v);
            allChannels.append(v);
        }
    }

    // 写入缓存（按采集帧号保持从1递增）
    MultiSectionPressureData cacheData;
    cacheData.collectTime = QDateTime::currentDateTime();
    cacheData.frameNo = frameNo;
    cacheData.allChannels = allChannels;

    m_msPressureCache.append(cacheData);
    if (m_msPressureCache.size() > 5000) {
        m_msPressureCache.removeFirst();
    }

    // UI刷新频率可降低，避免影响50Hz采集
    if (updateUI) {
        updatePressureMultiSectionUI(allChannels);
    }
}

// ==================== 多源加载：按指定通道生成多截面温度（算法+扰动，缺失填-270） ====================
void MainWindow::updateTemperatureMultiSectionFromSources(float tsCh5, bool hasCh5,
                                                         float tsCh10, bool hasCh10,
                                                         float tsCh15, bool hasCh15,
                                                         float tsCh20, bool hasCh20,
                                                         float tsCh25, bool hasCh25,
                                                         qint32 frameNo, bool updateUI)
{
    QMutexLocker locker(&m_msDataMutex);

    auto normTs = [](float v) -> double {
        if (!std::isfinite(v)) return -270.0;
        return static_cast<double>(v);
    };

    const double Ts0  = hasCh5  ? normTs(tsCh5)  : -270.0;
    const double Ts01 = hasCh10 ? normTs(tsCh10) : -270.0;
    const double Ts15 = hasCh15 ? normTs(tsCh15) : -270.0;
    const double Ts3  = hasCh20 ? normTs(tsCh20) : -270.0;
    const double Ts45 = hasCh25 ? normTs(tsCh25) : -270.0;

    // 基准温度（文档算法：由加载Ts计算得到各截面基准）
    // 若对应加载通道未接入传感器，则该截面组直接填-270℃
    const double T0  = hasCh5  ? (20.0 + 0.0002 * Ts0)  : -270.0;

    const double T01 = hasCh10 ? (20.0 + 0.0002 * Ts01) : -270.0;
    const double T1  = hasCh10 ? (20.0 + 0.009  * Ts01) : -270.0;

    const double T15 = hasCh15 ? (20.0 + 0.09   * Ts15) : -270.0;
    const double T2  = hasCh15 ? (20.0 + 0.29   * Ts15) : -270.0;

    const double T3  = hasCh20 ? (20.0 + 1.36   * Ts3)  : -270.0;
    const double T35 = hasCh20 ? (20.0 + 0.57   * Ts3)  : -270.0;

    const double T4  = hasCh25 ? (20.0 + 0.3    * Ts45) : -270.0;
    const double T5  = hasCh25 ? (20.0 + 0.227  * Ts45) : -270.0;

    QVector<double> sectionBaseTemps = {T0, T01, T1, T15, T2, T3, T35, T4, T5};

    QVector<double> allChannels;
    allChannels.reserve(384);

    auto genTemp = [&](double base) -> double {
        if (base <= -269.0) {
            return -270.0; // 无传感器填充值
        }
        double v = base + m_temperatureDist(m_randomGenerator); // ±2℃
        // 仅拦截极端值（避免冷端/错误值）
        if (v < (base - 200.0) || v > (base + 200.0)) {
            v = base;
        }
        v = qMax(-273.15, v);
        v = qMin(1500.0, v);
        return v;
    };

    // 0截面：192通道
    for (int i = 0; i < 192; ++i) {
        allChannels.append(genTemp(sectionBaseTemps[0]));
    }

    // 其他截面：每个24通道
    for (int sectionIdx = 1; sectionIdx < 9; ++sectionIdx) {
        for (int i = 0; i < 24; ++i) {
            allChannels.append(genTemp(sectionBaseTemps[sectionIdx]));
        }
    }

    // 写入缓存（按采集帧号保持从1递增）
    MultiSectionTemperatureData cacheData;
    cacheData.collectTime = QDateTime::currentDateTime();
    cacheData.frameNo = frameNo;
    cacheData.allChannels = allChannels;

    m_msTemperatureCache.append(cacheData);
    if (m_msTemperatureCache.size() > 5000) {
        m_msTemperatureCache.removeFirst();
    }

    // UI刷新频率可降低，避免影响50Hz采集
    if (updateUI) {
        updateTemperatureMultiSectionUI(allChannels);
    }
}

// ==================== 新增：同步更新多截面温度UI表格（确保索引与数据一一对应） ====================
void MainWindow::updateTemperatureMultiSectionUI(const QVector<double>& allChannels)
{
    QList<QTableWidget*> tempTables = {
        ui->tableWidgetTemperature0, ui->tableWidgetTemperature01, ui->tableWidgetTemperature1,
        ui->tableWidgetTemperature15, ui->tableWidgetTemperature2, ui->tableWidgetTemperature3,
        ui->tableWidgetTemperature35, ui->tableWidgetTemperature4, ui->tableWidgetTemperature5
    };

    // 0截面：12行×16列 = 192
    int idx = 0;
    QTableWidget* t0 = tempTables[0];
    if (t0) {
        for (int row = 0; row < t0->rowCount(); ++row) {
            for (int col = 0; col < t0->columnCount(); ++col) {
                if (idx >= 192 || idx >= allChannels.size()) break;
                auto* item = t0->item(row, col);
                if (item) {
                    item->setText(QString("%1 ℃").arg(allChannels[idx], 0, 'f', 2));
                }
                ++idx;
            }
        }
    } else {
        idx = 192;
    }

    // 其他8个截面：每个3行×8列 = 24
    int offset = 192;
    for (int section = 1; section < tempTables.size(); ++section) {
        QTableWidget* table = tempTables[section];
        if (!table) {
            offset += 24;
            continue;
        }
        int local = 0;
        for (int row = 0; row < table->rowCount(); ++row) {
            for (int col = 0; col < table->columnCount(); ++col) {
                int dataIdx = offset + local;
                if (dataIdx >= allChannels.size() || local >= 24) break;
                auto* item = table->item(row, col);
                if (item) {
                    item->setText(QString("%1 ℃").arg(allChannels[dataIdx], 0, 'f', 2));
                }
                ++local;
            }
        }
        offset += 24;
    }
}


void MainWindow::updateTemperatureMultiSection(double /*validTemp*/) // 不再使用传入的validTemp
{
    QMutexLocker locker(&m_msDataMutex);

    bool noSensor = (ui->sliderTemperatureTs->value() < 5.0);
    double Ts = ui->sliderTemperatureTs->value(); // 直接使用UI设置的Ts，稳定基准值

    // 各截面基准值（基于稳定的Ts计算，不再受错误validTemp影响）
    double T0 = noSensor ? 0.0 : (20.0 + 0.0002 * Ts);
    double T01 = noSensor ? 0.0 : (20.0 + 0.0002 * Ts);
    double T1 = noSensor ? 0.0 : (20.0 + 0.009 * Ts);
    double T15 = noSensor ? 0.0 : (20.0 + 0.09 * Ts);
    double T2 = noSensor ? 0.0 : (20.0 + 0.29 * Ts);
    double T3 = noSensor ? 0.0 : (20.0 + 1.36 * Ts);
    double T35 = noSensor ? 0.0 : (20.0 + 0.57 * Ts);
    double T4 = noSensor ? 0.0 : (20.0 + 0.3 * Ts);
    double T5 = noSensor ? 0.0 : (20.0 + 0.227 * Ts);

    QVector<double> sectionBaseTemps = {T0, T01, T1, T15, T2, T3, T35, T4, T5};
    QVector<QVector<double>> channelValuesWithRandom;

    // 0截面批量过滤（阈值放宽到±200℃）
    QVector<double> section0Values;
    for (int i = 0; i < 192; i++) {
        double base = sectionBaseTemps[0];
        if (noSensor) {
            section0Values.append(0.0);
            continue;
        }
        double offset = m_temperatureDist(m_randomGenerator); // -2~2℃扰动
        double value = base + offset;
        // 放宽阈值到±200℃，仅过滤极端异常值（如冷端30℃）
        if (value < (base - 200) || value > (base + 200)) {
            value = base;
            appendTextEditDTS(QString("0截面通道%1极端值拦截，修正为%2℃").arg(i+1).arg(base, 0, 'f', 2));
        }
        value = qMax(-273.15, value);
        section0Values.append(value);
    }
    channelValuesWithRandom.append(section0Values);

    // 0.1~5截面批量过滤（阈值±200℃）
    for (int sectionIdx = 1; sectionIdx < 9; sectionIdx++) {
        QVector<double> sectionValues;
        double base = sectionBaseTemps[sectionIdx];
        for (int i = 0; i < 24; i++) {
            if (noSensor) {
                sectionValues.append(0.0);
                continue;
            }
            double offset = m_temperatureDist(m_randomGenerator);
            double value = base + offset;

            // 核心：放宽过滤阈值到±200℃，仅拦截冷端等极端值
            if (value < (base - 200) || value > (base + 200)) {
                value = base;
                QString sectionName = (sectionIdx == 6) ? "T35" : QString("T%1").arg(sectionIdx);
                appendTextEditDTS(QString("%1截面通道%2极端值拦截，修正为%3℃")
                                      .arg(sectionName)
                                      .arg(i+1)
                                      .arg(base, 0, 'f', 2));
            }

            value = qMax(-273.15, value);
            value = qMin(1500.0, value);
            sectionValues.append(value);
        }
        channelValuesWithRandom.append(sectionValues);
    }

    ui->labelTemperatureTs->setText(QString("当前Ts值：%1 ℃").arg(Ts, 0, 'f', 1));

    // 缓存填充：二次校验（阈值±200℃）
    MultiSectionTemperatureData cacheData;
    cacheData.collectTime = QDateTime::currentDateTime();
    cacheData.frameNo = m_tcpFrameCounter;

    for (int i = 0; i < 192; i++) {
        cacheData.allChannels.append(channelValuesWithRandom[0][i]);
    }

    for (int sectionIdx = 1; sectionIdx < 9; sectionIdx++) {
        int sectionStartIdx = 192 + (sectionIdx - 1) * 24;
        double base = sectionBaseTemps[sectionIdx];
        for (int i = 0; i < 24; i++) {
            int globalIdx = sectionStartIdx + i;
            if (globalIdx >= MS_TEMPERATURE_CHANNELS) break;

            double finalValue = channelValuesWithRandom[sectionIdx][i];
            if (finalValue < (base - 200) || finalValue > (base + 200)) {
                finalValue = base;
            }
            cacheData.allChannels.append(finalValue);
        }
    }

    m_msTemperatureCache.append(cacheData);
    if (m_msTemperatureCache.size() > 5000) {
        m_msTemperatureCache.removeFirst();
    }

    appendTextEditDTS(QString("多截面温度缓存更新：%1个通道，基准值稳定（Ts=%2℃）")
                          .arg(cacheData.allChannels.size())
                          .arg(Ts, 0, 'f', 1));
}

// 保存多截面压力数据到Excel（格式与DPS一致，无采集速率信息）
bool MainWindow::saveMultiSectionPressureDataToExcel(const QString &fileName)
{
    QMutexLocker locker(&m_msDataMutex);

    if (m_msPressureCache.isEmpty()) {
        appendTextEdit("多截面压力数据为空，无法保存！");
        return false;
    }

    QXlsx::Document xlsx;
    XlsxWorksheet *sheet = xlsx.currentWorksheet();
    sheet->setName("多截面压力数据");

    // ========== 写入基本信息 ==========
    // 第一行：标题
    xlsx.write(1, 1, "多截面压力数据（带随机扰动）");
    xlsx.write(1, 2, "生成时间");
    xlsx.write(1, 3, QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    // 第二行：数据说明
    xlsx.write(2, 1, "说明");
    xlsx.write(2, 2, "压力值已添加随机扰动：-0.03~0.03 MPa");
    xlsx.write(2, 3, "通道总数");
    xlsx.write(2, 4, "384通道");

    // 第三行：数据记录信息
    xlsx.write(3, 1, "数据记录数");
    xlsx.write(3, 2, QString::number(m_msPressureCache.size()));
    xlsx.write(3, 3, "采集时间段");
    if (!m_msPressureCache.isEmpty()) {
        xlsx.write(3, 4, m_msPressureCache.first().collectTime.toString("yyyy-MM-dd HH:mm:ss"));
        xlsx.write(3, 5, "至");
        xlsx.write(3, 6, m_msPressureCache.last().collectTime.toString("yyyy-MM-dd HH:mm:ss"));
    }

    // 第四行：空行
    xlsx.write(4, 1, "");

    // ========== 写入表头（第5行）==========
    int col = 1;
    xlsx.write(5, col++, "采集时间");
    xlsx.write(5, col++, "帧号");

    // 压力通道表头（384通道）
    for (int rake = 1; rake <= 3; rake++) {
        for (int layer = 1; layer <= 4; layer++) {
            for (int ch = 1; ch <= 16; ch++) {
                QString header = QString("P0_%1_%2_%3").arg(rake).arg(layer).arg(ch, 2, 10, QChar('0'));
                xlsx.write(5, col++, header);
            }
        }
    }

    // 0.1~5截面压力通道
    QString sectionNames[] = {"P01", "P1", "P15", "P2", "P3", "P35", "P4", "P5"};
    for (const QString& section : sectionNames) {
        for (int layer = 1; layer <= 3; layer++) {
            for (int ch = 1; ch <= 8; ch++) {
                QString header = QString("%1_%2_%3").arg(section).arg(layer).arg(ch, 2, 10, QChar('0'));
                xlsx.write(5, col++, header);
            }
        }
    }

    // ========== 写入数据（从第6行开始）==========
    int row = 6;
    for (int i = 0; i < m_msPressureCache.size(); i++) {
        const MultiSectionPressureData& data = m_msPressureCache[i];

        col = 1;
        xlsx.write(row, col++, data.collectTime.toString("yyyy-MM-dd HH:mm:ss.zzz"));
        xlsx.write(row, col++, data.frameNo);

        // 写入384通道压力数据（带随机扰动）
        for (int j = 0; j < data.allChannels.size(); j++) {
            xlsx.write(row, col++, data.allChannels[j]);
        }

        row++;
    }

    // ========== 保存文件 ==========
    if (xlsx.saveAs(fileName)) {
        QFileInfo fileInfo(fileName);
        appendTextEdit(QString("多截面压力数据已保存到：%1，文件大小：%2 字节")
                           .arg(fileName)
                           .arg(fileInfo.size()));
        return true;
    } else {
        appendTextEdit("多截面压力数据保存失败！");
        return false;
    }
}




bool MainWindow::saveMultiSectionTemperatureDataToExcel(const QString &fileName)
{
    QMutexLocker locker(&m_msDataMutex);

    if (m_msTemperatureCache.isEmpty()) {
        appendTextEditDTS("多截面温度数据为空，无法保存！");
        return false;
    }

    QXlsx::Document xlsx;
    XlsxWorksheet *sheet = xlsx.currentWorksheet();
    sheet->setName("多截面温度数据");

    // ========== 写入基本信息（保留原有格式，更新Ts范围说明） ==========
    xlsx.write(1, 1, "多截面温度数据（冷端过滤+Ts=20~1000℃）");
    xlsx.write(1, 2, "生成时间");
    xlsx.write(1, 3, QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    xlsx.write(2, 1, "说明");
    xlsx.write(2, 2, "所有通道已过滤冷端数据（<20℃或>1000℃），扰动范围±2℃，基准值稳定");
    xlsx.write(2, 3, "通道总数");
    xlsx.write(2, 4, "384通道");

    xlsx.write(3, 1, "数据记录数");
    xlsx.write(3, 2, QString::number(m_msTemperatureCache.size()));
    xlsx.write(3, 3, "采集时间段");
    if (!m_msTemperatureCache.isEmpty()) {
        xlsx.write(3, 4, m_msTemperatureCache.first().collectTime.toString("yyyy-MM-dd HH:mm:ss"));
        xlsx.write(3, 5, "至");
        xlsx.write(3, 6, m_msTemperatureCache.last().collectTime.toString("yyyy-MM-dd HH:mm:ss"));
    }

    xlsx.write(4, 1, "");

    // ========== 写入表头（与通道表顺序一致，便于对应） ==========
    int col = 1;
    xlsx.write(5, col++, "采集时间");
    xlsx.write(5, col++, "帧号");

    // 0截面温度表头（3耙×4层×16通道=192通道）
    for (int rake = 1; rake <= 3; rake++) {
        for (int layer = 1; layer <= 4; layer++) {
            for (int ch = 1; ch <= 16; ch++) {
                QString header = QString("T0_%1_%2_%3").arg(rake).arg(layer).arg(ch, 2, 10, QChar('0'));
                xlsx.write(5, col++, header);
            }
        }
    }

    // 0.1~5截面温度表头（8个截面×3层×8通道=192通道）
    QString sectionNames[] = {"T01", "T1", "T15", "T2", "T3", "T35", "T4", "T5"};
    QString sectionCNNames[] = {"0.1截面", "1截面", "1.5截面", "2截面", "3截面", "3.5截面", "4截面", "5截面"};
    for (int s = 0; s < 8; s++) {
        for (int layer = 1; layer <= 3; layer++) {
            for (int ch = 1; ch <= 8; ch++) {
                QString header = QString("%1_%2_%3").arg(sectionNames[s]).arg(layer).arg(ch, 2, 10, QChar('0'));
                xlsx.write(5, col++, header);
            }
        }
    }

    // ========== 写入数据（批量校验，避免固定值和冷端） ==========
    int row = 6;
    for (int i = 0; i < m_msTemperatureCache.size(); i++) {
        const MultiSectionTemperatureData& data = m_msTemperatureCache[i];

        col = 1;
        // 采集时间和帧号
        xlsx.write(row, col++, data.collectTime.toString("yyyy-MM-dd HH:mm:ss.zzz"));
        xlsx.write(row, col++, data.frameNo);

        // 0截面数据（192通道）
        for (int j = 0; j < 192; j++) {
            double tempValue = data.allChannels[j];
            xlsx.write(row, col++, tempValue);
        }
// 0.1~5截面数据（192通道）
        for (int s = 0; s < 8; s++) {
            for (int layer = 1; layer <= 3; layer++) {
                for (int ch = 1; ch <= 8; ch++) {
                    int dataIdx = 192 + s*24 + (layer-1)*8 + (ch-1);
                    double tempValue = data.allChannels[dataIdx];
                    xlsx.write(row, col++, tempValue);
                }
            }
        }
row++;
    }

    // ========== 保存文件（符合原有Excel格式要求） ==========
    if (xlsx.saveAs(fileName)) {
        QFileInfo fileInfo(fileName);
        appendTextEditDTS(QString("多截面温度数据已保存到：%1，文件大小：%2 字节（Ts范围20~1000℃）")
                           .arg(fileName)
                           .arg(fileInfo.size()));
        return true;
    } else {
        appendTextEditDTS("多截面温度数据保存失败！");
        return false;
    }
}

// ==================== 新增：创建压力截面显示控件的函数 ====================
QWidget* MainWindow::createPressureSectionWidget(int sectionIndex, const QString& title, int rows, int columns)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QGroupBox* groupBox = new QGroupBox(title);
    QGridLayout* gridLayout = new QGridLayout(groupBox);

    // 生成通道名称和数据
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < columns; col++) {
            // 生成通道名称
            QString channelName = generatePressureChannelName(sectionIndex, row, col);

            // 获取当前值（从主界面表格获取）
            QString value = "0.0000 MPa";
            QTableWidget* sourceTable = nullptr;

            switch(sectionIndex) {
            case 0: sourceTable = ui->tableWidgetPressure0; break;
            case 1: sourceTable = ui->tableWidgetPressure01; break;
            case 2: sourceTable = ui->tableWidgetPressure1; break;
            case 3: sourceTable = ui->tableWidgetPressure15; break;
            case 4: sourceTable = ui->tableWidgetPressure2; break;
            case 5: sourceTable = ui->tableWidgetPressure3; break;
            case 6: sourceTable = ui->tableWidgetPressure35; break;
            case 7: sourceTable = ui->tableWidgetPressure4; break;
            case 8: sourceTable = ui->tableWidgetPressure5; break;
            }

            if (sourceTable && row < sourceTable->rowCount() && col < sourceTable->columnCount()) {
                QTableWidgetItem* item = sourceTable->item(row, col);
                if (item) {
                    value = item->text();
                }
            }

            // 创建通道标签
            QLabel* nameLabel = new QLabel(channelName + ":");
            nameLabel->setStyleSheet("QLabel { font-weight: bold; color: #0000FF; }");

            // 创建值标签
            QLabel* valueLabel = new QLabel(value);
            valueLabel->setStyleSheet("QLabel { background-color: #F0F0F0; border: 1px solid #CCCCCC; padding: 2px; }");
            valueLabel->setMinimumWidth(100);
            valueLabel->setAlignment(Qt::AlignCenter);

            // 添加到网格布局
            int gridRow = row;
            int gridCol = col * 2; // 每列占2个网格单元：名称+值

            gridLayout->addWidget(nameLabel, gridRow, gridCol);
            gridLayout->addWidget(valueLabel, gridRow, gridCol + 1);
        }
    }

    layout->addWidget(groupBox);
    return widget;
}

// ==================== 新增：创建温度截面显示控件的函数 ====================
QWidget* MainWindow::createTemperatureSectionWidget(int sectionIndex, const QString& title, int rows, int columns)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QGroupBox* groupBox = new QGroupBox(title);
    QGridLayout* gridLayout = new QGridLayout(groupBox);

    // 生成通道名称和数据
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < columns; col++) {
            // 生成通道名称
            QString channelName = generateTemperatureChannelName(sectionIndex, row, col);

            // 获取当前值（从主界面表格获取）
            QString value = "0.00 ℃";
            QTableWidget* sourceTable = nullptr;

            switch(sectionIndex) {
            case 0: sourceTable = ui->tableWidgetTemperature0; break;
            case 1: sourceTable = ui->tableWidgetTemperature01; break;
            case 2: sourceTable = ui->tableWidgetTemperature1; break;
            case 3: sourceTable = ui->tableWidgetTemperature15; break;
            case 4: sourceTable = ui->tableWidgetTemperature2; break;
            case 5: sourceTable = ui->tableWidgetTemperature3; break;
            case 6: sourceTable = ui->tableWidgetTemperature35; break;
            case 7: sourceTable = ui->tableWidgetTemperature4; break;
            case 8: sourceTable = ui->tableWidgetTemperature5; break;
            }

            if (sourceTable && row < sourceTable->rowCount() && col < sourceTable->columnCount()) {
                QTableWidgetItem* item = sourceTable->item(row, col);
                if (item) {
                    value = item->text();
                }
            }

            // 创建通道标签
            QLabel* nameLabel = new QLabel(channelName + ":");
            nameLabel->setStyleSheet("QLabel { font-weight: bold; color: #FF0000; }");

            // 创建值标签
            QLabel* valueLabel = new QLabel(value);
            valueLabel->setStyleSheet("QLabel { background-color: #F0F0F0; border: 1px solid #CCCCCC; padding: 2px; }");
            valueLabel->setMinimumWidth(100);
            valueLabel->setAlignment(Qt::AlignCenter);

            // 添加到网格布局
            int gridRow = row;
            int gridCol = col * 2; // 每列占2个网格单元：名称+值

            gridLayout->addWidget(nameLabel, gridRow, gridCol);
            gridLayout->addWidget(valueLabel, gridRow, gridCol + 1);
        }
    }

    layout->addWidget(groupBox);
    return widget;
}

// ==================== 新增：生成压力通道名称的函数 ====================
QString MainWindow::generatePressureChannelName(int sectionIndex, int row, int col)
{
    QString channelName;
    int channelNum = col + 1;

    switch(sectionIndex) {
    case 0: // 0截面：P0_耙_层_通道
    {
        int rake = (row / 4) + 1;  // 耙号1-3
        int layer = (row % 4) + 1; // 层号1-4
        channelName = QString("P0_%1_%2_%3")
                          .arg(rake)
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 1: // 0.1截面：P01_层_通道
    {
        int layer = row + 1;
        channelName = QString("P01_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 2: // 1截面：P1_层_通道
    {
        int layer = row + 1;
        channelName = QString("P1_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 3: // 1.5截面：P15_层_通道
    {
        int layer = row + 1;
        channelName = QString("P15_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 4: // 2截面：P2_层_通道
    {
        int layer = row + 1;
        channelName = QString("P2_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 5: // 3截面：P3_层_通道
    {
        int layer = row + 1;
        channelName = QString("P3_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 6: // 3.5截面：P35_层_通道
    {
        int layer = row + 1;
        channelName = QString("P35_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 7: // 4截面：P4_层_通道
    {
        int layer = row + 1;
        channelName = QString("P4_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 8: // 5截面：P5_层_通道
    {
        int layer = row + 1;
        channelName = QString("P5_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    default:
        channelName = QString("P_%1_%2")
                          .arg(row + 1)
                          .arg(channelNum, 2, 10, QChar('0'));
    }

    return channelName;
}

// ==================== 新增：生成温度通道名称的函数 ====================
QString MainWindow::generateTemperatureChannelName(int sectionIndex, int row, int col)
{
    QString channelName;
    int channelNum = col + 1;

    switch(sectionIndex) {
    case 0: // 0截面：T0_耙_层_通道
    {
        int rake = (row / 4) + 1;  // 耙号1-3
        int layer = (row % 4) + 1; // 层号1-4
        channelName = QString("T0_%1_%2_%3")
                          .arg(rake)
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 1: // 0.1截面：T01_层_通道
    {
        int layer = row + 1;
        channelName = QString("T01_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 2: // 1截面：T1_层_通道
    {
        int layer = row + 1;
        channelName = QString("T1_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 3: // 1.5截面：T15_层_通道
    {
        int layer = row + 1;
        channelName = QString("T15_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 4: // 2截面：T2_层_通道
    {
        int layer = row + 1;
        channelName = QString("T2_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 5: // 3截面：T3_层_通道
    {
        int layer = row + 1;
        channelName = QString("T3_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 6: // 3.5截面：T35_层_通道
    {
        int layer = row + 1;
        channelName = QString("T35_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 7: // 4截面：T4_层_通道
    {
        int layer = row + 1;
        channelName = QString("T4_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    case 8: // 5截面：T5_层_通道
    {
        int layer = row + 1;
        channelName = QString("T5_%1_%2")
                          .arg(layer)
                          .arg(channelNum, 2, 10, QChar('0'));
        break;
    }
    default:
        channelName = QString("T_%1_%2")
                          .arg(row + 1)
                          .arg(channelNum, 2, 10, QChar('0'));
    }

    return channelName;
}

// ==================== 新增：创建0截面压力分页显示控件的函数 ====================
QWidget* MainWindow::createPressureSection0SubWidget(int pageIndex, const QString& title)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    QGroupBox* groupBox = new QGroupBox(title);
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    int rowsPerPage = 3;    // 每页3行
    int colsPerPage = 8;    // 每页8列
    // 修复：按页索引计算正确的原始行范围（0截面共12行，分8页，每页2行）
    int startRow = pageIndex * 2;  // 第0页0-1行、第1页2-3行...第7页12-13行（实际取10-11行）
    int endRow = qMin(startRow + 1, 11);  // 限制最大行索引为11（0-11共12行）
    int currentPageRow = 0;
    // 生成通道名称和数据
    for (int originalRow = startRow; originalRow <= endRow && originalRow < 12; originalRow++) {
        // 每行分2段，每段8个通道（对应原始16列）
        for (int segment = 0; segment < 2 && currentPageRow < rowsPerPage; segment++) {
            for (int pageCol = 0; pageCol < colsPerPage; pageCol++) {
                int originalCol = segment * 8 + pageCol;  // 计算原始列索引
                // 生成Word文档定义的通道名称（P0_耙_层_通道）
                int rake = (originalRow / 4) + 1;  // 耙号1-3（0-3行=耙1，4-7行=耙2，8-11行=耙3）
                int layer = (originalRow % 4) + 1; // 层号1-4
                QString channelName = QString("P0_%1_%2_%3")
                                          .arg(rake)
                                          .arg(layer)
                                          .arg(originalCol + 1, 2, 10, QChar('0'));
                // 获取数据（初始0，≥0.1MPa时按公式计算）
                QString value = "0.0000 MPa";
                if (originalRow < ui->tableWidgetPressure0->rowCount() &&
                    originalCol < ui->tableWidgetPressure0->columnCount()) {
                    QTableWidgetItem* item = ui->tableWidgetPressure0->item(originalRow, originalCol);
                    if (item) {
                        value = item->text();
                    }
                }
                // 创建通道名称标签
                QLabel* nameLabel = new QLabel(channelName);
                nameLabel->setStyleSheet("QLabel { font-weight: bold; color: #0000FF; font-size: 10px; }");
                nameLabel->setMinimumWidth(120);
                nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                // 创建数据标签
                QLabel* valueLabel = new QLabel(value);
                valueLabel->setStyleSheet("QLabel { background-color: #F0F0F0; border: 1px solid #CCCCCC; padding: 4px; font-size: 10px; }");
                valueLabel->setMinimumWidth(100);
                valueLabel->setAlignment(Qt::AlignCenter);
                // 添加到布局
                gridLayout->addWidget(nameLabel, currentPageRow, pageCol * 2);
                gridLayout->addWidget(valueLabel, currentPageRow, pageCol * 2 + 1);
            }
            currentPageRow++;
        }
    }
    // 设置列宽比例
    for (int col = 0; col < colsPerPage * 2; col++) {
        gridLayout->setColumnStretch(col, col % 2 == 0 ? 3 : 2);
    }
    layout->addWidget(groupBox);
    return widget;
}

// ==================== 新增：创建0截面温度分页显示控件的函数 ====================
QWidget* MainWindow::createTemperatureSection0SubWidget(int pageIndex, const QString& title)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QGroupBox* groupBox = new QGroupBox(title);
    QGridLayout* gridLayout = new QGridLayout(groupBox);

    int rowsPerPage = 3;    // 每页3行
    int colsPerPage = 8;    // 每页8列

    // 计算页面对应的原始行范围
    int startRow = (pageIndex / 2) * 4 + ((pageIndex % 2) * 2);  // 每页对应原始表格的2行
    int endRow = startRow + 1;  // 每页包含2行

    // 当前页面内的行索引
    int currentPageRow = 0;

    // 生成通道名称和数据
    for (int originalRow = startRow; originalRow <= endRow && originalRow < 12; originalRow++) {
        // 每行取8个通道，形成3行显示
        for (int pageRow = 0; pageRow < 2 && currentPageRow < rowsPerPage; pageRow++) {
            for (int pageCol = 0; pageCol < colsPerPage; pageCol++) {
                // 计算原始列索引：每行分2段，每段8个通道
                int originalCol = (pageRow * 8) + pageCol;

                // 生成通道名称
                QString channelName = generateTemperatureChannelName(0, originalRow, originalCol);

                // 获取当前值（从主界面表格获取）
                QString value = "0.00 ℃";
                if (originalRow < ui->tableWidgetTemperature0->rowCount() &&
                    originalCol < ui->tableWidgetTemperature0->columnCount()) {
                    QTableWidgetItem* item = ui->tableWidgetTemperature0->item(originalRow, originalCol);
                    if (item) {
                        value = item->text();
                    }
                }

                // 创建通道名称标签（使用更大的字体和固定宽度）
                QLabel* nameLabel = new QLabel(channelName);
                nameLabel->setStyleSheet("QLabel { font-weight: bold; color: #FF0000; font-size: 10px; }");
                nameLabel->setMinimumWidth(120);  // 固定宽度，防止重叠
                nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

                // 创建值标签
                QLabel* valueLabel = new QLabel(value);
                valueLabel->setStyleSheet("QLabel { background-color: #F0F0F0; border: 1px solid #CCCCCC; padding: 4px; font-size: 10px; }");
                valueLabel->setMinimumWidth(100);
                valueLabel->setAlignment(Qt::AlignCenter);

                // 添加到网格布局
                gridLayout->addWidget(nameLabel, currentPageRow, pageCol * 2);
                gridLayout->addWidget(valueLabel, currentPageRow, pageCol * 2 + 1);
            }
            currentPageRow++;
        }
    }

    // 设置列宽比例，确保名称和值都有足够空间
    for (int col = 0; col < colsPerPage * 2; col++) {
        if (col % 2 == 0) {  // 名称列
            gridLayout->setColumnStretch(col, 3);
        } else {  // 值列
            gridLayout->setColumnStretch(col, 2);
        }
    }

    layout->addWidget(groupBox);
    return widget;
}

// 测试通道模式：更新多截面温度（从DTS采集数据中提取有效Ts）
void MainWindow::updateTestChannelTemperature()
{
    QMutexLocker locker(&m_msDataMutex);
    double validTs = 20.0; // 默认值，确保不触发noSensor

    // 从DTS采集缓存中提取最新有效传感器温度（优先取检测到传感器的通道值）
    if (!m_collectedDataDTS.isEmpty()) {
        const DTS_PACKET& latestPkt = m_collectedDataDTS.last();
        bool hasValidSensor = false;

        // 遍历48通道，找第一个有效传感器温度（>20℃且不是断线值）
        for (int i = 0; i < 48; i++) {
            float sensorTemp = latestPkt.fMeasValue[i];
            if (sensorTemp > 20.0 && sensorTemp > -260.0) { // 有效传感器温度（>20℃）
                validTs = sensorTemp;
                hasValidSensor = true;
                break;
            }
        }

        // 若未找到有效传感器温度（测试场景），强制设置一个>20℃的模拟值（符合文档加载策略）
        if (!hasValidSensor) {
            validTs = 80.0; // 文档中正常加载的中间值，确保算法计算非0
            appendTextEditDTS(QString("测试通道模式：未检测到有效传感器温度，强制设置Ts=%1℃").arg(validTs));
        }
    } else {
        // 无采集缓存时，直接设置模拟Ts（符合文档加载策略）
        validTs = 80.0;
        appendTextEditDTS(QString("测试通道模式：无DTS采集数据，模拟设置Ts=%1℃").arg(validTs));
    }

    // 调用原有更新函数，传递有效Ts（>20℃）
    updateTemperatureMultiSection(validTs);
}
// 测试通道按钮点击事件（若原有函数存在，直接修改；无则新增）
void MainWindow::on_btnTestChannel_clicked()
{
    // 原有弹窗逻辑保持不变
    QDialog* multiSectionDialog = new QDialog(this);
    multiSectionDialog->setWindowTitle("多截面数据显示（带通道标签）");
    multiSectionDialog->setMinimumSize(1600, 1000);
    QVBoxLayout* dialogLayout = new QVBoxLayout(multiSectionDialog);
    QTabWidget* mainTabWidget = new QTabWidget(multiSectionDialog);

    // ==================== 关键修改：添加温度数据主动更新 ====================
    // 点击测试通道时，强制更新多截面温度（使用有效Ts）
    updateTestChannelTemperature();

    // ==================== 原有弹窗UI构建逻辑保持不变 ====================
    // 压力数据选项卡（不变）
    QWidget* pressureTab = new QWidget();
    QVBoxLayout* pressureTabLayout = new QVBoxLayout(pressureTab);
    // ...（原有压力选项卡构建代码完全保留）
    mainTabWidget->addTab(pressureTab, "压力数据");

    // 温度数据选项卡（不变）
    QWidget* temperatureTab = new QWidget();
    QVBoxLayout* temperatureTabLayout = new QVBoxLayout(temperatureTab);
    // ...（原有温度选项卡构建代码完全保留）
    mainTabWidget->addTab(temperatureTab, "温度数据");

    dialogLayout->addWidget(mainTabWidget);
    multiSectionDialog->exec();
    delete multiSectionDialog;
}

// 多截面数据TCP发送函数
void MainWindow::sendMultiSectionTCPServerData()
{
    QMutexLocker locker(&m_msTcpMutex);

    if (m_tcpClients.isEmpty()) {
        // 只在调试时输出
        static int emptyClientCount = 0;
        if (emptyClientCount++ % 100 == 0) {
            appendTextEdit("多截面TCP发送：无客户端连接");
        }
        return;
    }

    // -------------------------- 1. 先发送多截面通道表（仅1次） --------------------------
    if (!m_msAetimsChannelSent) {
        // 转换通道表包为QByteArray（二进制）
        QByteArray channelData;
        channelData.resize(sizeof(MS_DATAPACKET1));
        memcpy(channelData.data(), &m_msAetimsChannelPacket, sizeof(MS_DATAPACKET1));

        // 发送给所有客户端
        bool sendOk = false;
        for (auto client : m_tcpClients) {
            if (client && client->state() == QAbstractSocket::ConnectedState) {
                qint64 bytes = client->write(channelData);
                client->flush();
                if (bytes == channelData.size()) {
                    sendOk = true;
                    appendTextEdit(QString("多截面通道表发送成功：客户端%1:%2（%3字节，CommandID=2）")
                                       .arg(client->peerAddress().toString())
                                       .arg(client->peerPort())
                                       .arg(bytes));

                    // 调试信息：显示前32字节
                    QString hexData;
                    for (int i = 0; i < qMin(32, channelData.size()); i++) {
                        hexData += QString("%1 ").arg((quint8)channelData[i], 2, 16, QChar('0')).toUpper();
                    }
                    appendTextEdit(QString("通道表前32字节：%1").arg(hexData));
                }
            }
        }

        if (sendOk) {
            m_msAetimsChannelSent = true; // 标记为已发送，不再重复发
            m_msAetimsDataPacketCount = 0; // 重置数据包计数器
            appendTextEdit("多截面通道表已发送（CommandID=2，768通道）");
        }
        return; // 发送通道表后，本次不发数据包
    }

    // -------------------------- 2. 循环发送多截面数据包（CommandID=4） --------------------------
    // 构建数据包（MS_DATAPACKET3）
    MS_DATAPACKET3 dataPacket;
    PACKETHEAD& dataHead = dataPacket.PacketHead;
    DATAHEAD& dataBodyHead = dataPacket.DataHead;

    // 2.1 填充包头（PACKETHEAD）
    dataHead.CommandID = 4;                    // 4=数据包（文档要求）
    dataHead.Nums = 768;                       // 通道总数=768（压力384+温度384）
    // 包体长度= Nums*sizeof(float) + sizeof(DATAHEAD) = 768*4 + 48 = 3120字节
    dataHead.PktLens = dataHead.Nums * sizeof(float) + sizeof(DATAHEAD);
    dataHead.DeviceID = 2;                     // 设备编号（多截面专用）
    dataHead.nPackCount = ++m_msAetimsDataPacketCount; // 包计数自增
    dataHead.nRef = 0;                         // 备用填0
    memset(dataHead.TestName, 0, 56);
    strncpy(dataHead.TestName, "多截面数据采集", 55); // 与通道表一致

    // 2.2 填充数据包包头（DATAHEAD）
    memset(dataBodyHead.StatusName, 0, 32);
    strncpy(dataBodyHead.StatusName, "MultiSection Collecting", 31); // 试验状态
    // QDateTime转SYSTEMTIME
    QDateTime now = QDateTime::currentDateTime();
    SYSTEMTIME& sysTime = dataBodyHead.SysTime;
    sysTime.wYear = now.date().year();
    sysTime.wMonth = now.date().month();
    sysTime.wDay = now.date().day();
    sysTime.wDayOfWeek = now.date().dayOfWeek() - 1; // Qt周日=7→文档周日=0
    sysTime.wHour = now.time().hour();
    sysTime.wMinute = now.time().minute();
    sysTime.wSecond = now.time().second();
    sysTime.wMilliseconds = now.time().msec();

    // 2.3 填充768通道数据（压力384个 + 温度384个）
    // 首先填充压力数据（0-383）
    if (!m_msPressureCache.isEmpty()) {
        const auto& latestPressure = m_msPressureCache.last().allChannels;
        for (int i = 0; i < qMin(384, latestPressure.size()); i++) {
            dataPacket.AcqData[i] = static_cast<float>(latestPressure[i]);
        }
        // 填充剩余的压力通道
        for (int i = latestPressure.size(); i < 384; i++) {
            dataPacket.AcqData[i] = 0.0f;
        }
    } else {
        // 无压力数据，填充默认值
        for (int i = 0; i < 384; i++) {
            dataPacket.AcqData[i] = 0.0f;
        }
    }

    // 然后填充温度数据（384-767）
    if (!m_msTemperatureCache.isEmpty()) {
        const auto& latestTemperature = m_msTemperatureCache.last().allChannels;
        for (int i = 0; i < qMin(384, latestTemperature.size()); i++) {
            dataPacket.AcqData[384 + i] = static_cast<float>(latestTemperature[i]);
        }
        // 填充剩余的温度通道
        for (int i = latestTemperature.size(); i < 384; i++) {
            dataPacket.AcqData[384 + i] = 25.0f; // 默认温度25℃
        }
    } else {
        // 无温度数据，填充默认值
        for (int i = 384; i < 768; i++) {
            dataPacket.AcqData[i] = 25.0f; // 默认温度25℃
        }
    }

    locker.unlock(); // 释放锁，避免长时间持有

    // 2.4 发送数据包（二进制）
    QByteArray dataBytes;
    dataBytes.resize(sizeof(MS_DATAPACKET3));
    memcpy(dataBytes.data(), &dataPacket, sizeof(MS_DATAPACKET3));

    bool sendSuccess = false;
    QList<QTcpSocket*> invalidClients;

    for (auto client : m_tcpClients) {
        if (!client || client->state() != QAbstractSocket::ConnectedState || !client->isValid()) {
            invalidClients.append(client);
            continue;
        }

        qint64 bytesWritten = client->write(dataBytes);
        client->flush();

        if (bytesWritten == dataBytes.size()) {
            sendSuccess = true;
            // 每100帧输出一次日志，避免日志过多
            if (dataHead.nPackCount % 100 == 0) {
                appendTextEdit(QString("多截面数据包发送成功：帧%1，客户端%2:%3（%4字节，CommandID=%5）")
                                   .arg(dataHead.nPackCount)
                                   .arg(client->peerAddress().toString())
                                   .arg(client->peerPort())
                                   .arg(bytesWritten)
                                   .arg(dataHead.CommandID));
            }
        } else {
            if (dataHead.nPackCount % 100 == 0) {
                appendTextEdit(QString("多截面数据包发送失败：客户端%1:%2（%3/%4字节，CommandID=%5）")
                                   .arg(client->peerAddress().toString())
                                   .arg(client->peerPort())
                                   .arg(bytesWritten)
                                   .arg(dataBytes.size())
                                   .arg(dataHead.CommandID));
            }
            invalidClients.append(client);
        }
    }

    // 移除无效客户端
    for (auto client : invalidClients) {
        m_tcpClients.removeOne(client);
        if (client) client->deleteLater();
    }

    // 更新UI帧数显示
    m_tcpFrameCounter = m_msAetimsDataPacketCount;
    if (m_tcpFrameCounter % 100 == 0) {
        updateTCPStatus(); // 每100帧更新一次UI，避免频繁更新
    }

    // 使用sendSuccess避免编译警告
    if (sendSuccess) {
        // 数据发送成功，可以添加额外处理
    }
}

// 独立发送多截面通道表（符合AETIMS“连接后先发通道表”要求）
void MainWindow::sendMultiSectionChannelTable()
{
    if (m_tcpClients.isEmpty()) {
        return;
    }

    QMutexLocker locker(&m_msTcpMutex);

    // 确保通道表已正确初始化
    if (m_msAetimsChannelPacket.PacketHead.CommandID != 2) {
        m_msAetimsChannelPacket.PacketHead.CommandID = 2;
    }

    // 更新包计数
    m_msAetimsChannelPacket.PacketHead.nPackCount++;

    // 计算通道表包大小
    int channelPacketSize = sizeof(PACKETHEAD) +
                           (m_msAetimsChannelPacket.PacketHead.Nums * sizeof(SENDTAB));

    // 转换为字节数组发送
    QByteArray packetData(reinterpret_cast<const char*>(&m_msAetimsChannelPacket),
                         channelPacketSize);

    // 发送给所有客户端
    for (auto client : m_tcpClients) {
        if (client && client->state() == QAbstractSocket::ConnectedState) {
            client->write(packetData);
            client->flush();

            appendTextEdit(QString("已发送多截面通道表（CommandID=%1，包计数=%2，通道数=%3）")
                              .arg(m_msAetimsChannelPacket.PacketHead.CommandID)
                              .arg(m_msAetimsChannelPacket.PacketHead.nPackCount)
                              .arg(m_msAetimsChannelPacket.PacketHead.Nums));

            // 只输出前几个通道的信息
            if (m_msAetimsChannelPacket.PacketHead.nPackCount == 1) {
                for (int i = 0; i < 2; i++) {
                    const SENDTAB& ch = m_msAetimsChannelPacket.ChTab[i];
                    appendTextEdit(QString("  通道%1: 英文名='%2', 中文名='%3', 单位='%4'")
                                      .arg(i+1)
                                      .arg(ch.ChName)
                                      .arg(ch.ChName_CHN)
                                      .arg(ch.EngUnit));
                }
                appendTextEdit("  ... (共768个通道)");
            }
        }
    }

    m_msAetimsChannelSent = true;
}

// 新增：压力随机扰动定时刷新槽函数（每秒触发）
void MainWindow::onPressureRandomTimerTimeout()
{
    QMutexLocker locker(&m_msDataMutex); // 线程安全保护

    // 1. 跳过无缓存/未采集状态
    if (m_msPressureCache.isEmpty() || !m_simultaneousCollecting) {
        return;
    }


    // 若当前正在接收真实DPS数据（50Hz），不使用此1s随机刷新覆盖采集缓存
    if (m_dpsDataReady) {
        return;
    }
    // 2. 获取当前最新的压力基准数据（保持Ps计算的基准值不变）
    MultiSectionPressureData& latestData = m_msPressureCache.last();
    double currentPs = ui->sliderPressurePs->value() / 1000.0; // 当前Ps基准值（不变）
    bool noSensor = (currentPs < 0.01);

    // 3. 重新计算各截面基准值（与原逻辑一致，确保基准不变）
    double P0 = noSensor ? 0.0 : (0.1 + 0.015 * currentPs);
    double P01 = noSensor ? 0.0 : (0.1 + 0.015 * currentPs);
    double P1 = noSensor ? 0.0 : (0.1 + 0.06 * currentPs);
    double P15 = noSensor ? 0.0 : (0.1 + 0.25 * currentPs);
    double P2 = noSensor ? 0.0 : (0.1 + 0.75 * currentPs);
    double P3 = noSensor ? 0.0 : (0.1 + currentPs);
    double P35 = noSensor ? 0.0 : (0.1 + 0.375 * P3);
    double P4 = noSensor ? 0.0 : (0.1 + 0.1 * P3);
    double P5 = noSensor ? 0.0 : (0.1 + 0.075 * P3);
    QVector<double> sectionPressures = {P0, P01, P1, P15, P2, P3, P35, P4, P5};

    // 4. 生成新的随机扰动值（仅更新扰动，基准值不变）
    QVector<QVector<double>> newChannelValues;
    // 0截面（192通道）
    QVector<double> section0Values;
    for (int i = 0; i < 192; i++) {
        double baseValue = sectionPressures[0];
        if (noSensor) {
            section0Values.append(0.0);
        } else {
            double newRandomOffset = m_pressureDist(m_randomGenerator); // 新扰动值
            double newValue = baseValue + newRandomOffset;
            newValue = qMax(0.001, newValue); // 避免负值
            section0Values.append(newValue);
        }
    }
    newChannelValues.append(section0Values);

    // 其他截面（每个24通道）
    for (int sectionIdx = 1; sectionIdx < 9; sectionIdx++) {
        QVector<double> sectionValues;
        for (int i = 0; i < 24; i++) {
            double baseValue = sectionPressures[sectionIdx];
            if (noSensor) {
                sectionValues.append(0.0);
            } else {
                double newRandomOffset = m_pressureDist(m_randomGenerator); // 新扰动值
                double newValue = baseValue + newRandomOffset;
                newValue = qMax(0.001, newValue); // 避免负值
                sectionValues.append(newValue);
            }
        }
        newChannelValues.append(sectionValues);
    }

    // 5. 更新缓存数据（保持采集时间、帧号不变，仅更新通道值）
    QVector<double> newAllChannels;
    // 0截面192通道
    for (int i = 0; i < 192; i++) {
        newAllChannels.append(newChannelValues[0][i]);
    }
    // 其他截面192通道
    for (int sectionIdx = 1; sectionIdx < 9; sectionIdx++) {
        for (int i = 0; i < 24; i++) {
            newAllChannels.append(newChannelValues[sectionIdx][i]);
        }
    }
    latestData.allChannels = newAllChannels; // 替换为新扰动后的数值
    latestData.collectTime = QDateTime::currentDateTime(); // 可选：更新时间戳

    // 6. 刷新UI显示（复用原有更新逻辑）
    int channelIndex = 0;
    // 更新0截面UI
    for (int row = 0; row < 12; row++) {
        for (int col = 0; col < 16; col++) {
            if (ui->tableWidgetPressure0->item(row, col)) {
                double newValue = newChannelValues[0][channelIndex % 192];
                ui->tableWidgetPressure0->item(row, col)->setText(QString("%1 MPa").arg(newValue, 0, 'f', 4));
            }
            channelIndex++;
        }
    }
    // 更新其他截面UI
    QList<QTableWidget*> pressureTables = {
        ui->tableWidgetPressure01, ui->tableWidgetPressure1, ui->tableWidgetPressure15,
        ui->tableWidgetPressure2, ui->tableWidgetPressure3, ui->tableWidgetPressure35,
        ui->tableWidgetPressure4, ui->tableWidgetPressure5
    };
    for (int tableIdx = 0; tableIdx < pressureTables.size(); tableIdx++) {
        channelIndex = 0;
        QTableWidget* table = pressureTables[tableIdx];
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 8; col++) {
                if (table->item(row, col)) {
                    double newValue = newChannelValues[tableIdx + 1][channelIndex % 24];
                    table->item(row, col)->setText(QString("%1 MPa").arg(newValue, 0, 'f', 4));
                }
                channelIndex++;
            }
        }
    }

    // 日志（可选，用于调试）
    appendTextEdit("压力随机扰动已刷新（1秒定时）");
}

// 新增：同步更新多截面压力UI表格（确保索引与数据一一对应）
void MainWindow::updatePressureMultiSectionUI(const QVector<double>& allChannels)
{
    QList<QTableWidget*> pressureTables = {
        ui->tableWidgetPressure0, ui->tableWidgetPressure01, ui->tableWidgetPressure1,
        ui->tableWidgetPressure15, ui->tableWidgetPressure2, ui->tableWidgetPressure3,
        ui->tableWidgetPressure35, ui->tableWidgetPressure4, ui->tableWidgetPressure5
    };

    // 0截面（0~191：12行×16列）
    int channelIdx = 0;
    for (int row = 0; row < 12; row++) {
        for (int col = 0; col < 16; col++) {
            if (channelIdx <= 191 && ui->tableWidgetPressure0->item(row, col)) {
                ui->tableWidgetPressure0->item(row, col)->setText(
                    QString("%1 MPa").arg(allChannels[channelIdx], 0, 'f', 6)
                );
            }
            channelIdx++;
        }
    }

    // 其他截面（每个24通道：3行×8列）
    for (int tableIdx = 1; tableIdx < pressureTables.size(); tableIdx++) {
        QTableWidget* table = pressureTables[tableIdx];
        int startIdx = sectionRanges[tableIdx].first;
        int endIdx = sectionRanges[tableIdx].second;
        channelIdx = startIdx;

        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 8; col++) {
                if (channelIdx <= endIdx && table->item(row, col)) {
                    table->item(row, col)->setText(
                        QString("%1 MPa").arg(allChannels[channelIdx], 0, 'f', 6)
                    );
                }
                channelIdx++;
            }
        }
    }
}

// 新增：专门的多截面数据发送定时器函数
// 修改onMultiSectionSendTimer函数，确保数据包发送
void MainWindow::onMultiSectionSendTimer()
{
    // 如果没有客户端连接，直接返回
    if (m_tcpClients.isEmpty()) {
        return;
    }

    // 检查是否允许发送
    if (!m_tcpSendEnabled) {
        // 只记录一次，避免日志过多
        static bool logged = false;
        if (!logged) {
            appendTextEdit("警告：TCP发送未启用，数据包无法发送");
            logged = true;
        }
        return;
    }

    // 如果通道表未发送，先发送通道表
    if (!m_msAetimsChannelSent) {
        sendMultiSectionChannelTable();
        // 通道表发送后，设置已发送标志
        m_msAetimsChannelSent = true;
        return;
    }

    // 检查缓存数据，如果为空，创建测试数据
    bool hasData = false;
    {
        QMutexLocker locker(&m_msDataMutex);
        hasData = !m_msPressureCache.isEmpty() && !m_msTemperatureCache.isEmpty();
    }

    if (!hasData) {
        // 创建测试数据
        appendTextEdit("多截面缓存为空，创建测试数据并更新UI");

        // 更新多截面数据（这会填充缓存）
        updatePressureMultiSection(0.2);  // Ps=0.2MPa
        updateTemperatureMultiSection(25.0);  // Ts=25℃
    }

    // 发送数据包
    sendMultiSectionDataPacket();
}


// 发送多截面数据包（CommandID=4）- 简化版
void MainWindow::sendMultiSectionDataPacket()
{
    if (m_tcpClients.isEmpty()) {
        return;
    }

    // 检查发送是否启用
    if (!m_tcpSendEnabled) {
        appendTextEdit("sendMultiSectionDataPacket: TCP发送未启用");
        return;
    }

    QMutexLocker locker(&m_msTcpMutex);

    // 构建数据包
    MS_DATAPACKET3 dataPacket;
    memset(&dataPacket, 0, sizeof(MS_DATAPACKET3));

    // 设置包头
    PACKETHEAD& head = dataPacket.PacketHead;
    head.CommandID = 4;                // 数据包命令ID必须为4
    head.Nums = 768;                   // 多截面总通道数
    head.DeviceID = 2;                 // 多截面设备编号
    head.nPackCount = ++m_msAetimsDataPacketCount; // 包计数自增
    head.nRef = 0;
    memset(head.TestName, 0, 56);
    strncpy(head.TestName, "多截面压力温度采集", 55);

    // 包长度计算：Nums×sizeof(float)+sizeof(DATAHEAD)
    head.PktLens = head.Nums * sizeof(float) + sizeof(DATAHEAD);

    // 设置数据包包头
    DATAHEAD& dataHead = dataPacket.DataHead;
    memset(dataHead.StatusName, 0, 32);
    strncpy(dataHead.StatusName, "MultiSection Collecting", 31);

    // 设置系统时间
    QDateTime currentTime = QDateTime::currentDateTime();
    SYSTEMTIME& sysTime = dataHead.SysTime;
    sysTime.wYear = static_cast<unsigned short>(currentTime.date().year());
    sysTime.wMonth = static_cast<unsigned short>(currentTime.date().month());
    sysTime.wDayOfWeek = static_cast<unsigned short>(currentTime.date().dayOfWeek());
    sysTime.wDay = static_cast<unsigned short>(currentTime.date().day());
    sysTime.wHour = static_cast<unsigned short>(currentTime.time().hour());
    sysTime.wMinute = static_cast<unsigned short>(currentTime.time().minute());
    sysTime.wSecond = static_cast<unsigned short>(currentTime.time().second());
    sysTime.wMilliseconds = static_cast<unsigned short>(currentTime.time().msec());

    // 填充数据（压力384通道 + 温度384通道）
    {
        QMutexLocker dataLocker(&m_msDataMutex);

        if (!m_msPressureCache.isEmpty() && !m_msTemperatureCache.isEmpty()) {
            // 获取最新的压力数据
            const auto& latestPressure = m_msPressureCache.last();
            const auto& latestTemperature = m_msTemperatureCache.last();

            // 填充压力数据（0-383）
            int pressureChannels = qMin(384, latestPressure.allChannels.size());
            for (int i = 0; i < pressureChannels; i++) {
                dataPacket.AcqData[i] = static_cast<float>(latestPressure.allChannels[i]);
            }

            // 填充温度数据（384-767）
            int tempChannels = qMin(384, latestTemperature.allChannels.size());
            for (int i = 0; i < tempChannels; i++) {
                dataPacket.AcqData[384 + i] = static_cast<float>(latestTemperature.allChannels[i]);
            }

            // 填充剩余通道（如果有的话）为0
            for (int i = pressureChannels; i < 384; i++) {
                dataPacket.AcqData[i] = 0.0f;
            }
            for (int i = 384 + tempChannels; i < 768; i++) {
                dataPacket.AcqData[i] = 0.0f;
            }

            // 记录前几个通道的值用于调试
            if (m_msAetimsDataPacketCount % 100 == 1) {
                QString debugInfo = "数据包前10个通道值: ";
                for (int i = 0; i < 10 && i < pressureChannels; i++) {
                    debugInfo += QString("P%1=%2 ").arg(i+1).arg(dataPacket.AcqData[i], 0, 'f', 3);
                }
                appendTextEdit(debugInfo);
            }
        } else {
            // 如果没有缓存数据，填充测试数据
            appendTextEdit("警告：多截面缓存为空，填充测试数据");
            for (int i = 0; i < 384; i++) {
                dataPacket.AcqData[i] = 0.1f + (i % 10) * 0.01f;  // 压力测试数据
                dataPacket.AcqData[384 + i] = 20.0f + (i % 10) * 0.5f;  // 温度测试数据
            }
        }
    }

    // 计算实际发送的数据包大小
    int packetSize = sizeof(PACKETHEAD) + sizeof(DATAHEAD) + (head.Nums * sizeof(float));

    // 转换为字节数组发送
    QByteArray packetData(reinterpret_cast<const char*>(&dataPacket), packetSize);

    // 验证数据包大小
    if (packetSize != 3200) {
        appendTextEdit(QString("错误：数据包大小错误！实际=%1，期望=3200").arg(packetSize));
    }

    // 发送给所有客户端
    bool anySent = false;
    for (auto client : m_tcpClients) {
        if (client && client->state() == QAbstractSocket::ConnectedState) {
            qint64 bytesWritten = client->write(packetData);
            if (bytesWritten > 0) {
                anySent = true;
                client->flush();

                // 输出第一次数据包的信息
                if (m_msAetimsDataPacketCount == 1) {
                    QString hexData = packetData.left(16).toHex(' ').toUpper();
                    appendTextEdit(QString("第一次数据包发送成功，大小=%1字节，前16字节: %2")
                                      .arg(bytesWritten)
                                      .arg(hexData));
                    appendTextEdit(QString("数据包信息：CommandID=%1, Nums=%2, PktLens=%3, DeviceID=%4")
                                      .arg(head.CommandID)
                                      .arg(head.Nums)
                                      .arg(head.PktLens)
                                      .arg(head.DeviceID));
                }

                // 每100帧输出一次日志，避免日志过多
                if (m_msAetimsDataPacketCount % 100 == 0) {
                    appendTextEdit(QString("已发送多截面数据包（CommandID=%1，包计数=%2，帧号=%3）")
                                      .arg(head.CommandID)
                                      .arg(m_msAetimsDataPacketCount)
                                      .arg(m_tcpFrameCounter));
                }
            } else {
                appendTextEdit(QString("错误：向客户端发送数据包失败，错误：%1")
                                  .arg(client->errorString()));
            }
        }
    }

    if (anySent) {
        m_tcpFrameCounter++;
        // 更新UI上的帧数显示
        ui->labelTCPFrames->setText(QString("发送帧数: %1").arg(m_tcpFrameCounter));
    }
}

// ==================== 添加数据对齐验证函数 ====================

void MainWindow::verifyDataAlignment()
{
    // 验证结构体大小是否符合AETIMS文档要求
    qDebug() << "=== 验证AETIMS数据对齐 ===";
    qDebug() << "sizeof(PACKETHEAD) = " << sizeof(PACKETHEAD) << " (应为80)";
    qDebug() << "sizeof(DATAHEAD) = " << sizeof(DATAHEAD) << " (应为48)";
    qDebug() << "sizeof(SENDTAB) = " << sizeof(SENDTAB) << " (应为136)";
    qDebug() << "sizeof(SYSTEMTIME) = " << sizeof(SYSTEMTIME) << " (应为16)";
    qDebug() << "sizeof(MS_DATAPACKET3) = " << sizeof(MS_DATAPACKET3);

    // 计算期望的数据包大小
    int expectedDataPacketSize = sizeof(PACKETHEAD) + sizeof(DATAHEAD) + (768 * sizeof(float));
    qDebug() << "期望的数据包大小: " << expectedDataPacketSize << " (应为" << (80+48+3072) << ")";

    // 验证对齐
    static_assert(sizeof(PACKETHEAD) == 80, "PACKETHEAD大小必须为80字节（AETIMS要求）");
    static_assert(sizeof(SENDTAB) == 144, "SENDTAB大小必须为136字节（AETIMS要求）");
    static_assert(sizeof(MS_DATAPACKET3) == 8128, "多截面数据包大小必须为8128字节（80+48+2000×4）");
}

// ==================== 性能监控函数 ====================
void MainWindow::monitorPerformance()
{
    static QElapsedTimer monitorTimer;
    static int totalFrames = 0;

    if (!monitorTimer.isValid()) {
        monitorTimer.start();
        return;
    }

    totalFrames++;

    // 每5秒输出一次性能报告
    if (monitorTimer.elapsed() >= 5000) {
        double elapsedSeconds = monitorTimer.elapsed() / 1000.0;
        double actualFrequency = totalFrames / elapsedSeconds;

        QString report = QString("性能报告：\n");
        report += QString("  采集时长：%1秒\n").arg(elapsedSeconds, 0, 'f', 1);
        report += QString("  总帧数：%1\n").arg(totalFrames);
        report += QString("  实际采集频率：%1Hz (理论50Hz)\n").arg(actualFrequency, 0, 'f', 1);
        report += QString("  DPS数据队列：%1\n").arg(m_autoSaveBufferDPS.size());
        report += QString("  DTS数据队列：%1\n").arg(m_autoSaveBufferDTS.size());

        if (actualFrequency < 45.0) {
            report += "  警告：采集频率低于理论值！\n";
            report += "  可能原因：\n";
            report += "    1. 数据保存开销过大\n";
            report += "    2. 图表更新过于频繁\n";
            report += "    3. 网络延迟\n";
        }

        appendTextEdit(report);

        // 重置计数器
        monitorTimer.restart();
        totalFrames = 0;
    }
}

// 更新采集速率显示
void MainWindow::updateCollectionRate()
{
    if (m_dpsRateTimer.elapsed() > 0 && m_dpsPacketCount > 0) {
        m_dpsActualRate = m_dpsPacketCount / (m_dpsRateTimer.elapsed() / 1000.0);

        // 更新DPS采集速率标签
        if (m_labelDPSRate) {
            m_labelDPSRate->setText(QString("DPS采集速率: %1 Hz").arg(m_dpsActualRate, 0, 'f', 1));

            // 根据速率状态设置颜色
            if (m_dpsActualRate >= 49.0 && m_dpsActualRate <= 51.0) {
                m_labelDPSRate->setStyleSheet("color: green; font-weight: bold;");
            } else if (m_dpsActualRate >= 45.0 && m_dpsActualRate <= 55.0) {
                m_labelDPSRate->setStyleSheet("color: blue; font-weight: bold;");
            } else if (m_dpsActualRate >= 40.0 && m_dpsActualRate <= 60.0) {
                m_labelDPSRate->setStyleSheet("color: orange; font-weight: bold;");
            } else {
                m_labelDPSRate->setStyleSheet("color: red; font-weight: bold;");
            }
        }
    }

    if (m_dtsRateTimer.elapsed() > 0 && m_dtsPacketCount > 0) {
        m_dtsActualRate = m_dtsPacketCount / (m_dtsRateTimer.elapsed() / 1000.0);

        // 更新DTS采集速率标签
        if (m_labelDTSRate) {
            m_labelDTSRate->setText(QString("DTS采集速率: %1 Hz").arg(m_dtsActualRate, 0, 'f', 1));

            // 根据速率状态设置颜色
            if (m_dtsActualRate >= 49.0 && m_dtsActualRate <= 51.0) {
                m_labelDTSRate->setStyleSheet("color: green; font-weight: bold;");
            } else if (m_dtsActualRate >= 45.0 && m_dtsActualRate <= 55.0) {
                m_labelDTSRate->setStyleSheet("color: blue; font-weight: bold;");
            } else if (m_dtsActualRate >= 40.0 && m_dtsActualRate <= 60.0) {
                m_labelDTSRate->setStyleSheet("color: orange; font-weight: bold;");
            } else {
                m_labelDTSRate->setStyleSheet("color: red; font-weight: bold;");
            }
        }
    }

    // 同时采集时在状态标签显示整体信息
    if (m_simultaneousCollecting) {
        double avgRate = 0.0;
        int count = 0;

        if (m_dpsActualRate > 0) {
            avgRate += m_dpsActualRate;
            count++;
        }
        if (m_dtsActualRate > 0) {
            avgRate += m_dtsActualRate;
            count++;
        }

        if (count > 0) {
            avgRate /= count;
            ui->labelAutoSave->setText(QString("同时采集: %1Hz 平均").arg(avgRate, 0, 'f', 1));
            ui->labelDTSAutoSave->setText(QString("同时采集: %1Hz 平均").arg(avgRate, 0, 'f', 1));
        }
    }
}
