#include <filesystem/NDS_rom.h>
#include <NDS_logging.h>
#include <NDS_memory.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>




NDS_Rom* NDS_OpenRom(char *path)
{
    // Open file streams, allocate memory etc.
    FILE* romFile = fopen(path, "r");
    if(!romFile)
    {
        NDS_SetError("Failed to open rom path");
        return NULL;
    }

    NDS_Rom *rom;
    if((rom = (NDS_Rom*)NDS_Malloc(sizeof(NDS_Rom))) == NULL)
        return NULL;

    rom->romFileStream = romFile;
    fseek(romFile, 0, SEEK_SET);

    if(fread(&rom->header, sizeof(NDS_RomHeader), 1, romFile) != 1)
    {
        NDS_SetError("Failed to parse rom header. Could not read full rom header");
        return NULL;
    }

    

    return rom;
}

int NDS_CloseRom(NDS_Rom *rom)
{
    int8_t res = 0;
    // Close file streams, delete pointer etc.
    if(fclose(rom->romFileStream))
    {
        res = -1;
        NDS_SetError("Failed to close rom file stream");
    }

    free(rom);
    
    return res;
}

const NDS_RomHeader* NDS_GetRomHeader(const NDS_Rom *rom)
{
    NDS_CHECK_ROM_PTR(rom);
    return &rom->header;
}

/*
static int NDS_ParseRomHeader(NDS_RomHeader* header, FILE* file)
{
    uint8_t resCode = 0;
    resCode += fread(&header->gameTitle, 1, 12, file);             // Read game title
    resCode += fread(&header->gamecode, 1, 4, file);               // Read game code
    resCode += fread(&header->makerCode, 1, 2, file);              // Read maker code
    resCode += fread(&header->unitCode, 1, 1, file);               // Read unitCode
    resCode += fread(&header->encryptionSeedSelect, 1, 1, file);   // Read encryption seed select
    resCode += fread(&header->deviceCapacity, 1, 1, file);         // Read device capacity
    // uint8_t reserved[7];
    // uint8_t reserved;
    resCode += fseek(file, 8, SEEK_CUR);                           // Skip reserved
    resCode += fread(&header->ndsRegion, 1, 1, file);              // Read nds region
    resCode += fread(&header->romVersion, 1, 1, file);             // Read rom version
    resCode += fread(&header->autostart, 1, 1, file);              // Read autostart
    resCode += NDS_ReadByteFlipU32(&header->romOffsetArm9, file);          // Read rom offset of arm9
    resCode += NDS_ReadByteFlipU32(&header->entryAddressArm9, file);       // Read entry address of arm9
    resCode += NDS_ReadByteFlipU32(&header->ramAddressArm9, file);         // Read ram address of arm9
    resCode += NDS_ReadByteFlipU32(&header->sizeArm9, file);               // Read size of arm9
    resCode += NDS_ReadByteFlipU32(&header->romOffsetArm7, file);          // Read rom offset of arm7
    resCode += NDS_ReadByteFlipU32(&header->entryAddressArm7, file);       // Read entry address of arm7
    resCode += NDS_ReadByteFlipU32(&header->ramAddressArm7, file);         // Read ram address of arm7
    resCode += NDS_ReadByteFlipU32(&header->sizeArm7, file);               // Read size of arm7
    resCode += NDS_ReadByteFlipU32(&header->offsetFnt, file);              // Read offset of FNT FileNameTable
    resCode += NDS_ReadByteFlipU32(&header->sizeFnt, file);                // Read size of FNT
    resCode += NDS_ReadByteFlipU32(&header->offsetFat, file);              // Read offset of FAT FileAllocationTable
    resCode += NDS_ReadByteFlipU32(&header->sizeFat, file);                // Read size of FAT
    resCode += NDS_ReadByteFlipU32(&header->fileOverlayOffsetArm9, file);  // Read file overlay offset of arm9
    resCode += NDS_ReadByteFlipU32(&header->fileOverlaySizeArm9, file);    // Read file overlay size of arm9
    resCode += NDS_ReadByteFlipU32(&header->fileOverlayOffsetArm7, file);  // Read file overlay offset of arm7
    resCode += NDS_ReadByteFlipU32(&header->fileOverlaySizeArm7, file);    // Read file overlay size of arm7
    resCode += NDS_ReadByteFlipU32(&header->portSettingCommandsNormal, file);      //
    resCode += NDS_ReadByteFlipU32(&header->portSettingCommandsKey1, file);        //
    resCode += NDS_ReadByteFlipU32(&header->offsetIcon, file);                     // Read offset of game icon
    resCode += NDS_ReadByteFlipU16(&header->secureAreaChecksum, file);             //
    resCode += NDS_ReadByteFlipU16(&header->secureAreaDelay, file);                //
    resCode += NDS_ReadByteFlipU32(&header->autoLoadListHookRamAddressArm9, file); //
    resCode += NDS_ReadByteFlipU32(&header->autoLoadListHookRamAddressArm7, file); //
    resCode += NDS_ReadByteFlipU64(&header->secureAreaDisable, file);              //
    resCode += NDS_ReadByteFlipU32(&header->totalRomSize, file);                   //
    resCode += NDS_ReadByteFlipU32(&header->romHeaderSize, file);                  //
    // uint32_t unknown;
    // uint64_t reserved;
    resCode += fseek(file, 12, SEEK_CUR);                       // Skip reserved/unknown
    resCode += NDS_ReadByteFlipU16(&header->nandEndRomArea, file);     //
    resCode += NDS_ReadByteFlipU16(&header->nandStartRwArea, file);    //
    // uint8_t reserved[24];
    // uint8_t reserved[16];
    resCode += fseek(file, 40, SEEK_CUR);                      // Skip reserved
    resCode += fread(&header->nintendoLogo, 1, 156, file);     // Read nintendo logo (compressed bitmap)
    resCode += NDS_ReadByteFlipU16(&header->nintendoLogoChecksum, file);   // Read nintendo logo checksum
    resCode += NDS_ReadByteFlipU16(&header->headerChecksum, file);         // Read header checksum
    resCode += NDS_ReadByteFlipU32(&header->debugRomOffset, file);         // Read debug rom offset
    resCode += NDS_ReadByteFlipU32(&header->debugSize, file);              // Read debug size
    resCode += NDS_ReadByteFlipU32(&header->debugRamAddress, file);        // Read debug ram address
    // uint32_t reserved;
    // uint8_t reserved[144];
    // uint8_t reserved[3584];

    // Everything added should give this value
    if(resCode == 212)
        return 0;
    return resCode;
}
*/