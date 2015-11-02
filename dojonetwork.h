#ifndef DOJONETWORK_H
#define DOJONETWORK_H

#include <QObject>
#include <QTimer>

#include "dojostorage.h"
#include "dojoioserver.h"
#include "dojowsserver.h"
#include "dojologger.h"

class dojoWsServer;

class dojoNetwork : public QObject
{
    Q_OBJECT
public:
    explicit dojoNetwork(QObject *parent = 0);
    void start();
    dojoID createNode(QVector3D pos, QVector3D axon, float size);
    void bindNodes(dojoID source, dojoID target);

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
