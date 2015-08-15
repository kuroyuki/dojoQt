#include "dojosynapse.h"

dojoSynapse::dojoSynapse(double len, double perm){
    cleft = 0;
    lastAction = QDateTime::currentMSecsSinceEpoch();
    permability = perm;

    length  = len;
}
double dojoSynapse::getVoltage(qint64 now){
    //calc time-depend value
    float binded = 1-exp(-(now - lastAction)/TIME_CONST);

    //binded mediator
    float voltage = cleft*binded;

    //remove binded mediator from cleft
    cleft -= voltage;

    //return voltage which depends on length of this synapse and its permability
    return voltage*exp(-length/LENGTH_CONST)*permability;
}
void dojoSynapse::ap(double terminals){
    //save moment of this action
    lastAction = QDateTime::currentMSecsSinceEpoch();
    //release mediator from terminals to cleft
    cleft += terminals;
}
void dojoSynapse::changePermability(qint64 now){
    qint64 diff = now-lastAction;
    //means AP happend
    //if this synapse is took part in AP e.g. lastAction < 20 ms

    //icrease permability
    //if
    //qDebug()<<"time diff :"<<diff;
}

/*void dojoSynapse::updateLength(double new_length){
    length = new_length;
}*/

