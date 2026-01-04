// SPDX-License-Identifier: GPL-3.0-only
#ifndef NDS_nitrorom_h_
#define NDS_nitrorom_h_

#include "../NDS_rom.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NDS_NITRO_ROOT_ID 0xF000
#define NDS_FNT_MAX_NAME_LEN 0x7F // 01111111 first bit is folder

typedef struct NDS_NitroFsInf NDS_NitroFsInf;
typedef struct NDS_NitroFs NDS_NitroFs;
typedef struct NDS_NitroFsDir NDS_NitroFsDir;
typedef struct NDS_NitroFsIt NDS_NitroFsIt;
typedef struct NDS_NitroFsItEntry NDS_NitroFsItEntry;
typedef struct NDS_NitroFsFatEntry NDS_NitroFsFatEntry;


struct NDS_NitroFsInf
{
    uint32_t fnt_offset;
    uint32_t fnt_size;
    uint32_t fat_offset;
    uint32_t fat_size;
};

struct NDS_NitroFs
{
    NDS_RomAccess *acc;
    NDS_NitroFsDir *fnt_dirs;
    NDS_NitroFsFatEntry *fat_files;
    uint16_t num_dirs;
    uint16_t num_files;
    uint32_t fnt_offset;
};

// Folder entry at begin of fnt
#pragma pack(push, 1)
struct NDS_NitroFsDir
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
struct NDS_NitroFsFatEntry
{
    uint32_t file_offset;
    uint32_t file_end;
};
#pragma pack(pop)

// NitroFs iterator (iterates over files in specific dir)
struct NDS_NitroFsIt
{
    NDS_NitroFs *fs;
    uint32_t start_pos; // both relative to rom start
    uint32_t cur_pos;
    uint16_t start_id;
    uint16_t curr_id;
};

struct NDS_NitroFsItEntry
{
    char name[NDS_FNT_MAX_NAME_LEN + 1]; // 0 terminator
    uint8_t name_len;
    uint16_t id;
    bool is_dir;
    // Maybe file type? unknown, dir, narc, sseq ...
};


NDS_NitroFs* NDS_NitroFsOpen(const NDS_NitroFsInf inf, NDS_Rom *rom);
int NDS_NitroFsClose(NDS_NitroFs *fs);

int NDS_NitroFsItCreate(NDS_NitroFsIt *dest, uint16_t dir_id, NDS_NitroFs *fs);
// -1 on error, 0 on success, 1 on end
int NDS_NitroFsItRead(NDS_NitroFsItEntry *dest, NDS_NitroFsIt *it);
void NDS_NitroFsItRewind(NDS_NitroFsIt *it);

// -1 on failure
int32_t NDS_NitroFsFileGetOffset(uint16_t file_id, const NDS_NitroFs *fs);
// -1 on failure
int32_t NDS_NitroFsFileGetSize(uint16_t file_id, const NDS_NitroFs *fs);
int NDS_NitroFsFileRead(void *dest, uint32_t offset, size_t n, uint16_t file_id, const NDS_NitroFs *fs);

// Returns parent dir id, -1 on failure
int16_t NDS_NitroFsDirGetParent(uint16_t dir_id, NDS_NitroFs *fs);
// Returns file_id, -1 on failure
int16_t NDS_NitroFsResolvePath(char *path, uint16_t start_id, NDS_NitroFs *fs);

#endif