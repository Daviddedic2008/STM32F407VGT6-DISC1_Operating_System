#include "../general/mcuHeader.h"
#include "asmRoutines.h"
#include <stdlib.h>

#define SETSECTOR(sector) FLASH_CR &= ~(0xF << 3); /*clear sector bits 3-6*/ FLASH_CR |= (sector << 3)
#define SETSECTORERASE() FLASH_CR |= 2 //
#define BUFFERSECTOR 11
#define LOCKFLASH() FLASH_CR |= (1 << 31);

#define START_FLASH 0x08000000

// This file was made as I didnt have the willpower to set up actual flash controller headers

/*
 * Here is my summary of flash programming on the STM board:
 *
 * Sectors(128KB) of flash are manipulated at a time
 * Flash memory can only switch states from 1->0
 * To reset all memory cells to 1, a sector-wide erase must be performed BEFORE writing to flash
 * Writing to flash incurs significant overhead,(40 ns per 32 bit word) due to the flash controller being stupid
 * My controller for flash isnt the best :)
 * Although memory is unified on the M4 in the sense that the same opcodes access flash, ram, registers, etc,
*/

const uint32_t flash_sector_offset[] = {
    0x00000,  // Sector 0 - 16 KB
    0x04000,  // Sector 1 - 16 KB
    0x08000,  // Sector 2 - 16 KB
    0x0C000,  // Sector 3 - 16 KB
    0x10000,  // Sector 4 - 64 KB
    0x20000,  // Sector 5 - 128 KB
    0x40000,  // Sector 6 - 128 KB
    0x60000,  // Sector 7 - 128 KB
    0x80000,  // Sector 8 - 128 KB
    0xA0000,  // Sector 9 - 128 KB
    0xC0000,  // Sector 10 - 128 KB
    0xE0000   // Sector 11 - 128 KB
};

inline void unlockFlash(const unsigned char sector){
	FLASH_KEYR = 0x45670123;
	FLASH_KEYR = 0xCDEF89AB; // idk what this is for tbh
	SETSECTORERASE();
	SETSECTOR(sector);
	while (FLASH_SR & (1 << 16)); // wait for flash controller to erase sector
	FLASH_CR &= ~(1 << 1); // clear sector erase bit
}

inline void writeWordToFlash(const uint32_t addr, const uint32_t val){
	FLASH_CR |= 1; // set programming bit(allow write to flash)
	// now flash is open
	(*(volatile uint32_t*)addr) = val;
	while (FLASH_SR & (1 << 16)); // wait for flash controller to write data
	FLASH_CR &= ~1; // set programming bit(allow write to flash)
}

inline void writeDataToFlash(const uint32_t addr, const uint32_t* val, const uint32_t wrSz){
	FLASH_CR |= 1; // set programming bit(allow write to flash)
	// now flash is open
	for(uint32_t idx = 0; idx < wrSz; idx++){
		(*(volatile uint32_t*)addr) = val[idx];
		while (FLASH_SR & (1 << 16)); // wait for flash controller to write data
	}
	FLASH_CR &= ~1; // set programming bit(allow write to flash)
}

inline void* writeFlashToRamBuffer(const uint32_t addr, const uint32_t wrSz){
	void* ret = malloc(sizeof(uint32_t)*wrSz);
	for(uint32_t o = 0; o < wrSz; o++){
		((uint32_t*)ret)[o] = *(volatile uint32_t*)addr;
	}
}

inline void writeDataToSector(const uint32_t addr, const uint8_t sector, const uint32_t* val, const uint32_t wrSz){
	writeDataToFlash(addr + flash_sector_offset[sector], val, wrSz);
}

typedef struct {
	uint32_t addr, sz;
} flashPkg;

inline void addFlashPkg(const uint32_t size, const uint8_t sector){
;
}
