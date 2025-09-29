// general purpose io header(exti, ports/pins)

#ifndef MCUH
#define MCUH
#include <stdint.h>

#define SystemCoreClock 168000000

// GPIOA
#define GPIOA_MODER 0x40020000
#define GPIOA_IDR   0x40020010
#define GPIOA_ODR   0x40020014

// GPIOB
#define GPIOB_MODER 0x40020400
#define GPIOB_IDR   0x40020410
#define GPIOB_ODR   0x40020414

// GPIOC
#define GPIOC_MODER 0x40020800
#define GPIOC_IDR   0x40020810
#define GPIOC_ODR   0x40020814

// GPIOD
#define GPIOD_MODER 0x40020C00
#define GPIOD_IDR   0x40020C10
#define GPIOD_ODR   0x40020C14

// GPIOE
#define GPIOE_MODER 0x40021000
#define GPIOE_IDR   0x40021010
#define GPIOE_ODR   0x40021014

// GPIOF
#define GPIOF_MODER 0x40021400
#define GPIOF_IDR   0x40021410
#define GPIOF_ODR   0x40021414

// GPIOG
#define GPIOG_MODER 0x40021800
#define GPIOG_IDR   0x40021810
#define GPIOG_ODR   0x40021814

// exti line stuff

#define EXTI_BASE        0x40013C00

#define EXTI_IMR         (EXTI_BASE + 0x00)  // interrupt mask register
#define EXTI_EMR         (EXTI_BASE + 0x04)  // event mask register
#define EXTI_RTSR        (EXTI_BASE + 0x08)  // rising trigger selection
#define EXTI_FTSR        (EXTI_BASE + 0x0C)  // falling trigger selection
#define EXTI_SWIER       (EXTI_BASE + 0x10)  // software interrupt event
#define EXTI_PR          (EXTI_BASE + 0x14)  // pending register

#define SYSCFG_BASE      0x40013800

#define SYSCFG_EXTICR1   (SYSCFG_BASE + 0x08)  // EXTI0–EXTI3
#define SYSCFG_EXTICR2   (SYSCFG_BASE + 0x0C)  // EXTI4–EXTI7
#define SYSCFG_EXTICR3   (SYSCFG_BASE + 0x10)  // EXTI8–EXTI11
#define SYSCFG_EXTICR4   (SYSCFG_BASE + 0x14)  // EXTI12–EXTI15

#define RCC_APB2ENR      0x40023844
#define NVIC_ISER0       0xE000E100

#define SET_PIN(port, pin) (*(volatile uint32_t*)port) |= (1 << 5) // lil macro
#define CLEAR_PIN(port, pin) (*(volatile uint32_t*)port) &= ~(1 << 5) // lil macro
#define SET_OUTPUT_PORT(port_moder) (*(volatile uint32_t*)port_moder) &= ~(0x3 << (5 * 2)); /* clear the 2 bits */ (*(volatile uint32_t*)port_moder) |= (0x1 << (5 * 2));
#define SET_INPUT_PORT(port_moder) (*(volatile uint32_t*)port_moder) &= ~(0x3 << (5 * 2)) /* clear the 2 bits */
#define READ_PIN(port, pin) (*(volatile uint32_t*)port) & (1 << pin)
#define WRITE_BYTE_PORT_LO(port, byte) (*(volatile uint8_t*)port) = byte
#define WRITE_BYTE_PORT_HI(port, byte) (*(volatile uint8_t*)port+1) = byte
#define WRITE_UINT16_PORT(port, uint) (*(volatile uint32_t*)port) & uint

// CONSTS BASED ON CORTEX
#define FLASHEND 0x80FFFFF
#define SRAMEND 2001FFFF
#define SRAMSTART 0x20000000
#define FLASHUSED (*(volatile uint32_t*)0x08000000)

// FLASH STUFF
#define FLASH_KEYR (*(volatile uint32_t*)0x40023C04)
#define FLASH_SR (*(volatile uint32_t*)0x40023C0C)
#define FLASH_CR (*(volatile uint32_t*)0x40023C10)

// CONSTS BASED ON OS ITSELF
#define USEDFLASH 0x0 // unsure for now

#endif

