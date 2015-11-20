#ifndef DOJOCLIENT_H
#define DOJOCLIENT_H

#include <QObject>
#include <QTimer>
#include <QUdpSocket>

#define UDP_SERVER_PORT 25098
#define UDP_CLIENT_PORT 26128

typedef int dojoID;

struct dojoAp{
    dojoID id;
    float data;
};

class dojoClient : public QObject
{
    Q_OBJECT
public:
    explicit dojoClient(QHostAddress host, QObject *parent = 0);
    ~dojoClient();

    void connectToServer();
    void sendAp(dojoID source, float data);

signals:
    void dojoEvent(dojoAp ap);

public slots:
    void slotUdpReadyRead();
    void slotUdpError(QAbstractSocket::SocketError error);

private:
    QHostAddress dojoHost;
    QUdpSocket* udpSocket;
};

#endif // DOJOCLIENT_H
