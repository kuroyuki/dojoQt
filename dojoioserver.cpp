#include "dojoioserver.h"

dojoIOServer::dojoIOServer(QObject *parent) : QObject(parent)
{
    //connect UDP
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, UDP_SERVER_PORT, QAbstractSocket::ShareAddress);

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(slotUdpReadyRead()));
    connect(udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotUdpError(QAbstractSocket::SocketError)));

}
void dojoIOServer::slotUdpError(QAbstractSocket::SocketError error){
    QJsonObject json;
    json.insert("server UDP error", error);    
}

void dojoIOServer::slotUdpReadyRead(){
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        udpSocket->readDatagram(datagram.data(), datagram.size());

        while(datagram.size()>11){
            dojoID source = *reinterpret_cast<const int*>(datagram.left(4).data());
            double value = *reinterpret_cast<const double*>(datagram.mid(4,8).data());            

            if(sensors.contains(source)){
                emit serverAp(source, sensors[source].target, value);

                /*
                QJsonObject json;
                json.insert("command", "ap from outside");
                json.insert("source", source);
                json.insert("target", sensors[source].target);
                json.insert("data", (double)value);
                json.insert("address", sensors[source].address.toString());
                json.insert("port", sensors[source].port);
                emit serverEvent(json);
                */
            }

            datagram.remove(0,12);
        }
    }
}

void dojoIOServer::ap(dojoID source, double terminals){
    if(actuators.contains(source)){
        dojoUdpAct act = actuators[source];

        QByteArray datagram;
        datagram.append(reinterpret_cast<const char*>(&act.target), sizeof(act.target));
        datagram.append(reinterpret_cast<const char*>(&terminals), sizeof(terminals));

        udpSocket->writeDatagram(datagram.data(), datagram.size(),
                                    act.address, act.port);

        /*QJsonObject json;
        json.insert("command", "ap to outside");
        json.insert("source", source);
        json.insert("target", (double)act.target);
        json.insert("data", QString(datagram));
        json.insert("address", act.address.toString());
        json.insert("port", act.port);
        emit serverEvent(json);*/
    }
}
void dojoIOServer::eventHandler(QJsonObject event){
    QString command = event.take("command").toString();
    if(command == "ao"){
        dojoUdpAct actuator;
        actuator.address = QHostAddress::LocalHost;
        actuator.port = UDP_CLIENT_PORT;
        actuator.target = event.take("target").toInt();

        actuators.insert(event.take("source").toInt(), actuator);

        qDebug()<<"dojoIO - add actuator";
    }
    else if(command == "ai"){
        dojoUdpSensor sensor;
        sensor.address = QHostAddress::LocalHost;
        sensor.port = UDP_CLIENT_PORT;
        sensor.target = event.take("target").toInt();

        sensors.insert(event.take("source").toInt(), sensor);
        qDebug()<<"dojoIO - add sensor";
    }
    else{
        QJsonDocument  jdoc(event);
        qDebug()<<"dojoIO  - unknown event"<<jdoc.toJson(QJsonDocument::Compact);
    }
}

