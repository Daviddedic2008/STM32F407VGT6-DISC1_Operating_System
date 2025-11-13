/*
 * ps-2Interface.h
 *
 *  Created on: Sep 25, 2025
 *      Author: david
 */

// Done over NVIC(nested vector interrupt controller)
// Couple things to keep in mind: NVIC_ENABLEIRQ allows interrupt line to fire
// NVIC needs 2 things, allowance to trigger interrupt and interrupt priority(4 bits, top nibble)
// I am mapping EXTI(external interrupt) to trigger on the falling edge of the clock pin
// Then I read data pin to recieve a bit of data from the keyboard
// Repeat until a whole char is recieved


#ifndef SOURCES_PS_2INTERFACE_H_
#define SOURCES_PS_2INTERFACE_H_
#include "../general/mcuHeader.h"

char readNewChar();

void idleUntilPress();

char readLastChar();

char idleUntilNextChar();

void initKeyboardInterface();

#endif /* SOURCES_PS_2INTERFACE_H_ */
