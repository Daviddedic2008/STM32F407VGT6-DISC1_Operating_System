#include "../general/mcuHeader.h"
#include "asmRoutines.h"
#include "flashManager.h"
#include "../lcd/screenDriver.h"
#include "../general/allocator.h"

#define SETSECTOR(sector) FLASH_CR &= ~(0xF << 3); /*clear sector bits 3-6*/ FLASH_CR |= (sector << 3)
#define SETSECTORERASE() FLASH_CR |= 2 //
#define BUFFERSECTOR 11
#define LOCKFLASH() FLASH_CR |= (1 << 31)
#define WRITEERROR() (FLASH_SR & ((1 << 2) | (1 << 4)))

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
/*
 * NOTES TO SELF TO REMEMBER
 * flash programming requires that you first specify the size of each write in the pwrite bits(setPwrite())
 * only have to unlock flash once at the beginning of the program
 * disable interrupts while writing just in case debugger decides to act up(not important for final build)
 * this short flash controller uses 32 bit writes, so adresses have to be aligned to that!!!!
 */

unsigned char sectorDir = 10;

#define swapDir() sectorDir = sectorDir == 10 ? 11 : 10
#define bufDir ((sectorDir == 10) * 11 + (sectorDir == 11) * 10)

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

void unlockFlash(){
	FLASH_KEYR = 0x45670123;
	FLASH_KEYR = 0xCDEF89AB; // idk what this is for tbh
}

void setPsize(){
	// 32 bit write size, must be clarified for flash controller to recognize 32 bit writes
	// write 0b10 into [9:8} of flash_cr without changing rest
	uint32_t ogcr = FLASH_CR;
	// (3<<8) is used to clear WHOLE psize mask, not just the msb(9)
	FLASH_CR = (ogcr & ~(3 << 8)) | (2 << 8);
}

void prepareSector(const unsigned char sector){
	// clears sector and sets it to the write domain
	// must be done before each write
	SETSECTORERASE();
	SETSECTOR(sector);
	FLASH_CR |= (1 << 16); // send start signal to begin erase(almost forgot this oops)
	int i = 0;
	while (FLASH_SR & (1 << 16)){i++;} // wait for flash controller to erase sector
	FLASH_CR &= ~(1 << 1); // clear sector erase bit
	// check and clear end of operation op
	if(FLASH_SR & 1){
		FLASH_SR = 1;
	}
}

void writeWordToFlash(const uint32_t addr, const uint32_t val){
	__disable_irq();
	FLASH_CR |= 1; // set programming bit(allow write to flash)
	// now flash is open
	(*(volatile uint32_t*)addr) = val;
	while (FLASH_SR & (1 << 16));// wait for flash controller to write val
	FLASH_CR &= ~1; // clear programming bit(disallow write to flash)
	__enable_irq();
}

void resetFlash(){
	prepareSector(0);
	writeWordToFlash(NUMPKG, 0);
	writeWordToFlash(FLASHUSED, 0);
}

void writeDataToFlash(const uint32_t addr, const uint32_t* val, const uint32_t wrSz){
	__disable_irq();
	FLASH_CR |= 1; // set programming bit(allow write to flash)
	// now flash is open
	for(uint32_t idx = 0; idx < wrSz/4; idx++){
		(*((volatile uint32_t*)(addr)+idx)) = val[idx];
		while (FLASH_SR & (1 << 16)); // wait for flash controller to write data
	}
	FLASH_CR &= ~1; // set programming bit(allow write to flash)
	__enable_irq();
}

void* writeFlashToRamBuffer(const uint32_t addr, const uint32_t wrSz){
	void* ret = alloc(wrSz);
	for(uint32_t o = 0; o < wrSz/4; o++){
		((uint32_t*)ret)[o] = *(volatile uint32_t*)(addr+o);
	}
	return ret;
}

void writeDataToSector(const uint32_t addr, const uint8_t sector, const uint32_t* val, const uint32_t wrSz){
	writeDataToFlash(addr + flash_sector_offset[sector], val, wrSz);
}

void addFlashPkg(const uint32_t size, const char name){
	const uint32_t flashUsed = *(volatile uint32_t*)(FLASHUSED);
	const uint32_t pkgsAllocated = *(volatile uint32_t*)(NUMPKG);
	const uint32_t startAddr = flashUsed;
	uint32_t* buf = writeFlashToRamBuffer(STARTPKG, pkgsAllocated * sizeof(flashPkg));
	// clear sector to increment numPkg and flashUsed
	prepareSector(0); // clear it wohoo
	// set flash used and numpkg
	writeWordToFlash(FLASHUSED, flashUsed + size);
	writeWordToFlash(NUMPKG, pkgsAllocated + 1);
	if(pkgsAllocated != 0){
		writeDataToFlash(STARTPKG, buf, pkgsAllocated * sizeof(flashPkg));
	}
	// all written :)
	// write new one
	const flashPkg add = {name, startAddr, size};
	writeDataToFlash(STARTPKG + pkgsAllocated * sizeof(flashPkg), (uint32_t*)&add, sizeof(flashPkg));
	discard(buf);
	// should all be written
}

flashPkg retrievePkg(const char name){
	int np = *(volatile uint32_t*)NUMPKG;
	flashPkg* pkgs = (flashPkg*)STARTPKG;
	for(; pkgs->name != name; pkgs++, np--){
		if(np == -1){
			const flashPkg ret = {'~', 0, 0};
			return ret;
		}
	}
	return *pkgs;
}

void removePkg(const char name){
	int np = *(volatile uint32_t*)NUMPKG;
	const int npb = np; // save for later
	if(np == 0){
		return;
	}
	uint32_t flashUsed = *(volatile uint32_t*)FLASHUSED;
	flashPkg* pkgs = (flashPkg*)STARTPKG;
	for(; pkgs->name != name; pkgs++, np--){
		if(np == -1){
			return;
		}
	}
	// found pkg, have access to np and pkg*

	// remove from pkg metadata
	// dont reduce flashUsed as it is onyl reduced when compressing
	uint32_t* buf = writeFlashToRamBuffer(STARTPKG, np * sizeof(flashPkg));
	prepareSector(0);
	writeWordToFlash(FLASHUSED, flashUsed);
	writeWordToFlash(NUMPKG, npb-1);
	writeDataToFlash(STARTPKG, buf, sizeof(flashPkg) * np);
	writeDataToFlash(STARTPKG + sizeof(flashPkg) * np + sizeof(flashPkg), (uint32_t*)((flashPkg*)buf + npb), (npb-np-1) * sizeof(flashPkg));
}

void compressPkgs(){
	uint32_t numPkg = *(volatile uint32_t*)NUMPKG;
	flashPkg* pkgs = writeFlashToRamBuffer(STARTPKG, sizeof(flashPkg) * numPkg);
	prepareSector(0); // sadly gotta rewrite
	writeWordToFlash(NUMPKG, numPkg);
	uint32_t offset = 0; // total memory actually used by files
	prepareSector(bufDir);
	// compress by re-writing files to other sector,
	for(int np = numPkg; np != -1; pkgs++, np--){
		// start loc is offset
		offset += pkgs->sz;
		flashPkg tmp = {pkgs->name, offset + flash_sector_offset[sectorDir], pkgs->sz};
		writeDataToFlash(STARTPKG + sizeof(flashPkg) * np, (uint32_t*)&tmp, sizeof(flashPkg));
		writeDataToFlash(flash_sector_offset[bufDir] + offset, (uint32_t*)pkgs, pkgs->sz);
	}
	writeWordToFlash(FLASHUSED, offset);
}

void writeToFileSpace(const char filename, const uint32_t* data){
	const flashPkg p = retrievePkg(filename);
	writeDataToSector(p.addr, 11, data, p.sz);
}
