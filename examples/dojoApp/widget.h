#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QRectF>

#include "../../dojoclient.h"
#include "../../dojonetwork.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void mouseMoveEvent(QMouseEvent* event);
    void updateData(dojoAp ap);

    void slotTimeout();

private:
    void paintEvent(QPaintEvent *);

    dojoNetwork* network;
    dojoClient* dojo;

    QPointF point;
    QPointF mousePoint;

    QRectF leftTopRect;
    QRectF rightTopRect;
    QRectF leftBottomRect;
    QRectF rightBottomRect;

    QTimer* timer;
};

#endif // WIDGET_H
