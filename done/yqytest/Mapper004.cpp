/*
#include "stdafx.h"
#include "NES.h"
#include "PPU.h"
#include "Mapper.h"
#include "Mapper004.h"

int nPRGIndex[2];
int nCHRIndex[8];

int nIRQLatch     = 0xFF;
int nIRQCounter   = 0;
int nIRQEnable    = 0;
int nIRQPreset    = 0;
int nIRQPresetVbl = 0;

void Mapper004_SetCPUBank()
{
    if (NES_Mapper.reg[0] & 0x40)
        SetPROM_32K_Bank(PROM_8K_SIZE - 2, nPRGIndex[1], nPRGIndex[0], PROM_8K_SIZE - 1);
    else
        SetPROM_32K_Bank(nPRGIndex[0], nPRGIndex[1], PROM_8K_SIZE - 2, PROM_8K_SIZE - 1);
}

void Mapper004_SetPPUBank()
{
    if (VROM_1K_SIZE)
    {
        if (NES_Mapper.reg[0] & 0x80)
            SetVROM_8K_Bank(nCHRIndex[4], nCHRIndex[5], nCHRIndex[6], nCHRIndex[7],
            nCHRIndex[0], nCHRIndex[1], nCHRIndex[2], nCHRIndex[3]);
        else
            SetVROM_8K_Bank(nCHRIndex[0], nCHRIndex[1], nCHRIndex[2], nCHRIndex[3],
            nCHRIndex[4], nCHRIndex[5], nCHRIndex[6], nCHRIndex[7]);
    }
    else
    {
        if (NES_Mapper.reg[0] & 0x80)
        {
            SetCRAM_1K_Bank(4, nCHRIndex[0] & 0x07);
            SetCRAM_1K_Bank(5, nCHRIndex[1] & 0x07);
            SetCRAM_1K_Bank(6, nCHRIndex[2] & 0x07);
            SetCRAM_1K_Bank(7, nCHRIndex[3] & 0x07);
            SetCRAM_1K_Bank(0, nCHRIndex[4] & 0x07);
            SetCRAM_1K_Bank(1, nCHRIndex[5] & 0x07);
            SetCRAM_1K_Bank(2, nCHRIndex[6] & 0x07);
            SetCRAM_1K_Bank(3, nCHRIndex[7] & 0x07);
        }
        else
        {
            SetCRAM_1K_Bank(0, nCHRIndex[0] & 0x07);
            SetCRAM_1K_Bank(1, nCHRIndex[1] & 0x07);
            SetCRAM_1K_Bank(2, nCHRIndex[2] & 0x07);
            SetCRAM_1K_Bank(3, nCHRIndex[3] & 0x07);
            SetCRAM_1K_Bank(4, nCHRIndex[4] & 0x07);
            SetCRAM_1K_Bank(5, nCHRIndex[5] & 0x07);
            SetCRAM_1K_Bank(6, nCHRIndex[6] & 0x07);
            SetCRAM_1K_Bank(7, nCHRIndex[7] & 0x07);
        }
    }
}

void Mapper004_Reset()
{
    for (int i = 0; i < 8; i++)
        NES_Mapper.reg[i] = 0x00;

    nPRGIndex[0] = 0;
    nPRGIndex[1] = 1;
    Mapper004_SetCPUBank();

    for (int i = 0; i < 8; i++)
        nCHRIndex[i] = i;
    Mapper004_SetPPUBank();

    nIRQCounter = 0;
}

void Mapper004_Write(WORD addr, BYTE data)
{
    switch (addr & 0xE001)
    {
    case 0x8000 :
        NES_Mapper.reg[0] = data;
        Mapper004_SetCPUBank();
        Mapper004_SetPPUBank();
        break;
    case 0x8001 :
        NES_Mapper.reg[1] = data;
        switch (NES_Mapper.reg[0] & 0x07)
        {
        case 0 :
            nCHRIndex[0] = data & 0xFE;
            nCHRIndex[1] = nCHRIndex[0] + 1;
            Mapper004_SetPPUBank();
            break;
        case 1 :
            nCHRIndex[2] = data & 0xFE;
            nCHRIndex[3] = nCHRIndex[2] + 1;
            Mapper004_SetPPUBank();
            break;
        case 2 :
            nCHRIndex[4] = data;
            Mapper004_SetPPUBank();
            break;
        case 3 :
            nCHRIndex[5] = data;
            Mapper004_SetPPUBank();
            break;
        case 4 :
            nCHRIndex[6] = data;
            Mapper004_SetPPUBank();
            break;
        case 5 :
            nCHRIndex[7] = data;
            Mapper004_SetPPUBank();
            break;
        case 6 :
            nPRGIndex[0] = data;
            Mapper004_SetCPUBank();
            break;
        case 7 :
            nPRGIndex[1] = data;
            Mapper004_SetCPUBank();
            break;
        }
        break;
    case 0xA000 :
        NES_Mapper.reg[2] = data;
        if(data & 0x01)
            SetNameTable_Bank(0, 0, 1, 1);
        else
            SetNameTable_Bank(0, 1, 0, 1);
        break;
    case 0xA001:
        NES_Mapper.reg[3] = data;
        break;
    case 0xC000 :
        NES_Mapper.reg[4] = data;
        nIRQLatch = data;
        break;
    case 0xC001:
        NES_Mapper.reg[5] = data;
        if ((Nes_GetScanline() < 240))
        {
            nIRQCounter |= 0x80;
            nIRQPreset = 0xFF;
        }
        else
        {
            nIRQCounter |= 0x80;
            nIRQPresetVbl = 0xFF;
            nIRQPreset = 0;
        }
        break;
    case 0xE000 :
        NES_Mapper.reg[6] = data;
        nIRQEnable = 0;
        break;
    case 0xE001 :
        NES_Mapper.reg[7] = data;
        nIRQEnable = 1;
        break;
    }
}

void Mapper004_HSync(int nScanline)
{
    BOOL bIsDispOn = PPU.m_REG[1] & (PPU_SHOWBG | PPU_SHOWSPR);
    if ((nScanline >= 0 && nScanline <= 239) && bIsDispOn)
    {
        if (nIRQPresetVbl)
        {
            nIRQCounter = nIRQLatch;
            nIRQPresetVbl = 0;
        }
        if (nIRQPreset)
        {
            nIRQCounter = nIRQLatch;
            nIRQPreset = 0;
        }
        else if (nIRQCounter > 0)
        {
            nIRQCounter--;
        }

        if (nIRQCounter == 0)
        {
            if (nIRQEnable)
            {
                nIRQPreset = 0xFF;
            }
            nIRQPreset = 0xFF;
        }
    }
}

void Mapper004_Init()
{
    NES_Mapper.Reset = Mapper004_Reset;
    NES_Mapper.Write = Mapper004_Write;
    NES_Mapper.HSync = Mapper004_HSync;
}

*/
