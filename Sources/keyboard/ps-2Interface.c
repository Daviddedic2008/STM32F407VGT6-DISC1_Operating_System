/*
 * ps-2Interface.c
 *
 *  Created on: Sep 25, 2025
 *      Author: david
 */
#include "ps-2Interface.h"
#include "../general/interruptController.h"
#include "../lcd/screenDriver.h"

// full 128 byte lookup arrays

// unshifted ASCII for PS/2 Set 2 (US layout)
const char ascii_unshifted[128] = {

    // 0x10–0x7F (designated initializers for non-zero entries)
    [0x15] = 'q',
    [0x16] = '1',
    [0x1A] = 'z',
    [0x1B] = 's',
    [0x1C] = 'a',
    [0x1D] = 'w',
    [0x1E] = '2',
    [0x21] = 'c',
    [0x22] = 'x',
    [0x23] = 'd',
    [0x24] = 'e',
    [0x25] = '4',
    [0x26] = '3',
    [0x29] = ' ',
    [0x2A] = 'v',
    [0x2B] = 'f',
    [0x2C] = 't',
    [0x2D] = 'r',
    [0x2E] = '5',
    [0x31] = 'n',
    [0x32] = 'b',
    [0x33] = 'h',
    [0x34] = 'g',
    [0x35] = 'y',
    [0x36] = '6',
    [0x3A] = 'm',
    [0x3B] = 'j',
    [0x3C] = 'u',
    [0x3D] = '7',
    [0x3E] = '8',
    [0x41] = ',',
    [0x42] = 'k',
    [0x43] = 'i',
    [0x44] = 'o',
    [0x45] = '0',
    [0x46] = '9',
    [0x49] = '.',
    [0x4A] = '/',
    [0x4B] = 'l',
    [0x4C] = ';',
    [0x4D] = 'p',
    [0x4E] = '-',
    [0x52] = '\'',
    [0x54] = '[',
    [0x55] = '=',
    [0x5A] = '\n',  // enter
    [0x5B] = ']',
    [0x5D] = '\\',  // iSO extra key (may be absent on ANSI)
    [0x66] = '\b',  // backspace
	[0x75] = -3, // up arrow
	[0x72] = -4, // down arrow
	[0x6B] = -2, // left arrow
	[0x74] = -1, // right arrow
    [0x0D] = '\t',  // tab
    [0x0E] = '`',   // backtick
};

// u 75 d 72 l 6B r 74

// shifted ASCII for PS/2 Set 2 (US layout)
const char ascii_shifted[128] = {

    // 0x10–0x7F (designated initializers for non-zero entries)
    [0x15] = 'Q',
    [0x16] = '!',
    [0x1A] = 'Z',
    [0x1B] = 'S',
    [0x1C] = 'A',
    [0x1D] = 'W',
    [0x1E] = '@',
    [0x21] = 'C',
    [0x22] = 'X',
    [0x23] = 'D',
    [0x24] = 'E',
    [0x25] = '$',
    [0x26] = '#',
    [0x29] = ' ',   // space stays space with Shift
    [0x2A] = 'V',
    [0x2B] = 'F',
    [0x2C] = 'T',
    [0x2D] = 'R',
    [0x2E] = '%',
    [0x31] = 'N',
    [0x32] = 'B',
    [0x33] = 'H',
    [0x34] = 'G',
    [0x35] = 'Y',
    [0x36] = '^',
    [0x3A] = 'M',
    [0x3B] = 'J',
    [0x3C] = 'U',
    [0x3D] = '&',
    [0x3E] = '*',
    [0x41] = '<',
    [0x42] = 'K',
    [0x43] = 'I',
    [0x44] = 'O',
    [0x45] = ')',
    [0x46] = '(',
    [0x49] = '>',
    [0x4A] = '?',
    [0x4B] = 'L',
    [0x4C] = ':',
    [0x4D] = 'P',
    [0x4E] = '_',
    [0x52] = '"',
    [0x54] = '{',
    [0x55] = '+',
    [0x5A] = '\n',  // enter
    [0x5B] = '}',
    [0x5D] = '|',   // ISO extra key (may be absent on ANSI)
    [0x66] = '\b',  // backspace
	[0x75] = -3, // up arrow
	[0x72] = -4, // down arrow
	[0x6B] = -2, // left arrow
	[0x74] = -1, // right arrow
    [0x0D] = '\t',  // tab
    [0x0E] = '~',   // tilde
};

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

unsigned char updateScanCode(){
	lastFullScanCode = 0xFF;
	while(lastFullScanCode == 0xFF){}
	return lastFullScanCode;
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
	unsigned char sc;
	unsigned char saw_break = 0;

	for (;;) {
		sc = updateScanCode();
		if (sc == 0xF0) {
			saw_break = 1;
			continue;
		}
		if(sc == 0xE0){
			continue;
		}
		if (saw_break) {
			saw_break = 0;
			continue;
		}
		// here sc is a make (press) scancode for a non-extended key
		char c = convertScanCode(sc);
		if (c != 0) return c;
	}
}

void initKeyboardInterface(){
	enableFallingEdgeD6(interruptHandler);
}
