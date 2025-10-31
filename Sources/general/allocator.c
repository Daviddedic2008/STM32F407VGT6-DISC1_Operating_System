/*
 * allocator.c
 *
 *  Created on: Oct 23, 2025
 *      Author: david
 */
#include <stdint.h>
#include "../lcd/screenDriver.h"

#define TAKEN 1
#define FREE 0
#define MAX 10000

uint32_t heap[MAX]; // 40KB bytes of heap, used for loading files and OS mem allocations
uint8_t reserved[MAX/8]; // each bit for one uint32

#define reserveWord(word) reserved[word / 8] |= (1 << (word % 8))
#define freeWord(word) reserved[word / 8] &= ~(1 << (word % 8))
#define checkWord(word) (reserved[word / 8] & 1 << (word % 8))
#define checkBlock(blockStart) (reserved[blockStart / 8] != 0)
#define reserveBlock(blockStart) reserved[blockStart] = ~(unsigned char)0

void initHeap(){
	for(uint16_t i = 0; i < MAX; i++, heap[i] = 0){}
}

void* alloc(const uint32_t sz){
	// no defrag simple implementation
	uint32_t found = 0;
	uint16_t loc = 0;
	printNum(sz);
	while(found < sz && loc < MAX){
		if((sz - found) > (8*4)){
			if(checkBlock(loc)){
				goto restart;
			}
			found += 8 * 4;
			loc += 8;
			continue;
		}
		if(checkWord(loc)){
			goto restart;
		}
		found += 4;
		loc++;
		continue;
		restart:
		found = 0;
		loc++;
	}
	for(uint16_t i = 0; i < sz; i++){
		reserveWord(loc + i);
	}
	return (void*)(heap + loc);
}

void discard(void* ptr){
	for(uint16_t i = 0; i < MAX; i++){
		reserved[i] = 0; // free all for now
	}
}
