/*
 * allocator.h
 *
 *  Created on: Oct 30, 2025
 *      Author: david
 */

#ifndef SOURCES_PROGRAMS_ALLOCATOR_H_
#define SOURCES_PROGRAMS_ALLOCATOR_H_
#include <stdint.h>

void discard(const void* ptr);

void* alloc(const uint32_t sz);

void memcopy(void* src, void* dest, const uint32_t sz);

void initHeap();

#endif /* SOURCES_PROGRAMS_ALLOCATOR_H_ */
