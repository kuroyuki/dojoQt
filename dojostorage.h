#ifndef DOJOSTORAGE_H
#define DOJOSTORAGE_H

#include <QObject>
#include <QVector3D>
#include <QTcpSocket>

typedef int dojoID;

class dojoStorage : public QObject
{
    Q_OBJECT
public:
    explicit dojoStorage(QObject *parent = 0);

    //Network API
    void setNextID(dojoID next);
    dojoID getNextID();

    //Neurons API
    void addNeuron(dojoID id, QVector3D position, QVector3D axon, float size, float terminals);
    bool isNeuronExist(dojoID id);

    void setNeuronPosition(dojoID id, QVector3D new_position);
    void setNeuronAxon(dojoID id, QVector3D new_axon);
    void setNeuronSize(dojoID id, float new_size);
    void setNeuronTerminals(dojoID id, float newTerminals);

    QVector3D getNeuronPosition(dojoID id);
    QVector3D getNeuronAxon(dojoID id);
    float getNeuronSize(dojoID id);
    float getNeuronTerminals(dojoID id);

    void deleteNeuron(dojoID id);

    QList<dojoID> getNeuronsInArea(QVector3D point, float atDistance);
    QList<dojoID> getAllNeurons();

    //Synapses API
    void addSynapse(QString synapse, float perm, float length);
    void deleteSynapse(QString synapse);
    bool isSynapseExist(QString synapse);

    void setSynapseLength(QString synapse,float length);
    float getSynapseLength(QString synapse);

    void setSynapsePermability(QString synapse,float perm);
    float getSynapsePermability(QString synapse);

    QList<QString> getAllSynapses();

signals:
    void storageEvent(QString event);

public slots:

private:

    QString writeCommand(QString cmd);

    QTcpSocket* redis;
};

#endif // DOJOSTORAGE_H
