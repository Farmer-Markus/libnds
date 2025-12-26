#ifndef NDS_ROM_H
#define NDS_ROM_H

// src/filesystem/NDS_rom.h/c
typedef struct NDS_Rom NDS_Rom;


// Noch komplett rein kopieren!
typedef struct NDS_RomHeader NDS_RomHeader;

extern NDS_Rom* NDS_OpenRom(char* path);
extern int NDS_CloseRom(NDS_Rom *rom);

extern NDS_RomHeader* NDS_GetRomHeader(const NDS_Rom* rom);




#endif