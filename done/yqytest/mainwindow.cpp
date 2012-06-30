#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "stdafx.h"
#include "NES.h"
#include "CPU.h"
#include "PPU.h"
#include "PPUINC.h"
#include "Mapper.h"
#include "NES.h"


#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <QPainter>
#include <QImage>
#include <QMainWindow>
#include <Qt>
#include <QtCore>
#include <QThread>
#include <myui.h>

#include <QFileDialog>

#pragma comment(lib, "Winmm.lib")








MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    timer->start(40);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
}



DWORD Nes_Crc = 0;
int PROM_8K_SIZE;
int PROM_16K_SIZE;
int PROM_32K_SIZE;

int VROM_1K_SIZE;
int VROM_2K_SIZE;
int VROM_4K_SIZE;
int VROM_8K_SIZE;

RENDERMETHOD Nes_RenderMethod;

LPBYTE CPU_MEM_BANK[8];		//每个元� 指向8K数据

LPBYTE PPU_MEM_BANK[12];	//每个元� 指向1K数据

BYTE CRAM[ 8 * 1024];
BYTE VRAM[ 4 * 1024];
const char* strFileName;

BYTE RomHeader[16];

BYTE RAM[0x2000];			//供CPU使用的8K内存
BYTE SRAM[0x2000];
BYTE * PRGBlock = NULL;		//指向program rom

BYTE * PatternTable = NULL;	//图案表
BYTE NameTable[0x800];		//命名表
BYTE BGPal[0x10];			//背景调色板
BYTE SPPal[0x10];			//精灵调色板
BYTE SPRAM[0x100];			//精灵RAM，256字节，可存放64个精灵
SPRITE * Sprite = (SPRITE *)SPRAM;
BYTE RevertByte[256];

QVector<QRgb> colorTable;
BYTE ScreenBit[256 * 240];
int flag = 1;
QImage drawi;

NESCONFIG NESCONFIG_NTSC =
{
        21477270.0f,		// Base clock
        1789772.5f,		// Cpu clock
        262,			// Total scanlines
        1364,			// Scanline total cycles(15.75KHz)
        1024,			// H-Draw cycles
        340,			// H-Blank cycles
        4,				// End cycles
        1364 * 262,		// Frame cycles
        29830,			// FrameIRQ cycles
        60,				// Frame rate(Be originally 59.94Hz)
        1000.0f / 60.0f	// Frame period(ms)
};


NESCONFIG * NesCfg = &NESCONFIG_NTSC;

BOOL bIsRunning = false;






MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::paintEvent(QPaintEvent *)
{
    if(bIsRunning)
    {
    /*QImage image(ScreenBit,256,240,QImage::Format_Indexed8);

    image.setColorTable(colorTable);*/

    this->setAttribute(Qt::WA_PaintOutsidePaintEvent);
    QPainter painter(this);

    QRect rec;
    rec.setX(0);
    rec.setY(0);
    rec.setWidth(256*2);
    rec.setHeight(240*2);

    //drawi = drawi.mirrored(0,1);


    painter.drawImage(rec,drawi,drawi.rect());
    }

}






void NES_Init()
{
        for (int i = 0; i < 256; i++)
        {
                BYTE c = 0;
                BYTE mask = 0x80;
                for (int j = 0; j < 8; j++)
                {
                        if (i & (1 << j))
                                c |= (mask >> j);
                }
                RevertByte[i] = c;
        }
}

//void MainWindow::LoadRomFile()

void MainWindow::fileOpen()
{
    QString fileName =QFileDialog::getOpenFileName(this, tr("open files"),"/home/rs/Desktop", tr("files(*.NES *.nes)"));
    strFileName=fileName.toLatin1().data();
    printf(strFileName);
}

void* LoadRomFile(void *arg)
{
    FILE* pRomFile = NULL;
    while(pRomFile==NULL){
   // MainWindow::fileOpen();
        printf(strFileName);
    pRomFile = fopen(strFileName,"r");
    }
    ::memset(CPU_MEM_BANK, 0, sizeof(CPU_MEM_BANK));
    ::memset(PPU_MEM_BANK, 0, sizeof(PPU_MEM_BANK));
    ::memset(CRAM, 0, sizeof(CRAM));
    ::memset(VRAM, 0, sizeof(VRAM));
    fread(RomHeader, 16, 1, pRomFile);
    if (RomHeader[6] & 0x04)    // 看 D2 是否有一个512字节的trainer
            {
                    fseek( pRomFile, 512, 1);
                    //　int fseek(FILE 1*stream, long offset, int fromwhere);
                    //	重定位文件内部指针
            }
    if (RomHeader[6] & 0x04)    // 看 D2 是否有一个512字节的trainer
            {
                    fseek( pRomFile, 512, 1);
                    //　int fseek(FILE *stream, long offset, int fromwhere);
                    //	重定位文件内部指针
            }

            if (RomHeader[4] > 0)
            {
                    PRGBlock = new BYTE[RomHeader[4] * PRG_BLOCK_SIZE];   //k*0X4000    16K = 0X4000位
                    fread(PRGBlock, PRG_BLOCK_SIZE, RomHeader[4], pRomFile);
            }
            if (RomHeader[5] > 0)
            {
                    PatternTable = new BYTE[RomHeader[5] * PAT_BLOCK_SIZE];
                    fread(PatternTable, PAT_BLOCK_SIZE, RomHeader[5], pRomFile);
            }
            fclose(pRomFile);
            PROM_8K_SIZE  = RomHeader[4] * 2;

                    SetCRAM_8K_Bank();
                    if (RomHeader[6] & 0x2)
                            CPU_MEM_BANK[3] = SRAM;			//SRAM: 存放游戏档案

                    if (RomHeader[6] & 0x01)	//垂直镜像   //Name表和Attribute表的镜像关系
                            SetNameTable_Bank(0, 1, 0, 1);		//N和A的#0和#2使用前400字节空间，#1和#3使用后400字节空间
                    else						//水平镜像
                            SetNameTable_Bank(0, 0, 1, 1);

                    int MapperNo = RomHeader[6] >> 4;   //保存ROM MAPPER信息，不同卡带ROM MAPPER信息不一� �。

                    CreateMapper(MapperNo);
                    NES_Mapper.Reset();


    QRgb qrgb[64] = {
                    qRgb(0x7F, 0x7F, 0x7F), qRgb(0x20, 0x00, 0xB0), qRgb(0x28, 0x00, 0xB8), qRgb(0x60, 0x10, 0xA0),
                    qRgb(0x98, 0x20, 0x78), qRgb(0xB0, 0x10, 0x30), qRgb(0xA0, 0x30, 0x00), qRgb(0x78, 0x40, 0x00),
                    qRgb(0x48, 0x58, 0x00), qRgb(0x38, 0x68, 0x00), qRgb(0x38, 0x6C, 0x00), qRgb(0x30, 0x60, 0x40),
                    qRgb(0x30, 0x50, 0x80), qRgb(0x00, 0x00, 0x00), qRgb(0x00, 0x00, 0x00), qRgb(0x00, 0x00, 0x00),

                    qRgb(0xBC, 0xBC, 0xBC), qRgb(0x40, 0x60, 0xF8), qRgb(0x40, 0x40, 0xFF), qRgb(0x90, 0x40, 0xF0),
                    qRgb(0xD8, 0x40, 0xC0), qRgb(0xD8, 0x40, 0x60), qRgb(0xE0, 0x50, 0x00), qRgb(0xC0, 0x70, 0x00),
                    qRgb(0x88, 0x88, 0x00), qRgb(0x50, 0xA0, 0x00), qRgb(0x48, 0xA8, 0x10), qRgb(0x48, 0xA0, 0x68),
                    qRgb(0x40, 0x90, 0xC0), qRgb(0x00, 0x00, 0x00), qRgb(0x00, 0x00, 0x00), qRgb(0x00, 0x00, 0x00),

                    qRgb(0xFF, 0xFF, 0xFF), qRgb(0x60, 0xA0, 0xFF), qRgb(0x50, 0x80, 0xFF), qRgb(0xA0, 0x70, 0xFF),
                    qRgb(0xF0, 0x60, 0xFF), qRgb(0xFF, 0x60, 0xB0), qRgb(0xFF, 0x78, 0x30), qRgb(0xFF, 0xA0, 0x00),
                    qRgb(0xE8, 0xD0, 0x20), qRgb(0x98, 0xE8, 0x00), qRgb(0x70, 0xF0, 0x40), qRgb(0x70, 0xE0, 0x90),
                    qRgb(0x60, 0xD0, 0xE0), qRgb(0x60, 0x60, 0x60), qRgb(0x00, 0x00, 0x00), qRgb(0x00, 0x00, 0x00),

                    qRgb(0xFF, 0xFF, 0xFF), qRgb(0x90, 0xD0, 0xFF), qRgb(0xA0, 0xB8, 0xFF), qRgb(0xC0, 0xB0, 0xFF),
                    qRgb(0xE0, 0xB0, 0xFF), qRgb(0xFF, 0xB8, 0xE8), qRgb(0xFF, 0xC8, 0xB8), qRgb(0xFF, 0xD8, 0xA0),
                    qRgb(0xFF, 0xF0, 0x90), qRgb(0xC8, 0xF0, 0x80), qRgb(0xA0, 0xF0, 0xA0), qRgb(0xA0, 0xFF, 0xC8),
                    qRgb(0xA0, 0xFF, 0xF0), qRgb(0xA0, 0xA0, 0xA0), qRgb(0x00, 0x00, 0x00), qRgb(0x00, 0x00, 0x00)
                  };

                    int j;
                    for(j = 0 ; j < 64 ; j++)
                    {
                        colorTable.push_back(qrgb[j%64]);
                        }
                bIsRunning = true;
                NES_Start();
                return((void*)0);

}


void NES_ThreadProc()
{
        long FrameStartTime;
        long SleepTime;
        double FramePeriod = 1000.0 / NesCfg->FrameRate;
        struct timeval thetime;
        while (1)
        {
                gettimeofday(&thetime,NULL);
                FrameStartTime = (long)(thetime.tv_usec / 1000);

                NES_FrameExec();
                gettimeofday(&thetime,NULL);
                SleepTime = (long)FramePeriod - ((long)(thetime.tv_usec / 1000) - FrameStartTime);
                if (SleepTime > 0)
                      sleep((SleepTime - 1)/10000);
                else
                    sleep(0);

        }
}
void MainWindow::keyPressEvent(QKeyEvent *k)
{
    switch(k->key())
      {
        case Qt::Key_1:
        SetState(JOY_PAD_1, JOY_PAD_START, 1);
        break;
      case Qt::Key_2:
              SetState(JOY_PAD_1, JOY_PAD_SELECT, 1);
              break;
      case Qt::Key_W:
          SetState(JOY_PAD_1, JOY_PAD_UP, 1);
          break;
      case Qt::Key_D:
          SetState(JOY_PAD_1, JOY_PAD_RIGHT, 1);
          break;
      case Qt::Key_S:
          SetState(JOY_PAD_1, JOY_PAD_DOWN, 1);
          break;
      case Qt::Key_A:
          SetState(JOY_PAD_1, JOY_PAD_LEFT, 1);
          break;
      case Qt::Key_J:
          SetState(JOY_PAD_1, JOY_PAD_B, 1);
          break;
      case Qt::Key_K:
          SetState(JOY_PAD_1, JOY_PAD_A, 1);
          break;
      }
}
void MainWindow::keyReleaseEvent(QKeyEvent *k)
{
  switch(k->key())
    {
  case Qt::Key_1:
  SetState(JOY_PAD_1, JOY_PAD_START, 0);
  break;
case Qt::Key_2:
        SetState(JOY_PAD_1, JOY_PAD_SELECT, 0);
        break;
case Qt::Key_W:
    SetState(JOY_PAD_1, JOY_PAD_UP, 0);
    break;
case Qt::Key_D:
    SetState(JOY_PAD_1, JOY_PAD_RIGHT, 0);
    break;
case Qt::Key_S:
    SetState(JOY_PAD_1, JOY_PAD_DOWN, 0);
    break;
case Qt::Key_A:
    SetState(JOY_PAD_1, JOY_PAD_LEFT, 0);
    break;
case Qt::Key_J:
    SetState(JOY_PAD_1, JOY_PAD_B, 0);
    break;
case Qt::Key_K:
    SetState(JOY_PAD_1, JOY_PAD_A, 0);
    break;
}


}

DWORD NES_GetPROM_CRC()
{
    return Nes_Crc;
}
void NES_SetRenderMethod(RENDERMETHOD method)
{
    Nes_RenderMethod = method;
}


void NES_FrameExec()
{

        int i;
        PPU.RanderBottomBG(ScreenBit);

        CPU.ExecOnBaseCycle(NesCfg->HDrawCycles);
        CPU.ExecOnBaseCycle(FETCH_CYCLES * 32);
        PPU.ScanLine(ScreenBit, 0);
        PPU.ScanlineStart();
        CPU.ExecOnBaseCycle(FETCH_CYCLES * 10 + NesCfg->ScanlineEndCycles);

        for (i = 1; i < 240; i++)
                {
                        PPU.ScanLine(ScreenBit, i);
                        CPU.ExecOnBaseCycle(NesCfg->HDrawCycles);
                        CPU.ExecOnBaseCycle(FETCH_CYCLES * 32);
                        PPU.ScanlineStart();
                        CPU.ExecOnBaseCycle(FETCH_CYCLES * 10 + NesCfg->ScanlineEndCycles);
                }

        QImage image(ScreenBit,256,240,QImage::Format_Indexed8);

        image.setColorTable(colorTable);
        drawi = image.mirrored(0,1);
        for(int iii = 0; iii < 100000 ; iii ++)
        {
            for(int jjj = 0 ; jjj < 31 ; jjj ++)
        iii = iii;
        }


        CPU.ExecOnBaseCycle(NesCfg->HDrawCycles);
        CPU.ExecOnBaseCycle(NesCfg->HBlankCycles);

        // Scanline 241
        PPU.VBlankStart();
        if (PPU.m_REG[0] & PPU_VBLANK_BIT )
        CPU.NMI();
        CPU.ExecOnBaseCycle(NesCfg->HDrawCycles);
        CPU.ExecOnBaseCycle(NesCfg->HBlankCycles);


        for (i = 242; i < NesCfg->TotalScanlines - 1; i++)
            {
                 CPU.ExecOnBaseCycle(NesCfg->HDrawCycles);
                 CPU.ExecOnBaseCycle(NesCfg->HBlankCycles);
            }

                // Scanline NesCfg->TotalScanlines-1
        PPU.VBlankEnd();
        CPU.ExecOnBaseCycle(NesCfg->HDrawCycles);
        CPU.ExecOnBaseCycle(NesCfg->HBlankCycles);
}

void MainWindow::on_RE_clicked()
{
    //NES_Init();
    pthread_t ntid;
    MainWindow::fileOpen();
    int wwww = pthread_create(&ntid,NULL,LoadRomFile,NULL);

    //LoadRomFile();

}


void NES_Start()
{

        ::memset(RAM, 0, sizeof(RAM));
        ::memset(SRAM, 0, sizeof(SRAM));
        ::memset(NameTable, 0, sizeof(NameTable));
        NES_Init();
        CPU.Reset();
        PPU.Reset();


        NES_ThreadProc();
        bIsRunning = true;
}




void SetPROM_8K_Bank(int page, int bank)
{
        bank %= PROM_8K_SIZE;
        CPU_MEM_BANK[page] = PRGBlock + 0x2000 * bank;
}

void SetPROM_16K_Bank(int page, int bank)
{
        SetPROM_8K_Bank(page + 0, bank * 2 + 0);
        SetPROM_8K_Bank(page + 1, bank * 2 + 1);
}
void SetPROM_32K_Bank(int bank)
{
    SetPROM_8K_Bank(4, bank * 4 + 0);
    SetPROM_8K_Bank(5, bank * 4 + 1);
    SetPROM_8K_Bank(6, bank * 4 + 2);
    SetPROM_8K_Bank(7, bank * 4 + 3);
}
void SetPROM_32K_Bank(int bank0, int bank1, int bank2, int bank3)
{
        SetPROM_8K_Bank(4, bank0);
        SetPROM_8K_Bank(5, bank1);
        SetPROM_8K_Bank(6, bank2);
        SetPROM_8K_Bank(7, bank3);
}
void SetVROM_1K_Bank(BYTE page, int bank)
{
    bank %= VROM_1K_SIZE;
    PPU_MEM_BANK[page] = PatternTable + 0x0400 * bank;
}

void SetVROM_2K_Bank(BYTE page, int bank)
{
    SetVROM_1K_Bank(page + 0, bank * 2 + 0);
    SetVROM_1K_Bank(page + 1, bank * 2 + 1);
}

void SetVROM_4K_Bank(BYTE page, int bank)
{
    SetVROM_1K_Bank(page + 0, bank * 4 + 0);
    SetVROM_1K_Bank(page + 1, bank * 4 + 1);
    SetVROM_1K_Bank(page + 2, bank * 4 + 2);
    SetVROM_1K_Bank(page + 3, bank * 4 + 3);
}
void SetVROM_8K_Bank(int bank)
{
    for(int i = 0; i < 8; i++)
        SetVROM_1K_Bank(i, bank * 8 + i);
}


void SetVROM_8K_Bank(int bank0, int bank1, int bank2, int bank3,
                     int bank4, int bank5, int bank6, int bank7)
{
    SetVROM_1K_Bank(0, bank0);
    SetVROM_1K_Bank(1, bank1);
    SetVROM_1K_Bank(2, bank2);
    SetVROM_1K_Bank(3, bank3);
    SetVROM_1K_Bank(4, bank4);
    SetVROM_1K_Bank(5, bank5);
    SetVROM_1K_Bank(6, bank6);
    SetVROM_1K_Bank(7, bank7);
}

void SetCRAM_1K_Bank(int page, int bank)
{

        PPU_MEM_BANK[page] = CRAM + 0x0400 * bank;      //0x400 = 1K , 将PPU的前8K与CRAM前8K对应起来 CRAM用来申请内存 32K
}

void SetCRAM_8K_Bank()
{
        for (int i = 0; i < 8; i++)
                SetCRAM_1K_Bank(i, i);
}

void SetVRAM_1K_Bank(int page, int bank)
{

        PPU_MEM_BANK[page] = VRAM + 0x0400 * bank;
}


void SetNameTable_Bank(int bank0, int bank1, int bank2, int bank3)
{
        SetVRAM_1K_Bank( 8, bank0);
        SetVRAM_1K_Bank( 9, bank1);
        SetVRAM_1K_Bank(10, bank2);
        SetVRAM_1K_Bank(11, bank3);
}

BYTE NES_ReadReg(WORD addr)
{
        switch (addr & 0xFF)
        {
        case	0x14:								//� 送精灵属性
                return addr & 0xFF;
                break;
        case	0x16:
                return (GetValue(0) | 0x40);		//游戏手柄输入
                break;
        case	0x17:
                return GetValue(1) ;
                break;
        }
}
BYTE NES_Read(WORD addr)
{
        switch ( addr>>13 )
        {
        case 0x00 :	// $0000-$1FFF
                return RAM[addr & 0x07FF]; //0000 0111 1111 1111
        case 0x01 :	// $2000-$3FFF
                return	PPU.ReadFromPort(addr & 0xE007); //PPU内容
        case 0x02 :	// $4000-$5FFF
                if (addr < 0x4100)
                        return	NES_ReadReg(addr);		//其他内容，分别处理
                else
                        return	NES_Mapper.ReadLow(addr);
                break;
        case 0x03 :	// $6000-$7FFF
                return	NES_Mapper.ReadLow(addr); //存档
        case 0x04 :	// $8000-$9FFF
        case 0x05 :	// $A000-$BFFF
        case 0x06 :	// $C000-$DFFF
        case 0x07 :	// $E000-$FFFF
                return	CPU_MEM_BANK[addr >> 13][addr & 0x1FFF];
        }

        return	0x00;	// Warning
}
void NES_WriteReg(WORD addr, BYTE val)
{
        /* 0x4000 - 0x40FF */
        switch ( addr & 0xFF )
        {
        case 0x00: case 0x01: case 0x02: case 0x03:
        case 0x04: case 0x05: case 0x06: case 0x07:
        case 0x08: case 0x09: case 0x0A: case 0x0B:
        case 0x0C: case 0x0D: case 0x0E: case 0x0F:
        case 0x10: case 0x11: case 0x12: case 0x13:
        case 0x15 :
                break;
        case 0x14 :
                ::memcpy(Sprite, CPU.GetRAM(((WORD)val) << 8), 256);
                CPU.m_DMACycle += 514;
                break;
        case 0x16 :
                NES_Mapper.ExWrite(addr, val);	// For VS-Unisystem
                InputBursh(val);
                break;
        case 0x17 :
                break;
                // VirtuaNES屌桳億乕僩
        case 0x18 :
                break;
        default:
                NES_Mapper.ExWrite(addr, val);
                break;
        }
}
void NES_Write(WORD addr, BYTE val)
{
        switch (addr >> 13)
        {
        case 0x00 :	// $0000-$1FFF
                RAM[addr & 0x07FF] = val;
                break;
        case 0x01 :	// $2000-$3FFF
                PPU.WriteToPort(addr & 0xE007, val);
                break;
        case 0x02 :	// $4000-$5FFF
                if (addr < 0x4100)
                        NES_WriteReg(addr, val);
                else
                        NES_Mapper.WriteLow(addr, val);
                break;
        case 0x03 :	// $6000-$7FFF
                NES_Mapper.WriteLow(addr, val);
                break;
        case 0x04 :	// $8000-$9FFF
        case 0x05 :	// $A000-$BFFF
        case 0x06 :	// $C000-$DFFF
        case 0x07 :	// $E000-$FFFF
                NES_Mapper.Write(addr, val);
                break;
        }
}
BYTE * NES_GetRAM(WORD addr)
{
        switch ( addr>>13 )
        {
        case 0x00 :	// $0000-$1FFF
                return &RAM[addr & 0x07FF];
        case 0x04 :	// $8000-$9FFF
        case 0x05 :	// $A000-$BFFF
        case 0x06 :	// $C000-$DFFF
        case 0x07 :	// $E000-$FFFF
                return	&CPU_MEM_BANK[addr >> 13][addr & 0x1FFF];
        }
        //ASSERT(FALSE);
        return NULL;
}







void MainWindow::on_choose1_clicked()
{
    SetState(JOY_PAD_1, JOY_PAD_START, 0);
    SetState(JOY_PAD_1, JOY_PAD_START, 1);
}

void MainWindow::on_choose1_clicked(bool checked)
{
    SetState(JOY_PAD_1, JOY_PAD_START, 1);
}





