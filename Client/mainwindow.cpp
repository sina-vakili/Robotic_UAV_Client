#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "setting.h"

#define joystick_read_speed_millisecond         25
#define server_send_speed_millisecond           8
#define server_ip_address                       "192.168.1.130"
//#define server_ip_address                     "127.0.0.1"
#define conection_port                          1234

qint32 tmp_ertefa ;
String name_vidio = "Quad_Video.avi";

//---------------------------->> global variable

quint8 mbut[12];
quint8 mhat;
quint8 server_speed;
quint8 Primary_height;
quint8 camera_display=1;
char flag ;
quint8 flag_recorde=2;
quint16 yaw , roll , pitch , throttle ;
qint32 Height_quad , ertefa , offset  ;


//---------------------------->> mainwindow class
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->cb_invert->setChecked(1);
    ui->btn_puase->setEnabled(0);

    opcv_imshow_timer=new QTimer(this);
    connect(opcv_imshow_timer,SIGNAL(timeout()),SLOT(display_camera()));

    //--------------------------------------> mohasebeye zaman daryaft az server
    ssocket=new QTcpSocket(this);

    rresive_server_timer= new QElapsedTimer;
    rresive_server_timer->start();

    joystick();

    //-------------------------------------->essential buttons
    connect(ui->btn_quit,SIGNAL(clicked()),SLOT(close()));
    connect(ui->btn_connect,SIGNAL(clicked()),SLOT(fnc_Connect()));
    connect(ui->btn_setting,SIGNAL(clicked()),SLOT(func_set())) ;

    //sett.show();

    //------------------------------------->joystick timer
    timer_read_joy = new QTimer(this);
    connect(timer_read_joy,SIGNAL(timeout()),SLOT(joy_raed()));
    timer_read_joy->start(joystick_read_speed_millisecond);

    //------------------------------------->
    timerr_send_client = new QTimer(this);
    connect(timerr_send_client,SIGNAL(timeout()),SLOT(fnc_Send()));
    timerr_send_client->start(server_send_speed_millisecond );
}



MainWindow::~MainWindow()
{
    delete ui;
    //  sett.close();
}


//=========================================>read joystick

//---------------------------------------->check joystick connection
void MainWindow::joystick() {
    SDL_JoystickEventState(SDL_ENABLE);
    SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO);

    if(SDL_NumJoysticks()>0)
    {
        joy=SDL_JoystickOpen(0);

        if(joy)
        {

            qDebug() <<"Opened Joystick 0\n" ;
            qDebug() << "name:" << SDL_JoystickName(0);
            qDebug() <<"Number of Buttons"<< SDL_JoystickNumButtons(joy);
            qDebug() <<"Number of Balls"<< SDL_JoystickNumBalls(joy);
            qDebug() <<"Number of hat"<<SDL_JoystickNumHats(joy);
            qDebug() <<"Number of axes"<<SDL_JoystickNumAxes(joy);

        }
        else
        {
            qDebug() <<"Couldn't open Joystick 0\n";
        }
    }
    else
    {
        qDebug() <<"not joystick found \n";
    }
}

//----------------------------------------> read data from
void MainWindow::joy_raed()
{
    if(SDL_NumJoysticks()>0)
    {
        SDL_Event event;
        SDL_PollEvent(&event);

        for(int i=0;i<12;i++)
        {
            mbut[i]= SDL_JoystickGetButton(joy, i);
        }

        mhat= SDL_JoystickGetHat(joy, 0);

        switch(mhat)
        {
        case SDL_HAT_CENTERED:

            break;

        case SDL_HAT_UP:

            Height_quad ++ ;

            break;

        case SDL_HAT_RIGHT:

            break;

        case SDL_HAT_DOWN:

            Height_quad -- ;

            break;

        case SDL_HAT_LEFT:

            break;

        case SDL_HAT_RIGHTUP:

            break;

        case SDL_HAT_RIGHTDOWN:

            break;

        case SDL_HAT_LEFTUP:

            break;

        case SDL_HAT_LEFTDOWN:

            break;
        }

        ui->lbl_now_h->setNum(Height_quad);

        //----------------------------------------------------------------progress Bar

        roll=((250)* ( SDL_JoystickGetAxis(joy,0))/32768+1500);
        ui->prb_roll->setValue(roll);

        pitch=((-250)* ( SDL_JoystickGetAxis(joy,1))/32768+1500);
        ui->prb_pitch->setValue(pitch);


        throttle=((-500)* ( SDL_JoystickGetAxis(joy,3))/32768+1500);
        ui->prb_throttle->setValue(throttle);


        if ( mbut[2]==1 && throttle <1100)
        {
            yaw=1000;
        }
        else if(mbut[3]==1 && throttle <1100)
        {
            yaw=2000;
        }

        else
        {
            yaw=((250)* ( SDL_JoystickGetAxis(joy,2))/32768+1500);
        }

        ui->prb_yaw->setValue(yaw);


        //------------------------> safe mode button

        if (mbut[1])
        {
            close() ;
        }

        //--------------------------> servo flag

        if (mbut[8] == 1)
        {
            flag = flag & ~(0b100) ;
            qDebug() << "servo off";
        }

        if (mbut[9] == 1)
        {
            flag = flag | 0b100 ;
            qDebug() << "servo on";
        }

        //-------------------------->mode ertefa

        if (mbut[10]==1)  //    gayre faal sazi mode ertefa
        {
            flag = flag & ~(0b10) ;
            qDebug()<<"ertefa mode off";
            Height_quad=0;
        }


        if (mbut[11]==1)//       faal sazi mode ertefa
        {
            flag = flag | 0b10;
            qDebug()<<"ertefa mode On";
            offset =tmp_ertefa ;
            Height_quad=0;
        }
    }
}
//=======================================================> end of joystick

//=======================================================> connecting button

//----------------------------------------------> tabea etesal
void MainWindow::fnc_Connect()
{
    if (! ssocket->isOpen())
    {
        QHostAddress addr(server_ip_address);            // ip etesal
        ssocket->connectToHost(addr,conection_port );    // connecting port
        connect (ssocket,SIGNAL(readyRead()),SLOT(readdser()));
        connect (ssocket,SIGNAL(connected()),SLOT(conct_stablishid()));
        ui->statusBar->showMessage(tr("socket _open"));
    }
}
//=======================================================> connection End

//=======================================================> Sending to server

//---------------------------------------------> tabea ersal data be server

void MainWindow::fnc_Send()
{
    quint16 c ;
    char high_value , low_value ;
    char chksum=0;
    if(ssocket->isOpen())
    {
        //@ & *  data data .... xor;

        //---------ramz

        ssocket->write("@");                                 // D:00   ramz_@
        ssocket->write("&");                                 // D:01   ramz_&
        ssocket->write("*");                                 // D:02   ramz_*

        //-----------data

        c = roll ;

        high_value = c >> 8 ;
        low_value = c & 0xff;

        ssocket->write(&high_value,1);                       // D:03    roll_high
        ssocket->write(&low_value,1);                        // D:04    roll_low

        chksum^=high_value;
        chksum^=low_value;


        c=pitch;

        high_value = c >> 8 ;
        low_value = c & 0xff;

        ssocket->write(&high_value,1);                       // D:05    pitch_high
        ssocket->write(&low_value,1);                        // D:06    pitch_low

        chksum^=high_value;
        chksum^=low_value;


        c=yaw;

        high_value = c >> 8 ;
        low_value = c & 0xff;

        ssocket->write(&high_value,1);                        // D:07   yaw_high
        ssocket->write(&low_value,1);                         // D:08   yaw_low

        chksum^=high_value;
        chksum^=low_value;


        c=throttle;

        high_value = c >> 8 ;
        low_value = c & 0xff;

        ssocket->write(&high_value,1);                        // D:09   throttle_high
        ssocket->write(&low_value,1);                         // D:10   throttle_low

        chksum^=high_value;
        chksum^=low_value;


        //---------------------------------------> new Height to quad
        char low_new_quad_height , h_new_quad_height , hh_new_quad_height , hhh_new_quad_height ;

        //qint32 Height_quad = ui->lbl_now_h->text().toInt();

        qint32 Height_quad_send;

        Height_quad_send = Height_quad + offset ;


        low_new_quad_height = (Height_quad_send & 0xff);
        h_new_quad_height = (Height_quad_send >> 8)& 0xff;
        hh_new_quad_height = (Height_quad_send >> 16)& 0xff;
        hhh_new_quad_height = (Height_quad_send >> 24)& 0xff;

        ssocket->write(&low_new_quad_height,1);                 // D:11     alt_low
        ssocket->write(&h_new_quad_height,1);                   // D:12     alt_high
        ssocket->write(&hh_new_quad_height,1);                  // D:13     alt_h_high
        ssocket->write(&hhh_new_quad_height,1);                 // D:14     alt_h_h_high

        chksum^=low_new_quad_height;
        chksum^=h_new_quad_height;
        chksum^=hh_new_quad_height;
        chksum^=hhh_new_quad_height;

        ssocket->write(&flag,1);                               // D:15     flag
        chksum ^= flag;

        chksum=chksum & 0xff;
        ssocket->write(&chksum,1);                              // D:16     checksum


        if( flag & 01 )
        {
            flag = flag & 0xfe ;
        }
    }

    else
        ui->statusBar->showMessage(tr("socket_not _open"));
}


//===========================================================> gereftan data az server

void MainWindow::readdser()
{
    quint8 cheksum = 0 ;

    QByteArray s = ssocket->readAll();

    cheksum ^= static_cast<quint8>(s[3]) & 0xff;   cheksum ^= static_cast<quint8>(s[4]) & 0xff;
    cheksum ^= static_cast<quint8>(s[5]) & 0xff;   cheksum ^= static_cast<quint8>(s[6]) & 0xff;
    cheksum ^= static_cast<quint8>(s[7]) & 0xff;   cheksum ^= static_cast<quint8>(s[8]) & 0xff;
    cheksum ^= static_cast<quint8>(s[9]) & 0xff;   cheksum ^= static_cast<quint8>(s[10]) & 0xff;
    cheksum ^= static_cast<quint8>(s[11]) & 0xff;  cheksum ^= static_cast<quint8>(s[12]) & 0xff;
    cheksum &= 0xff;

    if (cheksum == (static_cast<quint8>(s[13])))
    {
        //QString str = QString::number(static_cast<quint8>(s[0]));
        QString roll_angl = QString::number(qint16((static_cast<quint8>(s[4])<<8)+((static_cast<quint8>(s[3])& 0xff))));
        QString pitch_angl = QString::number(qint16((static_cast<quint8>(s[6])<<8)+((static_cast<quint8>(s[5])& 0xff))));
        tmp_ertefa = (qint32(((static_cast<quint8>(s[7])) & 0xff)+(((static_cast<quint8>(s[8])<<8))& 0xff00)
                + ((static_cast<quint8>(s[9])<<16)&0xff0000)+((((static_cast<quint8>(s[10])<<24))) & 0xff000000)));
        QString msp_speed = QString::number(quint8(static_cast<quint8>(s[11])));
        QString client_time = QString::number(quint8(static_cast<quint8>(s[12])));

        qDebug()<<"tmp_ertefa"<<tmp_ertefa;
        qDebug()<<"offset"<<offset;

        ui->angl_roll->setText(roll_angl);
        ui->pitch_angl->setText(pitch_angl);

        //----------------------------------------------------->namayesh ertefaye bad az automat
        //    if (Atl_mode)
        //    {
        ertefa = tmp_ertefa - offset ;

        //     }

        QString new_ertefa = QString::number(ertefa);
        ui->input_new_quad_Heigh->setText(new_ertefa);

        //    else
        //    {
        //        ui->input_new_quad_Heigh->setText(Height_quad);
        //    }

        //---------------------------------------------------<

        ui->msp_speeder->setText(msp_speed);
        ui->speed_to_server->setText(client_time);

        //qDebug() << "read data from server" << rresive_server_timer->elapsed();

        server_speed = rresive_server_timer->elapsed();
        QString hi = QString::number(server_speed);
        rresive_server_timer->start();
        ui->server_speeder->setText(hi);
    }
}

//-----------------------------> tabe anjam baed az vasl shodan
void MainWindow::conct_stablishid()
{
    ui->btn_connect->setEnabled(0);
}

void MainWindow::func_set()
{
    sett.show();
}

//============================================> Land OF Open CV

//---------------------------------> show camera code
void MainWindow::display_camera()
{

    Mat frame;

    if(cap.isOpened())
    {

        cap.read(frame);

        if(ui->cb_invert->checkState())
            flip(frame,frame,0);

        if(ui->cb_miror->checkState())
            flip(frame,frame,1);


        if (flag_recorde==0)
        {
            vidio.write(frame);
        }
        cv::resize(frame,frame,Size(ui->lbl->width(),ui->lbl->height()));
        cvtColor(frame,frame,CV_BGR2RGB);
        QImage img=QImage((uchar*)frame.data,frame.cols,frame.rows,frame.step,QImage::Format_RGB888);
        QPixmap pix=QPixmap::fromImage(img);
        ui->lbl->setPixmap(pix);

    }
}

//-------------------------------------> camera open button
void MainWindow::on_btn_cam_clicked()
{
    if (camera_display)
    {
        cap.open(ui->lin_cam->text().toInt());

        if (cap.isOpened())
        {
            opcv_imshow_timer->start(50);// if 33 fps =30  if 50 fps=20
            camera_display=0;
            ui->lbl->show();
            ui->lbl_camera_mode->setText("Camera Open");
        }
    }

    else
    {
        opcv_imshow_timer->stop();
        camera_display=1;
        ui->lbl->hide();
        cap.release();
        ui->lbl_camera_mode->setText("Camera Close");
    }
}

//---------------------------------------> camera recording button
void MainWindow::on_btn_recorde_clicked()
{
    if(flag_recorde==2)
        vidio.open(name_vidio,CV_FOURCC('M','J','P','G'),20,cv::Size(640,480),true);

    flag_recorde = 0;
    qDebug()<<"flag_recorde=1";

    ui->btn_puase->setEnabled(1);
    ui->lbl_camera_mode->setText("Recording");
}


//--------------------------------------> camera stop button
int i;

void MainWindow::on_btn_stop_clicked()
{
    if(flag_recorde!=2)
    {
        flag_recorde = 2;

        i++;
        string qq = to_string(i);
        name_vidio="Quad_Video"+qq+".avi";
        qDebug()<<"flag_recorde=0";
        ui->btn_puase->setEnabled(0);
        ui->lbl_camera_mode->setText("Stop Recording");
    }
}

//--------------------------------------> camera puse button
void MainWindow::on_btn_puase_clicked()
{
    flag_recorde = 1;
    qDebug()<<"flag_recorde=0";
    ui->btn_puase->setEnabled(0);
    ui->lbl_camera_mode->setText("Pause");
}

//=======================================================================> End of Land



//=======================================================================>autonamous planet

//----------------------------------------> quad landing
void MainWindow::on_btn_landing_clicked()
{
    Height_quad=-40;
}

//----------------------------------------> quad takeoff
void MainWindow::on_btn_takeoff_clicked()
{
    flag = flag | 0b10;
    offset =tmp_ertefa ;
    Height_quad=ui->lin_takeoff->text().toInt();
}
//========================================================================>End of autonamous planet
