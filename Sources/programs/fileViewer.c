#include "../flash/flashManager.h"
#include "../lcd/screenDriver.h"
#include "../keyboard/ps-2Interface.h"
#include "fileViewer.h"
#include "terminal/terminal.h"

void displayFileTxt(const char name){
	clearLCD();
	moveCursor(0,0);
	const char* s1 = "+----------------------------+\n|          TXT VIEW          |\n+----------------------------+\n\n";
	putString(s1, 94);
	flashPkg p = retrievePkg(name);
	uint16_t off = 0;
	while(off < p.sz){
		if(off == 1200){
			p.sz -= 1200;
			off = 0;
			idleUntilPress();
			clearLCD();
			moveCursor(0,0);
		}
		putChar(*(volatile char*)readDataSafe(p.addr + off));
		off++;
	}
}

void editFilePg(const char name, const uint8_t pg){
	clearLCD();
	moveCursor(0,0);
	const char* s1 = "+----------------------------+\n|          TXT EDIT          |\n+----------------------------+\n\n";
	putString(s1, 94);
	flashPkg p = retrievePkg(name);
	uint16_t off = pg * 30 * 40;
	while(off < p.sz && off < (pg+1) * 30 * 40){
		putChar(*(volatile char*)readDataSafe(p.addr + off));
		off++;
	}
	while(1){
		const char c = idleUntilNextChar();
		if(c == '~'){break;} // save or dont save
		putChar(c);
	}
	clearLCD_b();
	const char* s2 = "+----------------------------+\n|          save y/n          |\n+----------------------------+\n\n";
	putString(s2, 94);
	while(1){
		const char c = idleUntilNextChar();
		if(c == 'y'){
			writeToPkg(p, (void*)&screenBuf[0][0], pg * 30 * 40, 30*40);
			return;
		}
	}
}

void displayFilePicture(){
	; // add later
}

void displayFileAsm(){
	// boring, add later
}
