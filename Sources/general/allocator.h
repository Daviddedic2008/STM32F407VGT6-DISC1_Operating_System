/*
 * allocator.h
 *
 *  Created on: Oct 30, 2025
 *      Author: david
 */

#ifndef SOURCES_PROGRAMS_ALLOCATOR_H_
#define SOURCES_PROGRAMS_ALLOCATOR_H_

void discard(void* ptr);

void* alloc(const uint32_t sz);

void initHeap();

#endif /* SOURCES_PROGRAMS_ALLOCATOR_H_ */
