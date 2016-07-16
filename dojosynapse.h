#ifndef DOJOSYNAPSE_H
#define DOJOSYNAPSE_H

#include "dojoneuron.h"

//defines how much synapse is affecting neuron by each AP depending on its length
#define LENGTH_CONST  0.5//in mm - neurons closer than 0.5 is affecting more than 37%
//defines how long synapse is affecting neuron by each AP
#define TIME_CONST 50.0// in ms - about 30 ms to add above 80% to neuron voltage
//defines how quick synapse is changing its perambility
#define LEARNING_RATE 0.01//

class dojoSynapse
{
public :
    dojoSynapse(QString syn, dojoStorage* str);
    void handleSpike(float transmiterValue, qint64 time);
    float getVoltage();

    void postNeuronSpike(qint64 spikeTime);
    qint64 lastSpike;

private :
    QString synapse;
    dojoStorage* storage;
    float cleft;    
    qint64 neuronSpikeTime;
};

#endif // DOJOSYNAPSE_H
