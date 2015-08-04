#ifndef DOJONEURON_H
#define DOJONEURON_H

#include "dojo.h"

class dojoSynapse;
class dojoServer;

class dojoNeuron
{
public:
    dojoNeuron(dojoID new_id, QVector3D new_pos, QVector3D new_axon);
    void ap(dojoID source, double terminals);
    void process();

    QVector3D getAxonPosition(){return axon;}
    QVector3D getPosition(){return position;}

    void addSource(dojoID source, dojoSynapse* synapse);
    void removeSource(dojoID source);

    void addTarget(dojoNeuron* target);
    void removeTarget(dojoNeuron* target);

    void addAct(dojoServer* server);

    qint64 getNextCheck(){return nextCheck;}
    dojoID getID(){return id;}

private:

    QHash<dojoID, dojoSynapse*> sources;
    QList<dojoNeuron*> targets;

    dojoID id;

    qint64 lastAction;
    double voltage;
    double axonTerminals;

    QVector3D axon;
    QVector3D position;
    double size;

    qint64 nextCheck;
    dojoServer* actuator;
};

#endif // DOJONEURON_H
