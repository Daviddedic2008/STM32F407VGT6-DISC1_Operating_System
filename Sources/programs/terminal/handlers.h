/*
 * handlers.h
 *
 *  Created on: Nov 18, 2025
 *      Author: david
 */

#ifndef SOURCES_PROGRAMS_TERMINAL_HANDLERS_H_
#define SOURCES_PROGRAMS_TERMINAL_HANDLERS_H_

#include "tokenizer.h"
#include "../bootScreen.h"
#include "../../flash/flashManager.h"
#include "../fileViewer.h"

void exitHandler(args a){
	freeArgs(a);
	startupScreen(); // just go to startup screen
}

void resetHandler(args a){
	freeArgs(a);
	resetFlash();
}

void saveHandler(args a){
	freeArgs(a);
	saveMetaBuf();
}

void openFileHandler(args a){

}

#endif /* SOURCES_PROGRAMS_TERMINAL_HANDLERS_H_ */
