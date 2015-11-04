#include "dojologger.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>

dojoLogger::dojoLogger(QObject *parent) : QObject(parent)
{
    logFile = new QFile("dojo.log");
    logFile->open(QIODevice::WriteOnly | QIODevice::Append |  QIODevice::Text);

}
void dojoLogger::logEntry(QString entry){
    QDateTime now = QDateTime::currentDateTime();
    qDebug()<<now.date().toString("dd.MM.yyyy")<<" "<<now.time().toString("HH:mm:ss:zzz")<<" "<<entry;
    QTextStream logStream(logFile);
    logStream<<now.date().toString("dd.MM.yyyy")<<" "<<now.time().toString("HH:mm:ss:zzz")<<" "<<entry<<endl;
}
