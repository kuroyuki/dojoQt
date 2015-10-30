#include "dojoioserver.h"

dojoIOServer::dojoIOServer(dojoStorage* storage) : dojoNeuron(storage, 0)
{
    //reserve id 0 for IO server
    dojoID id = storage->getNextID();
    if(id == 0)
        storage->setNextID(1);

    //setup UDP
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, UDP_SERVER_PORT, QAbstractSocket::ShareAddress);

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(slotUdpReadyRead()));

}
void dojoIOServer::ap(dojoID source, float terminals){
    QByteArray datagram;

    datagram.append(reinterpret_cast<const char*>(&source), sizeof(source));
    datagram.append(reinterpret_cast<const char*>(&terminals), sizeof(terminals));

    udpSocket->writeDatagram(datagram.data(), datagram.size(),
                                QHostAddress::Broadcast, UDP_CLIENT_PORT);
}
void dojoIOServer::addInput(dojoID source, dojoNeuron* target){
    if(inputs.contains(source)){
        if(!inputs[source].contains(target))
            inputs[source]<<target;
    }
    else{
        QList<dojoNeuron*> list;
        list<<target;
        inputs[source] = list;
    }
}

void dojoIOServer::slotUdpReadyRead(){
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        udpSocket->readDatagram(datagram.data(), datagram.size());

        while(datagram.size()>11){
            dojoID source = *reinterpret_cast<const int*>(datagram.left(4).data());
            double value = *reinterpret_cast<const double*>(datagram.mid(4,8).data());

            if(inputs.contains(source)){
                QList<dojoNeuron*> targetsList =  inputs[source];
                for(int i=0;i<targetsList.size();i++){
                   targetsList.at(i)->ap(source, value);
                }
            }

            datagram.remove(0,12);
        }
    }
}

