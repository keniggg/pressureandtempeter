#include "tcpclient.h"
#include <QDebug>
#include <QDateTime>
#include <QHostAddress>

TcpClient::TcpClient(QObject *parent) :
    QObject(parent)
{
    clientIP = "192.168.100.105";
    clientPort = 9000;
    connectBool = false;
    m_error = QAbstractSocket::ConnectionRefusedError;

    socket = new QTcpSocket(this);
    connect(socket,&QTcpSocket::readyRead,this,&TcpClient::readMessage);
    connect(socket,&QTcpSocket::connected,this,&TcpClient::isConnect);
    connect(socket,&QTcpSocket::disconnected,this,&TcpClient::isDisconnect);
    connect(socket,&QTcpSocket::errorOccurred,this,&TcpClient::isError);

    scanTimer = new QTimer(this);
    scanTimer->setInterval(2);
    connect(scanTimer, &QTimer::timeout,this,&TcpClient::scanTimerSlot);
    scanTimer->start();
}

TcpClient::~TcpClient()
{
    delete socket;
    delete scanTimer;
}

void TcpClient::scanTimerSlot()
{
    while(!ReceiveBytes.isEmpty())
    {
        QByteArray pkt = ReceiveBytes.at(0);
        ReceiveBytes.removeAt(0);
        emit alreadyRead(pkt);
    }
}

void TcpClient::newConnect()
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        qDebug() << "Already connected!";
        return;
    }
    socket->abort();
    socket->connectToHost(QHostAddress(clientIP), clientPort);
}

void TcpClient::closeConnect()
{
    socket->disconnectFromHost();
}

void TcpClient::setIP(QString ip)
{
    clientIP = ip;
}

void TcpClient::setPort(int port)
{
    clientPort = port;
}

void TcpClient::sendMessage(QString str)
{
    if (socket->state() != QAbstractSocket::ConnectedState)
    {
        qDebug() << "Not connected!";
        return;
    }
    qDebug() << "[Send] " << str;
    socket->write(str.toUtf8());
    socket->flush();
}

bool TcpClient::returnConnectState()
{
    return connectBool;
}

QString TcpClient::returnIP()
{
    return clientIP;
}

QString TcpClient::returnPort()
{
    return QString::number(clientPort);
}

int TcpClient::returnError()
{
    return m_error;
}

void TcpClient::readMessage()
{
    int bytes = socket->bytesAvailable();
    if (bytes > 0)
    {
        QByteArray rcvbytes = socket->read(bytes);
        ReceiveBytes.append(rcvbytes);
        qDebug() << "[Receive] " << rcvbytes.toHex();
    }
}

void TcpClient::isConnect()
{
    connectBool = true;
    qDebug() << "Connected to " << clientIP << ":" << clientPort;
    emit alreadyConnected();
}

void TcpClient::isDisconnect()
{
    connectBool = false;
    qDebug() << "Disconnected from " << clientIP << ":" << clientPort;
    emit alreadyDisconnected();
}

void TcpClient::isError(QAbstractSocket::SocketError er)
{
    m_error = er;
    qDebug() << "TCP Error:" << socket->errorString();
}

QString TcpClient::returnMessage()
{
    return message;
}
