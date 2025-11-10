/*
 * interruptController.h
 *
 *  Created on: Nov 10, 2025
 *      Author: david
 */

#ifndef SOURCES_KEYBOARD_INTERRUPTCONTROLLER_H_
#define SOURCES_KEYBOARD_INTERRUPTCONTROLLER_H_

void enableFallingEdge(const unsigned char pin);

void setHandlerAddress(void(*fp)(void));

#endif /* SOURCES_KEYBOARD_INTERRUPTCONTROLLER_H_ */
