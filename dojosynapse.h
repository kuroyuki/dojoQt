#ifndef DOJOSYNAPSE_H
#define DOJOSYNAPSE_H

//defines how much synapse is affecting neuron by each AP depending on its length
#define LENGTH_CONST 0.5 //in mm - neurons closer than 0.5 is affecting more than 37%
//defines how long synapse is affecting neuron by each AP
#define TIME_CONST 50.0 // in ms - about 15 ms to add above 10% to neuron voltage

#include "dojoneuron.h"

class dojoSynapse
{
public :
    dojoSynapse(QString syn, dojoStorage* str);
    void handleSpike(float terminals);
    float getVoltage(qint64 now);
    void changePermeability(qint64 now);

    qint64 lastAction;

private :
    QString synapse;
    dojoStorage* storage;
    float cleft;
};

#endif // DOJOSYNAPSE_H
