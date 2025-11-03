/*
 * flashManager.h
 *
 *  Created on: Oct 14, 2025
 *      Author: david
 */

#ifndef SOURCES_FLASH_FLASHMANAGER_H_
#define SOURCES_FLASH_FLASHMANAGER_H_
#include <stdint.h>
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

#endif /* SOURCES_FLASH_FLASHMANAGER_H_ */
