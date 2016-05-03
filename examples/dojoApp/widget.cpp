#include "widget.h"
#include "math.h"

#include <QPainter>
#include <QVector2D>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent)
{
    //Widow setup
    resize(QSize(600,400));
    this->setMouseTracking(true);

    //Dojo server setup
    network = new dojoNetwork();    

    dojoID node1 = network->createNode(QVector3D(0,0,1), QVector3D(0,0,3), 0.6);
    dojoID node2 = network->createNode(QVector3D(0,0,4), QVector3D(0,0,5), 0.5);

    network->registerInput(-1);
    network->bindNodes(-1, node1);

    network->bindNodes(node1, node2);
    network->registerOutput(-100);
    network->bindNodes(node2, -100);

    //Drawing setup
    point.setX(qrand()%this->width());
    point.setY(qrand()%this->height());

    //starting all the stuff
    network->start();

    //init dojo client
    dojo = new dojoClient(QHostAddress::LocalHost);
    connect(dojo, SIGNAL(dojoEvent(dojoAp)), this, SLOT(updateData(dojoAp)));
    dojo->connectToServer();

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));

    timer->setInterval(20);
    timer->start();
}

Widget::~Widget()
{

}
void Widget::mouseMoveEvent(QMouseEvent* event){
    mousePoint = event->pos();
    /*
    QVector2D vect(mousePoint-point);
    QVector2D normalized = vect.normalized();

    qDebug()<<acos(normalized.x())*180/3.1426<<","<<asin(normalized.y())*180/3.1426;
    */
}
void Widget::slotTimeout(){

    if(leftTopRect.contains(mousePoint)){
        dojo->sendAp(-1, 2);
    }
    if(rightTopRect.contains(mousePoint)){
        dojo->sendAp(-2, 2);
    }
    if(leftBottomRect.contains(mousePoint)){
        dojo->sendAp(-3, 2);
    }
    if(rightBottomRect.contains(mousePoint)){
        dojo->sendAp(-4, 2);
    }
}

void Widget::updateData(dojoAp ap){
    //sort by source of spike
    if(ap.id == -101){
        point.setX(point.x()-ap.data);
        point.setY(point.y()-ap.data);
    }
    else if(ap.id == -102){
        point.setX(point.x()+ap.data);
        point.setY(point.y()-ap.data);
    }
    else if(ap.id == -103){
        point.setX(point.x()-ap.data);
        point.setY(point.y()+ap.data);
    }
    else if(ap.id == -104){
        point.setX(point.x()+ap.data);
        point.setY(point.y()+ap.data);
    }

    //redraw
    update();
}

void Widget::paintEvent(QPaintEvent* ){    
    QPainter painter(this);
    painter.setRenderHint(
        QPainter::Antialiasing);
    painter.setPen(Qt::darkGreen);


    leftTopRect.setRect(point.x()-40, point.y()-40, 55, 55);
    rightTopRect.setRect(point.x()-10, point.y()-40, 55, 55);

    leftBottomRect.setRect(point.x()-40, point.y()-10, 55, 55);
    rightBottomRect.setRect(point.x()-10, point.y()-10, 55, 55);

    painter.drawRect(leftTopRect);
    painter.drawRect(rightTopRect);
    painter.drawRect(leftBottomRect);
    painter.drawRect(rightBottomRect);
}

