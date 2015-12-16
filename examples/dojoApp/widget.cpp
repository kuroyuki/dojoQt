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

    dojoID node1 = network->createNode(QVector3D(0,0,0), QVector3D(0,0,2), 0.99);
    dojoID node2 = network->createNode(QVector3D(2,0,0), QVector3D(2,0,2), 0.99);
    dojoID node3 = network->createNode(QVector3D(0,2,0), QVector3D(0,2,2), 0.99);
    dojoID node4 = network->createNode(QVector3D(2,2,0), QVector3D(2,2,2), 0.99);
    /*
    //output nodes
    dojoID outNode1 = network->createNode(QVector3D(0,0,4), QVector3D(0,0,6), 0.99);
    dojoID outNode2 = network->createNode(QVector3D(2,0,4), QVector3D(2,0,6), 0.99);
    dojoID outNode3 = network->createNode(QVector3D(0,2,4), QVector3D(0,2,6), 0.99);
    dojoID outNode4 = network->createNode(QVector3D(2,2,4), QVector3D(2,2,6), 0.99);

    //internodes
    dojoID interNode1 = network->createNode(QVector3D(-0.5,-0.5, 3.1), QVector3D(-0.5,-0.5, 3.6), 0.1);
    */
    //bind inputs
    network->bindNodes(-1, node1);
    network->bindNodes(-2, node2);
    network->bindNodes(-3, node3);
    network->bindNodes(-4, node4);
    /*
    //bind nodes
    for(int i=1;i<5;i++)
        for(int j=5;j<9;j++)
            network->bindNodes(i, j);

    //interconnections
    network->bindNodes(node1, interNode1);
    network->bindNodes(interNode1, outNode1);

    //bind outputs
    network->bindNodes(outNode1,-10);
    network->bindNodes(outNode2,-10);
    network->bindNodes(outNode3,-10);
    network->bindNodes(outNode4,-10);
    */

    //bind outputs
    network->bindNodes(node1,-10);
    network->bindNodes(node2,-10);
    network->bindNodes(node3,-10);
    network->bindNodes(node4,-10);

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
    if(ap.id == 1){
        point.setX(point.x()-ap.data);
        point.setY(point.y()-ap.data);
    }
    else if(ap.id == 2){
        point.setX(point.x()+ap.data);
        point.setY(point.y()-ap.data);
    }
    else if(ap.id == 3){
        point.setX(point.x()-ap.data);
        point.setY(point.y()+ap.data);
    }
    else if(ap.id == 4){
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

