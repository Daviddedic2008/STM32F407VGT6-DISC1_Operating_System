/*
 * pendSVHandler.S
 *
 *  Created on: Nov 20, 2025
 *      Author: david
 */

.extern current_task_pointer

.global pendSV_Handler
pendSV_Handler:
	cpsid i ; disable interrupts
	mrs r0, psp ; move from special register psp(stack pointer when in thread mode)
	ldr r1, =current_task_pointer ; load address of current_task_pointer
	ldr r2, [r1] ; dereference and obtain actual current_task_pointer
	stmdb r0!, {r4-r11} ; push callee registers onto the stack, increment r0 by 8 as well
	str r0, [r2] ; store the current stack address into current_task_pointer->sp (stack pointer of current task)
	ldr r2, [r2, #4] ; this loads the next task pointer into r2
	str r2, [r1] ; update current task pointer to point to the next task pointer
	ldr r0, [r2] ; load new tasks stack pointer into r0
	ldmia r0!, {r4-r11} ; pop callee registers for the next task thats being called
	msr psp, r0 ; set the program stack pointer to the beginning of next task
	cpsie i ; enable interrupts
	bx lr ; return from exception (tell it to return in thread mode and use psp, not msp
