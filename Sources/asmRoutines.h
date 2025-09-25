#ifndef ASMR
#define ASMR

void callFlashRoutine(uint32_t);
// pretty much set PC to addr in flash, save return address, and branch back when ret is called
// address must be multiple of 4(aligned) because reasons

#endif
