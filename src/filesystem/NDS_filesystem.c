#include "../NDS_logging.h"
#include "../NDS_memory.h"
#include "NDS_rom.h"
#include "NDS_filesystem.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define NDS_FNT_RESERVED 0x80 // Value > 0x80 is folder in FNT
#define NDS_PATHSEPERATOR '/'

static NDS_Dir* NDS_OpenDirId(const uint16_t id, const NDS_Rom *rom);
// 0 on success, -1 on error
static int32_t NDS_ReadDirent(NDS_Dirent *entrptr, const uint32_t offset, const NDS_Dir *dirptr);
static int NDS_ReadDirents(NDS_Dirent *dirents, const NDS_Dir *dirptr);
static int16_t NDS_CountDirents(const NDS_Dir *dirptr);



NDS_Dir* NDS_Opendir(const char *path, const NDS_Rom *rom)
{
    const char *p1 = path;
    if(path[0] == NDS_PATHSEPERATOR)
        p1 = &path[1];

    FILE *strm = rom->r_fstream;
    NDS_Dir currDir;
    NDS_Dir nextDir;

    // Read root folder
    fseek(strm, rom->r_header.fntOffset, SEEK_SET);
    fread(&currDir.d_inf, sizeof(NDS_FntDirInfo), 1, strm);






    return NDS_OpenDirId(20, rom);
}

void NDS_Seekdir(NDS_Dir *dirptr, int16_t feoffset, uint8_t mode)
{
    switch(mode)
    {
        case NDS_SEEK_SET:
            dirptr->d_dirent_cur = feoffset;
            break;

        case NDS_SEEK_CUR:
            dirptr->d_dirent_cur += feoffset;
            break;

        case NDS_SEEK_END:
            dirptr->d_dirent_cur = dirptr->d_ndirents + feoffset;
            break;

        default:
            break;
    }
}

NDS_Dirent* NDS_Readdir(NDS_Dir *dirptr)
{
    if(dirptr->d_dirent_cur >= dirptr->d_ndirents)
        return NULL;

    NDS_Dirent *ent = &dirptr->d_dirents[dirptr->d_dirent_cur];
    dirptr->d_dirent_cur++;
    return ent;
}

int NDS_Closedir(NDS_Dir *dirptr)
{
    for(uint16_t i = 0; i < dirptr->d_ndirents; i++)
        free(dirptr->d_dirents[i].de_name);

    free(dirptr->d_dirents);
    free(dirptr);
    return 0;
}

// Try to open directory by id (0=root)
static NDS_Dir* NDS_OpenDirId(const uint16_t id, const NDS_Rom *rom)
{
    uint32_t offset = rom->r_header.fntOffset + sizeof(NDS_FntDirInfo) * id;
    if(offset >= rom->r_header.fntOffset + rom->r_header.fntSize)
    {
        NDS_SetError("Failed to open directory. Tried to read outside of file name table");
        return NULL;
    }
    // Allocate mem for the dir obj
    fseek(rom->r_fstream, offset, SEEK_SET);
    NDS_Dir *dir;
    if((dir = NDS_Malloc(sizeof(NDS_Dir))) == NULL) return NULL;
    dir->rom = rom;
    dir->d_dirent_cur = 0;

    // Read fnt folder header
    fread(&dir->d_inf, sizeof(NDS_FntDirInfo), 1, rom->r_fstream);
    int16_t nentries;
    if((nentries = NDS_CountDirents(dir)) == -1)
        return NULL;

    // Allocate mem for all entries
    dir->d_ndirents = nentries;
    if((dir->d_dirents = NDS_Malloc(dir->d_ndirents * sizeof(NDS_Dirent))) == NULL) return NULL;

    // Read all entries to memory
    int16_t currOffset = 0;
    for(uint16_t i = 0; i < nentries; i++)
    {
        if((currOffset += NDS_ReadDirent(&dir->d_dirents[i], currOffset, dir)) == -1)
            return NULL;
        dir->d_dirents[i].de_id = dir->d_inf.subId + i;
        if(dir->d_dirents[i].de_type == NDS_DIR)
            currOffset += 2; // Skip 2 dir bytes
    }

    return dir;
}

// Offset relative to NDS_FntDirInfo.fntOffset
static int32_t NDS_ReadDirent(NDS_Dirent *entrptr, const uint32_t offset, const NDS_Dir *dirptr)
{
    FILE *strm = dirptr->rom->r_fstream;
    fseek(strm, dirptr->rom->r_header.fntOffset + dirptr->d_inf.fntOffset + offset, SEEK_SET);

    uint8_t status;
    fread(&status, 1, 1, strm);
    if(status == 0x00 || status == NDS_FNT_RESERVED)
    {
        NDS_SetError("Failed to read directory entry. Reached end");
        return -1;
    }
    else if(status > NDS_FNT_RESERVED)
    {
        entrptr->de_type = NDS_DIR;
        status -= NDS_FNT_RESERVED;
    }

    // Allocate mem for name
    if((entrptr->de_name = NDS_Malloc(status + 1)) == NULL) return -1;
    entrptr->de_name[status] = 0;

    fread(entrptr->de_name, 1, status, strm);
    return status + 1; // name + statusbyte
}

/*
static int NDS_ReadDirents(NDS_Dirent *buff, const NDS_Dir *dirptr)
{
    FILE *strm = dirptr->rom->r_fstream;
    fseek(strm, dirptr->rom->r_header.fntOffset + dirptr->d_inf.fntOffset, SEEK_SET);

    uint8_t status = 0x01;
    uint16_t i = 0;
    for(; i < dirptr->d_ndirents && status != 0x00; i++)
    {
        NDS_DirentType type = NDS_REG;
        fread(&status, 1, 1, strm);
        // Is folder?
        if(status > NDS_FNT_RESERVED)
        {
            type = NDS_DIR;
            status -= NDS_FNT_RESERVED; // Used to get name lenght
        } else if(status == NDS_FNT_RESERVED)
            continue;

        // Allocate memory for name
        buff[i].de_name = malloc(status + 1);
        buff[i].de_name[status] = 0; // string terminator

        // Read name into buffer
        fread(buff[i].de_name, 1, status, strm);
        buff[i].de_type = type;
        buff[i].de_id = dirptr->d_inf.subId + i;
        if(type == NDS_DIR)
            fseek(strm, 2, SEEK_CUR);
    }

    if(i < dirptr->d_ndirents - 1)
    {
        NDS_SetError("Failed to read directory entries. Reached end before finished reading");
        return 1;
    }

    return 0;
}
*/

static int16_t NDS_CountDirents(const NDS_Dir *dirptr)
{
    uint16_t res = 0;
    uint32_t fntend = dirptr->rom->r_header.fntOffset + dirptr->rom->r_header.fntSize;
    FILE *f = dirptr->rom->r_fstream;
    fseek(f, dirptr->rom->r_header.fntOffset + dirptr->d_inf.fntOffset, SEEK_SET);
    uint8_t status = 0x00;

    do
    {
        fread(&status, 1, 1, f);
        if(status > NDS_FNT_RESERVED)
            status -= NDS_FNT_RESERVED - 2; // +2: Skip sub dir id on folder entries
        else if(status == NDS_FNT_RESERVED)
            continue;

        // Skip name -> next entry
        fseek(f, status, SEEK_CUR);
        
        res++;
        if(ftell(f) >= fntend)
        {
            NDS_SetError("Failed to count directory entries. Reached end of FNT file name table");
            return -1;
        }
    } while(status != 0x00);

    // Without latest 0x00
    return res - 1;
}








































/*
static NDS_FileInfo* NDS_SearchFilesystem(const char *path, NDS_FntFolder currDir, const NDS_Rom *rom)
{
    uint8_t nBufferSize = 32;
    char *nameBuffer = malloc(nBufferSize);
    const char *pathSep;
    pathSep = strchr(path, NDS_PATHSEPERATOR);
    // Find next seperator

    // Jump to fnt
    fseek(rom->romFileStream, rom->header.fntOffset + currDir.fntOffset, SEEK_SET);
    uint8_t status = 0; // Status byte (name lenght, is folder)
    fread(&status, 1, 1, rom->romFileStream);
    bool isDir = false;

    while(status != 0x00) // while not at end and no errors
    {
        // Is directory
        if(status > NDS_FNT_RESERVED)
        {
            isDir = true;
            status -= NDS_FNT_RESERVED;

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
            file->fntOffset = ftell(rom->romFileStream) - rom->header.fntOffset - status - 1; // To current offset(based on fnt begin)
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

            fseek(rom->romFileStream, rom->header.fntOffset + (dirIds & 0x0FFF) * sizeof(NDS_FntFolder), SEEK_SET);
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
    fseek(rom->romFileStream, rom->header.fntOffset, SEEK_SET);
    fread(&start, sizeof(NDS_FntFolder), 1, rom->romFileStream);
    if(path[0] == NDS_PATHSEPERATOR)
        return NDS_SearchFilesystem(&path[1], start, rom);

    return NDS_SearchFilesystem(path, start, rom);
}

size_t NDS_FileGetName(char *ptr, const NDS_FileInfo *file)
{
    uint32_t lenght = 0;
    fseek(file->rom->romFileStream, file->rom->header.fntOffset + file->fntOffset, SEEK_SET);

    if(fread(&lenght, 1, 1, file->rom->romFileStream) != 1 || lenght == 0)
    {
        NDS_SetError("Failed to read name. File is not valid or name is 0 char long");
        return 0;
    }

    // Is folder?
    if(lenght > NDS_FNT_RESERVED)
        lenght -= NDS_FNT_RESERVED;

    if(fread(ptr, 1, lenght, file->rom->romFileStream) != lenght)
    {
        NDS_SetError("Failed to read name. Couldn't read all characters");
        return 0;
    }

    ptr[lenght] = 0;

    return lenght;
}*/











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
    fseek(rom->romFileStream, rom->header.fntOffset, SEEK_SET); // Read root folder
    if(fread(&dir, sizeof(NDS_FntFolder), 1, rom->romFileStream) != 1)
    {
        NDS_SetError("Failed to read fnt folder.");
        return NULL;
    }

    entry->fntOffset = rom->header.fntOffset + dir.fntOffset;
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