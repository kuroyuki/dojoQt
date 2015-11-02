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

    }
    else if(command == "as"){
        dojoID source = json.take("source").toInt();
        dojoID target = json.take("target").toInt();

        network->bindNodes(source, target);
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

            for(int i=2;i<list.length();i=i+2){
                json.insert(list[i], list[i+1].toFloat());
            }
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
