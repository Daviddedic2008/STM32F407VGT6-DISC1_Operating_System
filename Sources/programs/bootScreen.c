/*
 * bootScreen.c
 *
 *  Created on: Oct 2, 2025
 *      Author: david
 */
#include "bootScreen.h"
#include "../lcd/screenDriver.h"
#include "../keyboard/ps-2Interface.h"
#include "terminal/terminal.h"
#include "../general/mcuHeader.h"

void startupScreen(){
	underline = 0;
	changeColor(0xF000);
	changeBackdrop(0xA00A);
	clearLCD();
	const char* l1 = "\n\n\n___  ____                \n|  \\/  (_)               \n| .  . |_  ___ _ __ ___  \n| |\\/| | |/ __| '__/ _ \\ \n| |  | | | (__| | | (_) |\n\\_|  |_/_|\\___|_|  \\___/ \n                          \n                          \n _____                   \n|  _  |                  \n| | | | ___              \n| | | |/ __|             \n\\ \\_/ /\\__ \\             \n \\___/ |___/             \n                          \n                         ";
	putString(l1, 530-120);
	delay_ms(200);
	playTone(2000, 50);
	clearLCD();
	moveCursor(0,0);
	changeColor(0xFF00);
	changeBackdrop(0x0000);
	clearLCD();
	const char* s1 = "+----------------------------+\n|        MicroOS v0.9        |\n|   tiny, fast, cool kernel  |\n+----------------------------+\n\n\n";

	const char* s2 = "              /\\               \n             /  \\              \n            /====\\             \n           |      |            \n           MicroOS!           \n\n            \\____/            ";
	putString(s1, 126);
	delay_ms(50);
	changeColor(0xF00F);
	putString(s2, 185);
	playTone(2000, 50);
	playTone(3000, 50);
	playTone(4000, 100);
	changeColor(0xFFFF);
	putString("\n\n\n\n   Press any key to start!    ", 34);
	idleUntilPress();
	beginTerminal();
}
