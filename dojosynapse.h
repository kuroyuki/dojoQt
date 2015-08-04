#ifndef DOJOSYNAPSE_H
#define DOJOSYNAPSE_H

#include "dojo.h"

class dojoSynapse
{
public :
    dojoSynapse(double len);
    void ap(double terminals);
    double getVoltage(qint64 now);
    void changePermability(qint64 now);

    //void updateLength(double new_length);
    double getLength(){return length;}
    double getPermability(){return permability;}

    qint64 lastAction;

private :
    double length;
    double permability;
    double cleft;
};

#endif // DOJOSYNAPSE_H
