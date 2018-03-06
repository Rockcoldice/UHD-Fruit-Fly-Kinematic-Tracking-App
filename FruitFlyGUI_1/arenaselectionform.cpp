#include "arenaselectionform.h"
#include "ui_arenaselectionform.h"

ArenaSelectionForm::ArenaSelectionForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ArenaSelectionForm)
{
    ui->setupUi(this);
    QImage myImage(frame.data,frame.cols,frame.rows,frame.step,QImage::Format_RGB888);
    QPixmap p = QPixmap::fromImage(myImage);
    int w = p.width();
    int h = p.height();
    qDebug()<<"W: "<<w<<" H: "<<h<<"\n\n\n";
    ui->lblFrame->setPixmap(p.scaled(w,h,Qt::KeepAspectRatioByExpanding));

}

ArenaSelectionForm::~ArenaSelectionForm()
{
    qDebug()<<"Vector Size: "<<points.size()<<"\n\n\n";
    int size = points.size() -1;
    for(int i=0;i<=size;i++){
        qDebug()<<"X: "<<x[i]<<" Y: "<<y[i]<<"\n";
    }
    qDebug()<<"\n\n";
    delete ui;
}

void ArenaSelectionForm::MousePressed()
{
    QString px = QString::number(ui->lblFrame->x);
    QString py = QString::number(ui->lblFrame->y);
    if(counter<3){
        x[counter] = ui->lblFrame->x;
        y[counter] = ui->lblFrame->y;
        counter++;
        QPoint pt(x[counter],y[counter]);
        points.append(pt);
    }
    else if (counter == 3) {
        disconnect(ui->lblFrame);
        int size = points.size();
        size =size-1;
        for(int i =0;i<=size;i++){
            cv::circle(frame,cv::Point(x[i],y[i]),2,cv::Scalar(0,255,0),3);
        }
        cv::imshow("Frame",frame);
    }
}

void ArenaSelectionForm::MouseRelease()
{
    if (counter == 3) {
            disconnect(ui->lblFrame);
            int size = points.size();
            size =size-1;
            for(int i =0;i<=size;i++){
                cv::circle(frame,cv::Point(x[i],y[i]),2,cv::Scalar(0,255,0),3);
            }
            cv::imshow("Frame",frame);
      }
}
