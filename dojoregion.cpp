#include "dojoregion.h"

dojoRegion::dojoRegion(dojoStorage* str, QObject *parent) : QObject(parent)
{
    storage = str;
}

qint64 dojoRegion::process(){
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    foreach (dojoNeuron* neuron, neurons)
        if(neuron->getNextCheck() <= now){
           neuron->process();

           //neuron wants to grow
           if(neuron->isGrowing()){
               //get neigbours
               //QList<dojoID> neigbours = storage->getNeuronsInArea(neuron->getAxonPosition(), neuron->getGrowingRadius());
               QList<dojoID> neigbours = storage->getNeuronsInHemisphere(neuron->getAxonPosition(), neuron->getAxonPosition()-neuron->getPosition(), neuron->getGrowingRadius());

               dojoID sourceID = neuron->getID();
               //remove itself from the list
               if(neigbours.contains(sourceID))
                   neigbours.removeAll(sourceID);

               //make syanpses with discovered neurons
               for(int i=0;i<neigbours.length();i++){
                   //bindNeurons(sourceID, neigbours[i]);
               }
               neuron->stopGrowing();
           }
        }

    //find the least time to sleep
    qint64 new_timeout = neurons[0]->getNextCheck();
    foreach (dojoNeuron* neuron, neurons){
       if(neuron->getNextCheck()<new_timeout)
            new_timeout = neuron->getNextCheck();
    }

    qint64 timeout = new_timeout - QDateTime::currentMSecsSinceEpoch();
    //guard from negative timeout value
    if(timeout < 1)
        timeout = 1;

    return timeout;
}
