/*
#include "stdafx.h"
#include "NES.h"
#include "Mapper.h"
#include "Mapper007.h"

int patch = 0;

void Mapper007_Reset()
{
    patch = 0;

    SetPROM_32K_Bank(0);

    DWORD crc = NES_GetPROM_CRC();
    if (crc == 0x3c9fe649)
    {	// WWF Wrestlemania Challenge(U)
        SetNameTable_Bank(0, 1, 0, 1);
        patch = 1;
    }
    if (crc == 0x09874777)
    {	// Marble Madness(U)
        NES_SetRenderMethod(TILE_RENDER);
    }
}

void Mapper007_Write(WORD addr, BYTE data)
{
    SetPROM_32K_Bank(data & 0x0F);

    if (!patch)
    {
        if (data & 0x10)
            SetNameTable_Bank(1, 1, 1, 1);
        else
            SetNameTable_Bank(0, 0, 0, 0);
    }
}

void Mapper007_Init()
{
    NES_Mapper.Reset = Mapper007_Reset;
    NES_Mapper.Write = Mapper007_Write;
}

*/
