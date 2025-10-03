/*
 * terminal.c
 *
 *  Created on: Oct 2, 2025
 *      Author: david
 */
#include "../lcd/screenDriver.h"

void beginTerminal(){
	changeColor(0x0AA0);
	changeBackdrop(0x0000);
	clearLCD();
	moveCursor(0,0);
	const char* s1 = "+----------------------------+\n|          TERMINAL          |\n+----------------------------+\n\n";
	putString(s1, 94);
	putChar('>');
	highlightCursor();
	while(1){
		;
	}
}
