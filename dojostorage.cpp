#include "dojostorage.h"

dojoStorage::dojoStorage(QString name, QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(name+".dojo");
    if (db.open()) {
        QSqlQuery query;
        query.exec("create table neurons (id int primary key,"
                   "x real, y real, z real,"
                   "axon_x real, axon_y real, axon_z real)");

        query.exec("create table synapses (source int, target int, length real, permability real)");
    }
    else {
        qDebug()<<"Cannot open database";
    }
}

dojoStorage::~dojoStorage()
{
    db.close();
}
void dojoStorage::eventHandler(QJsonObject event){
    QString command = event.take("command").toString();
    if(command == "an"){
        qDebug()<<"dojoStorage - add neuron";

        QSqlQuery query;
        //INSERT INTO neurons (id, x, y, z, axon_x, axon_y, axon_z)VALUES (:id, :x, :y, :z, :axon_x, :axon_y, :axon_z) on duplicate key
        //query.exec("UPDATE employee SET salary = 70000 WHERE id = 1003");
        query.prepare("INSERT INTO neurons (id, x, y, z, axon_x, axon_y, axon_z)"
                      "VALUES (:id, :x, :y, :z, :axon_x, :axon_y, :axon_z)");

        query.bindValue(":id", event.take("id").toInt());

        QJsonObject pos = event.take("pos").toObject();

        query.bindValue(":x", pos.take("x").toDouble());
        query.bindValue(":y", pos.take("y").toDouble());
        query.bindValue(":z", pos.take("z").toDouble());

        QJsonObject axon = event.take("axon").toObject();
        query.bindValue(":axon_x", axon.take("x").toDouble());
        query.bindValue(":axon_y", axon.take("y").toDouble());
        query.bindValue(":axon_z", axon.take("z").toDouble());

        query.exec();
    }
    else if(command == "as"){
        qDebug()<<"dojoStorage - add synapse";        

        QSqlQuery query;
        query.prepare("INSERT INTO synapses (source, target, length, permability)"
                      "VALUES (:source, :target, :length, :permability)");

        query.bindValue(":source", event.take("source").toInt());
        query.bindValue(":target", event.take("target").toInt());
        query.bindValue(":length", event.take("length").toDouble());
        query.bindValue(":permability",event.take("permability").toDouble());

        query.exec();
    }
    else{
        QJsonDocument  jdoc(event);
        qDebug()<<"dojoStorage - unknown event"<<jdoc.toJson(QJsonDocument::Compact);
    }
}
void dojoStorage::getCurrentTables(){
    QSqlQuery query;

    //get all neurons
    query.exec("SELECT id, x, y, z, axon_x, axon_y, axon_z FROM neurons");
    bool isContinue = true;
    if(query.first()) {
        while(isContinue){
            QJsonObject json;
            json.insert("command", "an");
            json.insert("id", query.value(0).toInt());

            QJsonObject posJson;
            posJson.insert("x", query.value(1).toDouble());
            posJson.insert("y", query.value(2).toDouble());
            posJson.insert("z", query.value(3).toDouble());

            QJsonObject axonJson;
            axonJson.insert("x", query.value(4).toDouble());
            axonJson.insert("y", query.value(5).toDouble());
            axonJson.insert("z", query.value(6).toDouble());

            json.insert("pos", posJson);
            json.insert("axon", axonJson);

            emit storageEvent(json);

            if(!query.next())
                isContinue = false;
            else isContinue = true;
        }
    }
    isContinue = true;

    //get all syanpses
    query.exec("SELECT source, target, length, permability FROM synapses");
    if(query.first()) {
        while(isContinue){
            QJsonObject json;
            json.insert("command", "as");
            json.insert("source", query.value(0).toInt());
            json.insert("target", query.value(1).toInt());
            json.insert("length", query.value(2).toDouble());
            json.insert("permability", query.value(3).toDouble());

            emit storageEvent(json);

            if(!query.next())
                isContinue = false;
            else isContinue = true;
        }
    }
}
QList<dojoID> dojoStorage::getNeuronsInArea(QVector3D center, float radius){
    QList<dojoID> list;
    QSqlQuery query;
    //get all neurons
    query.exec("SELECT id, x, y, z FROM neurons");
    bool isContinue = true;
    if(query.first()) {
        while(isContinue){

            QVector3D neuronPos;
            neuronPos.setX(query.value(1).toDouble());
            neuronPos.setY(query.value(2).toDouble());
            neuronPos.setZ(query.value(3).toDouble());

            float distance = neuronPos.distanceToPoint(center);
            if(distance <= radius)
                list << query.value(0).toInt();

            if(!query.next())
                isContinue = false;
            else isContinue = true;
        }
    }
    return list;
}
 QList<dojoID> dojoStorage::getNeuronsInHemisphere(QVector3D plane, QVector3D normal,  float radius){
     QList<dojoID> list;
     QSqlQuery query;
     //get all neurons
     query.exec("SELECT id, x, y, z FROM neurons");
     bool isContinue = true;
     if(query.first()) {
         while(isContinue){

             QVector3D neuronPos;
             neuronPos.setX(query.value(1).toDouble());
             neuronPos.setY(query.value(2).toDouble());
             neuronPos.setZ(query.value(3).toDouble());

             float distance = neuronPos.distanceToPlane(plane, normal.normalized());

             if(distance >= 0 && distance <= radius)
                 list << query.value(0).toInt();

             if(!query.next())
                 isContinue = false;
             else isContinue = true;
         }
     }
     qDebug()<<"looking distance "<<radius;
     return list;
 }

