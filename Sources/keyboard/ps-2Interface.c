/*
 * ps-2Interface.c
 *
 *  Created on: Sep 25, 2025
 *      Author: david
 */
#include "ps-2Interface.h"

const char ascii_unshifted[128] = {
[0x1C] = 'a'
[0x32] = 'b'
[0x21] = 'c'
[0x23] = 'd'
[0x24] = 'e'
[0x2B] = 'f'
[0x34] = 'g'
[0x33] = 'h'
[0x43] = 'i'
[0x3B] = 'j'
[0x42] = 'k'
[0x4B] = 'l'
[0x3A] = 'm'
[0x31] = 'n'
[0x44] = 'o'
[0x4D] = 'p'
[0x15] = 'q'
[0x2D] = 'r'
[0x1B] = 's'
[0x2C] = 't'
[0x3C] = 'u'
[0x2A] = 'v'
[0x1D] = 'w'
[0x22] = 'x'
[0x35] = 'y'
[0x1A] = 'z'
[0x45] = '0'
[0x16] = '1'
[0x1E] = '2'
[0x26] = '3'
[0x25] = '4'
[0x2E] = '5'
[0x36] = '6'
[0x3D] = '7'
[0x3E] = '8'
[0x46] = '9'
[0x29] = ' '
[0x5A] = '\n'
[0x66] = '\b'
};

const char ascii_shifted[128] = {
[0x1C] = 'A'
[0x32] = 'B'
[0x21] = 'C'
[0x23] = 'D'
[0x24] = 'E'
[0x2B] = 'F'
[0x34] = 'G'
[0x33] = 'H'
[0x43] = 'I'
[0x3B] = 'J'
[0x42] = 'K'
[0x4B] = 'L'
[0x3A] = 'M'
[0x31] = 'N'
[0x44] = 'O'
[0x4D] = 'P'
[0x15] = 'Q'
[0x2D] = 'R'
[0x1B] = 'S'
[0x2C] = 'T'
[0x3C] = 'U'
[0x2A] = 'V'
[0x1D] = 'W'
[0x22] = 'X'
[0x35] = 'Y'
[0x1A] = 'Z'
[0x45] = ')'
[0x16] = '!'
[0x1E] = '@'
[0x26] = '#'
[0x25] = '$'
[0x2E] = '%'
[0x36] = '^'
[0x3D] = '&'
[0x3E] = '*'
[0x46] = '('
[0x29] = ' '
[0x5A] = '\n'
[0x66] = '\b'
};

unsigned char toggles = 0;
// bit 0 : shift
// bit 1 : caps lock
// bit 2 : break active

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

char convertKeycode(const char keycode){
	if(keycode == 0xF0){
		toggles |= (1 << 2); // set break
		return -1; // null
	}
	if(keycode == 0x12 || keycode == 0x59){
		toggles = (~((toggles >> 2) & 1) & 1) | (toggles & ~1); // if break, release shift. if not, set shift
	}
	if((toggles >> 2) & 1){
		toggles &= ~(1 << 2); // clear break if set
		return -1; // if break, just return the rest is useless
	}
	if(toggles & 1){
		// if shift, return shifted
		return ascii_shifted[keycode];
	}
	return ascii_unshifted[keycode]; // last possibility
}

char recieveChar(){
	return convertKeycode(recieveChar());
}

char attemptRecieve(){
	if(!CHECK_CLK()){
		return -1;
	}
	return recieveChar();
}
