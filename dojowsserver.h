#ifndef DOJOWSSERVER_H
#define DOJOWSSERVER_H

#include "dojo.h"

class dojoStorage;

class dojoWsServer : public QObject
{
    Q_OBJECT
public:
    explicit dojoWsServer(dojoStorage* str, QObject *parent = Q_NULLPTR);
    ~dojoWsServer();

Q_SIGNALS:
    void wsEvent(QJsonObject event);
    void closed();

private Q_SLOTS:
    void eventHandler(QJsonObject json);
    void onNewConnection();
    void processTextMessage(QString message);
    void socketDisconnected();

private:

    QWebSocketServer *wsServer;
    QList<QWebSocket *> wsClients;

    dojoStorage* storage;

};

#endif // DOJOWSSERVER_H
