#include "../flash/flashManager.h"
#include "../general/allocator.h"
#include "../lcd/screenDriver.h"
#include "../keyboard/ps-2Interface.h"
#include "fileViewer.h"
#include "terminal/terminal.h"

void displayFileTxt(const char name){
	underlined = 0;
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

void editFileTxt(const char name, const uint8_t pg){
;
}

void displayFilePicture(){
	; // add later
}

void displayFileAsm(){
	// boring, add later
}
