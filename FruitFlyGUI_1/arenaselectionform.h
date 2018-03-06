#ifndef ARENASELECTIONFORM_H
#define ARENASELECTIONFORM_H

#include <QtCore>
#include <QtGui>
#include <QDialog>
#include <QDebug>
#include <QVector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "myqlabel.h"
namespace Ui {
class ArenaSelectionForm;
}

class ArenaSelectionForm : public QDialog
{
    Q_OBJECT

public:
    explicit ArenaSelectionForm(QWidget *parent = 0);
    cv::Mat frame=cv::imread("C://Users//Jose Miguel Gonzalez//Desktop//test.png");
    ~ArenaSelectionForm();

private slots:
    void MousePressed();
    void on_pushButton_clicked();
    void MouseRelease();

private:
    Ui::ArenaSelectionForm *ui;
    int *x, *y;
    int ii;
    QVector<QPoint> points;
};

#endif // ARENASELECTIONFORM_H
