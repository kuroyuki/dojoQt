#ifndef DOJOSTORAGE_H
#define DOJOSTORAGE_H

#include <QObject>
#include <QtSql>

#include "dojo.h"

class dojoNeuron;

class dojoStorage : public QObject
{
    Q_OBJECT
public:
    explicit dojoStorage(QString name, QObject *parent = 0);
    ~dojoStorage();

    void addNeuron(dojoNeuron* neuron);
    dojoNeuron getNeuron(dojoID id);
    void updateNeuron(dojoNeuron* neuron);
    void removeNeuron(dojoID id);

    void addSynapse(dojoSynapse* synapse);
    //dojoSynapse getSynapse(dojoID source, dojoID target);
    void updateSynapse(dojoSynapse* synapse);
    void removeSynapse(dojoID source, dojoID target);

signals:
    void storageEvent(QJsonObject event);

public slots:
private :
    QSqlDatabase db;

};

#endif // DOJOSTORAGE_H
