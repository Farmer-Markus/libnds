#ifndef NDS_filesystem_h_
#define NDS_filesystem_h_

#include "NDS_rom.h"
#include <stddef.h>
#include <stdint.h>


typedef enum NDS_DirentType NDS_DirentType;
typedef enum NDS_SeekMode NDS_SeekMode;
typedef struct NDS_Dir NDS_Dir;
typedef struct NDS_FntDirInfo NDS_FntDirInfo;
typedef struct NDS_Dirent NDS_Dirent;


enum NDS_DirentType
{
    NDS_REG,    // Regular file
    NDS_DIR     // Directory
};

enum NDS_SeekMode
{
    NDS_SEEK_SET,
    NDS_SEEK_CUR,
    NDS_SEEK_END
};


// https://problemkaputt.de/gbatek.htm#dscartridgenitroromandnitroarcfilesystems
#pragma pack(push, 1)
struct NDS_FntDirInfo
{
    uint32_t fntOffset; // Where to find dir content. Offset of header->OffsetFnt + fntOffset
    uint16_t subId; // Id of first file (to locate in FAT)
    uint16_t parentId; // Id of parent dir. Root folder is 0xF000
};
#pragma pack(pop)

// Directory structure
struct NDS_Dir
{
    // Holds info read directly from rom folder table (right before fnt)
    NDS_FntDirInfo d_inf;
    // Holds all dir entries after opening
    struct NDS_Dirent* d_dirents;
    uint16_t d_ndirents;
    uint16_t d_dirent_cur;
    const NDS_Rom *rom; // Rom file opened on
};

// Direntry structure
struct NDS_Dirent
{
    uint32_t de_fntOffset;
    // unique id in filesystem
    uint16_t de_id;
    uint8_t de_type;
    char* de_name;
};


// Open directory, read all entries
NDS_Dir* NDS_Opendir(const char *pathstr, const NDS_Rom *romptr);
NDS_Dir* NDS_OpenParentDir(const NDS_Dir *dirptr);
int NDS_Closedir(NDS_Dir *dirptr);

void NDS_Seekdir(NDS_Dir *dirptr, int16_t feoffset, uint8_t mode);
NDS_Dirent* NDS_Readdir(NDS_Dir *dirptr);

#endif
