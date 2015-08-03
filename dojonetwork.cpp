#include "dojoNetwork.h"

dojoNetwork::dojoNetwork(QString name, QObject *parent) : QObject(parent)
{
    /*storage = new dojoStorage(name);
    connect(storage, SIGNAL(storageEvent(QJsonObject)), this, SLOT(handleEvent(QJsonObject)));
    */
    server = new dojoServer();
    connect(server, SIGNAL(serverEvent(QJsonObject)), this, SLOT(handleEvent(QJsonObject)));
    connect(server, SIGNAL(serverAp(dojoID,dojoID,double)), this, SLOT(externalAp(dojoID,dojoID,double)));

    timer = new QTimer();
    timeout = 10;
    nextId = 0;

    //
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    connect(this, SIGNAL(dojoProcess()), this, SLOT(process()));

    //handle all self events by itself
    connect(this, SIGNAL(dojoEvent(QJsonObject)), this, SLOT(handleEvent(QJsonObject)));
}

dojoNetwork::~dojoNetwork()
{

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

    QJsonArray posJson;
    posJson.append(pos.x());
    posJson.append(pos.y());
    posJson.append(pos.z());

    QJsonArray axonJson;
    axonJson.append(axon.x());
    axonJson.append(axon.y());
    axonJson.append(axon.z());

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
        if(!neurons.contains(target)){
            return;
        }
        //to exclude this id from future use
        if(nextId <= source)
            nextId = source+1;

        //create sensor        
        dojoUdpSensor sensor;
        sensor.target = target;
        sensor.address = QHostAddress::LocalHost;
        sensor.port = UDP_CLIENT_PORT;

        server->addSensor(source, sensor);

        //create synapse
        dojoSynapse* syn = new dojoSynapse(LENGTH_CONST);
        neurons[target]->addSource(source, syn);

        return;
    }
    //target is outside the current network
    if(!neurons.contains(target)){
        //to exclude this id from future use
        if(nextId <= target)
            nextId = target+1;

        //create actuator
        neurons[source]->addAct(server);
        dojoUdpAct act;
        act.target = target;
        act.address = QHostAddress::LocalHost;
        act.port = UDP_CLIENT_PORT;
        server->addActuator(source, act);
        return;
    }

    QVector3D diff = neurons[target]->getPosition() - neurons[source]->getAxonPosition();
    dojoSynapse* synapse = new dojoSynapse(diff.length());

    neurons[source]->addTarget(neurons[target]);
    neurons[target]->addSource(source, synapse);

    //storage->addSynapse(synapse);

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
void dojoNetwork::externalAp(dojoID source, dojoID target, double value){
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
void dojoNetwork::handleEvent(QJsonObject event){
    //QJsonDocument jdoc(event);
    //qDebug()<<jdoc.toJson(QJsonDocument::Compact)<<endl;
    /*
    QString request = json.take("command").toString();

    if(request == "an"){
                QJsonObject json;
                json.insert("command", "an");

                QJsonArray posJson;
                posJson.append(list[0].toDouble());
                posJson.append(list[1].toDouble());
                posJson.append(list[2].toDouble());

                QJsonArray axonJson;
                axonJson.append(list[3].toDouble());
                axonJson.append(list[4].toDouble());
                axonJson.append(list[5].toDouble());

                json.insert("pos", posJson);
                json.insert("axon", axonJson);

                dojo->handleCommand(json);


        QJsonArray posArray = json.take("pos").toArray();
        QVector3D position(
                    posArray.at(0).toDouble(),
                    posArray.at(1).toDouble(),
                    posArray.at(2).toDouble());

        posArray = json.take("axon").toArray();
        QVector3D axon(
                    posArray.at(0).toDouble(),
                    posArray.at(1).toDouble(),
                    posArray.at(2).toDouble());


        createNeuron(position, axon);
    }
    else if(request == "un"){
                QJsonObject json;
                json.insert("command", "un");
                json.insert("id", list[0].toDouble());

                QJsonArray posJson;
                posJson.append(list[1].toDouble());
                posJson.append(list[2].toDouble());
                posJson.append(list[3].toDouble());

                QJsonArray axonJson;
                axonJson.append(list[4].toDouble());
                axonJson.append(list[5].toDouble());
                axonJson.append(list[6].toDouble());

                json.insert("pos", posJson);
                json.insert("axon", axonJson);

                dojo->handleCommand(json);

        QJsonArray posArray = json.take("pos").toArray();
        QVector3D position(
                    posArray.at(0).toDouble(),
                    posArray.at(1).toDouble(),
                    posArray.at(2).toDouble());

        posArray = json.take("axon").toArray();
        QVector3D axon(
                    posArray.at(0).toDouble(),
                    posArray.at(1).toDouble(),
                    posArray.at(2).toDouble());

        dojoID id = json.take("id").toDouble();
        setNeuronPosition(id, position, axon);
    }
    else if(request == "dn"){

            QJsonObject json;
            json.insert("command", "dn");
            json.insert("id", data.toDouble());

        dojoID id = json.take("id").toDouble();
        deleteNeuron(id);
    }
    else if(request == "as"){

                QJsonObject json;
                json.insert("command", "as");
                json.insert("source", list[0].toInt());
                json.insert("target", list[1].toInt());
                dojo->handleCommand(json);

        bindNeurons(json.take("source").toDouble(), json.take("target").toDouble());
    }
    else if(request == "ds"){

    }
    */
}

