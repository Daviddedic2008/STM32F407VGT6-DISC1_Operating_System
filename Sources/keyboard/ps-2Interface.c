/*
 * ps-2Interface.c
 *
 *  Created on: Sep 25, 2025
 *      Author: david
 */
#include "ps-2Interface.h"
#include "../general/interruptController.h"
#include "../lcd/screenDriver.h"

// full 128-byte arrays (initialized to 0), then populate the mapped entries
const char ascii_unshifted[128] = { 0 };
const char ascii_shifted[128]   = { 0 };

#define R 0x74
#define L 0x6B
#define U 0x75
#define D 0x72

void init_ascii_maps(void)
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

#define KEYUP 0xF0 // scan code for charUp prefix, this means to ignore next send

volatile unsigned char bitPhase = 0;
volatile unsigned char currentScanCode = 0;
volatile unsigned char lastFullScanCode = 0xFF;
volatile unsigned char shiftToggle = 0;

void interruptHandler(void){
    unsigned char bit = READ_PIN(GPIOD_IDR, 7) ? 1 : 0;
    //printNum(bit);
    switch (bitPhase) {
        case 0: // Wait for start bit (should be 0)
            if (bit == 0) {
                currentScanCode = 0;
                bitPhase = 1;
            }
            break;

        case 1: case 2: case 3: case 4:
        case 5: case 6: case 7: case 8:
            currentScanCode |= (bit << (bitPhase - 1)); // LSB first
            bitPhase++;
            break;

        case 9: // Parity bit — ignore
            bitPhase++;
            break;

        case 10: // Stop bit — complete frame
            lastFullScanCode = currentScanCode;
            if (lastFullScanCode == 0x12) {
                shiftToggle = !shiftToggle;
            }
            bitPhase = 0;
            break;

        default:
            bitPhase = 0; // reset on error
            break;
    }
}

char convertScanCode(const unsigned char sc){
	if(shiftToggle){
		return ascii_shifted[sc];
	}
	return ascii_unshifted[sc];
}

char readNewChar(){
	if(lastFullScanCode == 0xFF){return -1;} // return null char
	const unsigned char lfc = lastFullScanCode;
	lastFullScanCode = 0xFF; // clear
	return convertScanCode(lfc); // read with saved code
}

void idleUntilPress(){
	bitPhase = 0;
	while(bitPhase == 0){;}
}

char readLastChar(){
	return convertScanCode(lastFullScanCode);
}

char idleUntilNextChar(){
	lastFullScanCode = 0xFF;
	while(lastFullScanCode == 0xFF){;}
	if(lastFullScanCode == 0xE0){ // if its an arrow prefix
		lastFullScanCode = 0xFF;
		while(lastFullScanCode == 0xFF){;} // wait for second byte
		return lastFullScanCode; // return raw arrow scan code
	}
	//printNum(lastFullScanCode);
	//putChar('\n');
	return convertScanCode(lastFullScanCode);
}

void initKeyboardInterface(){
	enableFallingEdgeD6(interruptHandler);
	init_ascii_maps();
}

