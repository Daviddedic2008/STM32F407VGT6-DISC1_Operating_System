/*
 * ps-2Interface.c
 *
 *  Created on: Sep 25, 2025
 *      Author: david
 */
#include "ps-2Interface.h"

/* full 128-byte arrays (initialized to 0), then populate the mapped entries */
const char ascii_unshifted[128] = { 0 };
const char ascii_shifted[128]   = { 0 };

static void init_ascii_maps(void)
{
    ((char *)ascii_unshifted)[0x1C] = 'a';
    ((char *)ascii_unshifted)[0x32] = 'b';
    ((char *)ascii_unshifted)[0x21] = 'c';
    ((char *)ascii_unshifted)[0x23] = 'd';
    ((char *)ascii_unshifted)[0x24] = 'e';
    ((char *)ascii_unshifted)[0x2B] = 'f';
    ((char *)ascii_unshifted)[0x34] = 'g';
    ((char *)ascii_unshifted)[0x33] = 'h';
    ((char *)ascii_unshifted)[0x43] = 'i';
    ((char *)ascii_unshifted)[0x3B] = 'j';
    ((char *)ascii_unshifted)[0x42] = 'k';
    ((char *)ascii_unshifted)[0x4B] = 'l';
    ((char *)ascii_unshifted)[0x3A] = 'm';
    ((char *)ascii_unshifted)[0x31] = 'n';
    ((char *)ascii_unshifted)[0x44] = 'o';
    ((char *)ascii_unshifted)[0x4D] = 'p';
    ((char *)ascii_unshifted)[0x15] = 'q';
    ((char *)ascii_unshifted)[0x2D] = 'r';
    ((char *)ascii_unshifted)[0x1B] = 's';
    ((char *)ascii_unshifted)[0x2C] = 't';
    ((char *)ascii_unshifted)[0x3C] = 'u';
    ((char *)ascii_unshifted)[0x2A] = 'v';
    ((char *)ascii_unshifted)[0x1D] = 'w';
    ((char *)ascii_unshifted)[0x22] = 'x';
    ((char *)ascii_unshifted)[0x35] = 'y';
    ((char *)ascii_unshifted)[0x1A] = 'z';
    ((char *)ascii_unshifted)[0x45] = '0';
    ((char *)ascii_unshifted)[0x16] = '1';
    ((char *)ascii_unshifted)[0x1E] = '2';
    ((char *)ascii_unshifted)[0x26] = '3';
    ((char *)ascii_unshifted)[0x25] = '4';
    ((char *)ascii_unshifted)[0x2E] = '5';
    ((char *)ascii_unshifted)[0x36] = '6';
    ((char *)ascii_unshifted)[0x3D] = '7';
    ((char *)ascii_unshifted)[0x3E] = '8';
    ((char *)ascii_unshifted)[0x46] = '9';
    ((char *)ascii_unshifted)[0x29] = ' ';
    ((char *)ascii_unshifted)[0x5A] = '\n';
    ((char *)ascii_unshifted)[0x66] = '\b';

    /* shifted */
    ((char *)ascii_shifted)[0x1C] = 'A';
    ((char *)ascii_shifted)[0x32] = 'B';
    ((char *)ascii_shifted)[0x21] = 'C';
    ((char *)ascii_shifted)[0x23] = 'D';
    ((char *)ascii_shifted)[0x24] = 'E';
    ((char *)ascii_shifted)[0x2B] = 'F';
    ((char *)ascii_shifted)[0x34] = 'G';
    ((char *)ascii_shifted)[0x33] = 'H';
    ((char *)ascii_shifted)[0x43] = 'I';
    ((char *)ascii_shifted)[0x3B] = 'J';
    ((char *)ascii_shifted)[0x42] = 'K';
    ((char *)ascii_shifted)[0x4B] = 'L';
    ((char *)ascii_shifted)[0x3A] = 'M';
    ((char *)ascii_shifted)[0x31] = 'N';
    ((char *)ascii_shifted)[0x44] = 'O';
    ((char *)ascii_shifted)[0x4D] = 'P';
    ((char *)ascii_shifted)[0x15] = 'Q';
    ((char *)ascii_shifted)[0x2D] = 'R';
    ((char *)ascii_shifted)[0x1B] = 'S';
    ((char *)ascii_shifted)[0x2C] = 'T';
    ((char *)ascii_shifted)[0x3C] = 'U';
    ((char *)ascii_shifted)[0x2A] = 'V';
    ((char *)ascii_shifted)[0x1D] = 'W';
    ((char *)ascii_shifted)[0x22] = 'X';
    ((char *)ascii_shifted)[0x35] = 'Y';
    ((char *)ascii_shifted)[0x1A] = 'Z';
    ((char *)ascii_shifted)[0x45] = ')';
    ((char *)ascii_shifted)[0x16] = '!';
    ((char *)ascii_shifted)[0x1E] = '@';
    ((char *)ascii_shifted)[0x26] = '#';
    ((char *)ascii_shifted)[0x25] = '$';
    ((char *)ascii_shifted)[0x2E] = '%';
    ((char *)ascii_shifted)[0x36] = '^';
    ((char *)ascii_shifted)[0x3D] = '&';
    ((char *)ascii_shifted)[0x3E] = '*';
    ((char *)ascii_shifted)[0x46] = '(';
    ((char *)ascii_shifted)[0x29] = ' ';
    ((char *)ascii_shifted)[0x5A] = '\n';
    ((char *)ascii_shifted)[0x66] = '\b';
}

/* Call init_ascii_maps() once before using the tables (e.g., at system init). */
unsigned char toggles = 0;
// bit 0 : shift
// bit 1 : caps lock
// bit 2 : break active

void initEXTI(){
	ENABLE_PORT_B();
	ENABLE_FALLING_EDGE(6);
	init_ascii_maps();
}

char recieveKeycode(){
	char keycode = 0; // fill bit by bit, send when 8 bits sent
	// assuming first start bit has been sent
	for(uint8_t bitsRead = 0; bitsRead < 8; bitsRead++){
		while(CHECK_CLK()){
			;
		}
		CLEAR_EXTI_FLAG(6); // wait for next drop, dont re-sample
		keycode |= (READ_PS2_DATA()) << bitsRead;
	}
	for(uint8_t bitsRead = 0; bitsRead < 2; bitsRead++){
		while(CHECK_CLK()){
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
		return ascii_shifted[(unsigned char)keycode];
	}
	return ascii_unshifted[(unsigned char)keycode]; // last possibility
}

char recieveChar(){
	return convertKeycode(recieveKeycode());
}

int attemptRecieve(){
	if(!CHECK_CLK()){
		return -1;
	}
	return recieveChar();
}
