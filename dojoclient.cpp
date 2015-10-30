#include "dojoclient.h"
#include <QDebug>

dojoClient::dojoClient( QString host, QObject *parent) : QObject(parent)
{
    dojoHost = host;

    /*//create TCP socket
    tcpSocket = new QTcpSocket();
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotTcpError(QAbstractSocket::SocketError)));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(slotTcpReadyRead()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(slotTcpDisconnected()));

    isTcpConnected = false;*/

    sendTimer = new QTimer(this);
    connect(sendTimer, SIGNAL(timeout()), this, SLOT(slotTimeout()));

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
    udpSocket->bind(QHostAddress::LocalHost, UDP_CLIENT_PORT);
    sendTimer->start(10);
}
void dojoClient::registerInput(dojoID id, float* data){

    if (!inputs.contains(id)){
        /*if(isTcpConnected){
            QByteArray ba;
            int t = 0;
            ba.append(t);
            ba.append(id>>24);
            ba.append(id>>16);
            ba.append(id>>8);
            ba.append(id);

            tcpSocket->write(ba, ba.length());
            tcpSocket->waitForBytesWritten();
        }*/

        //save data with id
        inputs.insert(id, data);
    }
}
void dojoClient::registerOutput(dojoID id, float* data){
    if(!outputs.contains(id)){
        /*if(isTcpConnected){
            //send request to get node id
            QByteArray ba;
            ba.append(id>>24);
            ba.append(id>>16);
            ba.append(id>>8);
            ba.append(id);
            ba.prepend(0x01);

            tcpSocket->write(ba, ba.length());
            tcpSocket->waitForBytesWritten();
        }
        */
        outputs.insert(id,  data);
    }
}
void dojoClient::slotTimeout(){
    /*
    //if TCP connected
    if(isTcpConnected){
    */
        //send update via UDP
        QByteArray ba;

        QList<dojoID> keys = inputs.keys();
        for(int i=0;i<keys.size();i++){
            dojoID target = keys[i];
            double dbl = *inputs.value(target);
            if(dbl>0) {
                ba.append(reinterpret_cast<const char*>(&target), sizeof(target));
                ba.append(reinterpret_cast<const char*>(&dbl), sizeof(dbl));
            }
        }
        if(ba.length()){
            udpSocket->writeDatagram(ba, QHostAddress::Broadcast, UDP_SERVER_PORT);
        }
    /*
    }
    else {
        //make connection with the server
        tcpSocket->connectToHost(dojoHost,TCP_PORT);
        if(tcpSocket->waitForConnected(1000)){

            //register all sensors
            QByteArray ba;
            QList<dojoID> list = inputs.keys();
            for(int i=0;i<list.size();i++){
                dojoID id = list[i];

                //send node id
                int t = 0;
                ba.append(t);
                ba.append(id>>24);
                ba.append(id>>16);
                ba.append(id>>8);
                ba.append(id);

            }
            //register all actuators
            list = outputs.keys();
            for(int i=0;i<list.size();i++){
                dojoID id = list[i];

                //send node id
                ba.append(0x01);
                ba.append(id>>24);
                ba.append(id>>16);
                ba.append(id>>8);
                ba.append(id);

            }

            tcpSocket->write(ba, ba.length());
            tcpSocket->waitForBytesWritten();
            isTcpConnected = true;

            qDebug()<<"Tcp connected "<<endl;

            udpSocket->bind(QHostAddress::LocalHost, UDP_CLIENT_PORT);
        }
    }
   */
}
/*
void dojoClient::slotTcpReadyRead(){

}
void dojoClient::slotTcpDisconnected(){
    //wait for 1 sec and retry to connect
    qDebug()<<"Tcp disconnected, reconnecting.. "<<endl;
    isTcpConnected = false;
}

void dojoClient::slotTcpError(QAbstractSocket::SocketError error){
    //wait for 1 sec and retry to connect
    qDebug()<<"Tcp error, reconnecting.. "<<error<<endl;
    isTcpConnected = false;

    tcpSocket->close();
    udpSocket->close();
}
*/
void dojoClient::slotUdpError(QAbstractSocket::SocketError error){
    //wait for 1 sec and retry to connect
    qDebug()<<"Udp error, reconnecting.. "<<error<<endl;
    //isTcpConnected = false;

    //tcpSocket->close();
    //udpSocket->close();
}

void dojoClient::slotUdpReadyRead(){
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        udpSocket->readDatagram(datagram.data(), datagram.size());

        while(datagram.size()>11){
            dojoID target = *reinterpret_cast<const int*>(datagram.left(4).data());
            float value = *reinterpret_cast<const float*>(datagram.mid(4,8).data());

            float* data = outputs.value(target);
            *data += value;

            datagram.remove(0,12);
        }
    }
}
