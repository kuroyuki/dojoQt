#ifndef DOJOCLIENT_H
#define DOJOCLIENT_H

#include <QObject>
#include <QTimer>
#include <QUdpSocket>

#define UDP_SERVER_PORT 25098
#define UDP_CLIENT_PORT 26128

typedef int dojoID;

class dojoClient : public QObject
{
    Q_OBJECT
public:
    explicit dojoClient( QString host, QObject *parent = 0);
    ~dojoClient();

    void connectToServer();
    void registerInput(dojoID id, float* data);
    void registerOutput(dojoID id, float* data);

signals:

public slots:
    /*
    void slotTcpReadyRead();
    void slotTcpDisconnected();
    void slotTcpError(QAbstractSocket::SocketError error);
    */
    void slotTimeout();

    void slotUdpReadyRead();
    void slotUdpError(QAbstractSocket::SocketError error);

private:
    QString dojoHost;
    QHash<dojoID, float*> inputs;
    QHash<dojoID, float*> outputs;

    QTimer* sendTimer;
    //QTcpSocket* tcpSocket;
    QUdpSocket* udpSocket;

    //bool isTcpConnected;
};

#endif // DOJOCLIENT_H
