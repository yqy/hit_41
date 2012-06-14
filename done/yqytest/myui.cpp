#include "myui.h"
#include "ui_myui.h"
#include "JOYPAD.h"







Myui::Myui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Myui)
{
    ui->setupUi(this);
}

Myui::~Myui()
{
    delete ui;
}

void Myui::on_pushButton_clicked()
{

    SetState(JOY_PAD_1, JOY_PAD_START, 0);
    sleep(1);
    SetState(JOY_PAD_1, JOY_PAD_START, 1);
}

void Myui::on_pushButton_2_clicked()
{
    SetState(JOY_PAD_1, JOY_PAD_SELECT, 1);
    SetState(JOY_PAD_1, JOY_PAD_SELECT, 0);
}

