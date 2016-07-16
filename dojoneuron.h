#ifndef DOJONEURON_H
#define DOJONEURON_H

#include <QObject>
#include "dojostorage.h"
#include "dojosynapse.h"

//defines how fast voltage will be removed from neuron
#define PUMPING_CONST 1000.0// in ms - about 50 ms to remove 70% and 70 ms to remove 90% of injected voltage

//defines varaible time between checks
#define TIME_SCALE  0.3//next check in 0.3 longer than current one
//defines how deep the voltage will be dropped after AP ->> how fast the neuron will be ready for next AP
#define DEFAULT_VOLTAGE  0.2//after AP the membrane will be hyperpolirized to -0.2 and will restored in 20 ms to -0.04
//defines speed of expanding area where axon can find another neurons to make synapses
#define GROWING_RATE 1.3

//defines presynaptic time const
#define RESTORE_TIME 20.0//in 36 ms it restores 83% of maximum transmiter value

class dojoSynapse;

class dojoNeuron : public QObject
{
    Q_OBJECT
public:
    explicit dojoNeuron(dojoStorage* str, dojoID node_id, QObject *parent = 0);

    void process();
    qint64 getNextCheck(){return nextCheck;}

    void addSource(dojoID source);

signals:
    void neuronEvent(QString event);
    void spike(dojoID id, float term);

public slots:
    virtual void handleSpike(dojoID source, float transmiterValue);
    virtual void slotUdpReadyRead(){};

protected:
    dojoStorage* storage;

private :
    float changeTerminals(qint64 time);

     dojoID id;

     QHash<dojoID, dojoSynapse*>sources;

     float voltage;

     qint64 nextCheck;
     qint64 lastSpike;
     qint64 lastAction;
};

#endif // DOJONEURON_H
