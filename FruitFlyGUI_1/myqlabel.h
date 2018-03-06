#ifndef MYQLABEL_H
#define MYQLABEL_H

#include <QLabel>
#include <QtCore>
#include <QtGui>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>

class myQLabel : public QLabel
{
    Q_OBJECT

public:
    explicit myQLabel(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent  *ev);
    int x,y;

signals:
    void Mouse_Pressed();
    void Mouse_Released();

};

#endif // MYQLABEL_H
