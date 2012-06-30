/*#pragma once

#ifndef MAPPER003_H
#define MAPPER003_H
void Mapper003_Reset()
{
    switch (PROM_16K_SIZE)
    {
        case	1:	// 16K only
            SetPROM_16K_Bank( 4, 0 );
            SetPROM_16K_Bank( 6, 0 );
            break;
        case	2:	// 32K
            SetPROM_32K_Bank( 0,0,0,0 );
            break;
    }

    DWORD crc = NES_GetPROM_CRC();

    if (crc == 0x2b72fe7e)
    {	// Ganso Saiyuuki - Super Monkey Dai Bouken(J)
        NES_SetRenderMethod(TILE_RENDER);
    }
}

void Mapper003_Write(WORD addr, BYTE data)
{
    SetVROM_8K_Bank(data);
}

void Mapper003_Init()
{
    NES_Mapper.Reset = Mapper003_Reset;
    NES_Mapper.Write = Mapper003_Write;
}
#endif // MAPPER003_H
*/
