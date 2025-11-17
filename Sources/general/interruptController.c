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
#include "../lcd/screenDriver.h"

__attribute__((section(".ram_vector_table"), aligned(128)))
volatile uint32_t ramVectorTable[NUMVECTORS];

void writeHandlerToTable(const uint32_t irq, void(*fp)(void)){
	// rewrite from old ram table to new table
	uint32_t vector = SCB_VTOR;
	for (uint32_t i = 0; i < NUMVECTORS; i++) {
	    ramVectorTable[i] = ((volatile uint32_t*)vector)[i];
	}
	ramVectorTable[16 + irq] = (uint32_t)fp; // load past 16 sys interrupts
	SCB_VTOR = (uint32_t)ramVectorTable; // set vtor addr to ram tables addr
}

void (*userHandler)(void);

void dispatcher(){
	// receive interrupt and clear
	*(volatile uint32_t*)EXTI_PR = (1 << 6);
	userHandler(); // call user handler
	// make sure we clear pin 6 interrupt
	//*(volatile uint32_t*)EXTI_IMR &= ~(1 << 6); // mask EXTI6

}

void enableFallingEdgeD6(void(*fp)(void)){
	__disable_irq();

	// set first 4 bits to 0b0011 to set it to port D
	*((volatile uint32_t*)SYSCFG_EXTICR2) &= ~(0xF << 8); // clear EXTI6 mapping
	*((volatile uint32_t*)SYSCFG_EXTICR2) |=  (0x3 << 8); // map PD6
	// clear pending EXTI6

	*(volatile uint32_t*)EXTI_PR = (1 << 6);

	// configure EXTI6 for falling edge
	*(volatile uint32_t*)EXTI_IMR |= (1 << 6);
	*(volatile uint32_t*)EXTI_RTSR &= ~(1 << 6);
	*(volatile uint32_t*)EXTI_FTSR |= (1 << 6);

	// install dispatcher
	userHandler = fp;
	writeHandlerToTable(23, dispatcher);

	// enable NVIC line 23 which corresponds to exti6
	*(volatile uint32_t*)NVIC_ISER0 |= (1 << 23);

	__enable_irq();
	delay_ms(50);
}
