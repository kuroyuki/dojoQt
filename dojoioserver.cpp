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
void dojoIOServer::handleSpike(dojoID source, float transmiterValue){
    if(transmiterValue > 0){
        QByteArray datagram;

        datagram.append(reinterpret_cast<const char*>(&source), sizeof(source));
        datagram.append(reinterpret_cast<const char*>(&transmiterValue), sizeof(transmiterValue));

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

            //if input is registered
            if(storage->isNeuronExist(source))
                //send spike from source to the network
                //if no subscribers for the source - nothing should happen
                //qDebug()<<"IO spikes as "<<source;
                emit spike(source, value);

            datagram.remove(0,8);
        }
    }
}

