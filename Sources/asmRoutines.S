.global callFlashRoutine

// pretty much set PC to addr in flash, save return adress, and branch back when ret is called
// address must be multiple of 4(aligned) because reasons
// bitwise or applied to set LSB. For some reason, blx wont take a value without a set LSB
callFlashRoutine:
	ldr R4, =1
	orr R0, R0, R4
	blx R0
