/*
 * flashManager.h
 *
 *  Created on: Oct 14, 2025
 *      Author: david
 */

#ifndef SOURCES_FLASH_FLASHMANAGER_H_
#define SOURCES_FLASH_FLASHMANAGER_H_
#include <stdint.h>

#define writeToPkgUint32(pkg, loc, val) *(volatile uint32_t*)(pkg.addr+loc) = val
#define writeToPkgUint8(pkg, loc, val) *(volatile uint8_t*)(pkg.addr+loc) = val

#define readFromPkgUint32(pkg, loc, val) (*(volatile uint32_t*)(pkg.addr+loc))
#define readFromPkgUint8(pkg, loc, val) (*(volatile uint8_t*)(pkg.addr+loc))

#pragma pack(push,4) // default pack
typedef struct {
	char name; // for aligned access
	uint32_t addr;
	uint32_t sz;
} flashPkg;
#pragma pack(pop)

void addFlashPkg(const uint32_t size, const char name);

void resetFlash();

void unlockFlash();

flashPkg retrievePkg(const char name);

void removePkg(const char name);

void compressPkgs();

void setPsize();

volatile void* readDataSafe(const uint16_t addr);

void writeToPkg(const flashPkg p, const uint32_t* ptr, const uint32_t offset, const uint32_t sz);

void allocMetadataBuf();

void saveMetaBuffer();

#endif /* SOURCES_FLASH_FLASHMANAGER_H_ */
