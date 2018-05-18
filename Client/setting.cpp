#include "setting.h"
#include "ui_setting.h"


extern char flag;


setting::setting(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::setting)
{
    ui->setupUi(this);
    connect(ui->btn_seting_close,SIGNAL(clicked()),SLOT(close()));
    connect(ui->btn_calibration,SIGNAL(pressed()),SLOT(do_calinration()));
    connect(ui->btn_calibration,SIGNAL(released()),SLOT(do_calinration()));
}

setting::~setting()
{
    delete ui;
}

void setting::do_calibration(){

    qDebug() << "Calibration didnt work" ;

    flag =flag | 00000001 ;

    if( flag )
    {
        qDebug()<<"calibration work";
    }
}


