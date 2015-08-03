#ifndef DOJONETWORK_H
#define DOJONETWORK_H

#include <QObject>
#include "dojo.h"

//class dojoStorage;
class dojoServer;

class dojoNetwork : public QObject
{
    Q_OBJECT
public:
    explicit dojoNetwork(QString name, QObject *parent = 0);
    ~dojoNetwork();

    void start(){
        timer->start(timeout);
    }
    void stop(){
        timer->stop();
    }

    dojoID createNeuron(QVector3D pos, QVector3D axon);
    //void setNeuronPosition(dojoID id, QVector3D pos, QVector3D axon);
    void deleteNeuron(dojoID id);

    void bindNeurons(dojoID source, dojoID target);
    void unbindNeurons(dojoID source, dojoID target);

public slots:
    void process();
    void slotTimeout();
    void handleEvent(QJsonObject event);
    void externalAp(dojoID source, dojoID target, double value);

signals:
    void dojoEvent(QJsonObject json);
    void dojoProcess();

private:

    QHash<dojoID,dojoNeuron*> neurons;
    //dojoStorage* storage;
    dojoServer* server;

    QTimer* timer;
    int timeout;

    dojoID nextId;
};

#endif // DOJONETWORK_H
