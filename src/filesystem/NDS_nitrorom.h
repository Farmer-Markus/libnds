#ifndef NDS_nitrorom_h_
#define NDS_nitrorom_h_

#include "NDS_rom.h"
#include <stddef.h>
#include <stdint.h>

#define NDS_NITROROM_ROOT_ID 0xF000
#define NDS_FNT_MAX_NAME_LEN 0x7F // 01111111 first bit is folder

typedef struct NDS_NitroRomFsInf NDS_NitroRomFsInf;
typedef struct NDS_NitroRomFs NDS_NitroRomFs;
typedef struct NDS_NitroRomFsDir NDS_NitroRomFsDir;
typedef struct NDS_NitroRomFsIt NDS_NitroRomFsIt;
typedef struct NDS_NitroRomFsDirEntry NDS_NitroRomFsDirEntry;
typedef struct NDS_NitroRomFsFatEntry NDS_NitroRomFsFatEntry;


struct NDS_NitroRomFsInf
{
    uint32_t fnt_offset;
    uint32_t fnt_size;
    uint32_t fat_offset;
    uint32_t fat_size;
};

struct NDS_NitroRomFs
{
    NDS_RomAccess *acc;
    NDS_NitroRomFsDir *fnt_dirs;
    NDS_NitroRomFsFatEntry *fat_files;
    uint16_t num_dirs;
    uint16_t num_files;
    uint32_t fnt_offset;
};

// Folder entry at begin of fnt
#pragma pack(push, 1)
struct NDS_NitroRomFsDir
{
    uint32_t sub_offset; // offset to fnt subtable with file entries
    uint16_t sub_id; // id of first item in subtable
    union
    {
        uint16_t total_dirs; // Info only in root dir
        uint16_t parent_id; // non-root dirs
    };
};
#pragma pack(pop)

#pragma pack(push, 1)
struct NDS_NitroRomFsFatEntry
{
    uint32_t file_offset;
    uint32_t file_size;
};
#pragma pack(pop)

// NitroRomFs iterator (iterates over files in specific dir)
struct NDS_NitroRomFsIt
{
    NDS_NitroRomFs *fs;
    uint32_t start_pos; // both relative to rom start
    uint32_t cur_pos;
    uint16_t start_id;
    uint16_t curr_id;
};

struct NDS_NitroRomFsDirEntry
{
    char name[NDS_FNT_MAX_NAME_LEN + 1]; // 0 terminator
    uint8_t name_len;
    uint16_t id;
    // Maybe file type? unknown, dir, narc, sseq ...
};


NDS_NitroRomFs* NDS_NitroRomFsOpen(NDS_NitroRomFsInf inf, NDS_RomAccess *acc);

NDS_NitroRomFsIt* NDS_NitroRomFsItOpen(NDS_NitroRomFs *fs, uint16_t dir_id);

// -1 on error, 0 on success, 1 on end
int NDS_NitroRomFsItRead(NDS_NitroRomFsDirEntry *dest, NDS_NitroRomFsIt *it);
void NDS_NitroRomFsItRewind(NDS_NitroRomFsIt *it);


// -1 on failure
int32_t NDS_NitroRomFsFileGetOffset(uint16_t file_id, NDS_NitroRomFs *fs);
// -1 on failure
int32_t NDS_NitroRomFsFileGetSize(uint16_t file_id, NDS_NitroRomFs *fs);
int NDS_NitroRomFsFileRead(void *restrict dest, uint32_t offset, size_t n, uint16_t file_id, NDS_NitroRomFs *fs);

#endif