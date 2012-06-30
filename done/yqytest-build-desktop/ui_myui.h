/********************************************************************************
** Form generated from reading UI file 'myui.ui'
**
** Created: Thu Jun 14 07:39:15 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYUI_H
#define UI_MYUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Myui
{
public:
    QWidget *centralwidget;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Myui)
    {
        if (Myui->objectName().isEmpty())
            Myui->setObjectName(QString::fromUtf8("Myui"));
        Myui->resize(800, 600);
        centralwidget = new QWidget(Myui);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(280, 60, 97, 27));
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(280, 110, 97, 27));
        Myui->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Myui);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 25));
        Myui->setMenuBar(menubar);
        statusbar = new QStatusBar(Myui);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        Myui->setStatusBar(statusbar);

        retranslateUi(Myui);

        QMetaObject::connectSlotsByName(Myui);
    } // setupUi

    void retranslateUi(QMainWindow *Myui)
    {
        Myui->setWindowTitle(QApplication::translate("Myui", "MainWindow", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("Myui", "PushButton", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("Myui", "PushButton", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Myui: public Ui_Myui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYUI_H
