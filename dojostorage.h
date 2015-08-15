#ifndef DOJOSTORAGE_H
#define DOJOSTORAGE_H

#include <QObject>

#include "dojo.h"

class dojoNeuron;

class dojoStorage : public QObject
{
    Q_OBJECT
public:
    explicit dojoStorage(QString name, QObject *parent = 0);
    ~dojoStorage();
    void getCurrentTables();

signals:
    void storageEvent(QJsonObject event);

public slots:
    void eventHandler(QJsonObject event);

private :
    QSqlDatabase db;
};

#endif // DOJOSTORAGE_H
