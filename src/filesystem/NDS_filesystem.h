#ifndef NDS_FILESYSTEM_H_INTERNAL
#define NDS_FILESYSTEM_H_INTERNAL

#include <filesystem/NDS_rom.h>
#include <stddef.h>
#include <stdint.h>

// https://problemkaputt.de/gbatek.htm#dscartridgenitroromandnitroarcfilesystems

typedef struct NDS_FileInfo
{
    const NDS_Rom *rom;

    uint32_t fntOffset;     // Offset of info in fnt table
    // uint32_t dataOffset;    // Offset of raw data
    // uint32_t dataLenght;
} NDS_FileInfo;

#pragma pack(push, 1)
typedef struct NDS_FntFolder
{
    uint32_t fntOffset; // Where to find dir content. Offset of header->OffsetFnt + fntOffset
    uint16_t subId; // Id of fist file (to locate in FAT)
    uint16_t parentId; // Id of parent dir. Root folder is 0xF000
} NDS_FntFolder;
#pragma pack(pop)


NDS_FileInfo* NDS_GetFile(const char *path, const NDS_Rom *rom);

// Get name of file
// Returns lenght of name
size_t NDS_FileGetName(char *ptr, const NDS_FileInfo *file);


#endif