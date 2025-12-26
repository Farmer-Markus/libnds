#ifndef NDS_ROM_H_INTERNAL
#define NDS_ROM_H_INTERNAL

#include <stdint.h>
#include <stdio.h>

#define NDS_CHECK_ROM(rom)                      \
    if(!rom)                                    \
    {                                           \
        NDS_SetError("Given rom is invalid!");  \
        return -1;                              \
    }

#define NDS_CHECK_ROM_PTR(rom)                  \
    if(!rom)                                    \
    {                                           \
        NDS_SetError("Given rom is invalid!");  \
        return NULL;                            \
    }



#pragma pack(push, 1)
typedef struct NDS_RomHeader
{   // https://problemkaputt.de/gbatek.htm#dscartridgeheader
    char gameTitle[12];
    char gamecode[4];
    char makerCode[2];
    uint8_t unitCode;
    uint8_t encryptionSeedSelect;
    uint8_t deviceCapacity;
    uint8_t reserved1[7];       // zero filled
    uint8_t reserved2;
    uint8_t ndsRegion;
    uint8_t romVersion;
    uint8_t autostart;
    uint32_t romOffsetArm9;
    uint32_t entryAddressArm9;
    uint32_t ramAddressArm9;
    uint32_t sizeArm9;
    uint32_t romOffsetArm7;
    uint32_t entryAddressArm7;
    uint32_t ramAddressArm7;
    uint32_t sizeArm7;
    uint32_t offsetFnt;
    uint32_t sizeFnt;
    uint32_t offsetFat;
    uint32_t sizeFat;
    uint32_t fileOverlayOffsetArm9;
    uint32_t fileOverlaySizeArm9;
    uint32_t fileOverlayOffsetArm7;
    uint32_t fileOverlaySizeArm7;
    uint32_t portSettingCommandsNormal;
    uint32_t portSettingCommandsKey1;
    uint32_t offsetIcon;
    uint16_t secureAreaChecksum;
    uint16_t secureAreaDelay;
    uint32_t autoLoadListHookRamAddressArm9;
    uint32_t autoLoadListHookRamAddressArm7;
    uint64_t secureAreaDisable;
    uint32_t totalRomSize;
    uint32_t romHeaderSize;
    uint8_t unknown[4];
    uint8_t reserved3[8];
    uint16_t nandEndRomArea;
    uint16_t nandStartRwArea;
    uint8_t reserved4[24];
    uint8_t reserved5[16];
    uint8_t nintendoLogo[156];
    uint16_t nintendoLogoChecksum;
    uint16_t headerChecksum;
    uint32_t debugRomOffset;
    uint32_t debugSize;
    uint32_t debugRamAddress;
    uint8_t reserved6[4];       // zero filled
    uint8_t reserved7[144];     // zero filled
    uint8_t reserved8[3584];    // zero filled
} NDS_RomHeader;
#pragma pack(pop)


typedef struct NDS_Rom
{
    NDS_RomHeader header;
    FILE* romFileStream;

} NDS_Rom;


// Open rom object from path
// return NULL on fail
NDS_Rom* NDS_OpenRom(char *path);

// Close/Destroy rom object
// 0 on Success, -1 Error
int NDS_CloseRom(NDS_Rom *rom);

const NDS_RomHeader* NDS_GetRomHeader(const NDS_Rom *rom);


#endif