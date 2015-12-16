#include "dojoioserver.h"

dojoIOServer::dojoIOServer(dojoStorage* storage) : dojoNeuron(storage, 0)
{
    //reserve id 0 for IO server
    dojoID id = storage->getNextID();
    if(id == 0)
        storage->setNextID(1);

    //setup UDP
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, UDP_SERVER_PORT, QAbstractSocket::ShareAddress);

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(slotUdpReadyRead()));

}
void dojoIOServer::handleSpike(dojoID source, float terminals){
    if(terminals > 0){
        QByteArray datagram;

        datagram.append(reinterpret_cast<const char*>(&source), sizeof(source));
        datagram.append(reinterpret_cast<const char*>(&terminals), sizeof(terminals));

        udpSocket->writeDatagram(datagram.data(), datagram.size(),
                                    QHostAddress::LocalHost, UDP_CLIENT_PORT);
    }
}

void dojoIOServer::slotUdpReadyRead(){
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        udpSocket->readDatagram(datagram.data(), datagram.size());

        while(datagram.size()>7){
            dojoID source = *reinterpret_cast<const int*>(datagram.left(4).data());
            float value = *reinterpret_cast<const float*>(datagram.mid(4,8).data());

            emit spike(source, value);

            datagram.remove(0,8);
        }
    }
}

