#ifndef DOJOWSSERVER_H
#define DOJOWSSERVER_H

#include <QObject>
#include <QWebSocket>
#include <QWebSocketServer>
#include <QJsonObject>
#include <QJsonDocument>
#include "dojostorage.h"
#include "dojonetwork.h"

#define WS_SERVER_PORT 10167

class dojoNetwork;

class dojoWsServer : public QObject
{
    Q_OBJECT

public:
    explicit dojoWsServer(dojoStorage* str, dojoNetwork *dojo, QObject* parent = 0);
    ~dojoWsServer();

signals:
    void wsEvent(QString event);
    void closed();

public slots:
    void eventHandler(QString event);
    void handleSpike(dojoID id, float term);

    void onNewConnection();
    void processTextMessage(QString message);
    void socketDisconnected();

private:
    QJsonObject parseEvent(QString event);

    QWebSocketServer *wsServer;
    QList<QWebSocket *> wsClients;

    dojoStorage* storage;
    dojoNetwork* network;
};

#endif // DOJOWSSERVER_H
