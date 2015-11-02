#ifndef DOJOLOGGER_H
#define DOJOLOGGER_H

#include <QObject>

class dojoLogger : public QObject
{
    Q_OBJECT
public:
    explicit dojoLogger(QObject *parent = 0);

signals:

public slots:
    void logEntry(QString entry);

private :

};

#endif // DOJOLOGGER_H
