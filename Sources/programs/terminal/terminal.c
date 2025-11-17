/*
 * terminal.c
 *
 *  Created on: Oct 2, 2025
 *      Author: david
 */
#include "../../lcd/screenDriver.h"
#include "../../keyboard/ps-2Interface.h"
#include "../bootScreen.h"

char ln[30];

static inline void clearLn(){
	for(unsigned char i = 0; i < 30; i++, ln[i]=0){;}
}

// commands
#define EXIT_CMD 0

static inline unsigned char getCmd(){
	if(ln[0]=='e' && ln[1] == 'x' && ln[2] == 'i' && ln[3] == 't'){
		return EXIT_CMD;
	}
	return -1;
}

static inline void runCmd(){
	unsigned char c = getCmd();
	switch(c){
	case EXIT_CMD:
		startupScreen();
		break;
	default:
		changeColor(0xA000);
		putString("command does not exist!\n", 24);
		changeColor(0x0AA0);
	}
}

void beginTerminal(){
	underline = 1;
	changeColor(0x0AA0);
	changeBackdrop(0x0000);
	clearLCD();
	moveCursor(0,0);
	const char* s1 = "+----------------------------+\n|          TERMINAL          |\n+----------------------------+\n\n";
	putString(s1, 94);
	putChar('>');
	while(1){
		char c = idleUntilNextChar();
		ln[getRow()] = c;
		putChar(c);
		if(c == '\n'){
			runCmd();
			putChar('>');
			clearLn();
			// run command, do later
		}

	}
}
