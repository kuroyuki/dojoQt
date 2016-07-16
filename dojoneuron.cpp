#include "dojoneuron.h"
#include <QDateTime>
#include "math.h"
/*!
    \class dojoNeuron
    \brief This class provides API to simulate neurons.

    dojoNeuron is an implementation of neuron behavior like summator or synaptic inputs.

 */
/*!
 * \brief dojoNeuron::dojoNeuron
 * \param str
 * \param node_id
 * \param *parent
 * \desc The constructor stores dojoStorage pointer to get/update neuron data, this node_id which uses
 * during spikes emmiting. Also it is registering as Qt object
 */
dojoNeuron::dojoNeuron(dojoStorage* str, dojoID node_id, QObject *parent) : QObject(parent)
{
    storage = str;
    id = node_id;

    voltage = 0;


    lastAction = QDateTime::currentMSecsSinceEpoch();
    lastSpike = lastAction;
    nextCheck = lastAction+1;
}
void dojoNeuron::handleSpike(dojoID source, float transmiterValue){
     if(sources.contains(source)){
        lastAction = QDateTime::currentMSecsSinceEpoch();
        sources[source]->handleSpike(transmiterValue, lastAction);
        nextCheck = lastAction+1;
    }
}
void dojoNeuron::process(){
    //setup next check
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    //Process()
    //sum all sources
    float synapticVoltage = 0;
    foreach (dojoSynapse* synapse, sources){
        synapticVoltage += synapse->getVoltage();

        if(synapse->lastSpike > lastAction)
            lastAction = synapse->lastSpike;
    }
    //add new voltage
    voltage += synapticVoltage;


    //is spike should be generated
    if(voltage > 1){
        float term = changeTerminals(now-lastSpike);

        //save spike time in all synapses
        foreach (dojoSynapse* synapse, sources){
            synapse->postNeuronSpike(now);
        }

        //send spike to all targets
        emit spike(id, term);

        //reset voltage to default
        voltage = DEFAULT_VOLTAGE;

        //save spike moment
        lastSpike = now;

        //next check right now (in 1 ms)
        nextCheck = now+1;
    }
    //no spike at this time - pumping
    else{
        //safe guard to be not too much depoliarized
        if(voltage < -1)
            voltage = -1;
        //pump out some voltage from neuron
        else{
            double pumped = voltage*(1-exp(-(now - lastAction)/PUMPING_CONST));
            voltage -= pumped;
        }
        nextCheck = now+TIME_SCALE*(now-lastAction);
    }
}
void dojoNeuron::addSource(dojoID source){
    if(!sources.contains(source)){
        QString synapse = QString::number(source)+":"+QString::number(id);
        sources[source] = new dojoSynapse(synapse, storage);
    }
}
/*!
 * \brief dojoNeuron::changeTerminals
 * This is the main function to implement STDP for synapses.
 *
 * (wiki)
 * Synapses with a low initial probability of vesicle release act as high-pass filters:
 * because the release probability is low, a higher-frequency signal is needed to trigger release,
 * and the synapse thus selectively responds to high-frequency signals.
 *
 * Likewise, synapses with high initial release probabilities serve as low-pass filters,
 * responding to lower-frequency signals.
 *
 * Synapses with an intermediate probability of release act as band-pass filters that selectively
 * respond to a specific range of frequencies.
 *
 * (implementation)
 */
float dojoNeuron::changeTerminals(qint64 time){
    float newProb = 1-tanh(1-time/RESTORE_TIME);
    //qDebug()<<id<<" - ls "<<time<<", term: "<<newProb;
    storage->setNeuronReleaseProbability(id, newProb);
    return newProb;
}
