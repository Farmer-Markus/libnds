#include "../NDS_logging.h"
#include "../NDS_memory.h"
#include "NDS_rom.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>



NDS_Rom* NDS_RomOpen(char *path)
{
    // Open file streams, allocate memory etc.
    FILE* romFile = fopen(path, "r");
    if(!romFile)
    {
        NDS_SetError("Failed to open rom path");
        return NULL;
    }

    NDS_Rom *rom = NDS_Malloc(sizeof(NDS_Rom));
    if(!rom) return NULL;

    rom->acc.r_fstream = romFile;
    fseek(romFile, 0, SEEK_SET);

    if(NDS_RomRead(&rom->r_header, 0, sizeof(NDS_RomHeader), &rom->acc) != sizeof(NDS_RomHeader))
    {
        NDS_SetError("Failed to parse rom header. Could not read full rom header");
        return NULL;
    }

    return rom;
}

int NDS_RomClose(NDS_Rom *romptr)
{
    NDS_CHECK_PTR_RINT(romptr);
    int8_t res = 0;
    // Close file streams, delete pointer etc.
    if(fclose(romptr->acc.r_fstream) != 0)
    {
        res = -1;
        NDS_SetError("Failed to close rom file stream");
    }

    free(romptr);
    
    return res;
}

int NDS_RomRead(void *restrict dest, uint32_t offset, size_t n, NDS_RomAccess *acc)
{
    NDS_CHECK_PTR_RINT(acc);
    // Todo: Implement mutex, thread safety !!!

    if(fseek(acc->r_fstream, offset, SEEK_SET) != 0)
    {
        NDS_SetError("Failed to read data from rom. fseek failed");
        return -1;
    }
    // Read bytes
    int res = fread(dest, 1, n, acc->r_fstream);
    if(res != n)
    {
        NDS_SetError("Failed to read data from rom. Not all requested bytes read");
        return -1;
    }
    return 0;
}

const NDS_RomHeader* NDS_RomGetHeader(const NDS_Rom *romptr)
{
    NDS_CHECK_PTR(romptr);
    return &romptr->r_header;
}
