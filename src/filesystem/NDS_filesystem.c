#include "../NDS_logging.h"
#include "../NDS_memory.h"
#include "NDS_rom.h"
#include "NDS_filesystem.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NDS_FNT_RESERVED 0x80 // Value > 0x80 is folder in FNT
#define NDS_ROOT_ID 0xF000
#define NDS_PATHSEPERATOR '/'


static NDS_Dir* NDS_OpenDirId(const uint16_t id, const NDS_Rom *rom);
// 0 on success, -1 on error
//static int32_t NDS_ReadDirent(NDS_Dirent *entrptr, const uint32_t offset, const NDS_Dir *dirptr);
static int NDS_ReadDirentries(NDS_Dirent *buff, const NDS_Dir *dirptr);
static int16_t NDS_CountDirentries(const NDS_Dir *dirptr);


NDS_Dir* NDS_Opendir(const char *path, const NDS_Rom *rom)
{
    NDS_CHECK_PTR(rom);
    const char *p = path;
    size_t nsize = 0;
    if(path[0] == NDS_PATHSEPERATOR)
    {
        if(strlen(path) == 1)
            return NDS_OpenDirId(NDS_ROOT_ID, rom);
        p = &path[1];
    }

    NDS_Dir *dir;
    NDS_Dirent *ent;
    uint16_t dirId = NDS_ROOT_ID; // Start at root dir

    while(true)
    {
        bool found = false;
        // [1] to skip last seperator
        char *pSep = strchr(p, NDS_PATHSEPERATOR);
        if(!pSep) // At end of path
            nsize = strlen(p);
        else
            nsize = pSep - p;

        dir = NDS_OpenDirId(dirId, rom);
        while((ent = NDS_Readdir(dir)) != NULL)
        {
            if(strlen(ent->de_name) == nsize && strncmp(ent->de_name, p, nsize) == 0) // Found
            {
                if(!pSep && ent->de_type != NDS_DIR) // Path end & !dir
                {
                    NDS_SetError("Failed to open directory at path. Is not a directory");
                    NDS_Closedir(dir);
                    return NULL;
                }

                fseek(rom->r_fstream, rom->r_header.fntOffset + ent->de_fntOffset + 1 + strlen(ent->de_name), SEEK_SET);
                NDS_Closedir(dir);
                if(fread(&dirId, 2, 1, rom->r_fstream) != 1) return NULL;
                
                if(!pSep) // Path end & dir
                    return NDS_OpenDirId(dirId, rom);
                                
                found = true;
                // + 1 to go behind seperator
                p += nsize + 1;
                break;
            }
        }

        if(!found) break;
    }
        
    NDS_SetError("Could not find directory at given path");
    NDS_Closedir(dir);
    return NULL;
}

NDS_Dir* NDS_OpenParentDir(const NDS_Dir *dirptr)
{
    NDS_CHECK_PTR(dirptr);
    if(!(dirptr->d_inf.parentId & NDS_ROOT_ID))
    {
        NDS_SetError("Cant open parent directory of root");
        return NULL;
    }
    
    return NDS_OpenDirId(dirptr->d_inf.parentId, dirptr->rom);
}

int NDS_Closedir(NDS_Dir *dirptr)
{   
    NDS_CHECK_PTR_RINT(dirptr);
    if(!dirptr) return -1;

    for(uint16_t i = 0; i < dirptr->d_ndirents; i++)
        free(dirptr->d_dirents[i].de_name);

    free(dirptr->d_dirents);
    free(dirptr);
    return 0;
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

// Open directory by id (0xF000=root)
static NDS_Dir* NDS_OpenDirId(const uint16_t id, const NDS_Rom *rom)
{
    NDS_CHECK_PTR(rom);
    uint32_t offset = rom->r_header.fntOffset + sizeof(NDS_FntDirInfo) * (id - NDS_ROOT_ID);
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
    if((nentries = NDS_CountDirentries(dir)) == -1)
        return NULL;

    // Allocate mem for all entries
    dir->d_ndirents = nentries;
    if((dir->d_dirents = NDS_Malloc(dir->d_ndirents * sizeof(NDS_Dirent))) == NULL) return NULL;

    // Read all entries to memory
    if(NDS_ReadDirentries(dir->d_dirents, dir) == -1)
        return NULL;

    return dir;
}

static int NDS_ReadDirentries(NDS_Dirent *buff, const NDS_Dir *dirptr)
{
    NDS_CHECK_PTR_RINT(dirptr);
    FILE *strm = dirptr->rom->r_fstream;
    fseek(strm, dirptr->rom->r_header.fntOffset + dirptr->d_inf.fntOffset, SEEK_SET);

    uint8_t status = 0x01;
    uint16_t i = 0;
    for(; i < dirptr->d_ndirents && status != 0x00; i++)
    {
        NDS_DirentType type = NDS_REG; // Regular file
        fread(&status, 1, 1, strm);
        // Is folder?
        if(status > NDS_FNT_RESERVED)
        {
            type = NDS_DIR;
            status -= NDS_FNT_RESERVED; // Used to get directory name lenght
        } else if(status == NDS_FNT_RESERVED)
            continue;

        // -1 for status byte read above
        buff[i].de_fntOffset = ftell(strm) - dirptr->rom->r_header.fntOffset - 1;
        // Allocate memory for name
        if((buff[i].de_name = NDS_Malloc(status + 1)) == NULL) return -1;
        buff[i].de_name[status] = 0;

        // Read name into buffer
        fread(buff[i].de_name, 1, status, strm);
        buff[i].de_type = type;
        buff[i].de_id = dirptr->d_inf.subId + i;
        if(type == NDS_DIR) // Skip 2 dir id bytes
            fseek(strm, 2, SEEK_CUR);
    }

    if(i < dirptr->d_ndirents - 1)
    {
        NDS_SetError("Failed to read directory entries. Reached end before finished reading");
        return -1;
    }

    return 0;
}

static int16_t NDS_CountDirentries(const NDS_Dir *dirptr)
{
    NDS_CHECK_PTR_RINT(dirptr);
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
