#include "../NDS_logging.h"
#include "../NDS_memory.h"
#include "../NDS_rom.h"
#include "NDS_nitrorom.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_FID(id, max)                                                    \
    if(id > max)                                                              \
    {                                                                         \
        NDS_SetError("Cannot open file. File with id %i does not exist", id); \
        return -1;                                                            \
    }

#define CHECK_DID(id, max)                                                              \
    if(id - NDS_NITRO_ROOT_ID > max || id < NDS_NITRO_ROOT_ID)                          \
    {                                                                                   \
        NDS_SetError("Cannot open directory. Directory with id %i does not exist", id); \
        return -1;                                                                      \
    }

#define PATH_SEPERATOR '/'


NDS_NitroFs* NDS_NitroFsOpen(const NDS_NitroFsInf inf, NDS_Rom *rom)
{
    NDS_CHECK_PTR(rom);
    NDS_RomAccess *acc = &rom->acc;
    NDS_NitroFs *fs = NDS_Malloc(sizeof(NDS_NitroFs));
    if(!fs) return NULL;
    memset(fs, 0, sizeof(NDS_NitroFs));

    NDS_NitroFsDir root;
    if(NDS_RomRead(&root, inf.fnt_offset, sizeof(NDS_NitroFsDir), acc) != 0)
    {
        free(fs);
        return NULL;
    }

    fs->num_dirs = root.total_dirs;
    
    // Allocate mem for folders and fat
    fs->fnt_dirs = NDS_Malloc(sizeof(NDS_NitroFsDir) * root.total_dirs);
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
    root.parent_id = NDS_NITRO_ROOT_ID;
    fs->fnt_dirs[0] = root;
    // Now read every other dir. We've already put in the root dir
    if(NDS_RomRead(&fs->fnt_dirs[1], inf.fnt_offset + sizeof(NDS_NitroFsDir), sizeof(NDS_NitroFsDir) * (fs->num_dirs - 1), acc) != 0)
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

    fs->num_files = inf.fat_size / sizeof(NDS_NitroFsFatEntry);
    fs->acc = acc;
    fs->fnt_offset = inf.fnt_offset;
    return fs;
}

int NDS_NitroFsClose(NDS_NitroFs *fs)
{
    free(fs->fnt_dirs);
    free(fs->fat_files);
    free(fs);
    return 0;
}

int NDS_NitroFsItCreate(NDS_NitroFsIt *dest, uint16_t dir_id, NDS_NitroFs *fs)
{
    NDS_CHECK_PTR(fs);
    CHECK_DID(dir_id, fs->num_dirs);

    dest->fs = fs;
    dest->start_id = fs->fnt_dirs[dir_id - NDS_NITRO_ROOT_ID].sub_id;
    dest->curr_id = dest->start_id;
    dest->start_pos = fs->fnt_offset + fs->fnt_dirs[dir_id - NDS_NITRO_ROOT_ID].sub_offset;
    dest->cur_pos = dest->start_pos;

    return 0;
}

int NDS_NitroFsItRead(NDS_NitroFsItEntry *dest, NDS_NitroFsIt *it)
{
    NDS_CHECK_PTR_RINT(it);
    uint8_t raw_byte;
    if(NDS_RomRead(&raw_byte, it->cur_pos, 1, it->fs->acc) != 0)
        return -1;

    uint8_t name_len = raw_byte & 0x7F;
    bool is_dir = raw_byte & 0x80;

    if(name_len == 0)
        return 1; // End of entries

    it->cur_pos++;
    dest->is_dir = is_dir;
    dest->name_len = name_len;
    if(NDS_RomRead(&dest->name, it->cur_pos, name_len, it->fs->acc) != 0) return -1;
    dest->name[name_len] = 0;
    it->cur_pos += name_len;

    if(is_dir)
    {
        if(NDS_RomRead(&dest->id, it->cur_pos, sizeof(uint16_t), it->fs->acc) != 0) return -1;
        it->cur_pos += sizeof(uint16_t);
    } else
        dest->id = it->curr_id++;

    return 0;
}

void NDS_NitroFsItRewind(NDS_NitroFsIt *it)
{
    it->cur_pos = it->start_pos;
    it->curr_id = it->start_id;
}

int32_t NDS_NitroFsFileGetOffset(uint16_t file_id, const NDS_NitroFs *fs)
{
    CHECK_FID(file_id, fs->num_files);
    return fs->fat_files[file_id].file_offset;
}

int32_t NDS_NitroFsFileGetSize(uint16_t file_id, const NDS_NitroFs *fs)
{
    CHECK_FID(file_id, fs->num_files);
    NDS_NitroFsFatEntry e = fs->fat_files[file_id];
    return e.file_end - e.file_offset;
}

int NDS_NitroFsFileRead(void *restrict dest, uint32_t offset, size_t n, uint16_t file_id, const NDS_NitroFs *fs)
{
    CHECK_FID(file_id, fs->num_files);
    NDS_NitroFsFatEntry e = fs->fat_files[file_id];
    uint32_t availabe = e.file_end - e.file_offset - offset;
    if(n > availabe)
    {
        NDS_SetError("Cannot read over the end of file. Tried to read %i bytes but only %i available at offset %i", n, availabe, offset);
        return -1;
    }
    if(NDS_RomRead(dest, e.file_offset + offset, n, fs->acc) != 0)
        return -1;

    return 0;
}

int16_t NDS_NitroFsDirGetParent(uint16_t dir_id, NDS_NitroFs *fs)
{
    NDS_CHECK_PTR_RINT(fs);
    CHECK_DID(dir_id, fs->num_dirs);
    return fs->fnt_dirs[dir_id - NDS_NITRO_ROOT_ID].parent_id;
}

int16_t NDS_NitroFsResolvePath(char *path, uint16_t start_id, NDS_NitroFs *fs)
{
    NDS_CHECK_PTR_RINT(fs);
    CHECK_DID(start_id, fs->num_dirs);
    uint16_t curr_dir = start_id;
    // Start in root dir
    if(path[0] == PATH_SEPERATOR)
        curr_dir = NDS_NITRO_ROOT_ID;

    char *path2;
    uint8_t part_len;

    for(;; path = path2)
    {
        // Skip slashes
        while(*path == PATH_SEPERATOR) path++;
        path2 = path;
        while(*path2 != PATH_SEPERATOR && *path2 != 0) path2++;
        part_len = path2 - path;

        if(path == path2)
            break; // At end?

        if(*path == '.')
        {
            if(part_len == 1)
                continue;
            else if(part_len == 2 && path[1] == '.')
            {
                curr_dir = NDS_NitroFsDirGetParent(curr_dir, fs);
                continue;
            }
        }

        bool found = false;
        NDS_NitroFsIt it;
        if(NDS_NitroFsItCreate(&it, curr_dir, fs) != 0) return -1;
        NDS_NitroFsItEntry entry;
        while(NDS_NitroFsItRead(&entry, &it) == 0)
        {
            if(entry.name_len == part_len && strncmp(entry.name, path, part_len) == 0)
            {
                // Entry found -> exit iteration
                found = true;
                curr_dir = entry.id;
                break;
            }
        }

        if(!found)
        {
            NDS_SetError("Failed to find %s. File/folder might not exist", path);
            return -1;
        }
    }
    
    return curr_dir;
}