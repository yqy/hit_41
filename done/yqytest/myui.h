#ifndef MYUI_H
#define MYUI_H


#define JOY_PAD_1		0
#define JOY_PAD_2		1
#define JOY_PAD_3		2
#define JOY_PAD_4		3

#define JOY_PAD_A		0x01
#define JOY_PAD_B		0x02
#define JOY_PAD_SELECT	0x04
#define JOY_PAD_START	0x08
#define JOY_PAD_UP		0x10
#define JOY_PAD_DOWN	0x20
#define JOY_PAD_LEFT	0x40
#define JOY_PAD_RIGHT	0x80
#include <mytype.h>

void SetState(int pad, int index, BYTE val);
void InputBursh(BYTE burst);
BYTE GetValue(int padbit);


#include <QMainWindow>

namespace Ui {
    class Myui;
}

class Myui : public QMainWindow
{
    Q_OBJECT

public:
    explicit Myui(QWidget *parent = 0);
    ~Myui();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Myui *ui;
};

#endif // MYUI_H
