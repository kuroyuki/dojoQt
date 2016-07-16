#ifndef DOJOIOSERVER_H
#define DOJOIOSERVER_H

#include <QObject>
#include <QUdpSocket>

#include "dojoneuron.h"

#define UDP_SERVER_PORT 25098
#define UDP_CLIENT_PORT 26128

class dojoIOServer : public dojoNeuron
{
public:
    dojoIOServer(dojoStorage* storage);
    void handleSpike(dojoID source, float transmiterValue);

    void addInput(dojoID source, dojoNeuron* target);

public slots:
    void slotUdpReadyRead();

private:
    QUdpSocket* udpSocket;
};


#endif // DOJOIOSERVER_H
