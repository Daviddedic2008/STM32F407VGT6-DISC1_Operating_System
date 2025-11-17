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
 * flash addr 0(0x800....0) CANNOT BE WRITTEN TO! Is it write-protected or something? actually not sure about this one..
 */

unsigned char sectorDir;

#define swapDir() sectorDir = sectorDir == bd1 ? bd2 : bd1
#define bufDir ((sectorDir == bd1) * bd2 + (sectorDir == bd2) * bd1)

void* pkgMetaBuf; // ram buffer for sector 0. At each reset, flash controller will load all flash package metadata into ram
// it is loaded into ram to prevent temp structs/buffers the compiler makes from fucking up the flash reads
// also done to increase speed slightly
uint32_t numPkg, flashUsed;

const uint32_t flash_sector_offset[] = {
    0x00000+FLASHSTART,  // Sector 0 - 16 KB
    0x04000+FLASHSTART,  // Sector 1 - 16 KB
    0x08000+FLASHSTART,  // Sector 2 - 16 KB
    0x0C000+FLASHSTART,  // Sector 3 - 16 KB
    0x10000+FLASHSTART,  // Sector 4 - 64 KB
    0x20000+FLASHSTART,  // Sector 5 - 128 KB
    0x40000+FLASHSTART,  // Sector 6 - 128 KB
    0x60000+FLASHSTART,  // Sector 7 - 128 KB
    0x80000+FLASHSTART,  // Sector 8 - 128 KB
    0xA0000+FLASHSTART,  // Sector 9 - 128 KB
    0xC0000+FLASHSTART,  // Sector 10 - 128 KB
    0xE0000+FLASHSTART   // Sector 11 - 128 KB
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
	while (FLASH_SR & (1 << 16)){} // wait for flash controller to erase sector
	FLASH_CR &= ~(1 << 1); // clear sector erase bit
	// check and clear end of operation op
	if(FLASH_SR & 1){
		FLASH_SR = 1;
	}
}

void writeWordToFlash(const uint32_t addr, const uint32_t val){
	FLASH_ACR &= ~(1 << 9);  // Disable ICEN
	FLASH_ACR &= ~(1 << 10); // Disable DCEN
	__disable_irq();
	FLASH_CR |= 1; // set programming bit(allow write to flash)
	// now flash is open
	(*(volatile uint32_t*)addr) = val;
	while (FLASH_SR & (1 << 16));// wait for flash controller to write val
	FLASH_CR &= ~1; // clear programming bit(disallow write to flash)
	__enable_irq();
	FLASH_ACR |= (1 << 9);  // enable ICEN
	FLASH_ACR |= (1 << 10); // enable DCEN
}

void resetFlash(){
	prepareSector(0);
	writeWordToFlash(NUMPKG, 0);
	writeWordToFlash(FLASHUSED, 0);
	writeWordToFlash(MAINSECTOR, bd1);
}

void writeDataToFlash(const uint32_t addr, const uint32_t* val, const uint32_t wrSz){
	FLASH_ACR &= ~(1 << 9);  // Disable ICEN
	FLASH_ACR &= ~(1 << 10); // Disable DCEN
	__disable_irq();
	FLASH_CR |= 1; // set programming bit(allow write to flash)
	// now flash is open
	for(uint32_t idx = 0; idx < wrSz/4; idx++){
		(*((volatile uint32_t*)(addr)+idx)) = val[idx];
		while (FLASH_SR & (1 << 16)); // wait for flash controller to write data
	}
	FLASH_CR &= ~1; // set programming bit(allow write to flash)
	__enable_irq();
	FLASH_ACR |= (1 << 9);  // enable ICEN
	FLASH_ACR |= (1 << 10); // enable DCEN
}

void* writeFlashToRamBuffer(const uint32_t addr, const uint32_t wrSz){
	void* ret = alloc(wrSz);
	for(uint32_t o = 0; o < wrSz/4; o++){
		((uint32_t*)ret)[o] = *(volatile uint32_t*)(addr+o*4);
	}
	return ret;
}

void allocMetadataBuf(){
	numPkg = *((volatile uint32_t*)NUMPKG);
	flashUsed = *((volatile uint32_t*)FLASHUSED);
	pkgMetaBuf = writeFlashToRamBuffer(STARTPKG, numPkg * sizeof(flashPkg));
	sectorDir = *((volatile uint32_t*)MAINSECTOR);
}

void writeToPkg(const char c, const uint32_t* ptr, const uint32_t startOffset, const uint32_t sz){
	const flashPkg p = retrievePkg(c);
	prepareSector(bufDir);
	for(uint16_t off = 0; off < flashUsed; off+=4){
		uint32_t wrWord;
		if(off >= p.addr + startOffset && off < p.addr+startOffset+sz && off < p.addr+p.sz){
			wrWord = ptr[(off-p.addr)/4];
		}
		else{
			wrWord = *(volatile uint32_t*)(flash_sector_offset[sectorDir]+off);
		}
		writeWordToFlash(flash_sector_offset[bufDir] + off, wrWord);
	}
	swapDir();
}

void writeDataToSector(const uint32_t addr, const uint8_t sector, const uint32_t* val, const uint32_t wrSz){
	writeDataToFlash(addr + flash_sector_offset[sector], val, wrSz);
}

void addFlashPkg(const uint32_t size, const char name){
	const uint32_t startAddr = flashUsed;
	// clear sector to increment numPkg and flashUsed
	prepareSector(0); // clear it wohoo
	// set flash used and numpkg
	flashUsed+=size;
	numPkg++;
	// all written :)
	// write new one
	const flashPkg add = {name, startAddr, size};
	((flashPkg*)pkgMetaBuf)[numPkg-1] = add;
	// should all be written
}

flashPkg retrievePkg(const char name){
	int np = numPkg;
	flashPkg* pkgs = (flashPkg*)pkgMetaBuf;
	for(; np>0; pkgs++, np--){
		if(pkgs->name == name){
			return *pkgs;
		}
	}
	const flashPkg ret = {'~', 0, 0};
	return ret;
}

void* loadPkgIntoRam(const char name){
	const flashPkg tmp = retrievePkg(name);
	if(tmp.name == '~'){return (void*)0;}
	return writeFlashToRamBuffer(tmp.addr, tmp.sz);
}

volatile void* readDataSafe(const uint16_t addr){
	return (volatile void*)(flash_sector_offset[sectorDir] + addr);
}

void removePkg(const char name){
	int np = 0;
	if(numPkg == 0){
		return;
	}
	flashPkg* pkgs = (flashPkg*)pkgMetaBuf;
	for(; pkgs->name != name && np < numPkg; pkgs++, np++){;}
	// found pkg, have access to npc and pkg*

	// remove from pkg metadata buffer
	memcopy((uint8_t*)pkgMetaBuf + sizeof(flashPkg)*(np+1), (uint8_t*)pkgMetaBuf + sizeof(flashPkg)*np, (numPkg-np) * sizeof(flashPkg));
	numPkg--;
}

void compressPkgs(){
	uint32_t numPkg = *(volatile uint32_t*)NUMPKG;
	flashPkg* pkgs = (flashPkg*)pkgMetaBuf;
	uint32_t offset = 0; // total memory actually used by files
	prepareSector(bufDir);
	// compress by re-writing files to other sector,
	for(int np = numPkg; np != -1; pkgs++, np--){
		// start loc is offset
		offset += pkgs->sz;
		pkgs->addr = offset;
		writeDataToFlash(flash_sector_offset[bufDir] + offset, (uint32_t*)pkgs, pkgs->sz);
	}
	flashUsed = offset;
}

void saveMetaBuffer(){
	// call when resetting rtos or turning it off

	// write ram buffer to flash
	prepareSector(0);
	writeWordToFlash(FLASHUSED, flashUsed);
	writeWordToFlash(NUMPKG, numPkg);
	writeDataToFlash(STARTPKG, pkgMetaBuf, sizeof(flashPkg)*numPkg);
	writeWordToFlash(MAINSECTOR, sectorDir);
}
