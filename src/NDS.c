#include "NDS.h"
#include "filesystem/NDS_filesystem.h"
#include "filesystem/NDS_rom.h"
#include "utils/NDS_endian.h"
#include <NDS_logging.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



int NDS_Quit()
{
    int res = 0;




    return res;
}


int main()
{
    NDS_Rom* rom = NDS_OpenRom("/home/markus/Projekte/libnds/build/DsRom.nds");
    if(!rom)
        return -1;

    /*NDS_FileInfo file;
    file.fntOffset = 0x327ead;
    file.rom = rom;

    char name[100];
    int nameLen = NDS_FileGetName(name, file);

    printf("%i %s\n", nameLen, name);*/
    NDS_FileInfo* file;

    for(int i = 0; i < 1000000; i++)
    {
        if(!(file = NDS_GetFile("/Event/Kamishibai/op/OP_05.ntft", rom)))
        {
            printf("%s\n", NDS_GetError());
            return 1;
        }

        char test[100];
        if(NDS_FileGetName(test, file) == 0)
        {
            printf("%s\n", NDS_GetError());
        }

        printf("NAME: %s\n", test);
        free(file);
    }

    sleep(10);
    
    //const NDS_RomHeader* header = NDS_GetRomHeader(rom);
    //printf("FNT: %i\n", header->offsetFnt);


    //FILE* f = fopen("/home/markus/Projekte/libnds/build/out.bmp", "w");
    //fwrite(&header->ic, sizeof(header->nintendoLogo), 1, f);

    NDS_CloseRom(rom);

    return 0;
}