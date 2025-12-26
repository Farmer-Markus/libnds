#include "filesystem/NDS_rom.h"
#include <NDS_logging.h>
#include <filesystem/NDS_filesystem.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NDS_DIRVAL 0x80 // Value > 0x80 is folder in FNT
#define NDS_PATHSEPERATOR '/'


static NDS_FileInfo* NDS_SearchFilesystem(const char *path, NDS_FntFolder currDir, const NDS_Rom *rom)
{
    uint8_t nBufferSize = 32;
    char *nameBuffer = malloc(nBufferSize);
    const char *pathSep;
    pathSep = strchr(path, NDS_PATHSEPERATOR);
    // Find next seperator

    // Jump to fnt
    fseek(rom->romFileStream, rom->header.offsetFnt + currDir.fntOffset, SEEK_SET);
    uint8_t status = 0; // Status byte (name lenght, is folder)
    fread(&status, 1, 1, rom->romFileStream);
    bool isDir = false;

    while(status != 0x00) // while not at end and no errors
    {
        // Is directory
        if(status > NDS_DIRVAL)
        {
            isDir = true;
            status -= NDS_DIRVAL;

        } else
            isDir = false;

        // status becomes name lenght
        if(nBufferSize <= status) // <= because 0 string terminator
        {
            nBufferSize = status + 1;
            nameBuffer = realloc(nameBuffer, nBufferSize);
        }

        fread(nameBuffer, 1, status, rom->romFileStream);
        nameBuffer[status] = 0;
        if(pathSep == 0 && strcmp(nameBuffer, path) == 0) // End of path & name found
        {
            free(nameBuffer);
            NDS_FileInfo* file = malloc(sizeof(NDS_FileInfo));
            file->fntOffset = ftell(rom->romFileStream) - rom->header.offsetFnt - status - 1; // To current offset(based on fnt begin)
            file->rom = rom;
            return file;
        }

        // Found next folder?
        if(strncmp(nameBuffer, path, status) == 0)
        {
            if(!isDir) // Not found, path invalid
                break;

            uint16_t dirIds;
            if(fread(&dirIds, 2, 1, rom->romFileStream) != 1)
                break;

            // if(dirIds & 0xF000) // parent dir = root
            // dirIds & 0x0FFF = dir id

            fseek(rom->romFileStream, rom->header.offsetFnt + (dirIds & 0x0FFF) * sizeof(NDS_FntFolder), SEEK_SET);
            fread(&currDir, sizeof(NDS_FntFolder), 1, rom->romFileStream);

            free(nameBuffer);
            // Skip seperator
            return NDS_SearchFilesystem(pathSep + 1, currDir, rom);

        } else if(isDir)
            fseek(rom->romFileStream, 2, SEEK_CUR);
            // Skip next two bytes  (of folder entry)


        // Read next status byte
        fread(&status, 1, 1, rom->romFileStream);
    }

    free(nameBuffer);
    NDS_SetError("Failed to get File. File does not exist or filesystem is broken");

    return NULL;
}

NDS_FileInfo* NDS_GetFile(const char *path, const NDS_Rom *rom)
{
    NDS_FntFolder start;
    fseek(rom->romFileStream, rom->header.offsetFnt, SEEK_SET);
    fread(&start, sizeof(NDS_FntFolder), 1, rom->romFileStream);
    if(path[0] == NDS_PATHSEPERATOR)
        return NDS_SearchFilesystem(&path[1], start, rom);

    return NDS_SearchFilesystem(path, start, rom);
}







/*
NDS_FileInfo* NDS_GetFile(const char *path, const NDS_Rom *rom)
{
    // Safer to use path lenght
    size_t pathLen = strlen(path);
    char *pathBuffer = malloc(pathLen); // char is 1 byte, no sizeof needed
    char nameBuffer[100];
    const char *prevPtr = path[0] == NDS_PATHSEPERATOR ? path + 1 : path; // skip seperator if present
    const char *currPtr;
    NDS_FntFolder dir;
    NDS_FileInfo *entry = malloc(sizeof(NDS_FileInfo));
    entry->rom = rom;

    NDS_CHECK_ROM_PTR(rom);
    fseek(rom->romFileStream, rom->header.offsetFnt, SEEK_SET); // Read root folder
    if(fread(&dir, sizeof(NDS_FntFolder), 1, rom->romFileStream) != 1)
    {
        NDS_SetError("Failed to read fnt folder.");
        return NULL;
    }

    entry->fntOffset = rom->header.offsetFnt + dir.fntOffset;
    // First item

    while(prevPtr < path + pathLen)
    {
        if(!(currPtr = strchr(prevPtr, NDS_PATHSEPERATOR)))
            currPtr = path + pathLen;
        memcpy(pathBuffer, prevPtr, currPtr - prevPtr);
        pathBuffer[currPtr - prevPtr] = 0; // String terminator
        prevPtr = currPtr + 1; // Skip seperator
        printf("%s\n", pathBuffer);

        // Now read every file/folder entry until name(of path) or 00 found
        NDS_FileGetName(nameBuffer, *entry);
        if(!strcmp(pathBuffer, nameBuffer)) // if equal
            return entry; // File/Folder found
        
        entry->fntOffset      

        


    }

}
*/

size_t NDS_FileGetName(char *ptr, const NDS_FileInfo *file)
{
    uint32_t lenght = 0;
    fseek(file->rom->romFileStream, file->rom->header.offsetFnt + file->fntOffset, SEEK_SET);

    if(fread(&lenght, 1, 1, file->rom->romFileStream) != 1 || lenght == 0)
    {
        NDS_SetError("Failed to read name. File is not valid or name is 0 char long");
        return 0;
    }

    // Is folder?
    if(lenght > NDS_DIRVAL)
        lenght -= NDS_DIRVAL;

    if(fread(ptr, 1, lenght, file->rom->romFileStream) != lenght)
    {
        NDS_SetError("Failed to read name. Couldn't read all characters");
        return 0;
    }

    ptr[lenght] = 0;

    return lenght;
}