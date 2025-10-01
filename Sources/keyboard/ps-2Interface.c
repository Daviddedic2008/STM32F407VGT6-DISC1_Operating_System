/*
 * ps-2Interface.c
 *
 *  Created on: Sep 25, 2025
 *      Author: david
 */
#include "ps-2Interface.h"

void initEXTI(){
	ENABLE_PORT_B();
	ENABLE_FALLING_EDGE(6);
}

char recieveKeycode(){
	char keycode = 0; // fill bit by bit, send when 8 bits sent
	// assuming first start bit has been sent
	for(uint8_t bitsRead = 0; bitsRead < 8; bitsRead++){
		while(!CHECK_CLK()){
			;
		}
		CLEAR_EXTI_FLAG(6); // wait for next drop, dont re-sample
		keycode |= (READ_PS2_DATA()) << bitsRead;
	}
	for(uint8_t bitsRead = 0; bitsRead < 2; bitsRead++){
		while(!CHECK_CLK()){
			;
		}
	}
	return keycode;
}

