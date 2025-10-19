#include "../flash/flashManager.h"
#include "../lcd/screenDriver.h"
#include "../keyboard/ps-2Interface.h"
#include "terminal.h"

void displayFileTxt(const char name){
	clearLCD();
	flashPkg p = retrievePkg(name);
	const uint32_t sz1 = p.sz > 1200 ? p.sz : 1200;
	uint32_t sz2 = p.sz-1200;
	sz2 = (sz2 > 0) * sz2;
	uint32_t add = p.addr;
	for(; add < p.addr + sz1; add++){
		putChar(*(volatile char*)(add));
	}
	while(attemptRecieve() == -1){
		;
	}
	clearLCD();
	for(; add < p.addr + sz2; add++){
		putChar(*(volatile char*)(add));
	}
	while(attemptRecieve() == -1){
		;
	}
	beginTerminal();
}

void displayFilePicture(){
	; // add later
}

void displayFileAsm(){
	// boring, add later
}
