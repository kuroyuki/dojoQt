#include "dojowsserver.h"

dojoWsServer::dojoWsServer(dojoStorage* str, dojoNetwork *dojo, QObject* parent) :
    QObject(parent),
    wsServer(new QWebSocketServer(QStringLiteral("dojo WS server"),
                                            QWebSocketServer::NonSecureMode, this)),
    wsClients()
{
    storage = str;
    network = dojo;

    if (wsServer->listen(QHostAddress::Any, WS_SERVER_PORT)) {
       connect(wsServer, &QWebSocketServer::newConnection,
               this, &dojoWsServer::onNewConnection);
       connect(wsServer, &QWebSocketServer::closed, this, &dojoWsServer::closed);
   }
}

dojoWsServer::~dojoWsServer()
{
   wsServer->close();
   qDeleteAll(wsClients.begin(), wsClients.end());

   emit wsEvent("ws closed");
}

void dojoWsServer::onNewConnection()
{
   QWebSocket *newClient = wsServer->nextPendingConnection();

   connect(newClient, SIGNAL(textMessageReceived(QString)), this, SLOT(processTextMessage(QString)));
   connect(newClient, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));

   wsClients << newClient;

   QList<dojoID> neurons = storage->getAllNeurons();

   for(int i=0;i<neurons.size();i++){
       dojoID neuronID = neurons[i];

       QVector3D pos = storage->getNeuronPosition(neuronID);
       float size = storage->getNeuronSize(neuronID);
       QVector3D axon = storage->getNeuronAxon(neuronID);

       QJsonObject json;
       json.insert("command", "an");
       json.insert("id", neuronID);

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
       json.insert("size", size);

       QJsonDocument jdoc(json);

       newClient->sendTextMessage(QString::fromUtf8(jdoc.toJson()));
   }
   //restoring synapses
   QList<QString> synapses = storage->getAllSynapses();
   for(int i=0;i<synapses.size();i++){

       QStringList list = synapses[i].split(':');
       dojoID source = list[0].toInt();
       dojoID target = list[1].toInt();

       QJsonObject json;
       json.insert("command", "as");
       json.insert("source", source);
       json.insert("target", target);

       QString synapse = QString::number(source)+":"+QString::number(target);
       float perm = storage->getSynapsePermability(synapse);
       float length = storage->getSynapseLength(synapse);

       json.insert("perm", perm);
       json.insert("length", length);

       QJsonDocument jdoc(json);

       newClient->sendTextMessage(QString::fromUtf8(jdoc.toJson()));
   }
   emit wsEvent("new ws client from "+ newClient->peerAddress().toString());
}
void dojoWsServer::eventHandler(QString event){
    QJsonDocument jDoc(parseEvent(event));

    for(int i =0;i<wsClients.length();i++){
        wsClients[i]->sendTextMessage(QString::fromUtf8(jDoc.toJson()));
    }
    //qDebug()<<"dojoWS sent to clients:"<< QString::fromUtf8(jDoc.toJson(QJsonDocument::Compact));
}
void dojoWsServer::processTextMessage(QString message){
    //qDebug()<<"WS client message :"<<message;
    QJsonDocument jdoc;
    jdoc.fromJson(message.toLocal8Bit());

    QJsonObject json = jdoc.object();
    QString command = json.take("command").toString();

    if(command == "an"){
        QVector3D pos;
        QJsonObject posJson = json.take("pos").toObject();
        pos.setX(posJson.take("x").toDouble());
        pos.setY(posJson.take("y").toDouble());
        pos.setZ(posJson.take("z").toDouble());

        QVector3D axon;
        QJsonObject axonJson = json.take("axon").toObject();
        axon.setX(axonJson.take("x").toDouble());
        axon.setY(axonJson.take("y").toDouble());
        axon.setZ(axonJson.take("z").toDouble());

        network->createNode(pos, axon, json.take("source").toDouble());
    }
    else if(command == "as"){
        dojoID source = json.take("source").toInt();
        dojoID target = json.take("target").toInt();

        network->bindNodes(source, target);
    }
    else if(command == "un"){
        dojoID neuron = json.take("id").toInt();
        QStringList list = json.keys();
        QString param = list[0];
        if(param == "axon"){
            QVector3D axon;
            QJsonObject axonJson = json.take(param).toObject();
            axon.setX(axonJson.take("x").toDouble());
            axon.setY(axonJson.take("y").toDouble());
            axon.setZ(axonJson.take("z").toDouble());

            storage->setNeuronAxon(neuron, axon);
        }
        else if(param == "pos"){
            QVector3D pos;
            QJsonObject posJson = json.take("pos").toObject();
            pos.setX(posJson.take("x").toDouble());
            pos.setY(posJson.take("y").toDouble());
            pos.setZ(posJson.take("z").toDouble());

            storage->setNeuronPosition(neuron, pos);
        }
        else if(param == "size"){
            float size = json.take(param).toDouble();
            storage->setNeuronSize(neuron, size);
        }
        else if(param == "terminals"){
            float term = json.take(param).toDouble();
            storage->setNeuronTerminals(neuron, term);
        }
    }
    //update existing synapse params
    else if(command == "us"){

    }
    //subsribe for spikes from neuron
    else if(command == "sfs"){
        dojoID id = json.take("id").toInt();
        network->subscribeWsForSpikes(id);
    }
}
void dojoWsServer::socketDisconnected(){
    //qDebug()<<"WS client disconnected :";

    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
       if (pClient) {
        wsClients.removeAll(pClient);
        pClient->deleteLater();
    }
}
QJsonObject dojoWsServer::parseEvent(QString event){
     //parse Redis command to JSON for clients
    QJsonObject json;
    QStringList list = event.split(" ");
    if(list[0] == "hmset"){
        if(list[1].contains("neurons:")){
            //this is new entry with full info
            if(list.size()>8){
                json.insert("command", "an");
                json.insert("id", list[1].replace("neurons:", "").toInt());

                QJsonObject posJson;
                posJson.insert("x", list[3].toFloat());
                posJson.insert("y", list[5].toFloat());
                posJson.insert("z", list[7].toFloat());

                QJsonObject axonJson;
                axonJson.insert("x", list[9].toFloat());
                axonJson.insert("y", list[11].toFloat());
                axonJson.insert("z", list[13].toFloat());

                json.insert("pos", posJson);
                json.insert("axon", axonJson);
                json.insert("size", list[15].toFloat());
                json.insert("terminals", list[17].toFloat());
            }
            //this is single parameter changing (position or axon position)
            else{
                //set command type as Update
                json.insert("command", "un");
                json.insert("id", list[1].replace("neurons:", "").toInt());

                QJsonObject posJson;
                posJson.insert("x", list[3].toFloat());
                posJson.insert("y", list[5].toFloat());
                posJson.insert("z", list[7].toFloat());

                //Position updating
                if(list[2] == "pos"){
                    json.insert("pos", posJson);
                }
                //Axon Position updating
                else{
                   json.insert("axon", posJson);
                }
            }
        }
        else if(list[1].contains("synapses:")){
            QStringList synList = list[1].replace("synapses:", "").split(':');
            dojoID source = synList[0].toInt();
            dojoID target = synList[1].toInt();

            json.insert("command", "as");
            json.insert("source", source);
            json.insert("target", target);

            json.insert("perm", list[3].toFloat());
            json.insert("length", list[5].toFloat());
        }
    }
    else if(list[0] == "hset"){
        if(list[1].contains("neurons:")){
            json.insert("command", "un");
            json.insert("id", list[1].replace("neurons:", "").toInt());

            json.insert(list[2], list[3].toFloat());
        }
        else if(list[1].contains("synapses:")){
            QStringList synList = list[1].replace("synapses:", "").split(':');
            dojoID source = synList[0].toInt();
            dojoID target = synList[1].toInt();

            QJsonObject json;
            json.insert("command", "us");
            json.insert("source", source);
            json.insert("target", target);

            json.insert(list[2], list[3].toFloat());
        }
    }
    return json;
}
void dojoWsServer::handleSpike(dojoID id, float term){
    QJsonObject json;
    json.insert("command", "spike");
    json.insert("id", id);
    json.insert("term", term);

    QJsonDocument jDoc(json);

    for(int i =0;i<wsClients.length();i++){
        wsClients[i]->sendTextMessage(QString::fromUtf8(jDoc.toJson()));
    }
}
