#ifndef DOJONETWORK_H
#define DOJONETWORK_H

#include "dojo.h"

class dojoNeuron;
class dojoStorage;
class dojoWsServer;
class dojoIOServer;

class dojoNetwork : public QObject
{
    Q_OBJECT
public:
    explicit dojoNetwork(QString name, QObject *parent = 0);
    ~dojoNetwork();

    void start();
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
    void eventHandler(QJsonObject event);
    void externalApHandler(dojoID source, dojoID target, double value);

signals:
    void dojoEvent(QJsonObject json);
    void dojoProcess();

private:
    void restoreNode();
    void restoreSynapse();
    void restoreSensor();
    void restoreActuator();

    QHash<dojoID,dojoNeuron*> neurons;
    dojoStorage* storage;
    dojoWsServer* wsServer;
    dojoIOServer* io;

    QTimer* timer;
    int timeout;

    dojoID nextId;
};

#endif // DOJONETWORK_H
