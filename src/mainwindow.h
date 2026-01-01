#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tcpclient.h"
#include <QList>
#include <QQueue>
#include <QLabel>
#include "xlsxdocument.h" // QXlsx核心头文件
#include <QtCharts>        // 添加Qt Charts头文件
#include <QTimer>          // 添加定时器头文件
#include <QElapsedTimer>  // 添加计时器头文件
#include <QTcpServer>    // 添加TCP服务器头文件
#include <QTcpSocket>    // 添加TCP套接字头文件
#include <QRandomGenerator>
#include <random>  // 添加标准库随机数头文件
// 严格遵循AETIMS文档结构体定义（VC++6.0 4字节对齐）
#if defined(_MSC_VER)
// VC++6.0：4字节对齐（符合文档原生环境）
#pragma pack(push, 4)
#else
// MinGW：4字节对齐（与VC++6.0保持对齐规则一致）
#pragma pack(4)
#endif
// 全局变量声明
extern PSISTREAM g_DeviceStream;

namespace Ui {
class MainWindow;
}


// 系统时间结构体（文档定义）
typedef struct {
    unsigned short wYear;
    unsigned short wMonth;
    unsigned short wDayOfWeek;  // 0=周日，1=周一...6=周六
    unsigned short wDay;
    unsigned short wHour;
    unsigned short wMinute;
    unsigned short wSecond;
    unsigned short wMilliseconds;
} SYSTEMTIME;

// 包头结构体（通道表和数据包共用，文档定义）
typedef struct {
    int PktLens;         // 包体长度（字节）：通道表= Nums*sizeof(SENDTAB)；数据包= Nums*sizeof(float)+sizeof(DATAHEAD)
    int Nums;            // 通道个数（16+48=64）
    int CommandID;       // 2=通道表，4=数据包（文档强制定义）
    int DeviceID;        // 设备编号（固定为1）
    int nPackCount;      // 包计数（自增）
    int nRef;            // 备用（填0）
    char TestName[56];   // 试验名称（56字节，含终止符）
} PACKETHEAD;  // 大小=4*6 + 56 = 80字节（VC++6.0）

// 通道表信息结构体（文档定义）
typedef struct {
    char ChName[24];         // 通道英文名（24字节）
    char ChName_CHN[56];     // 通道中文名（56字节）
    char EngUnit[24];        // 工程单位（24字节）
    float DownLimit;         // 下限值（4字节）
    float UpLimit;           // 上限值（4字节）
    int ChType;              // 通道类型（4字节）
    int AlarmFlag;           // 报警标志（4字节）
    float pre_Alarm_low;     // 预警下限（4字节）
    float pre_Alarm_up;      // 预警上限（4字节）
    float Alarm_low;         // 报警下限（4字节）
    float Alarm_up;          // 报警上限（4字节）
    int ptnums;              // 小数显示位数（4字节） - AETIMS文档要求
    int bk;                  // 备用（4字节） - AETIMS文档要求
    // 在MinGW中，结构体大小为144字节，与VC++6.0的136字节不同
    // 这是编译器对齐规则的差异
} SENDTAB;  // 大小=24+56+24+4*6+4*4 = 104+24+16 = 144字节（MinGW）

// 数据包包头结构体（文档定义）
typedef struct {
    char StatusName[32]; // 试验状态名（如“同时采集”）
    SYSTEMTIME SysTime;  // 采集时间
} DATAHEAD;  // 大小=32 + 16 = 48字节（VC++6.0）

// 通道表包（文档定义）
typedef struct {
    PACKETHEAD PacketHead;  // 包头（80字节）
    SENDTAB ChTab[64];      // 64个通道（16DPS+48DTS）
} DATAPACKET1;  // 大小=80 + 64*136 = 80 + 8704 = 8784字节

// 数据包（文档定义）
typedef struct {
    PACKETHEAD PacketHead;  // 包头（80字节）
    DATAHEAD DataHead;      // 数据包包头（48字节）
    float AcqData[64];      // 64通道数据（16DPS+48DTS）
} DATAPACKET3;  // 大小=80 + 48 + 64*4 = 128 + 256 = 384字节


// 多截面通道表包（专用，符合AETIMS最大2000通道定义）
typedef struct {
    PACKETHEAD PacketHead;  // 包头（80字节）
    SENDTAB ChTab[2000];    // 最大2000通道（兼容AETIMS接口）
} MS_DATAPACKET1;  // 多截面专用通道表包

// 多截面数据包（专用）
typedef struct {
    PACKETHEAD PacketHead;  // 包头（80字节）
    DATAHEAD DataHead;      // 数据包包头（48字节）
    float AcqData[2000];    // 最大2000通道数据
} MS_DATAPACKET3;  // 多截面专用数据包


// DTS-48数据包结构体
typedef struct _DTS_PACKET
{
    int nPacketType;      // 数据包类型 0x55
    int nFrameNo;         // 帧计数
    int64_t nSecs;        // 时间戳秒
    int32_t nNSecs;       // 时间戳纳秒
    float fMeasValue[48]; // 采集数据（48通道）
    float fRefValue[48];  // 参考通道值
} DTS_PACKET;

// 恢复默认对齐
#if defined(_MSC_VER)
#pragma pack(pop)
#else
#pragma pack()
#endif


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 辅助函数
    void appendTextEdit(const QString &data);
    void appendTextEditDTS(const QString &data);
    void delayEventLoopMS(unsigned int msec);
    float  convertUnit(float  value, bool isKPa);
    void saveCalibCoeffs(bool saveZero, bool saveSpan);

    // DTS-48辅助函数
    void initDTSDisplay();
    void updateDTSDisplay(const DTS_PACKET &pkt);
    void parseDTSPacket(const QByteArray &data);

    // 图表相关函数
    void initPressureChart();
    void initTemperatureChart();
    void updatePressureChart(const DPS_PACKET &pkt);
    void updateTemperatureChart(const DTS_PACKET &pkt);
    void clearCharts();

    // 网络配置相关函数
    void changeDTSIPAddress(const QString &newIP, const QString &subnet = "255.255.255.0",
                            const QString &gateway = "192.168.100.1");
    void connectToDTSWithNewIP();

    // 自动保存相关函数
    void initAutoSave();
    void startAutoSave();
    void stopAutoSave();
    void autoSaveData();
    bool saveDPSDataToFile(const QList<DPS_PACKET> &dataList, const QString &fileName, bool isAutoSave);
    bool saveDTSDataToFile(const QList<DTS_PACKET> &dataList, const QString &fileName, bool isAutoSave);

    // 新增：单个Excel文件自动保存函数
    void createNewAutoSaveFile(bool forDPS, bool forDTS);
    void appendDataToCurrentExcel(bool forDPS, bool forDTS);

    // TCP服务器相关函数
    void initTCPServer();
    void startTCPServer();
    void stopTCPServer();
    void sendTCPServerData();
    void updateTCPStatus();

    // 新增：数据计算与UI初始化函数
    void initMultiSectionUI(); // 初始化多截面显示UI
    double calculatePressureBySection(int sectionIndex, double Ps); // 按截面计算压力
    double calculateTemperatureBySection(int sectionIndex, double Ts); // 按截面计算温度
    void updatePressureMultiSection(double Ps); // 更新所有压力截面数据
    void updateTemperatureMultiSection(double Ts); // 更新所有温度截面数据
    // ==================== 多源加载：按指定通道生成多截面数据（不影响50Hz采集） ====================
    // 压力：使用DPS指定通道作为各截面组的加载Ps（单位MPa），再按文档算法+扰动生成384通道
    void updatePressureMultiSectionFromSources(double psCh1, double psCh5, double psCh9,
                                               double psCh11, double psCh16,
                                               qint32 frameNo, bool updateUI);

    // 温度：使用DTS指定通道作为各截面组的加载Ts（单位℃），未接入传感器则对应截面填充值-270℃
    void updateTemperatureMultiSectionFromSources(float tsCh5, bool hasCh5,
                                                  float tsCh10, bool hasCh10,
                                                  float tsCh15, bool hasCh15,
                                                  float tsCh20, bool hasCh20,
                                                  float tsCh25, bool hasCh25,
                                                  qint32 frameNo, bool updateUI);

    void updateTemperatureMultiSectionUI(const QVector<double>& allChannels); // UI同步函数（温度）

    void initPressureTableHeaders(); // 初始化压力表格表头
    void initTemperatureTableHeaders(); // 初始化温度表格表头

    // 新增：多截面数据保存函数
    bool saveMultiSectionPressureDataToExcel(const QString &fileName);
    bool saveMultiSectionTemperatureDataToExcel(const QString &fileName);

    // 新增函数声明
    QWidget* createPressureSectionWidget(int sectionIndex, const QString& title, int rows, int columns);
    QWidget* createTemperatureSectionWidget(int sectionIndex, const QString& title, int rows, int columns);
    QString generatePressureChannelName(int sectionIndex, int row, int col);
    QString generateTemperatureChannelName(int sectionIndex, int row, int col);

    // 0截面分页显示函数（修改为按页显示）
    QWidget* createPressureSection0SubWidget(int pageIndex, const QString& title);
    QWidget* createTemperatureSection0SubWidget(int pageIndex, const QString& title);

    void sendMultiSectionTCPServerData();
    void sendMultiSectionChannelTable();
    void sendMultiSectionDataPacket();
    void updatePressureMultiSectionUI(const QVector<double>& allChannels); // UI同步函数
    void verifyDataAlignment();
    void monitorPerformance();
    void updateCollectionRate();

    // ==================== 50Hz稳定采集：TCP字节流重组/拆包 ====================
    // Qt的readyRead是“字节可读”通知，不保证一次回调=一帧。
    // 在UI负载/文件写入负载较高时，半包/粘包会导致原解析丢帧，从而采样率波动。
    // 这里仅做“拼帧/拆帧”，再调用原有解析函数，保持原功能不变。
    int calcDpsFrameSize(quint8 streamIndex) const;
    void processDpsRxBuffer();
    void processDtsRxBuffer();
private slots:
    // 原有槽函数（DPS-16）
    void on_pushButton_clicked();
    void readPacket(QByteArray pkt);
    void slotConnected();
    void slotDisonnected();
    void on_pushButton_update_clicked();
    void handleRcvTCPPktPSI(QByteArray pktArray);
    void on_pushButton_config_clicked();
    void on_pushButton_scan_clicked();
    void on_pushButton_stop_clicked();

    // 原有新增槽函数（DPS-16）
    void on_btnApplyNetwork_clicked();
    void on_btnApplyCollect_clicked();
    void on_btnApplyUnit_clicked();
    void on_btnZeroCalib_clicked();
    void on_btnSpanCalib_clicked();
    void on_btnMultiCalibStart_clicked();
    void on_btnMultiCalibCollect_clicked();
    void on_btnMultiCalibCalc_clicked();
    void on_btnMultiCalibAbort_clicked();
    void on_btnClearData_clicked();
    void on_btnSaveExcel_clicked();

    // 新增DTS-48槽函数
    void on_btnDTSConnect_clicked();
    void on_btnDTSApplyNetwork_clicked();
    void on_btnDTSQuickConfig_clicked();
    void on_btnDTSScan_clicked();
    void on_btnDTSStop_clicked();
    void on_btnDTSClear_clicked();
    void on_btnDTSSave_clicked();
    void readPacketDTS(QByteArray pkt);
    void slotDTSConnected();
    void slotDTSDisconnected();
    void readColdJunctionTemperature();  // 读取冷端温度

    // 新增IP修改相关槽函数
    void on_btnQuickSetIP_clicked();           // 快速设置IP
    void on_btnReconnectNewIP_clicked();       // 重新连接新IP
    void on_btnResetDefaultIP_clicked();       // 复位为默认IP

    // 新增自动保存定时器槽函数
    void onAutoSaveTimer();                    // 自动保存定时器触发

    // 新增：图表实时更新定时器（与采集解耦，保证50Hz）
    void onChartUpdateTimer();

    // 新增同时采集相关槽函数
    void on_btnSimultaneousStart_clicked();          // 同时采集
    void on_btnSimultaneousStop_clicked();           // 同时结束
    void updateSimultaneousButtonsState();           // 更新按钮状态

    // 新增TCP服务器槽函数
    void onNewTCPConnection();
    void onTCPClientDisconnected();
    void onTCPSendTimer();

    // 新增：DTS单独通道配置槽函数（点击"单独配置"按钮触发）
    void on_btnDTSSingleConfig_clicked();

    void on_btnTestChannel_clicked();
    void updateTestChannelTemperature();
    void onPressureRandomTimerTimeout();

    // 新增：专门的多截面数据发送定时器函数
    void onMultiSectionSendTimer();

    // 新增测试通道显示按钮槽函数（UI自动关联可省略）
    //void on_btnShowMultiSection_clicked();
private:
    Ui::MainWindow *ui;           // UI对象
    TcpClient *tcpClient;         // DPS-16 TCP客户端
    TcpClient *tcpClientDTS;      // DTS-48 TCP客户端
    QList<DPS_PACKET> m_collectedData;    // DPS采集数据缓存
    QList<DTS_PACKET> m_collectedDataDTS; // DTS采集数据缓存
    QList<QLabel*> m_dtsDisplayLabels;    // DTS数据显示标签

    // 图表相关成员变量
    QChart *pressureChart;                // 压力图表
    QChart *temperatureChart;             // 温度图表
    QList<QLineSeries*> pressureSeries;   // 16个压力通道曲线
    QList<QLineSeries*> temperatureSeries; // 48个温度通道曲线
    QValueAxis *pressureAxisX;            // 压力图表X轴
    QValueAxis *pressureAxisY;            // 压力图表Y轴
    QValueAxis *temperatureAxisX;         // 温度图表X轴
    QValueAxis *temperatureAxisY;         // 温度图表Y轴
    int maxPoints = 100000;                 // 图表显示的最大点数

// 新增：图表更新与采集解耦（50Hz实时曲线）
QTimer *m_chartUpdateTimer = nullptr;
QQueue<DPS_PACKET> m_pendingDpsPlot;   // 等待绘制的压力帧队列
QQueue<DTS_PACKET> m_pendingDtsPlot;   // 等待绘制的温度帧队列
quint32 m_dpsFrameSeq = 0;             // DPS帧号（从1开始）
quint32 m_dtsFrameSeq = 0;             // DTS帧号（从1开始）
int m_chartMaxPoints = 1000;           // 图表窗口点数（只影响显示，不影响保存/缓存）
int m_chartNameUpdateDiv = 10;         // 曲线名称更新分频（降低Legend刷新负担）


    // 自动保存相关成员变量
    QTimer *m_autoSaveTimer;              // 自动保存定时器
    QString m_autoSavePath;               // 自动保存路径
    bool m_autoSaveEnabled;               // 自动保存是否启用
    int m_autoSaveInterval;               // 自动保存间隔（毫秒）
    QList<DPS_PACKET> m_autoSaveBufferDPS; // DPS自动保存缓冲区
    QList<DTS_PACKET> m_autoSaveBufferDTS; // DTS自动保存缓冲区

    // 新增：单个Excel文件自动保存相关变量
    QXlsx::Document *m_currentDPSDoc;     // 当前DPS Excel文档
    QXlsx::Document *m_currentDTSDoc;     // 当前DTS Excel文档
    QString m_dpsAutoSaveFileName;        // DPS自动保存文件名
    QString m_dtsAutoSaveFileName;        // DTS自动保存文件名
    int m_dpsRowCounter;                  // DPS当前写入行
    int m_dtsRowCounter;                  // DTS当前写入行
    int m_autoSaveFileCounter;            // 自动保存文件计数器

    // 添加采集频率监控
    QElapsedTimer m_collectionTimer;
    int m_collectionCount;

    // 添加时间基准记录
    QDateTime m_dpsStartTime;     // DPS采集开始时的系统时间
    qint64 m_dpsStartTimestamp;   // DPS第一个数据包的时间戳（纳秒）
    bool m_dpsTimeBaseRecorded;   // 是否记录了DPS时间基准

    QDateTime m_dtsStartTime;     // DTS采集开始时的系统时间
    qint64 m_dtsStartTimestamp;   // DTS第一个数据包的时间戳（纳秒）
    bool m_dtsTimeBaseRecorded;   // 是否记录了DTS时间基准

    // 新增同时采集相关成员变量
    bool m_dpsReady;              // DPS准备状态（已连接且快速配置）
    bool m_dtsReady;              // DTS准备状态（已连接且快速配置）
    bool m_simultaneousCollecting; // 是否正在同时采集

    // 新增TCP服务器相关成员变量
    QTcpServer *m_tcpServer;             // TCP服务器
    QList<QTcpSocket*> m_tcpClients;     // 连接的客户端列表
    QTimer *m_tcpSendTimer;              // TCP发送定时器
    qint32  m_tcpFrameCounter;               // TCP发送帧计数器
    bool m_tcpSendEnabled;               // TCP发送是否启用

    // 数据缓存
    float m_lastDPSPressures[16];        // 最后接收的DPS压力数据
    float m_lastDTSTemperatures[48];     // 最后接收的DTS温度数据
    bool m_dpsDataReady;                 // DPS数据是否准备好
    bool m_dtsDataReady;                 // DTS数据是否准备好

    DATAPACKET1 m_aetimsChannelPacket; // AETIMS通道表包（仅发送1次）
    int m_aetimsDataPacketCount;      // AETIMS数据包计数（自增）
    bool m_aetimsChannelSent;         // 通道表是否已发送

    // 新增：DTS单独通道配置控件（用于输入通道号和触发单独配置）
    QLineEdit *lineEdit_dts_channel;   // 通道号输入框（1-48）
    QPushButton *btnDTSSingleConfig;   // 单独配置按钮

    // 新增：多截面压力数据相关
    QSlider *sliderPressurePs;
    QLabel *labelPressurePs;
    QTabWidget *tabWidgetPressure;
    QTableWidget *tableWidgetPressure0;
    QTableWidget *tableWidgetPressure01;
    QTableWidget *tableWidgetPressure1;
    QTableWidget *tableWidgetPressure15;
    QTableWidget *tableWidgetPressure2;
    QTableWidget *tableWidgetPressure3;
    QTableWidget *tableWidgetPressure35;
    QTableWidget *tableWidgetPressure4;
    QTableWidget *tableWidgetPressure5;

    // 新增：多截面温度数据相关
    QSlider *sliderTemperatureTs;
    QLabel *labelTemperatureTs;
    QTabWidget *tabWidgetTemperature;
    QTableWidget *tableWidgetTemperature0;
    QTableWidget *tableWidgetTemperature01;
    QTableWidget *tableWidgetTemperature1;
    QTableWidget *tableWidgetTemperature15;
    QTableWidget *tableWidgetTemperature2;
    QTableWidget *tableWidgetTemperature3;
    QTableWidget *tableWidgetTemperature35;
    QTableWidget *tableWidgetTemperature4;
    QTableWidget *tableWidgetTemperature5;

    // 新增：多截面数据缓存结构体
    struct MultiSectionPressureData {
        QDateTime collectTime;
        int frameNo;
        QVector<double> allChannels; // 存储所有截面通道数据（按表头顺序）
    };
    struct MultiSectionTemperatureData {
        QDateTime collectTime;
        int frameNo;
        QVector<double> allChannels; // 存储所有截面通道数据（按表头顺序）
    };
    // 多截面TCP发送相关变量
    // 多截面TCP发送相关变量（修正后）
    MS_DATAPACKET1 m_msAetimsChannelPacket; // 多截面专用通道表包
    MS_DATAPACKET3 m_msAetimsDataPacket;   // 多截面专用数据包
    int m_msAetimsDataPacketCount;        // 多截面数据包计数（自增）
    bool m_msAetimsChannelSent;           // 多截面通道表是否已发送
    QMutex m_msTcpMutex;                  // 多截面TCP发送锁
    const int MS_TOTAL_CHANNELS = 768;    // 多截面总通道数（压力384+温度384）
    const int MS_PRESSURE_CHANNELS = 384;  // 多截面压力通道数
    const int MS_TEMPERATURE_CHANNELS = 384;// 多截面温度通道数

    // 添加以下成员变量声明
    bool m_multiSectionSendEnabled;     // 多截面数据发送是否启用
    int m_multiSectionFrameCounter;     // 多截面数据帧计数器
    bool m_multiSectionChannelSent;     // 多截面通道表是否已发送

    // 新增：缓存容器
    QList<MultiSectionPressureData> m_msPressureCache;
    QList<MultiSectionTemperatureData> m_msTemperatureCache;

    // 随机数生成器
    std::mt19937 m_randomGenerator;
    std::uniform_real_distribution<double> m_pressureDist;
    std::uniform_real_distribution<double> m_temperatureDist;
    // 新增：压力随机扰动定时刷新定时器
    QTimer *m_pressureRandomTimer;

    const int sectionStartIndices[9] = {0, 192, 216, 240, 264, 288, 312, 336, 360}; // 硬编码固定起始索引

    // 多截面压力截面索引范围（与通道表强绑定）
    const QVector<QPair<int, int>> sectionRanges = {
        {0, 191}, {192, 215}, {216, 239}, {240, 263},
        {264, 287}, {288, 311}, {312, 335}, {336, 359}, {360, 383}
    };

    // 采集速率监测
    QElapsedTimer m_dpsRateTimer;
    QElapsedTimer m_dtsRateTimer;
    int m_dpsPacketCount;
    int m_dtsPacketCount;
    double m_dpsActualRate;
    double m_dtsActualRate;
    QTimer* m_rateMonitorTimer;

    // 采集速率显示标签
    QLabel* m_labelDPSRate;
    QLabel* m_labelDTSRate;

    // ==================== 50Hz稳定采集：TCP字节流重组/拆包 ====================
    // 接收缓冲区（TCP字节流可能出现半包/粘包）
    QByteArray m_dpsRxBuffer;
    QByteArray m_dtsRxBuffer;


    // 新增：缓存锁（避免线程冲突）
    QMutex m_msDataMutex;
};
static_assert(sizeof(PACKETHEAD) == 80, "PACKETHEAD大小必须为80字节（AETIMS要求）");
static_assert(sizeof(MS_DATAPACKET3) == 8128, "多截面数据包大小必须为8128字节（80+48+2000×4）");
#endif // MAINWINDOW_H
