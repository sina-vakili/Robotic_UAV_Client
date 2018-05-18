#ifndef SETTING_H
#define SETTING_H

#include <QMainWindow>
#include <QtCore>

namespace Ui {
class setting;
}

class setting : public QMainWindow
{
    Q_OBJECT

public:
    explicit setting(QWidget *parent = 0);
    ~setting();



public slots:

    void do_calibration();


private:
    Ui::setting *ui;



private slots:


};

#endif // SETTING_H
