#include "NDS.h"
#include "filesystem/NDS_filesystem.h"
#include "filesystem/NDS_rom.h"
#include "utils/NDS_endian.h"
#include "NDS_logging.h"
#include <linux/limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>



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


    for(int i = 0; i < 1; i++)
    {
        NDS_Dir *dir;
        if(!(dir = NDS_Opendir("sa", rom)))
        {
            printf("%s\n", NDS_GetError());
            return 1;
        }


        NDS_Dirent *entry;
        while((entry = NDS_Readdir(dir)) != NULL)
        {
            printf("%s\n", entry->de_name);
        }

        NDS_Closedir(dir);
    }
    NDS_CloseRom(rom);




    // NDS_Seekdir(dir, -2, NDS_SEEK_CUR);
    // entry = NDS_Readdir(dir);
    //printf("Nochmal: %s\n", entry->de_name);



    


    // struct dirent *de, *de2, *de3, *de4;
    // DIR* dir = opendir(".");
    // //while((de = readdir(dir)) != NULL)
    //   //  printf("%s\n", de->d_name);

    // de = readdir(dir);
    // de2 = readdir(dir);
    // de3 = readdir(dir);
    // de4 = readdir(dir);

    // closedir(dir);
    // // dir = opendir("../src");
    // // readdir(dir);


    // printf("%s\n", de->d_name);
    // printf("%s\n", de2->d_name);
    // printf("%s\n", de3->d_name);
    // printf("%s\n", de4->d_name);



    


 
    // NDS_CloseRom(rom);

    return 0;
}