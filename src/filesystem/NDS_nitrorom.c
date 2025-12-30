#include "../NDS_logging.h"
#include "../NDS_memory.h"
#include "NDS_rom.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "NDS_nitrorom.h"

#ifdef NDS_DEBUG_BUILD
#define CHECK_FID(id, max)                                      \
    if(id > max)                                                \
    {                                                           \
        NDS_SetError("Cannot open file. File does not exist");  \
        return -1;                                              \
    }
#else
#define CHECK_FID(id, max)
#endif




NDS_NitroRomFs* NDS_NitroRomFsOpen(NDS_NitroRomFsInf inf, NDS_RomAccess *acc)
{
    NDS_CHECK_PTR(acc);
    NDS_NitroRomFs *fs = NDS_Malloc(sizeof(NDS_NitroRomFs));
    if(!fs) return NULL;
    memset(fs, 0, sizeof(NDS_NitroRomFs));

    NDS_NitroRomFsDir root;
    if(NDS_RomRead(&root, inf.fnt_offset, sizeof(NDS_NitroRomFsDir), acc) != 0)
    {
        free(fs);
        return NULL;
    }

    fs->num_dirs = root.total_dirs;
    
    // Allocate mem for folders and fat
    fs->fnt_dirs = NDS_Malloc(sizeof(NDS_NitroRomFsDir) * root.total_dirs);
    if(root.total_dirs < 1 && !fs->fnt_dirs)
    {
        free(fs);
        return NULL;
    }
    fs->fat_files = NDS_Malloc(inf.fat_size);
    if(inf.fat_size < 1 && !fs->fat_files)
    {
        free(fs->fnt_dirs);
        free(fs);
        return NULL;
    }

    // Set parent id to self
    root.parent_id = NDS_NITROROM_ROOT_ID;
    fs->fnt_dirs[0] = root;
    // Now read every other dir. We've already put in the root dir
    if(NDS_RomRead(&fs->fnt_dirs[1], inf.fnt_offset + sizeof(NDS_NitroRomFsDir), sizeof(NDS_NitroRomFsDir) * (fs->num_dirs - 1), acc) != 0)
    {
        free(fs->fnt_dirs);
        free(fs->fat_files);
        free(fs);
        return NULL;
    }

    // Read every fat entry
    if(NDS_RomRead(fs->fat_files, inf.fat_offset, inf.fat_size, acc) != 0)
    {
        free(fs->fnt_dirs);
        free(fs->fat_files);
        free(fs);
        return NULL;
    }

    fs->num_files = inf.fat_size / sizeof(NDS_NitroRomFsFatEntry);
    fs->acc = acc;
    fs->fnt_offset = inf.fnt_offset;
    return fs;
}

NDS_NitroRomFsIt* NDS_NitroRomFsItOpen(NDS_NitroRomFs *fs, uint16_t dir_id)
{
    NDS_CHECK_PTR(fs);
    if(dir_id > fs->num_dirs)
    {
        NDS_SetError("Cannot open iterator to dir. Directory does not exist");
        return NULL;
    }
    NDS_NitroRomFsIt *it = NDS_Malloc(sizeof(NDS_NitroRomFsIt));
    if(!it) return NULL;

    it->fs = fs;
    it->start_id = fs->fnt_dirs[dir_id - NDS_NITROROM_ROOT_ID].sub_id;
    it->curr_id = it->start_id;
    it->start_pos = fs->fnt_offset + fs->fnt_dirs[dir_id - NDS_NITROROM_ROOT_ID].sub_offset;
    it->cur_pos = it->start_pos;

    return it;
}

int NDS_NitroRomFsItRead(NDS_NitroRomFsDirEntry *dest, NDS_NitroRomFsIt *it)
{
    NDS_CHECK_PTR_RINT(it);
    struct
    {
        uint8_t is_dir : 1; // First bit
        uint8_t name_len : 7; // Last 7 bits
    } status;

    if(NDS_RomRead(&status, it->cur_pos, 1, it->fs->acc) != 0)
        return -1;

    if(status.name_len == 0)
        return 1; // End of entries

    it->cur_pos++;
    dest->name_len = status.name_len;
    if(NDS_RomRead(&dest->name, it->cur_pos, status.name_len, it->fs->acc) != 0) return -1;
    it->cur_pos += status.name_len;

    if(status.is_dir)
    {
        if(NDS_RomRead(&dest->id, it->cur_pos, sizeof(uint16_t), it->fs->acc) != 0) return -1;
        it->cur_pos += sizeof(uint16_t);
    } else
        dest->id = it->curr_id++;

    return 0;
}

void NDS_NitroRomFsItRewind(NDS_NitroRomFsIt *it)
{
    it->cur_pos = it->start_pos;
    it->curr_id = it->start_id;
}

int32_t NDS_NitroRomFsFileGetOffset(uint16_t file_id, NDS_NitroRomFs *fs)
{
    CHECK_FID(file_id, fs->num_files);
    return fs->fat_files[file_id].file_offset;
}

int32_t NDS_NitroRomFsFileGetSize(uint16_t file_id, NDS_NitroRomFs *fs)
{
    CHECK_FID(file_id, fs->num_files);
    return fs->fat_files[file_id].file_size;
}

int NDS_NitroRomFsFileRead(void *restrict dest, uint32_t offset, size_t n, uint16_t file_id, NDS_NitroRomFs *fs)
{
    CHECK_FID(file_id, fs->num_files);
    NDS_NitroRomFsFatEntry e = fs->fat_files[file_id];
    uint32_t availabe = e.file_size - offset;
    if(n > availabe)
    {
        NDS_SetError("Cannot read over the end of file");
        return -1;
    }
    if(NDS_RomRead(dest, e.file_offset, n, fs->acc) != 0)
        return -1;

    return 0;
}
