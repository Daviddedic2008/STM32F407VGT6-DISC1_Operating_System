#include "screenDriver.h"
#include "../general/mcuHeader.h"
#include "font.h"

unsigned char row = 0; unsigned char col = 0;
// row 0-29 col 0-39

#define colToPx(col) (col*8)
#define rowToPy(row) (row*8)

#define COMMAND 1
#define DATA 0

// 1 is NOT enough, but it all depends how the lcd is feeling
// 2 is NOT enough for deterministic screen shenanigans :).
// 3 is BARELY enough, the clock speed and wr lock time barely lign up, slight artifacting
// 4 is def enough but just use 3 its fine
#define PULSE_DELAY() __asm__("nop") // guarantee 15ns pulse time to lock write signal(2 may be enough idk?)

// Pin Defs:
/*
 * GPIOB_1 register select(RS) selects is it command or data
 * GPIOB_0 write pulse(WR) pulse to lock data
 * GPIOC_5 chip select(CS) enables data read when low
 * GPIOC_4 reset pin(RST) resets
 * GPIOA is data bus(8bit)
 */

uint16_t color;
uint16_t backdrop = 0;

static inline void WRITE_LCD_BUS(const unsigned char data, const unsigned char command){
	// LCD_RS bit set for command, clear for data
	if(command == COMMAND){
		CLEAR_PIN(GPIOB_ODR, 1);
	}
	else{
		SET_PIN(GPIOB_ODR, 1);
	}
	// write byte on GPIOC
	WRITE_BYTE_PORT_LO(GPIOA_ODR, data);
	// LCD_WR set and clear to send byte
	CLEAR_PIN(GPIOB_ODR, 0);
	//PULSE_DELAY();
	SET_PIN(GPIOB_ODR, 0);
}

void changeColor(const uint16_t c){
	color = c;
}

void changeBackdrop(const uint16_t c){
	backdrop = c;
}

unsigned char getRow(){
	return row;
}

void LCD_INIT(){
	CLEAR_PIN(GPIOC_ODR, 4); // reset low
	delay_ms(10);
	SET_PIN(GPIOC_ODR, 4);
	delay_ms(120);
	CLEAR_PIN(GPIOC_ODR, 5); // chip select

	WRITE_LCD_BUS(0xEF, COMMAND);
	WRITE_LCD_BUS(0x03, DATA); WRITE_LCD_BUS(0x80, DATA); WRITE_LCD_BUS(0x02, DATA);

	WRITE_LCD_BUS(0xCF, COMMAND);
	WRITE_LCD_BUS(0x00, DATA); WRITE_LCD_BUS(0xC1, DATA); WRITE_LCD_BUS(0x30, DATA);

	WRITE_LCD_BUS(0xED, COMMAND);
	WRITE_LCD_BUS(0x64, DATA); WRITE_LCD_BUS(0x03, DATA); WRITE_LCD_BUS(0x12, DATA); WRITE_LCD_BUS(0x81, DATA);

	WRITE_LCD_BUS(0xCB, COMMAND);
	WRITE_LCD_BUS(0x39, DATA); WRITE_LCD_BUS(0x2C, DATA); WRITE_LCD_BUS(0x00, DATA);
	WRITE_LCD_BUS(0x34, DATA); WRITE_LCD_BUS(0x02, DATA);

	WRITE_LCD_BUS(0xF7, COMMAND);
	WRITE_LCD_BUS(0x20, DATA);

	WRITE_LCD_BUS(0xEA, COMMAND);
	WRITE_LCD_BUS(0x00, DATA); WRITE_LCD_BUS(0x00, DATA);

	WRITE_LCD_BUS(0xC0, COMMAND);
	WRITE_LCD_BUS(0x23, DATA);

	WRITE_LCD_BUS(0xC1, COMMAND);
	WRITE_LCD_BUS(0x10, DATA);

	WRITE_LCD_BUS(0xC5, COMMAND);
	WRITE_LCD_BUS(0x3E, DATA); WRITE_LCD_BUS(0x28, DATA);

	WRITE_LCD_BUS(0xC7, COMMAND);
	WRITE_LCD_BUS(0x86, DATA);

	WRITE_LCD_BUS(0x36, COMMAND);
	WRITE_LCD_BUS(0x48, DATA);  // MX, BGR

	WRITE_LCD_BUS(0x3A, COMMAND);
	WRITE_LCD_BUS(0x55, DATA);  // 16-bit/pixel

	WRITE_LCD_BUS(0xB1, COMMAND);
	WRITE_LCD_BUS(0x00, DATA); WRITE_LCD_BUS(0x18, DATA);

	WRITE_LCD_BUS(0xB6, COMMAND);
	WRITE_LCD_BUS(0x08, DATA); WRITE_LCD_BUS(0x82, DATA); WRITE_LCD_BUS(0x27, DATA);

	WRITE_LCD_BUS(0xF2, COMMAND);
	WRITE_LCD_BUS(0x00, DATA);

	WRITE_LCD_BUS(0x26, COMMAND);
	WRITE_LCD_BUS(0x01, DATA);

	WRITE_LCD_BUS(0xE0, COMMAND);
	uint8_t gamma_pos[] = {0x0F,0x31,0x2B,0x0C,0x0E,0x08,0x4E,0xF1,0x37,0x07,0x10,0x03,0x0E,0x09,0x00};
	for (int i = 0; i < 15; i++) WRITE_LCD_BUS(gamma_pos[i], DATA);

	WRITE_LCD_BUS(0xE1, COMMAND);
	uint8_t gamma_neg[] = {0x00,0x0E,0x14,0x03,0x11,0x07,0x31,0xC1,0x48,0x08,0x0F,0x0C,0x31,0x36,0x0F};
	for (int i = 0; i < 15; i++) WRITE_LCD_BUS(gamma_neg[i], DATA);

	WRITE_LCD_BUS(0x11, COMMAND);  // Sleep Out
	delay_ms(120);

	WRITE_LCD_BUS(0x29, COMMAND);  // Display ON
}

void clearLCD(){
	WRITE_LCD_BUS(0x2a, COMMAND);
	WRITE_LCD_BUS(0, DATA);
	WRITE_LCD_BUS(0, DATA);
	WRITE_LCD_BUS(0, DATA);
	WRITE_LCD_BUS(240, DATA);

	WRITE_LCD_BUS(0x2b, COMMAND);
	WRITE_LCD_BUS(0, DATA);
	WRITE_LCD_BUS(0, DATA);
	WRITE_LCD_BUS(1, DATA);
	WRITE_LCD_BUS(64, DATA);

	WRITE_LCD_BUS(0x2c, COMMAND);
	for(unsigned int x = 0; x < 320; x++){
		for(unsigned char y = 0; y < 240; y++){
			WRITE_LCD_BUS(backdrop >> 8, DATA); WRITE_LCD_BUS(backdrop & 0xFF, DATA);
		}
	}
}

void moveCursor(const uint8_t x, const uint8_t y){
	col = x; row = y;
}

void dispc(const unsigned int x, const unsigned int y, const char c){
	const unsigned int x2 = x + 7;
	const unsigned int y2 = y + 7;
	WRITE_LCD_BUS(0x2a, COMMAND);
	WRITE_LCD_BUS(x >> 8, DATA);
	WRITE_LCD_BUS(x & 0xFF, DATA);
	WRITE_LCD_BUS(x2 >> 8, DATA);
	WRITE_LCD_BUS(x2 & 0xFF, DATA);

	WRITE_LCD_BUS(0x2b, COMMAND);
	WRITE_LCD_BUS(y >> 8, DATA);
	WRITE_LCD_BUS(y & 0xFF, DATA);
	WRITE_LCD_BUS(y2 >> 8, DATA);
	WRITE_LCD_BUS(y2 & 0xFF, DATA);

	WRITE_LCD_BUS(0x2c, COMMAND);
	for(unsigned char r = 0; r < 8; r++){
		for(unsigned char offset = 0; offset < 8; offset++){
			if(font8x8_basic[(unsigned char)c][r] & (1 << offset)){
				WRITE_LCD_BUS(color >> 8, DATA); WRITE_LCD_BUS(color & 0xFF, DATA); continue;
			}
			WRITE_LCD_BUS(backdrop >> 8, DATA); WRITE_LCD_BUS(backdrop & 0xFF, DATA);
		}
	}
}

void textCoordChar(const char c, const unsigned int x, const unsigned int y){
	dispc(colToPx(x), rowToPy(y), c);
}

void putChar(const char c){
	if(c == '\n'){
		goto inc;
	}
	if(c == '\b' && (col + row) != 0){
		col--;
		if(col == -1){
			col = 29;
			row -=1;
		}
		textCoordChar(' ', col, row);
		return;
	}
	//pulse_speaker();
	textCoordChar(c, col, row);
	col++;
	if(col != 30){return;}
	inc:;
	if(row < 39){
		col = 0;
		row += 1;
	}
}

void putString(const char* str, const uint16_t length){
	char c = str[0];
	for(uint16_t idx = 0; idx < length; idx++, c = str[idx]){
		putChar(c);
	}
}

void printNum(uint16_t num){
    if (num == 0) {
        putChar('0');
        return;
    }

    uint32_t p = 1;
    uint32_t n = num;
    while (n >= 10) {
        p *= 10;
        n /= 10;
    }

    while (p > 0) {
        putChar('0' + (num / p) % 10);
        p /= 10;
    }
}
