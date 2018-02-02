#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>

QString VideoFileName;
QString OutputFileName;
QString OutputFileParse;

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
