#include "dojostorage.h"

dojoStorage::dojoStorage(QString name, QObject *parent) : QObject(parent)
{
    QString filename = name+".dojo";
    QFileInfo fileInfo(filename);
    if(fileInfo.isFile()){
        QJsonObject json;
        json.insert("state", "already_exist");
        emit storageEvent(json);
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(name+".dojo");

    if (db.open()) {
        QSqlQuery query;

        QString str = "CREATE TABLE neurons ("
                "id integer, "
                "size real, "
                "x real, "
                "y real, "
                "z real, "
                "axon_x real, "
                "axon_y real, "
                "axon_z real, "
                "dendrite_radius real"
                ");";
        bool b = query.exec(str);
        if (!b) {
            QJsonObject json;
            json.insert("state", "neuron_table_is_not_created");
            emit storageEvent(json);
        }
        else{
            QString str = "CREATE TABLE synapses ("
                    "source integer, "
                    "target integer, "
                    "permability real, "
                    "sign bool"
                    ");";
            bool b = query.exec(str);
            if (!b) {
                QJsonObject json;
                json.insert("state", "synapse_table_is_not_created");
                emit storageEvent(json);
            }            
        }
    }
}

dojoStorage::~dojoStorage()
{
    db.close();
}

void dojoStorage::addNeuron(dojoNeuron* neuron){
    QSqlQuery query;
    QString str = "INSERT INTO neurons(id, size, x, y, z, azon_x, axon_y, axon_z, dendrite_radius) "
            "VALUES ("+
            //QString::number(neuron->getID())+","+
            //QString::number(neuron->getSize())+","+
            QString::number(neuron->getPosition().x())+","+
            QString::number(neuron->getPosition().y())+","+
            QString::number(neuron->getPosition().z())+","+
            QString::number(neuron->getAxonPosition().x())+","+
            QString::number(neuron->getAxonPosition().y())+","+
            QString::number(neuron->getAxonPosition().z())+","+
            QString::number(0)+
            ");";

    query.exec(str);
}

dojoNeuron dojoStorage::getNeuron(dojoID id){
    QSqlQuery query("SELECT * FROM neurons WHERE id = " + QString::number(id)+";");

    QSqlRecord rec = query.record();

    while (query.next()){
        qDebug() << "id"<<query.value(0).toString()
                 << "size"<<query.value(1).toString()
                 << "x"<<query.value(rec.indexOf("x")).toString()
                 << "y"<<query.value(rec.indexOf("y")).toString()
                 << "z"<<query.value(rec.indexOf("z")).toString();
    }
    QVector3D pos(0,0,0);
    QVector3D axon(1,2,3);
    dojoNeuron newOne(id,pos, axon);
    return newOne;
}

void dojoStorage::updateNeuron(dojoNeuron* neuron){

}

void dojoStorage::removeNeuron(dojoID id){
    QJsonObject json;
    json.insert("state", "neuron_removed");
    json.insert("id", (double)id);
    emit storageEvent(json);
}

void dojoStorage::addSynapse(dojoSynapse* synapse){
    QSqlQuery query;
    /*QString str = "INSERT INTO synapses(source, target, permability, sign) "
            "VALUES ("+
            QString::number(synapse->getSource()->getID())+","+
            QString::number(synapse->getTarget()->getID())+","+
            QString::number(synapse->getPermability())+","+
            "true"+
            ");";

    query.exec(str);
    */
}
/*
dojoSynapse dojoStorage::getSynapse(dojoID source, dojoID target){
    QSqlQuery query("SELECT * FROM synapses WHERE source = " + QString::number(source)+" target = "+ QString::number(target)+";");
    QSqlRecord rec = query.record();

    while (query.next()){
        qDebug() << "id"<<query.value(0).toString()
                 << "size"<<query.value(1).toString()
                 << "x"<<query.value(rec.indexOf("x")).toString()
                 << "y"<<query.value(rec.indexOf("y")).toString()
                 << "z"<<query.value(rec.indexOf("z")).toString();
    }
    return dojoChemicalSynapse();
}
*/
void dojoStorage::updateSynapse(dojoSynapse* synapse){

}

void dojoStorage::removeSynapse(dojoID source, dojoID target){
    QJsonObject json;
    json.insert("state", "synapse_removed");
    json.insert("source", (double)source);
    json.insert("target", (double)target);
    emit storageEvent(json);
}

