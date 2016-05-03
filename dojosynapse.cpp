#include "dojosynapse.h"
#include <QDateTime>
#include "math.h"

dojoSynapse::dojoSynapse(QString syn, dojoStorage *str){
    synapse = syn;
    storage = str;

    cleft = 0;
    lastAction = QDateTime::currentMSecsSinceEpoch();
}
float dojoSynapse::getVoltage(qint64 now){
    //calc time-depend value
    float binded = 1-exp(-(now - lastAction)/TIME_CONST);

    //binded mediator
    float voltage = cleft*binded;

    //remove binded mediator from cleft
    cleft -= voltage;

    float lengthCoef = exp(-storage->getSynapseLength(synapse)/LENGTH_CONST);

    float vlt = voltage*lengthCoef*storage->getSynapsePermability(synapse);
    //DEBUG
    //qDebug()<<now<<" : "<<synapse<<'-'<<vlt;
    //return voltage which depends on length of this synapse and its permability
    return vlt;
}
void dojoSynapse::handleSpike(float terminals){
    //save moment of this action
    lastAction = QDateTime::currentMSecsSinceEpoch();
    //release mediator from terminals to cleft
    cleft += terminals;
}
void dojoSynapse::changePermeability(qint64 now){
    //qint64 diff = now-lastAction;
    //means AP happend
    //if this synapse is took part in AP e.g. lastAction < 20 ms

    //icrease permability
    //if
    //qDebug()<<"time diff :"<<diff;
}


