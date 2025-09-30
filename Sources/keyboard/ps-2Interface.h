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
#include "./general/mcuHeader.h"
#define KBRD_CLK_PIN 6 // port b 6
#define KBRD_DATA_PIN 7 // port b 7... 67!!
#define ENABLE_FALLING_EDGE(line) do {*((volatile uint32_t*)EXTI_IMR) |= (1 << line); *((volatile uint32_t*)EXTI_FTSR) |= (1 << line); *((volatile uint32_t*)EXTI_RTSR) &= ~(1 << line); } while(0)
#define CLEAR_EXTI_FLAG(line) EXTI_PR = 1 << line
#define READ_PS2_DATA() ((GPIOG_IDR /*temp port g, change later*/ >> KBRD_DATA_PIN) & 1)
#endif /* SOURCES_PS_2INTERFACE_H_ */
