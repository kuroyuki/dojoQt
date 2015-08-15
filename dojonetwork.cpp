#include "dojoNetwork.h"

dojoNetwork::dojoNetwork(QString name, QObject *parent) : QObject(parent)
{
    storage = new dojoStorage(name);
    io = new dojoIOServer();
    wsServer = new dojoWsServer(storage);

    //IO send events only to network
    connect(io, SIGNAL(serverAp(dojoID,dojoID,double)), this, SLOT(externalApHandler(dojoID,dojoID,double)));

    //Storage send events to network and admin
    connect(storage, SIGNAL(storageEvent(QJsonObject)), this, SLOT(eventHandler(QJsonObject)));
    connect(storage, SIGNAL(storageEvent(QJsonObject)), wsServer, SLOT(eventHandler(QJsonObject)));

    //admin send events to network and storage
    connect(wsServer, SIGNAL(wsEvent(QJsonObject)), this, SLOT(eventHandler(QJsonObject)));
    connect(wsServer, SIGNAL(wsEvent(QJsonObject)), storage, SLOT(eventHandler(QJsonObject)));

    //network send events to storage, IO and admin
    connect(this, SIGNAL(dojoEvent(QJsonObject)), wsServer, SLOT(eventHandler(QJsonObject)));
    connect(this, SIGNAL(dojoEvent(QJsonObject)), storage, SLOT(eventHandler(QJsonObject)));
    connect(this, SIGNAL(dojoEvent(QJsonObject)), io, SLOT(eventHandler(QJsonObject)));

    timer = new QTimer();
    timeout = 10;
    nextId = 0;

    //
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    connect(this, SIGNAL(dojoProcess()), this, SLOT(process()));

    //handle all self events by itself
    //connect(this, SIGNAL(dojoEvent(QJsonObject)), this, SLOT(handleEvent(QJsonObject)));
}

dojoNetwork::~dojoNetwork()
{

}
void dojoNetwork::start(){
    storage->getCurrentTables();
    //if we have something to process and its not already running
    if(neurons.size()>0 && !timer->isActive()){
       timer->start(timeout);
    }
}

void dojoNetwork::slotTimeout(){
    emit dojoProcess();
}

void dojoNetwork::process(){

    qint64 now = QDateTime::currentMSecsSinceEpoch();

    foreach (dojoNeuron* neuron, neurons)
        if(neuron->getNextCheck() <= now){
           neuron->process();
        }

    //find the least time to sleep
    qint64 new_timeout = neurons[0]->getNextCheck();
    foreach (dojoNeuron* neuron, neurons){
       if(neuron->getNextCheck()<new_timeout)
            new_timeout = neuron->getNextCheck();
    }

    timeout = new_timeout - QDateTime::currentMSecsSinceEpoch();
    //guard from negative timeout value
    if(timeout < 1)
        timeout = 1;

    //sleep until next check
    timer->setInterval(timeout);
}
dojoID dojoNetwork::createNeuron(QVector3D pos, QVector3D axon){

    dojoID id = nextId++;
    neurons.insert(id, new dojoNeuron(id, pos, axon));

    QJsonObject json;
    json.insert("command", "an");
    json.insert("id", id);

    QJsonObject posJson;
    posJson.insert("x", pos.x());
    posJson.insert("y", pos.y());
    posJson.insert("z", pos.z());

    QJsonObject axonJson;
    axonJson.insert("x", axon.x());
    axonJson.insert("y", axon.y());
    axonJson.insert("z", axon.z());

    json.insert("pos", posJson);
    json.insert("axon", axonJson);

    emit dojoEvent(json);

    return id;
}
void dojoNetwork::deleteNeuron(dojoID id){
    delete neurons[id];

    QJsonObject json;
    json.insert("command", "dn");
    json.insert("id", id);
    emit dojoEvent(json);
}

void dojoNetwork::bindNeurons(dojoID source, dojoID target){

    //source is outside the current network
    if(!neurons.contains(source)){
        //if no such target - cannot create synapse
        if(!neurons.contains(target)){
            return;
        }
        //to exclude this id from future use
        if(nextId <= source)
            nextId = source+1;

        //create synapse
        dojoSynapse* syn = new dojoSynapse(LENGTH_CONST, 1);
        neurons[target]->addSource(source, syn);        

        QJsonObject json;
        json.insert("command", "ai");
        json.insert("source", source);
        json.insert("target", target);
        emit dojoEvent(json);

        return;
    }
    //target is outside the current network
    if(!neurons.contains(target)){
        //to exclude this id from future use
        if(nextId <= target)
            nextId = target+1;

        //create actuator
        neurons[source]->addAct(io);

        QJsonObject json;
        json.insert("command", "ao");
        json.insert("source", source);
        json.insert("target", target);
        emit dojoEvent(json);

        return;
    }

    //both IDs are not exist in this network
    QVector3D diff = neurons[target]->getPosition() - neurons[source]->getAxonPosition();
    dojoSynapse* synapse = new dojoSynapse(diff.length(), 1);

    neurons[source]->addTarget(neurons[target]);
    neurons[target]->addSource(source, synapse);

    QJsonObject json;
    json.insert("command", "as");
    json.insert("source", source);
    json.insert("target", target);
    json.insert("length", (double)synapse->getLength());
    json.insert("permability", (double)synapse->getPermability());
    emit dojoEvent(json);
}

void dojoNetwork::unbindNeurons(dojoID source, dojoID target){
    neurons[source]->removeTarget(neurons[target]);
    neurons[target]->removeSource(source);

    QJsonObject json;
    json.insert("command", "ds");
    json.insert("source", source);
    json.insert("target", target);
    emit dojoEvent(json);
}
void dojoNetwork::externalApHandler(dojoID source, dojoID target, double value){
    if(neurons.contains(target)){
        neurons[target]->ap(source, value);
        timer->setInterval(1);
    }
    else{
        QJsonObject json;
        json.insert("command", "external AP");
        json.insert("status", "target not exist");
        json.insert("source", source);
        json.insert("target", target);
        emit dojoEvent(json);
    }
}
void dojoNetwork::eventHandler(QJsonObject event){

    QString command = event.take("command").toString();

    if(command == "an"){

        dojoID id = event.take("id").toInt();

        if(neurons.contains(id)){
            qDebug()<<"dojo - neuron "<<id<<" already exist";
            return;
        }

        if(id >= nextId)
            nextId = id+1;

        QJsonObject posJson = event.take("pos").toObject();
        QVector3D pos(posJson.take("x").toDouble(),posJson.take("y").toDouble(),posJson.take("z").toDouble());

        QJsonObject axonJson = event.take("axon").toObject();
        QVector3D axon(axonJson.take("x").toDouble(),axonJson.take("y").toDouble(),axonJson.take("z").toDouble());

        neurons.insert(id, new dojoNeuron(id, pos, axon));

        if(!timer->isActive())
            timer->start(timeout);
    }    
    else if(command == "as"){
        dojoID source = event.take("source").toInt();
        dojoID target = event.take("target").toInt();
        //source is outside the current network
        if(!neurons.contains(source)){
            qDebug()<<"dojo - source "<<source<<" is not exist";
            return;
        }
        //target is outside the current network
        if(!neurons.contains(target)){
            qDebug()<<"dojo - target "<<target<<" is not exist";
            return;
        }
        //both are exist
        dojoSynapse* synapse = new dojoSynapse(event.take("length").toDouble(), event.take("permability").toDouble());

        neurons[source]->addTarget(neurons[target]);
        neurons[target]->addSource(source, synapse);
    }
    else{
        QJsonDocument  jdoc(event);
        qDebug()<<"dojo - unknown event"<<jdoc.toJson(QJsonDocument::Compact);
    }
}

