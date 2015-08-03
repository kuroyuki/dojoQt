#include "dojoserver.h"

dojoServer::dojoServer(QObject *parent) : QObject(parent)
{
    //connect UDP
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, UDP_SERVER_PORT, QAbstractSocket::ShareAddress);

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(slotUdpReadyRead()));
    connect(udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotUdpError(QAbstractSocket::SocketError)));

}
void dojoServer::slotUdpError(QAbstractSocket::SocketError error){
    QJsonObject json;
    json.insert("server UDP error", error);
    emit serverEvent(json);
}

void dojoServer::slotUdpReadyRead(){
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        udpSocket->readDatagram(datagram.data(), datagram.size());

        while(datagram.size()>11){
            dojoID source = *reinterpret_cast<const int*>(datagram.left(4).data());
            double value = *reinterpret_cast<const double*>(datagram.mid(4,8).data());            

            if(sensors.contains(source)){
                emit serverAp(source, sensors[source].target, value);

                QJsonObject json;
                json.insert("command", "ap from outside");
                json.insert("source", source);
                json.insert("target", sensors[source].target);
                json.insert("data", (double)value);
                json.insert("address", sensors[source].address.toString());
                json.insert("port", sensors[source].port);
                emit serverEvent(json);
            }

            datagram.remove(0,12);
        }
    }
}

void dojoServer::ap(dojoID source, double terminals){
    if(actuators.contains(source)){
        dojoUdpAct act = actuators[source];

        QByteArray datagram;
        datagram.append(reinterpret_cast<const char*>(&act.target), sizeof(act.target));
        datagram.append(reinterpret_cast<const char*>(&terminals), sizeof(terminals));

        udpSocket->writeDatagram(datagram.data(), datagram.size(),
                                    act.address, act.port);

        QJsonObject json;
        json.insert("command", "ap to outside");
        json.insert("source", source);
        json.insert("target", (double)act.target);
        json.insert("data", QString(datagram));
        json.insert("address", act.address.toString());
        json.insert("port", act.port);
        emit serverEvent(json);
    }
}
void dojoServer::addActuator(dojoID source, dojoUdpAct actuator){
    actuators.insert(source, actuator);

    QJsonObject json;
    json.insert("source", (double)source);
    json.insert("target", (double)actuator.target);
    json.insert("address", actuator.address.toString());
    json.insert("port", actuator.port);
    emit serverEvent(json);
}
void dojoServer::addSensor(dojoID source, dojoUdpSensor sensor){
    sensors.insert(source, sensor);

    QJsonObject json;
    json.insert("source", (double)source);
    json.insert("target", (double)sensor.target);
    json.insert("address", sensor.address.toString());
    json.insert("port", sensor.port);
    emit serverEvent(json);
}
