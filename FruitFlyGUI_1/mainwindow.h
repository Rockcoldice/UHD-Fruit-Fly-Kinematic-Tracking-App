#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pbStart_clicked();

    void on_pbExit_clicked();

    void on_btnLoadVideo_clicked();

    void on_pbClear_clicked();

    void on_ckboxOutputName_stateChanged(int arg1);

    void FileNameParser(QString);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
