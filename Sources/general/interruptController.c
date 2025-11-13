/*
 * interruptController.c
 *
 *  Created on: Nov 10, 2025
 *      Author: david
 *
 *      TODO:
 *   		*set up function to set handler function addresses
 *   		*set up functions to set pins to falling edge interrupts
 *   		*lots of helper functions
 */

#include "interruptController.h"
#include "mcuHeader.h"

__attribute__((aligned(128))) uint32_t ramVectorTable[NUMVECTORS]; // align to the minimum 128 bytes

void writeHandlerToTable(const uint32_t irq, void(*fp)(void)){
	__disable_irq(); // disable/enable interrupts just in case :)
	// rewrite from old ram table to new table
	uint32_t vector = SCB_VTOR;
	for(uint32_t i = 0; i < NUMVECTORS; i++, ramVectorTable[i]=((volatile uint32_t*)vector)[i]);
	ramVectorTable[16 + irq] = (uint32_t)fp; // load past 16 sys interrupts
	SCB_VTOR = (uint32_t)ramVectorTable; // set vtor addr to ram tables addr
	__enable_irq();
}

void userHandler(void);

void dispatcher(){
	// receive interrupt and clear
	// make sure we clear pin 6 interrupt
	EXTI_PR |= *((volatile uint32_t*)EXTI_PR) & (1 << 6); // set 1 to pending
	userHandler(); // call user handler
}

void enableFallingEdgeB6(void(*fp)(void)){
	// set exti line for B6 to make it trigger on falling edge. load handler into proper vector table entry
	userHandler = fp; // save user handler to call in dispatcher

	const uint32_t field_shift = (6 - 4) * 4;
	*((volatile uint32_t*)SYSCFG_EXTICR2) = (*((volatile uint32_t*)SYSCFG_EXTICR2) & ~(0xF << field_shift)) | (1 << field_shift);
    *(volatile uint32_t*)EXTI_IMR |= (1 << 6); // unmask line 6
    *(volatile uint32_t*)EXTI_RTSR &= ~(1 << 6); // disable rising edge
    *(volatile uint32_t*)EXTI_FTSR |=  (1 << 6); // enable falling edge
    *(volatile uint32_t*)EXTI_PR =  (1 << 6); // clear pending register by writing 1
    writeHandlerToRamTable(23, dispatcher); // write general handler that will call user handler
    *(volatile uint32_t*)NVIC_ICPR0 |= (1 << 23); // set pending bit
    *(volatile uint32_t*)NVIC_ISER0 |= (1 << 23); // set enable bit
}
