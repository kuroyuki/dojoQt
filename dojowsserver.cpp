#include "dojowsserver.h"

dojoWsServer::dojoWsServer(dojoStorage* str, QObject *parent) :
    QObject(parent),
    wsServer(new QWebSocketServer(QStringLiteral("dojo WS server"),
                                            QWebSocketServer::NonSecureMode, this)),
    wsClients()
{
    storage = str;

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

   storage->getCurrentTables();
}

void dojoWsServer::eventHandler(QJsonObject json){
    QJsonDocument jDoc(json);

    for(int i =0;i<wsClients.length();i++){
        wsClients[i]->sendTextMessage(QString::fromUtf8(jDoc.toJson()));
    }
    QJsonDocument jdoc(json);
    qDebug()<<"dojoWS sent to clients:"<< jdoc.toJson(QJsonDocument::Compact);
}
void dojoWsServer::processTextMessage(QString message){
    qDebug()<<"WS client message :"<<message;
    QJsonDocument jdoc;
    jdoc.fromJson(message.toLocal8Bit());

    QJsonObject json = jdoc.object();
    QString command = json.take("command").toString();

    if(command == "an"){

    }
    else if(command == "as"){

    }   
}
void dojoWsServer::socketDisconnected(){
    qDebug()<<"WS client disconnected :";

    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
       if (pClient) {
        wsClients.removeAll(pClient);
        pClient->deleteLater();
    }
}

