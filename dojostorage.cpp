#include "dojostorage.h"

dojoStorage::dojoStorage(QObject *parent) : QObject(parent)
{
    redis = new QTcpSocket();

    redis->connectToHost("localhost", 6379);

    redis->waitForConnected();

    qDebug()<<"connected";
}
QString dojoStorage::writeCommand(QString cmd){
     redis->write(cmd.toLocal8Bit()+"\n");
     redis->waitForReadyRead();
     QByteArray data = redis->readAll();

     return QString(data);
}
void dojoStorage::setNextID(dojoID next){
    QString str;
    str = "hset network nextID "+QString::number(next);
    writeCommand(str);

    emit storageEvent(str);
}

dojoID dojoStorage::getNextID(){
    QString str;
    str = "hget network nextID";
    QStringList list = writeCommand(str).split("\r\n");

    return list[1].toInt();
}
void dojoStorage::addNeuron(dojoID id, QVector3D position, QVector3D axon, float size, float terminals){
    QString str;
    str = "hmset neurons:"+QString::number(id)+
            " x "+QString::number(position.x())+
            " y "+QString::number(position.y())+
            " z "+QString::number(position.z())+
            " axon_x "+QString::number(axon.x())+
            " axon_y "+QString::number(axon.y())+
            " axon_z "+QString::number(axon.z())+
            " size "+ QString::number(size)+
            " terminals "+QString::number(terminals);

    writeCommand(str);

    emit storageEvent(str);
}
bool dojoStorage::isNeuronExist(dojoID id){
    QString str;
    str = "exists neurons:"+QString::number(id);
    QString reply = writeCommand(str);
    reply.chop(2);
    reply.remove(0,1);
    if(reply == "1")
        return true;
    else return false;
}

void dojoStorage::setNeuronPosition(dojoID id, QVector3D new_position){
    QString str;
    str = "hmset neurons:"+QString::number(id)+
            " x "+QString::number(new_position.x())+
            " y "+QString::number(new_position.y())+
            " z "+QString::number(new_position.z());
    writeCommand(str);

    emit storageEvent(str);
}

void dojoStorage::setNeuronAxon(dojoID id, QVector3D new_axon){
    QString str;
    str = "hmset neurons:"+QString::number(id)+
            " axon_x "+QString::number(new_axon.x())+
            " axon_y "+QString::number(new_axon.y())+
            " axon_z "+QString::number(new_axon.z());
    writeCommand(str);

    emit storageEvent(str);
}

void dojoStorage::setNeuronSize(dojoID id, float new_size){
    QString str;
    str = "hset neurons:"+QString::number(id)+" size "+QString::number(new_size);
    writeCommand(str);

    emit storageEvent(str);
}
void dojoStorage::setNeuronTerminals(dojoID id, float newTerminals){
    QString str;
    str = "hset neurons:"+QString::number(id)+" terminals "+QString::number(newTerminals);
    writeCommand(str);

    emit storageEvent(str);
}
QVector3D dojoStorage::getNeuronPosition(dojoID id){
    QString str;
    str = "hmget neurons:"+QString::number(id)+" x y z";

    QStringList list = writeCommand(str).split("\r\n");

    return QVector3D(list[2].toFloat(),list[4].toFloat(),list[6].toFloat() ) ;
}

QVector3D dojoStorage::getNeuronAxon(dojoID id){
    QString str;
    str = "hmget neurons:"+QString::number(id)+" axon_x axon_y axon_z";

    QStringList list = writeCommand(str).split("\r\n");

    return QVector3D(list[2].toFloat(),list[4].toFloat(),list[6].toFloat() ) ;
}

float dojoStorage::getNeuronSize(dojoID id){
    QString str;
    str = "hget neurons:"+QString::number(id)+" size ";
    QStringList list = writeCommand(str).split("\r\n");

    return list[1].toFloat();
}
float dojoStorage::getNeuronTerminals(dojoID id){
    QString str;
    str = "hget neurons:"+QString::number(id)+" terminals ";

    QStringList list = writeCommand(str).split("\r\n");

    return list[1].toFloat();
}
void dojoStorage::deleteNeuron(dojoID id){
    QString str;
    str = "del neurons:"+QString::number(id);
    writeCommand(str.toLocal8Bit());

    emit storageEvent(str);
}
QList<dojoID> dojoStorage::getNeuronsInArea(QVector3D point, float atDistance){
    QList<dojoID> neurons;

    QString str;
    int start = 0;
    do{
        str = "scan "+QString::number(start)+" match \"neurons:*\" count 10";

        QStringList list = writeCommand(str).split("\r\n");
        for(int i=5;i<list.length();i=i+2){
            dojoID neuronID = list[i].replace("neurons:", "").toInt();

            QVector3D pos = getNeuronPosition(neuronID);
            float size = getNeuronSize(neuronID);

            QVector3D diff = pos-point;
            float length = diff.length();
            if(length-size <= atDistance)
                neurons<<neuronID;
        }

        start = list[2].toInt();
    }
    while(start != 0);
    return neurons;
}
QList<dojoID> dojoStorage::getAllNeurons(){
    QList<dojoID> neurons;

    QString str;
    int start = 0;
    do{
        str = "scan "+QString::number(start)+" match \"neurons:*\" count 10";

        QStringList list = writeCommand(str).split("\r\n");
        for(int i=5;i<list.length();i=i+2){
            dojoID neuronID = list[i].replace("neurons:", "").toInt();
            neurons<<neuronID;
        }

        start = list[2].toInt();
    }
    while(start != 0);
    return neurons;
}

void dojoStorage::addSynapse(QString synapse, float perm, float length){
    QString str;
    str = "hmset synapses:"+synapse
            +" perm "+QString::number(perm)
            +" length "+QString::number(length);

    writeCommand(str);

    emit storageEvent(str);
}
void dojoStorage::deleteSynapse(QString synapse){
    QString str;
    str = "del synapses:"+synapse;
    writeCommand(str);

    emit storageEvent(str);
}
bool dojoStorage::isSynapseExist(QString synapse){
    QString str;
    str = "exists synapses:"+synapse;
    QString reply = writeCommand(str);
    reply.chop(2);
    reply.remove(0,1);
    if(reply == "1")
        return true;
    else return false;
}
void dojoStorage::setSynapseLength(QString synapse,float length){
    QString str;
    str = "hset synapses:"+synapse
            +" length "+QString::number(length);
    writeCommand(str);

    emit storageEvent(str);
}

float dojoStorage::getSynapseLength(QString synapse){
    QString str;
    str = "hget synapses:"+synapse
            +" length ";

    QStringList list = writeCommand(str).split("\r\n");

    return list[1].toFloat();
}

void dojoStorage::setSynapsePermability(QString synapse, float perm){
    QString str;
    str = "hset synapses:"+synapse
            +" perm "+QString::number(perm);
    writeCommand(str);

    emit storageEvent(str);
}
float dojoStorage::getSynapsePermability(QString synapse){
    QString str;
    str = "hget synapses:"+synapse
            +" perm ";
    QStringList list = writeCommand(str).split("\r\n");

    return list[1].toFloat();
}
QList<QString> dojoStorage::getAllSynapses(){
    QList<QString> synapses;

    QString str;
    int start = 0;
    do{
        str = "scan "+QString::number(start)+" match \"synapses:*\" count 10";

        QStringList list = writeCommand(str).split("\r\n");
        for(int i=5;i<list.length();i=i+2){
            QString synapse = list[i].replace("synapses:", "");
            synapses<<synapse;
        }

        start = list[2].toInt();
    }
    while(start != 0);
    return synapses;
}
