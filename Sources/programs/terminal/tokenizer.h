/*
 * tokenizer.h
 *
 *  Created on: Nov 17, 2025
 *      Author: david
 */

#ifndef SOURCES_PROGRAMS_TERMINAL_TOKENIZER_H_
#define SOURCES_PROGRAMS_TERMINAL_TOKENIZER_H_

enum tokens{
	NEW,
	QUIT,
	OPEN,
	EDIT,
	RESET,
	SAVE
};

#define NUMTOKENS 6 // change to add more ig

typedef struct{
	char* str;
	unsigned char len;
}token;

typedef struct{
	token* params;
	unsigned char numParams;
}args;

void assignCaller(void (*caller)(args), uint8_t idx);

void executeSrc();

void freeToken(token t);

void freeArgs(args a);

const char* tokenStrs[NUMTOKENS] = {
		"new",
		"quit",
		"open",
		"edit",
		"reset",
		"save"
};

#endif /* SOURCES_PROGRAMS_TERMINAL_TOKENIZER_H_ */
