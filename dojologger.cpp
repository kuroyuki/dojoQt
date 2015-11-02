#include "dojologger.h"
#include <QDateTime>
#include <QDebug>

dojoLogger::dojoLogger(QObject *parent) : QObject(parent)
{

}
void dojoLogger::logEntry(QString entry){
    QDateTime now = QDateTime::currentDateTime();
    qDebug()<<now.date().toString("dd.MM.yyyy")<<now.time().toString("HH:mm:ss:zzz")<<":"<<entry;
}
