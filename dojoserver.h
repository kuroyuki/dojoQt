#ifndef DOJOSERVER_H
#define DOJOSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QHash>
#include <QTimer>
#include "dojo.h"

class dojoNeuron;

struct dojoUdpSensor{
    dojoID target;
    QHostAddress address;
    quint16 port;
};
struct dojoUdpAct{
    dojoID source;
    dojoID target;
    QHostAddress address;
    quint16 port;
};

class dojoActuator;

class dojoServer : public QObject
{
    Q_OBJECT
public:
    explicit dojoServer(QObject *parent = 0);
    void addSensor(dojoID source, dojoUdpSensor sensor);
    void addActuator(dojoID source, dojoUdpAct actuator);
    void ap(dojoID source, double terminals);

signals:
    void serverEvent(QJsonObject event);
    void serverAp(dojoID source, dojoID target, double value);

public slots:
    void slotUdpReadyRead();
    void slotUdpError(QAbstractSocket::SocketError error);

private:

    QUdpSocket* udpSocket;

    QHash<dojoID, dojoUdpSensor>sensors;
    QHash<dojoID, dojoUdpAct>actuators;

};

#endif // DOJOSERVER_H
