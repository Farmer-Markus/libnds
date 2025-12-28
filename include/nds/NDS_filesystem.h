#ifndef NDS_filesystem_h
#define NDS_filesystem_h

#include <stdint.h>
#include <nds/NDS_rom.h>

#ifdef __cplusplus
extern "C"
{
#endif

// dbl declarate for safety
enum NDS_DirentType
{
    NDS_REG = 0,
#define NDS_REG NDS_REG
    NDS_DIR = 1
#define NDS_DIR NDS_DIR
};

enum NDS_SeekMode
{
    NDS_SEEK_SET = 0,
#define NDS_SEEK_SET NDS_SEEK_SET
    NDS_SEEK_CUR = 1,
#define NDS_SEEK_CUR NDS_SEEK_CUR
    NDS_SEEK_END = 2
#define NDS_SEEK_END NDS_SEEK_END
};

// RomFS directory object
typedef struct NDS_Dir NDS_Dir;

// RomFS directory entry object
typedef struct NDS_Dirent
{
    uint32_t de_fntOffset;
    uint16_t de_id;
    uint8_t de_type;
    char* de_name;
} NDS_Dirent;


// Open RomFS directory and return dir object
extern NDS_Dir *NDS_Opendir(const char *path, const NDS_Rom *rom);

// Open RomFS parent directory and return dir object.
// NULL when root
extern NDS_Dir* NDS_OpenParentDir(const NDS_Dir *dirptr);

// Close RomFS dir oject
extern int NDS_Closedir(NDS_Dir *dirptr);

// Get directory contetn
extern NDS_Dirent* NDS_Readdir(NDS_Dir *dirptr);

// Seek to file entry offset feoffset
extern void NDS_Seekdir(NDS_Dir *dirptr, int16_t feoffset, uint8_t mode);


#ifdef __cplusplus
}
#endif
#endif