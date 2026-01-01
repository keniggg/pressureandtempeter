#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QTimer>
#include <QByteArray>
#include <QAbstractSocket>

// 联合体定义
typedef union
{
    float f;
    int32_t d;
    char bytes[4];
}BYTE4;

// 位域结构体
typedef union{
    struct STREAMDATABITS{
        unsigned ValveStatus	:1;
        unsigned TmpStatus		:1;
        unsigned Blank1			:2;
        unsigned PresEU			:1;
        unsigned PresAD			:1;
        unsigned PresVolt		:1;
        unsigned TmpEU			:1;
        unsigned TmpAD			:1;
        unsigned TmpVolt		:1;
        unsigned TimeStamp		:1;
        unsigned Blank2			:5;
    }StreamDataBits;
    uint16_t	bbbb;
}STREAMDATA_BITFIELD;

// 数据流配置结构体
typedef struct
{
    struct{
        QString  pppp;
        uint8_t  sync;
        uint32_t per;
        uint8_t  format;
        uint32_t num;
        STREAMDATA_BITFIELD  databit;
    }StreamInfo[3];
    uint8_t  pro;
    uint16_t remport;
    QString  ipaddr;
    int lenfix;
}PSISTREAM;

// 数据包结构体
typedef struct _DPS_PACKET
{
    int nPacketType;
    int nFrameNo ;
    QString fPress[16];
    QString fTemp[16];
    int32_t nFrameTime_s ;
    int32_t nFrameTime_ns ;
} DPS_PACKET;

class TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient();

    void newConnect();
    void closeConnect();
    void setIP(QString ip);
    void setPort(int port);
    void sendMessage(QString str);
    bool returnConnectState();
    QString returnIP();
    QString returnPort();
    int returnError();

signals:
    void alreadyRead(QByteArray);
    void alreadyConnected();
    void alreadyDisconnected();

public slots:
    QString returnMessage();

private slots:
    void scanTimerSlot();
    void readMessage();
    void isConnect();
    void isDisconnect();
    void isError(QAbstractSocket::SocketError er);

private:
    QTimer*               scanTimer;
    QVector<QByteArray>   ReceiveBytes;
    QTcpSocket *socket;
    QString clientIP;
    int clientPort;
    QString message;
    bool connectBool;
    QAbstractSocket::SocketError m_error;
};

#endif // TCPCLIENT_H
