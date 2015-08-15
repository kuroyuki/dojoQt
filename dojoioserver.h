#ifndef DOJOIOSERVER_H
#define DOJOIOSERVER_H

#include "dojo.h"

class dojoIOServer : public QObject
{
    Q_OBJECT
public:
    explicit dojoIOServer(QObject *parent = 0);

    void ap(dojoID source, double terminals);

signals:
    void serverAp(dojoID source, dojoID target, double value);

public slots:
    void eventHandler(QJsonObject event);

    void slotUdpReadyRead();
    void slotUdpError(QAbstractSocket::SocketError error);

private:

    QUdpSocket* udpSocket;

    QHash<dojoID, dojoUdpSensor>sensors;
    QHash<dojoID, dojoUdpAct>actuators;

};

#endif // DOJOIOSERVER_H
