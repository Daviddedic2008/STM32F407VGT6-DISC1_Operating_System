/*
 * tokenizer.h
 *
 *  Created on: Nov 17, 2025
 *      Author: david
 */

#include "tokenizer.h"
#include "../../general/allocator.h"
#include <stdint.h>

enum tokens{
	NEW,
	QUIT,
	OPEN,
	EDIT,
	RESET
};

const char* tokenStrs[] = {
		"new",
		"quit",
		"open",
		"edit",
		"reset"
};

typedef struct{
	char* str;
	unsigned char len;
}token;

char* src;

void setSrc(char* c);

token getNextToken(){
	for(;*src == ' ';src++){}
	char* tc = src;
	for(;*tc != ' ';tc++){}
	const uint32_t sz = tc-src;
	char* retp = alloc(sz);
	for(uint32_t i = 0; i < sz; i++){
		retp[i] = src[i];
	}
	const token ret = {retp, sz};
	return ret;
}

