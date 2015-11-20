#ifndef DOJOREGION_H
#define DOJOREGION_H

#include "dojo.h"

class dojoNeuron;
class dojoStorage;

class dojoRegion : public QObject
{
    Q_OBJECT
public:
    explicit dojoRegion(dojoStorage* str, QObject *parent = 0);

signals:

public slots:
    qint64 process();

private:
    QHash<dojoID,dojoNeuron*> neurons;
    int timeout;

    dojoStorage* storage;

    dojoID nextId;
};

#endif // DOJOREGION_H
