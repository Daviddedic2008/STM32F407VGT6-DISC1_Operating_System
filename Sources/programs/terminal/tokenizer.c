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

#define NUMTOKENS 5 // change to add more ig

const char* tokenStrs[5] = {
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

/*
 * each possible op token will have a caller function that takes its arguments string as an input
 * called from here when a token arg pair is detected
 */

void (*callers[5])(token);

void assignCaller(void (*caller)(token), uint8_t idx){
	callers[idx] = caller;
}

char* src;
uint32_t len;

void setSrc(char* c){
	src = c;
	len = 0;
	for(;*c != '\n'; c++, len++){;} // get sz
}

uint8_t compToLiteral(token t, const char* lit){
	char* tmp = lit;
	for(uint8_t i = 0; i < t.sz; t.str++, tmp++, i++){
		if(*t.str != *tmp || *tmp == '\0'){
			return 0;
		}
	}
	if(*(++tmp) != '\0'){return 0;} // too long
	return 1;
}

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

void executeSrc(){
	token op = getNextToken();
	token args = {op.str + op.len, len-op.len};
	for(uint8_t i = 0; i < NUMTOKENS; i++){
		if(compToLiteral(op, tokenStrs[i])){
			// invoke caller
			callers[i](args);
		}
	}
}


