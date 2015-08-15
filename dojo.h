#ifndef DOJO
#define DOJO

#include <QObject>
#include <QVector3D>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QHash>
#include <QDateTime>
#include <QTimer>
#include <QUdpSocket>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QtSql>
#include "math.h"

typedef int dojoID;

//defines how much synapse is affecting neuron by each AP depending on its length
#define LENGTH_CONST 0.5 //in mm - neurons closer than 0.5 is affecting more than 37%
//defines how long synapse is affecting neuron by each AP
#define TIME_CONST 50.0 // in ms - about 15 ms to add above 10% to neuron voltage
//defines how fast voltage will be removed from neuron
#define PUMPING_CONST 1000.0 // in ms - about 50 ms to remove 70% and 70 ms to remove 90% of injected voltage
//defines varaible time between checks
#define TIME_SCALE 0.3 //next check in 0.3 longer than current one
//defines how deep the voltage will be dropped after AP ->> how fast the neuron will be ready for next AP
#define DEFAULT_VOLTAGE 0

#define UDP_SERVER_PORT 25098
#define UDP_CLIENT_PORT 26128
#define WS_SERVER_PORT 1234

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

#include "dojosynapse.h"
#include "dojoneuron.h"
#include "dojoioserver.h"
#include "dojowsserver.h"
#include "dojostorage.h"
#include "dojonetwork.h"


#endif // DOJO

