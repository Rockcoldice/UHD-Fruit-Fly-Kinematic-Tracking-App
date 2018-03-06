#include "myqlabel.h"
myQLabel::myQLabel(QWidget *parent):
QLabel(parent){

}
void myQLabel::mousePressEvent(QMouseEvent *ev){
    this->x=ev->x();
    this->y=ev->y();
    qDebug()<<"X: "<<x<<" Y: "<<y<<"\n\n\n";
    emit Mouse_Pressed();
}

void myQLabel::mouseMoveEvent(QMouseEvent *ev)
{
    qDebug()<<"RELEASED!!";
    emit Mouse_Released();
}
