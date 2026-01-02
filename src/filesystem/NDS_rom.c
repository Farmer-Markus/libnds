#include "../NDS_logging.h"
#include "../NDS_memory.h"
#include "NDS_rom.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>



NDS_Rom* NDS_RomOpen(const char *path)
{
    FILE* romFile = fopen(path, "r");
    if(!romFile)
    {
        NDS_SetError("Failed to open %s", path);
        return NULL;
    }

    NDS_Rom *rom = NDS_Malloc(sizeof(NDS_Rom));
    if(!rom) return NULL;

    rom->acc.r_fstream = romFile;
    fseek(romFile, 0, SEEK_SET);

    if(NDS_RomRead(&rom->r_header, 0, sizeof(NDS_RomHeader), &rom->acc) != 0)
    {
        free(romFile);
        return NULL;
    }

    return rom;
}

int NDS_RomClose(NDS_Rom *rom)
{
    NDS_CHECK_PTR_RINT(rom);
    int8_t res = 0;
    if(fclose(rom->acc.r_fstream) != 0)
    {
        res = -1;
        NDS_SetError("Failed to close rom file stream");
    }

    free(rom);
    
    return res;
}

int NDS_RomRead(void *restrict dest, uint32_t offset, size_t n, const NDS_RomAccess *acc)
{
    NDS_CHECK_PTR_RINT(acc);
    // Todo: Implement mutex, thread safety !!!

    if(fseek(acc->r_fstream, offset, SEEK_SET) != 0)
    {
        NDS_SetError("Failed to read data from rom. fseek failed");
        return -1;
    }

    size_t res = fread(dest, 1, n, acc->r_fstream);
    if(res != n)
    {
        NDS_SetError("Failed to read data from rom. Not all requested bytes read");
        return -1;
    }
    return 0;
}

const NDS_RomHeader* NDS_RomGetHeader(const NDS_Rom *rom)
{
    NDS_CHECK_PTR(rom);
    return &rom->r_header;
}
