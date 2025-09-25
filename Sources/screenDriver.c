#include "screenDriver.h"
#include "mcuHeader.h"
#include "font.h"

unsigned char row = 0; unsigned char col = 0;
// row 0-29 col 0-39

#define colToPx(col) (col*8)
#define rowToPy(row) (row*8)

#define COMMAND 1
#define DATA 0

	void WRITE_LCD_BUS(const unsigned char data, const unsigned char command){
	// LCD_RS bit set for command, clear for data
	if(command == COMMAND){
		SET_PIN(GPIOA_ODR, 0);
	}
	else{
		CLEAR_PIN(GPIOA_ODR, 0);
	}
	// write byte on GPIOC
	WRITE_BYTE_PORT_LO(GPIOC_ODR, data);
	// LCD_WR set and clear to send byte
	CLEAR_PIN(GPIOA_ODR, 1);
	SET_PIN(GPIOA_ODR, 1);
}

inline void LCD_INIT(){
	// fill later twin
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
			WRITE_LCD_BUS(0, DATA);
			WRITE_LCD_BUS(0, DATA);
		}
	}
}

void dispc(const unsigned int x, const unsigned int y, const char c){
	const unsigned int x2 = x + 8;
	const unsigned int y2 = y + 8;
	WRITE_LCD_BUS(0x2a, COMMAND);
	WRITE_LCD_BUS(x & (0xFF), DATA);
	WRITE_LCD_BUS(x & (0xFF << 8), DATA);
	WRITE_LCD_BUS(x2 & (0xFF), DATA);
	WRITE_LCD_BUS(x2 & (0xFF << 8), DATA);

	WRITE_LCD_BUS(0x2b, COMMAND);
	WRITE_LCD_BUS(y & (0xFF), DATA);
	WRITE_LCD_BUS(y & (0xFF << 8), DATA);
	WRITE_LCD_BUS(y2 & (0xFF), DATA);
	WRITE_LCD_BUS(y2 & (0xFF << 8), DATA);

	WRITE_LCD_BUS(0x2c, COMMAND);
	for(unsigned char r = 0; r < 8; r++){
		for(unsigned char offset = 0; offset < 8; offset++){
			WRITE_LCD_BUS((font8x8_basic[(unsigned char)c][r] & (1 << offset)) && 1, DATA); // write pixel of char
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
	textCoordChar(c, col, row);
	col++;
	if(col != 40){return;}
	inc:;
	if(row < 29){
		col = 0;
		row += 1;
	}
}


