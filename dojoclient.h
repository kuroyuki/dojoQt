#ifndef DOJOCLIENT_H
#define DOJOCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include "dojo.h"

class dojoClient : public QObject
{
    Q_OBJECT
public:
    explicit dojoClient( QString host, QObject *parent = 0);
    ~dojoClient();

    void connectToServer();
    void registerInput(dojoID id, double* data);
    void registerOutput(dojoID id, double* data);

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
    QHash<dojoID, double*> inputs;
    QHash<dojoID, double*> outputs;

    QTimer* sendTimer;
    //QTcpSocket* tcpSocket;
    QUdpSocket* udpSocket;

    //bool isTcpConnected;
};

#endif // DOJOCLIENT_H
