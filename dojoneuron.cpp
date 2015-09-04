#include "dojoneuron.h"

dojoNeuron::dojoNeuron(dojoID new_id, QVector3D new_pos, QVector3D new_axon)
{
    id = new_id;
    axon = new_axon;
    position = new_pos;

    actuator = 0;

    lastAction = QDateTime::currentMSecsSinceEpoch();
    lastGrow = lastAction;
    voltage = DEFAULT_VOLTAGE;
    axonTerminals = 1;

    size = 1.0;

    isGrow = false;
    growingRadius = 1;

    nextCheck = lastAction + qrand()%1000;
}
void dojoNeuron::ap(dojoID source, double terminals){
    if(sources.contains(source)){
        sources[source]->ap(terminals);
        nextCheck = QDateTime::currentMSecsSinceEpoch()+1;
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
        //send ap to all targets
        for(int i=0;i<targets.size();i++){
            targets[i]->ap(id, axonTerminals);           
        }
        //special send AP to dojoIOServer if it requires
        if(actuator)
            actuator->ap(id, axonTerminals);

        //change synaptic coefficients  (Hebb rule??)
        foreach (dojoSynapse* synapse, sources){
            synapse->changePermability(now);
        }

        //reset voltage
        voltage = DEFAULT_VOLTAGE;
        lastAction = now;

        //next check right now (in 1 ms)
        nextCheck = now+1;

        /*
        //DEBUG
        QTime nowT = QDateTime::currentDateTime().time();
        qDebug()<<nowT.toString()<<':'<<nowT.msec()<<" AP "<<id<<':'<<synapticVoltage;

        */
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
            //qDebug()<<':'<<pumped<<" now :"<<(now - lastAction);
        }
        nextCheck = now+TIME_SCALE*(now-lastAction);
    }

    //Growing up synapses each 5 sec
    if((now-lastGrow)>5000*growingRadius){
        lastGrow = now;
        //growing
        growingRadius *= GROWING_RATE;
        isGrow = true;
    }
}

void dojoNeuron::addSource(dojoID source, dojoSynapse* synapse){
    sources.insert(source, synapse);
}

void dojoNeuron::removeSource(dojoID source){
    if(sources.contains(source))
        sources.remove(source);
}
void dojoNeuron::addAct(dojoIOServer* server){
    actuator = server;
}
void dojoNeuron::addTarget(dojoNeuron* target){
    if(!targets.contains(target))
        targets<<target;
}

void dojoNeuron::removeTarget(dojoNeuron* target){
    if(targets.contains(target))
        targets.removeOne(target);
}
bool dojoNeuron::isGrowing(){
    return isGrow;
}
float dojoNeuron::getGrowingRadius(){
    return growingRadius;
}
bool dojoNeuron::isSourceExist(dojoID source){
    return sources.contains(source);
}

bool dojoNeuron::isTargetExist(dojoNeuron* target){
    return targets.contains(target);
}
