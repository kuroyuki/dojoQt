#include "dojoclient.h"
#include <QDebug>

dojoClient::dojoClient(QHostAddress host, QObject *parent) : QObject(parent)
{
    dojoHost = host;

    //connect UDP
    udpSocket = new QUdpSocket();
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(slotUdpReadyRead()));
    connect(udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotUdpError(QAbstractSocket::SocketError)));

    qDebug()<<"dojoClient created"<<endl;
}
dojoClient::~dojoClient()
{

}
void dojoClient::connectToServer(){
    qDebug()<<"connecting to dojo server"<<endl;
    udpSocket->bind(QHostAddress::Any, UDP_CLIENT_PORT);

}
void dojoClient::sendAp(dojoID source, float data){
    if(data>0) {
        QByteArray ba;

        ba.append(reinterpret_cast<const char*>(&source), sizeof(source));
        ba.append(reinterpret_cast<const char*>(&data), sizeof(data));

        //send to server
        udpSocket->writeDatagram(ba, dojoHost, UDP_SERVER_PORT);
    }
}
void dojoClient::slotUdpError(QAbstractSocket::SocketError error){
    qDebug()<<"Udp error "<<error<<endl;
}

void dojoClient::slotUdpReadyRead(){
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        udpSocket->readDatagram(datagram.data(), datagram.size());

        while(datagram.size()>7){

            dojoAp ap;
            ap.id = *reinterpret_cast<const int*>(datagram.left(4).data());
            ap.data = *reinterpret_cast<const float*>(datagram.mid(4,8).data());
            emit dojoEvent(ap);

            datagram.remove(0,8);
        }
    }
}
