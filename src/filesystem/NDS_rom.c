#include "../NDS_logging.h"
#include "../NDS_memory.h"
#include "NDS_rom.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>



NDS_Rom* NDS_OpenRom(char *path)
{
    // Open file streams, allocate memory etc.
    FILE* romFile = fopen(path, "r");
    if(!romFile)
    {
        NDS_SetError("Failed to open rom path");
        return NULL;
    }

    NDS_Rom *rom;
    if((rom = NDS_Malloc(sizeof(NDS_Rom))) == NULL) return NULL;

    rom->r_fstream = romFile;
    fseek(romFile, 0, SEEK_SET);

    if(fread(&rom->r_header, sizeof(NDS_RomHeader), 1, romFile) != 1)
    {
        NDS_SetError("Failed to parse rom header. Could not read full rom header");
        return NULL;
    }

    return rom;
}

int NDS_CloseRom(NDS_Rom *romptr)
{
    NDS_CHECK_PTR_RINT(romptr);
    int8_t res = 0;
    // Close file streams, delete pointer etc.
    if(fclose(romptr->r_fstream))
    {
        res = -1;
        NDS_SetError("Failed to close rom file stream");
    }

    free(romptr);
    
    return res;
}

const NDS_RomHeader* NDS_GetRomHeader(const NDS_Rom *romptr)
{
    NDS_CHECK_PTR(romptr);
    return &romptr->r_header;
}
