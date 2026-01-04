// SPDX-License-Identifier: GPL-3.0-only
#ifndef NDS_filesystem_h
#define NDS_filesystem_h

#include <nds/NDS_rom.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define NDS_NITRO_MAX_NAME_LEN 0x7F
#define NDS_NITRO_ROOT_ID 0xF000


// Info object of NitroRom filesystem
typedef struct NDS_NitroFsInf
{
    uint32_t fnt_offset;
    uint32_t fnt_size;
    uint32_t fat_offset;
    uint32_t fat_size;
} NDS_NitroFsInf;

// NitroRom filesystem object
typedef struct NDS_NitroFs NDS_NitroFs;
// NitroRom filesystem iterator object
typedef struct NDS_NitroFsIt
{
    NDS_NitroFs *fs;
    uint32_t start_pos;
    uint32_t cur_pos;
    uint16_t start_id;
    uint16_t curr_id;
} NDS_NitroFsIt;

// NitroRom filesystem iterator entry onbject
typedef struct NDS_NitroFsItEntry
{
    char name[NDS_NITRO_MAX_NAME_LEN + 1];
    uint8_t name_len;
    uint16_t id;
    bool is_dir;
} NDS_NitroFsItEntry;


// Open NitroRom filesystem
// object pointer on success, NULL on failure
extern NDS_NitroFs* NDS_NitroFsOpen(const NDS_NitroFsInf inf, NDS_Rom *rom);
// Close NitroRom filesystem object
// 0 on success, -1 on failure
extern int NDS_NitroFsClose(NDS_NitroFs *fs);

// Open iterator at specific directory
// 0 on success, -1 on failure
extern int NDS_NitroFsItCreate(NDS_NitroFsIt *dest, uint16_t dir_id, NDS_NitroFs *fs);
// Read files from iterator
// 0 on success, 1 on end reached, -1 on failure
extern int NDS_NitroFsItRead(NDS_NitroFsItEntry *dest, NDS_NitroFsIt *it);
// Reset NitroRom filesystem iterator to begin
extern void NDS_NitroFsItRewind(NDS_NitroFsIt *it);

// Get offset of NitroRom file
// offset on success, -1 on failure
extern int32_t NDS_NitroFsFileGetOffset(uint16_t file_id, const NDS_NitroFs *fs);
// Get size of NitroRom file
// size on success, -1 on failure
extern int32_t NDS_NitroFsFileGetSize(uint16_t file_id, const NDS_NitroFs *fs);
// Read n bytes of NitroRom file
// 0 on success, -1 on failure
extern int NDS_NitroFsFileRead(void *dest, uint32_t offset, size_t n, uint16_t file_id, 
                                  const NDS_NitroFs *fs);

// Get parent directory from directory id
// dir id on success, -1 on failure
extern int16_t NDS_NitroFsDirGetParent(uint16_t dir_id, NDS_NitroFs *fs);
// Get NitroRom file/folder by path
// entry id on success, -1 on failure
extern int16_t NDS_NitroFsResolvePath(char *path, uint16_t start_id, NDS_NitroFs *fs);

#ifdef __cplusplus
}
#endif
#endif
