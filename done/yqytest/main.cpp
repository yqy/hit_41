#include <QtGui/QApplication>
#include "mainwindow.h"
#include "myui.h"


DWORD m_PadBit[2];
BOOL  m_bStrobe;
int   m_CurIndex[2];

void SetState(int pad, int index, BYTE val)
{
        DWORD * PadBit = &m_PadBit[pad & 0x1];
        DWORD mask = index;
        if (pad & 0x2)
                mask <<= 8;
        if (val)
                *PadBit |= mask;
        else
                *PadBit &= ~mask;
}


void InputBursh(BYTE burst)
{
        if (burst & 0x1 && m_bStrobe == false)
        m_bStrobe = true;
        else if (!(burst & 0x1) && m_bStrobe)
        {
                m_bStrobe = false;
                m_CurIndex[0] = m_CurIndex[1] = 0;
        }
}

BYTE GetValue(int padbit)
{
        BYTE ret = (BYTE)((m_PadBit[padbit] >> m_CurIndex[padbit]) & 0x1);
        m_CurIndex[padbit]++;
        if (m_CurIndex[padbit] >= 24)
                ::memset(m_CurIndex, 0, sizeof(m_CurIndex));
        return ret;
}












int main(int argc, char *argv[])
{
    m_bStrobe = false;
     ::memset(m_CurIndex, 0, sizeof(m_CurIndex));
      ::memset(m_PadBit, 0, sizeof(m_PadBit));
    /*int pid = fork();

    if(pid == 0){*/
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
    /*}
    else{

    QApplication b(argc, argv);
    Myui www;
    www.show();
    return b.exec();*/

}

