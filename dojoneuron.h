#ifndef DOJONEURON_H
#define DOJONEURON_H

#include <QObject>
#include "dojostorage.h"
#include "dojosynapse.h"

//defines how fast voltage will be removed from neuron
#define PUMPING_CONST 1000.0 // in ms - about 50 ms to remove 70% and 70 ms to remove 90% of injected voltage
//defines varaible time between checks
#define TIME_SCALE 0.3 //next check in 0.3 longer than current one
//defines how deep the voltage will be dropped after AP ->> how fast the neuron will be ready for next AP
#define DEFAULT_VOLTAGE 0
//defines speed of expanding area where axon can find another neurons to make synapses
#define GROWING_RATE 1.3

class dojoSynapse;

class dojoNeuron : public QObject
{
    Q_OBJECT
public:
    explicit dojoNeuron(dojoStorage* str, dojoID node_id, QObject *parent = 0);

    virtual void ap(dojoID source, float terminals);    
    void process();
    qint64 getNextCheck(){return nextCheck;}

    void addSource(dojoID source);
    void addTarget(dojoNeuron* target);

signals:

public slots:
    virtual void slotUdpReadyRead(){};

private :
     dojoStorage* storage;
     dojoID id;

     QList<dojoNeuron*> targets;
     QHash<dojoID, dojoSynapse*>sources;

     float voltage;

     qint64 nextCheck;
     qint64 lastAction;
};

#endif // DOJONEURON_H
