#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

#include "videodata.h"


VideoData FlyData;
QString VideoFileName;
QString OutputFileName;
QString OutputFileParse;

int theObject[2] = {0,0};
cv::Rect objectBoundingRect = cv::Rect(0,0,0,0);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnLoadVideo_clicked()
{
    VideoFileName = QFileDialog::getOpenFileName(this,("Open File"),"/home",("Video (*.mp4)"));
    ui->txtbxVideoName->setText(VideoFileName);
}

void MainWindow::on_pbClear_clicked()
{
    ui->txtbxOutputFileName->clear();
    OutputFileName.clear();
    ui->txtbxOutputFileName->clear();
}

void MainWindow::on_ckboxOutputName_stateChanged(int arg1)
{
    bool OutputFileCheckbox = false;
    OutputFileCheckbox = ui->ckboxOutputName->checkState();
    if(OutputFileCheckbox){
        FileNameParser(VideoFileName);
        ui->txtbxOutputFileName->setText(OutputFileParse);
    }
    else{
        ui->txtbxOutputFileName->clear();
        OutputFileName.clear();
    }
}

void MainWindow::on_pbExit_clicked()
{
    QCoreApplication::quit();
}

void MainWindow::on_pbStart_clicked()
{
//    if(VideoFileName.isEmpty()){
//        QMessageBox::warning(this,"Error!!","Please Load A Video");
//        return;
//    }
//    else if(OutputFileName.isEmpty()){
//        QMessageBox::warning(this,"Error!!","Please Give A Name to the Output File");
//        return;
//    }
//    else if(OutputFileParse.isEmpty()){
//        QMessageBox::warning(this,"Error!!","Please Give A Name to the Output File");
//        return;
//    }

VideoAnalyzer();
}

void MainWindow::FileNameParser(QString name){
    QString Parser = name;
    int size = name.size() -1;
    if(size <= 0){
        QMessageBox::warning(this,"Error!!","Please Ensure there a Video Loaded");
        return;
    }
    int DotLocation = -1;
    int SlashLocation = -1;
    for(int i = size; i>=0;i--){
        if(Parser[i] == '.'){
            DotLocation = i;
        }
        else if(Parser[i]==47){
            SlashLocation =i;
        }
        if(SlashLocation!=-1&&DotLocation!=-1) break;
    }
    if(DotLocation == -1 ||SlashLocation==-1){
        return;
    }
    for(int i= SlashLocation+1;i<=DotLocation-1;i++){
        OutputFileParse=OutputFileParse + Parser[i];
    }
   return;

}


std::string MainWindow::intToString(int number){
    std::stringstream ss;
    ss<< number;
    return ss.str();
}
void MainWindow::searchForMovement(cv::Mat thresholdImage,cv::Mat &cameraFeed){
    bool objectDetected = false;
    cv::Mat temp;
    thresholdImage.copyTo(temp);

    std::vector<std::vector<cv::Point>>contours;
    std::vector<cv::Vec4i> hierachy;

    cv::findContours(temp,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);
    if(contours.size()>0)objectDetected=true;
    else objectDetected=false;

    if(objectDetected){
       std::vector<std::vector<cv::Point>>largestContourVec;
       largestContourVec.push_back(contours.at(contours.size()-1));
       objectBoundingRect = cv::boundingRect(largestContourVec.at(0));
       int xpos = objectBoundingRect.x + objectBoundingRect.width/2;
       int ypos = objectBoundingRect.y + objectBoundingRect.height/2;
       theObject[0]=xpos;
       theObject[1]=ypos;
    }
    int x= theObject[0];
    int y= theObject[1];
    cv::circle(cameraFeed, cv::Point(x, y), 20, cv::Scalar(0, 255, 0), 2);
    cv::line(cameraFeed, cv::Point(x, y), cv::Point(x, y - 25), cv::Scalar(0, 255, 0), 2);
    cv::line(cameraFeed, cv::Point(x, y), cv::Point(x, y + 25), cv::Scalar(0, 255, 0), 2);
    cv::line(cameraFeed, cv::Point(x, y), cv::Point(x - 25, y), cv::Scalar(0, 255, 0), 2);
    cv::line(cameraFeed, cv::Point(x, y), cv::Point(x + 25, y), cv::Scalar(0, 255, 0), 2);
    cv::Point(x, y);
    FlyData.FrameDataStruct.X_Coord = x;
    FlyData.FrameDataStruct.Y_Coord = y;
    FlyData.FrameVector.push_back(FlyData.FrameDataStruct);

}
void MainWindow::ArenaSetup(int event, int x,int y,int flags,void* userData){
    int size =FlyData.ArenaCoord.size();
    if(size<=3){
        cv::circle((*(cv::Mat*)userData),cv::Point(x,y),2,cv::Scalar(0,255,0),3);
        FlyData.ArenaStruct.x_coord =x;
        FlyData.ArenaStruct.y_coord=y;
        FlyData.ArenaCoord.push_back(FlyData.ArenaStruct);
    }
    else if(size==3){
        FlyData.ArenaCalc();
       // cv::circle((*(cv::Mat)userData),cv::Point(FlyData.x_center,FlyData.y_center),FlyData.ArenaRadius,cv::Scalar(0,255,0),3);
        int rectP1x = FlyData.x_center - FlyData.ArenaRadius -5;
        int rectP1y = FlyData.y_center -FlyData.ArenaRadius -5;
        int rectP2x = FlyData.x_center + FlyData.ArenaRadius + 5;
        int rectP2y =FlyData.y_center + FlyData.ArenaRadius + 5;
        //cv::rectangle((*(cv::Mat)userData),cv::Point(rectP1x,rectP1y),cv::Point(rectP2x,rectP2y),cv::Scalar(0,255,255),3);
    }
    cv::imshow("Areana",(*(cv::Mat*)userData));

}
void MainWindow::VideoAnalyzer(){

    cv::Mat frame1, frame2;
    cv::Mat grayImage1, grayImage2;
    cv::Mat differenceImage;
    cv::Mat thresholdImage;

    cv::VideoCapture cap (VideoFileName.toStdString());

    if(!cap.isOpened()){
        QMessageBox::warning(this,"Error","Error Opening Video Stream on File");
        return;
    }
    cv::namedWindow("Frame");
    cap>>frame1;
    cap>>frame2;
    int counter =1;
    while(1){
        qDebug()<<counter;
        if(frame1.empty()||frame2.empty()) break;
        cv::cvtColor(frame1,grayImage1,cv::COLOR_BGR2GRAY);
        cv::cvtColor(frame2,grayImage2,cv::COLOR_BGR2GRAY);
        cv::absdiff(grayImage1,grayImage2,differenceImage);
        cv::threshold(differenceImage,thresholdImage,20,255,cv::THRESH_BINARY);
        cv::blur(thresholdImage,thresholdImage,cv::Size(10,10));
        cv::threshold(thresholdImage,thresholdImage,20,255,cv::THRESH_BINARY);
        searchForMovement(thresholdImage,frame1);
        cv::imshow("Frame",frame1);


        cap.read(frame1);
        counter=counter +1;
        char c = (char)cv::waitKey(25);
        if(c==27) break;
        frame1.release();
        frame2.release();
        cap>>frame1;
        cap>>frame2;
    }
    cap.release();
    cv::destroyAllWindows();

}
