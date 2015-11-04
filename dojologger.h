#ifndef DOJOLOGGER_H
#define DOJOLOGGER_H

#include <QObject>
#include <QFile>

class dojoLogger : public QObject
{
    Q_OBJECT
public:
    explicit dojoLogger(QObject *parent = 0);

signals:

public slots:
    void logEntry(QString entry);

private :
    QFile*  logFile;
};

#endif // DOJOLOGGER_H
