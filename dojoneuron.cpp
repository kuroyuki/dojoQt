#include "dojoneuron.h"
#include <QDateTime>
#include "math.h"

dojoNeuron::dojoNeuron(dojoStorage* str, dojoID node_id, QObject *parent) : QObject(parent)
{
    storage = str;
    id = node_id;

    voltage = 0;

    nextCheck = QDateTime::currentMSecsSinceEpoch();
}
void dojoNeuron::ap(dojoID source, float terminals){
    if(sources.contains(source)){
        sources[source]->ap(terminals);
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

        //send ap to all targets
        for(int i=0;i<targets.size();i++){
            targets[i]->ap(id, term);
        }
        //change synaptic coefficients  (Hebb rule??)
        foreach (dojoSynapse* synapse, sources){
            synapse->changePermability(now);
        }

        //reset voltage
        voltage = DEFAULT_VOLTAGE;
        lastAction = now;

        //next check right now (in 1 ms)
        nextCheck = now+1;

    }
    //pumping
    else{
        //safe guard to be not hyperpoliarized
        if(voltage < -1)
            voltage = -1;
        //pump some voltage to outside
        else{
            double pumped = voltage*(1-exp(-(now - lastAction)/PUMPING_CONST));
            voltage -= pumped;

            //DEBUG
            //qDebug()<<id<<':'<<pumped<<" now :"<<(now - lastAction);
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
void dojoNeuron::addTarget(dojoNeuron* target){
    if(!targets.contains(target))
        targets<<target;
}
