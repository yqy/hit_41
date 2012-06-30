#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <mytype.h>
#include <QtGui>
#include <QKeyEvent>
#include <QEvent>
#include <QFileDialog>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void paintEvent(QPaintEvent *);

     void keyPressEvent(QKeyEvent *);
     void keyReleaseEvent(QKeyEvent *);
     void fileOpen();
private slots:
    void on_RE_clicked();

    void on_choose1_clicked();

    void on_choose1_clicked(bool checked);

private:
    QString fileName;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
