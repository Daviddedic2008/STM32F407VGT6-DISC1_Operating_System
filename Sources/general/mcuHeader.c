/*
 * mcuHeader.c
 *
 *  Created on: Sep 30, 2025
 *      Author: david
 */


#include "mcuHeader.h"

uint32_t SystemCoreClock = 0;

void SystemInit(void) {
    // 1. Enable HSE (8 MHz external crystal)
    RCC_CR |= (1 << 16);  // HSEON
    while (!(RCC_CR & (1 << 17)));  // Wait HSERDY

    // 2. Configure PLL: 8 MHz → 168 MHz
    RCC_PLLCFGR = (8 << 0)     |  // PLLM = 8
                  (336 << 6)   |  // PLLN = 336
                  (0 << 16)    |  // PLLP = 2 (00)
                  (1 << 22)    |  // PLLSRC = HSE
                  (7 << 24);      // PLLQ = 7 (USB)

    // 3. Enable PLL
    RCC_CR |= (1 << 24);  // PLLON
    while (!(RCC_CR & (1 << 25)));  // Wait PLLRDY

    // 4. Configure Flash latency
    FLASH_ACR = (5 << 0) |  // LATENCY = 5 WS
                (1 << 8) |  // PRFTEN
                (1 << 9) |  // ICEN
                (1 << 10);  // DCEN

    // 5. Set prescalers
    RCC_CFGR |= (0 << 4);   // AHB = /1
    RCC_CFGR |= (5 << 10);  // APB1 = /4
    RCC_CFGR |= (4 << 13);  // APB2 = /2

    // 6. Switch SYSCLK to PLL
    RCC_CFGR |= (2 << 0);  // SW = PLL
    while ((RCC_CFGR & (3 << 2)) != (2 << 2));  // Wait SWS = PLL

    // 7. Update global clock variable
    SystemCoreClock = 168000000; // we got that sweet 168mhz clock now
}

void delay_ms(uint32_t ms) {
    uint32_t cycles = ms * (SystemCoreClock / 2000); // guessing empty while cycle is around 2 ops(maybe 3?)
    while (cycles--) {
        //__asm__("nop");
    }
}

void pulse_speaker(){
	SET_PIN(GPIOD_ODR, 0);
	CLEAR_PIN(GPIOD_ODR, 0);
}

void pinout(){
	// enable clocks/peripherals for each port
	RCC_AHB1ENR |= (1 << 0);  // GPIOA
	RCC_AHB1ENR |= (1 << 1);  // GPIOB
	RCC_AHB1ENR |= (1 << 2);  // GPIOC
	RCC_AHB1ENR |= (1 << 3);  // GPIOD
	RCC_AHB1ENR |= (1 << 4);  // GPIOE
	RCC_AHB1ENR |= (1 << 5);  // GPIOF
	RCC_AHB1ENR |= (1 << 6);  // GPIOG
	for(int i = 0; i < 8; i++){
		SET_OUTPUT_PIN(GPIOA_MODER, i);
		SET_PUSH_PULL(GPIOC_PUPDR, i);
	}
	SET_OUTPUT_PIN(GPIOB_MODER, 0);
	SET_PUSH_PULL(GPIOB_PUPDR, 0);
	SET_OUTPUT_PIN(GPIOB_MODER, 1);
	SET_PUSH_PULL(GPIOB_PUPDR, 1);
	SET_OUTPUT_PIN(GPIOC_MODER, 4);
	SET_PUSH_PULL(GPIOC_PUPDR, 4);
	SET_OUTPUT_PIN(GPIOC_MODER, 5);
	SET_PUSH_PULL(GPIOC_PUPDR, 5);
	// exti line for falling-edge for ps2
	SET_INPUT_PIN(GPIOB_MODER, 6);
	SET_PUSH_PULL(GPIOB_PUPDR, 6);
	SET_INPUT_PIN(GPIOB_MODER, 7);
	SET_PUSH_PULL(GPIOB_PUPDR, 7);

	SET_OUTPUT_PIN(GPIOD_MODER, 0); // speaker
}

void playTone(const uint32_t hz, const uint32_t duration_ms){
	const uint32_t delayNops = SystemCoreClock / hz / 10;
	const uint32_t numCycles = hz * duration_ms / 1000;
	for(uint32_t i = 0; i < numCycles; i++){
		SET_PIN(GPIOD_ODR, 0);
		volatile uint32_t x = 0;
		for(uint32_t i2 = 0; i2 < delayNops; i2++){
			x++;
		}
		CLEAR_PIN(GPIOD_ODR, 0);
		for(uint32_t i2 = 0; i2 < delayNops/5; i2++){
					x++;
				}
	}
}

void SYSRESET(){
	*(volatile uint32_t*)FLASHUSED = 0;
}
