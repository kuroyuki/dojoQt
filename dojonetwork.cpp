#include "dojonetwork.h"
#include <QDebug>

dojoNetwork::dojoNetwork(QObject *parent) : QObject(parent)
{
    storage = new dojoStorage();

    io = new dojoIOServer(storage);
    ws = new dojoWsServer(storage, this);
    connect(storage, SIGNAL(storageEvent(QString)), ws, SLOT(eventHandler(QString)));

    logger = new dojoLogger();
    connect(storage, SIGNAL(storageEvent(QString)), logger, SLOT(logEntry(QString)));
    connect(this, SIGNAL(networkEvent(QString)), logger, SLOT(logEntry(QString)));
    connect(ws, SIGNAL(wsEvent(QString)), logger, SLOT(logEntry(QString)));

    logger->logEntry("network starting...");

    restoreNetwork();

    timer = new QTimer();
    timeout = 10;

    //processing neurnos upon timer signals
    connect(timer, SIGNAL(timeout()), this, SLOT(process()));

    //bind spike from IO for immedate handling
    connect(io, SIGNAL(spike(dojoID,float)), this, SLOT(process()));
}
void dojoNetwork::start(){
    timer->start(timeout);
    emit networkEvent("network started");
}
void dojoNetwork::process(){
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    foreach (dojoNeuron* neuron, neurons)
        if(neuron->getNextCheck() <= now)
           neuron->process();

    //find the least time to sleep
    qint64 new_timeout = QDateTime::currentMSecsSinceEpoch()+1;
    foreach (dojoNeuron* neuron, neurons){
       if(neuron->getNextCheck()<new_timeout)
            new_timeout = neuron->getNextCheck();
    }

    timeout = new_timeout - QDateTime::currentMSecsSinceEpoch();
    //guard from negative timeout value
    if(timeout < 1)
        timeout = 1;

    //sleep until next check
    timer->start(timeout);
}
dojoID dojoNetwork::createNode(QVector3D pos, QVector3D axon, float size){
    //if no neurons in this area
    QList<dojoID> neighbours = storage->getNeuronsInArea(pos, size/2);
    if(!neighbours.size()){
        dojoID id = storage->getNextID();

        neurons[id] = new dojoNeuron(storage, id);
        storage->addNeuron(id, pos, axon, size, 1);
        storage->setNextID(id+1);

        connect(neurons[id], SIGNAL(neuronEvent(QString)), logger, SLOT(logEntry(QString)));

        return id;
    }
    //return previously created neuron
    else{
        dojoID id = neighbours[0];
        //updat existing neuron with new values
        storage->setNeuronAxon(id, axon);
        storage->setNeuronSize(id, size);
        return id;
    }
}
void dojoNetwork::bindNodes(dojoID source, dojoID target){
    QString synapse = QString::number(source)+":"+QString::number(target);
    //nothing to do if already exists
    if(storage->isSynapseExist(synapse))
        return;

    //source is IO and target is Neuron
    if(!neurons.contains(source) && neurons.contains(target)){
        //bind IO spikes from source and target
        neurons[target]->addSource(source);
        connect(io, SIGNAL(spike(dojoID,float)), neurons[target], SLOT(handleSpike(dojoID,float)));

        //save synapse with default params
        storage->addSynapse(synapse, 1, 1);
        //exclude source id and below for new neurons
        if(storage->getNextID() <= source)
            storage->setNextID(source+1);
    }
    //source is Neuron and target is IO
    else if(!neurons.contains(target)){
        //adding IO as target
       connect(neurons[source], SIGNAL(spike(dojoID,float)), io, SLOT(handleSpike(dojoID,float)));

       //save synapse with default perm
       storage->addSynapse(synapse, 1, 1);
       //exclude source id and below for new neurons
       if(storage->getNextID() <= target)
           storage->setNextID(target+1);
    }
    //both are Neurons
    else {
        //bind both neurons
        neurons[target]->addSource(source);
        connect(neurons[source], SIGNAL(spike(dojoID,float)), neurons[target], SLOT(handleSpike(dojoID,float)));

        //calculate length between axon of source and Target's position
        QVector3D diff = storage->getNeuronPosition(target) - storage->getNeuronAxon(source);
        //save synapse with default perm
        storage->addSynapse(synapse, DEFAULT_PERM, diff.length());
    }
}

void dojoNetwork::restoreNetwork(){
    //restoring neurons first
    QList<dojoID> nodes = storage->getAllNeurons();
    for(int i=0;i<nodes.size();i++){
        if(!neurons.contains(nodes[i])){
            neurons[nodes[i]] = new dojoNeuron(storage, nodes[i]);
            connect(neurons[nodes[i]], SIGNAL(neuronEvent(QString)), logger, SLOT(logEntry(QString)));
            emit networkEvent("restore neuron "+QString::number(nodes[i]));
        }
    }
    //restoring synapses
    QList<QString> synapses = storage->getAllSynapses();
    for(int i=0;i<synapses.size();i++){
        QStringList list = synapses[i].split(':');
        dojoID source = list[0].toInt();
        dojoID target = list[1].toInt();

        //source is IO and target is Neuron
        if(!neurons.contains(source) && neurons.contains(target)){
            neurons[target]->addSource(source);
            connect(io, SIGNAL(spike(dojoID,float)), neurons[target], SLOT(handleSpike(dojoID,float)));
        }
        //source is Neuron and target is IO
        else if(!neurons.contains(target)){
            //adding IO as target
           connect(neurons[source], SIGNAL(spike(dojoID,float)), io, SLOT(handleSpike(dojoID,float)));
        }
        //both are Neurons
        else {
            neurons[target]->addSource(source);
            connect(neurons[source], SIGNAL(spike(dojoID,float)), neurons[target], SLOT(handleSpike(dojoID,float)));
        }
        emit networkEvent("restore synapse "+synapses[i]);
    }
}
void dojoNetwork::subscribeWsForSpikes(dojoID neuron){
    connect(neurons[neuron], SIGNAL(spike(dojoID,float)), ws, SLOT(handleSpike(dojoID,float)));
}
