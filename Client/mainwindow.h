#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include<QtCore>
#include<iostream>
#include<SDL/SDL.h>
#include <QtNetwork>
#include <QTcpSocket>
#include <QHostAddress>
#include <QProgressBar>
#include"setting.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    QTimer *timer_read_joy;
    SDL_Joystick *joy;
    QTcpSocket *ssocket;   //taref tabeh dakhele class
    QTimer *timerr_send_client;
    QElapsedTimer *rresive_server_timer;
    QTimer *opcv_imshow_timer;
    VideoCapture  cap;
    VideoWriter vidio;

private:
    Ui::MainWindow *ui;
    setting sett;
    //setting do_calibration;

public slots:
    void joystick();
    void joy_raed();
    void fnc_Connect();  // tabea etesal
    void fnc_Send();   //daryaft dadeh va tabea ersale dade
    void readdser();
    void conct_stablishid();
    void func_set();
    void display_camera();

private slots:
    void on_btn_cam_clicked();
    void on_btn_recorde_clicked();
    void on_btn_stop_clicked();
    void on_btn_puase_clicked();
    void on_btn_landing_clicked();
    void on_btn_takeoff_clicked();
};

#endif // MAINWINDOW_H
