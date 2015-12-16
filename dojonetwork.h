#ifndef DOJONETWORK_H
#define DOJONETWORK_H

#define DEBUG

#include <QObject>
#include <QTimer>

#include "dojostorage.h"
#include "dojoioserver.h"
#include "dojowsserver.h"
#include "dojologger.h"

#define DEFAULT_PERM 5 //default permability for all new synapses

class dojoWsServer;

class dojoNetwork : public QObject
{
    Q_OBJECT
public:
    explicit dojoNetwork(QObject *parent = 0);
    void start();
    dojoID createNode(QVector3D pos, QVector3D axon, float size);
    void bindNodes(dojoID source, dojoID target);
    void subscribeWsForSpikes(dojoID neuron);

signals:
    void networkEvent(QString event);

public slots:
    void process();

private:
    void restoreNetwork();

    dojoStorage* storage;
    dojoIOServer* io;
    dojoWsServer* ws;
    dojoLogger* logger;

    QHash<dojoID, dojoNeuron*> neurons;

    QTimer* timer;
    int timeout;
};

#endif // DOJONETWORK_H
