/*
#include "stdafx.h"
#include "NES.h"
#include "Mapper.h"
#include "Mapper001.h"

int nShift;
WORD wLastAddr;
BYTE RegBuf;

void Mapper001_Reset()
{
    NES_Mapper.reg[0] = 0x0C;
    NES_Mapper.reg[1] = 0;
    NES_Mapper.reg[2] = 0;
    NES_Mapper.reg[3] = 0;

    nShift = 0;
    RegBuf = 0;

    if(PROM_16K_SIZE < 32)
        SetPROM_32K_Bank(0, 1, PROM_8K_SIZE - 2, PROM_8K_SIZE - 1);
    else
    {
        //512K1M
        SetPROM_16K_Bank(4, 0);
        SetPROM_16K_Bank(6, 16 - 1);
    }

    DWORD	crc = NES_GetPROM_CRC();

    if( crc == 0xc96c6f04 ) {	// Venus Senki(J)
        NES_SetRenderMethod(POST_ALL_RENDER);
    }

    if( crc == 0x4d2edf70 ) {	// Night Rider(J)
        NES_SetRenderMethod(TILE_RENDER );
    }
    if( crc == 0xcd2a73f0 ) {	// Pirates!(U)
        NES_SetRenderMethod(TILE_RENDER );
    }

    if( crc == 0xd878ebf5 ) {	// Ninja Ryukenden(J)
        NES_SetRenderMethod( POST_ALL_RENDER );
    }
    if( crc == 0x717e1169 ) {	// Cosmic Wars(J)
        NES_SetRenderMethod(PRE_ALL_RENDER );
    }
}

void Mapper001_Write(WORD addr, BYTE data)
{
    int nRegIdx = (addr >> 13) & 0x3;

    if (data & 0x80)
    {
        NES_Mapper.reg[nRegIdx] |= 0x0C;
        return;
    }

    if (data & 0x1)
        RegBuf |= (1 << nShift);
    if (++nShift < 5)
        return;

    NES_Mapper.reg[nRegIdx]= RegBuf;
    RegBuf = 0;
    nShift = 0;

    switch (nRegIdx)
    {
    case 0 :
        if (NES_Mapper.reg[0] & 0x02)
        {
            if (NES_Mapper.reg[0] & 0x01)
                SetNameTable_Bank(0, 1, 0, 1);
            else
                SetNameTable_Bank(0, 0, 1, 1);
        }
        else
        {
            SetNameTable_Bank(0, 0, 0, 0);
        }
        break;
    case 1 :
        if (VROM_1K_SIZE)
        {
            if (NES_Mapper.reg[0] & 0x10)
            {
                // CHR 4K bank lower($0000-$0FFF)
                SetVROM_4K_Bank(0, NES_Mapper.reg[1]);
                // CHR 4K bank higher($1000-$1FFF)
                SetVROM_4K_Bank(4, NES_Mapper.reg[2]);
            }
            else
            {
                // CHR 8K bank($0000-$1FFF)
                SetVROM_8K_Bank(NES_Mapper.reg[1] >> 1);
            }
        }
        else
        {
            // For Romancia
            if (NES_Mapper.reg[0] & 0x10)
            {
                SetCRAM_4K_Bank(0, NES_Mapper.reg[1]);
            }
        }
        break;
    case 2 :
        // Register #2
        if (VROM_1K_SIZE)
        {
            if (NES_Mapper.reg[0] & 0x10)
            {
                // CHR 4K bank lower($0000-$0FFF)
                SetVROM_4K_Bank(0, NES_Mapper.reg[1]);
                // CHR 4K bank higher($1000-$1FFF)
                SetVROM_4K_Bank(4, NES_Mapper.reg[2]);
            }
            else
            {
                // CHR 8K bank($0000-$1FFF)
                SetVROM_8K_Bank(NES_Mapper.reg[1] >> 1);
            }
        }
        else
        {
            // For Romancia
            if (NES_Mapper.reg[0] & 0x10)
            {
                SetCRAM_4K_Bank(4, NES_Mapper.reg[2]);
            }
        }
        break;
    case 3 :
        if (NES_Mapper.reg[0] & 0x8)
        {
            if (NES_Mapper.reg[0] & 0x4)
                SetPROM_16K_Bank(4, (NES_Mapper.reg[3] & 0xF));
            else
                SetPROM_16K_Bank(6, (NES_Mapper.reg[3] & 0xF));
        }
        else
            SetPROM_32K_Bank((NES_Mapper.reg[3] >> 1) & 0x7 );
        break;
    }
}

void Mapper001_Init()
{
    NES_Mapper.Reset = Mapper001_Reset;
    NES_Mapper.Write = Mapper001_Write;
}
*/
