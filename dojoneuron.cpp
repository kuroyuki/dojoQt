#include "dojoneuron.h"
#include <QDateTime>
#include "math.h"

dojoNeuron::dojoNeuron(dojoStorage* str, dojoID node_id, QObject *parent) : QObject(parent)
{
    storage = str;
    id = node_id;

    voltage = 0;

    nextCheck = QDateTime::currentMSecsSinceEpoch();
    lastAction = nextCheck;
}
void dojoNeuron::handleSpike(dojoID source, float terminals){
    if(sources.contains(source)){
        sources[source]->handleSpike(terminals);
        nextCheck = QDateTime::currentMSecsSinceEpoch();
    }
}
void dojoNeuron::process(){
    //setup next check
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    //Process()
    //sum all sources
    float synapticVoltage = 0;
    foreach (dojoSynapse* synapse, sources){
        synapticVoltage += synapse->getVoltage(now);

        if(synapse->lastAction > this->lastAction)
            this->lastAction = synapse->lastAction;
    }
    //add new voltage
    voltage += synapticVoltage;


    //check for threshold
    if(voltage > 1){
        float term = storage->getNeuronTerminals(id);

        //send spike to all targets
        emit spike(id, term);

        //change sources synaptic coefficients  (Hebb rule??)
        foreach (dojoSynapse* synapse, sources){
            synapse->changePermeability(now);
        }

        //reset voltage to default
        voltage = DEFAULT_VOLTAGE;

        //remember this moment
        lastAction = now;

        //next check right now (in 1 ms)
        nextCheck = now+1;
    }
    //pumping
    else{
        //safe guard to be not too much hyperpoliarized
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
