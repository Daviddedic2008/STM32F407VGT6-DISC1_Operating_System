/*
 * task.c
 *
 *  Created on: Nov 20, 2025
 *      Author: david
 */
#define maxTasks 30
#define taskSz 128

static uint32_t stackMem[maxTasks][taskSz];

typedef struct{
	uint32_t* psp;
	uint32_t* next_psp;
}task;

task tasks[maxTasks];

task* current_task_pointer; // used in pendSV handler

void initTask(void(*fn)(void), const uint32_t idx){
	// upon each task init, we must set up the "psp" in the stackmem arr
	// we are simulating an exception stack frame so when this pointer is passed to asm subroutine it works
	// this init code lets the asm routine know that it needs to run in psp mode
	uint32_t* stackPointer = &(stackMem[idx][taskSz]); // start at top of stack + 1
	*(--stackPointer) = 0x01000000; // some thing about a thumb bit having to be set in the first slot
	*(--stackPointer) = (uint32_t)fn; // pass function location;
	*(--stackPointer) = 0xFFFFFFFD; // LR — special EXC_RETURN value: return to Thread mode using PSP
	*(--stackPointer) = 0; // R12 — not used here
	*(--stackPointer) = 0; // R3
	*(--stackPointer) = 0; // R2
	*(--stackPointer) = 0; // R1
	*(--stackPointer) = 0; // R0 — argument to task function (none here)
	for(uint32_t i = 0; i < 8; i++, stackPointer--){*stackPointer = 0 /*init to zero ig*/} // reserve more of the stack for callee saved regs
	tasks[idx].psp = stackPointer; // save initial stack position after loading everything into stack
}


