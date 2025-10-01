/*
 * screenDriver.h
 *
 *  Created on: Sep 8, 2025
 *      Author: david
 */

#ifndef SOURCES_SCREENDRIVER_H_
#define SOURCES_SCREENDRIVER_H_
#include "font.h"
#include <stdint.h>
// screen dependent do later
void putChar(const char c);
void putString(const char* str, const uint16_t length);
void LCD_INIT();
void clearLCD();
void moveCursor(const uint8_t x, const uint8_t y);


#endif /* SOURCES_SCREENDRIVER_H_ */

