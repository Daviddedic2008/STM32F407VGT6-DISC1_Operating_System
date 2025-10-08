// general purpose io header(exti, ports/pins)

#ifndef MCUH
#define MCUH
#include <stdint.h>

extern uint32_t SystemCoreClock;

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

// otyper for config push pull inputs(actively drives high instead of draining current slowly)
#define GPIOA_OTYPER  0x40020004
#define GPIOB_OTYPER  0x40020404
#define GPIOC_OTYPER  0x40020804
#define GPIOD_OTYPER  0x40020C04
#define GPIOE_OTYPER  0x40021004
#define GPIOF_OTYPER  0x40021404
#define GPIOG_OTYPER  0x40021804
#define GPIOH_OTYPER  0x40021C04
#define GPIOI_OTYPER  0x40022004

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

#define NVIC_ISER0       0xE000E100

#define RCC_BASE        0x40023800
#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))

#define SET_PIN(port, pin) (*(volatile uint32_t*)port) |= (1 << pin) // lil macro
#define CLEAR_PIN(port, pin) (*(volatile uint32_t*)port) &= ~(1 << pin) // lil macro
#define SET_OUTPUT_PIN(port_moder, pin) (*(volatile uint32_t*)port_moder) &= ~(0x3 << (pin * 2)); /* clear the 2 bits */ (*(volatile uint32_t*)port_moder) |= (0x1 << (pin * 2))
#define SET_INPUT_PIN(port_moder, pin) (*(volatile uint32_t*)port_moder) &= ~(0x3 << (pin * 2)) /* clear the 2 bits */
#define READ_PIN(port, pin) (*(volatile uint32_t*)port) & (1 << pin)
#define WRITE_BYTE_PORT_LO(port, byte) (*(volatile uint8_t*)port) = byte
#define WRITE_BYTE_PORT_HI(port, byte) (*(volatile uint8_t*)port+1) = byte
#define WRITE_UINT16_PORT(port, uint) (*(volatile uint32_t*)port) & uint

// set pin to push-pull (clear bit)
#define SET_PUSH_PULL(reg_addr, pin) (*(volatile unsigned int*)(reg_addr) &= ~(1 << (pin)))
// set pin to open-drain (set bit)
#define SET_OPEN_DRAIN(reg_addr, pin) (*(volatile unsigned int*)(reg_addr) |= (1 << (pin)))

// CONSTS BASED ON CORTEX
#define FLASHEND 0x80FFFFF
#define SRAMEND 2001FFFF
#define SRAMSTART 0x20000000
#define FLASHUSED (*(volatile uint32_t*)0x08000000)
#define NUMPKG (*(volatile uint32_t*)0x08000004)
#define PKGSTART (*(volatile uint32_t*)0x08000008) // alignment stuff i have enough flash to spare a few bytes

// FLASH STUFF
#define FLASH_KEYR (*(volatile uint32_t*)0x40023C04)
#define FLASH_SR (*(volatile uint32_t*)0x40023C0C)
#define FLASH_CR (*(volatile uint32_t*)0x40023C10)

#define RCC_CR           (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR      (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR         (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_APB1ENR      (*(volatile uint32_t *)(RCC_BASE + 0x40))
#define RCC_APB2ENR      (*(volatile uint32_t *)(RCC_BASE + 0x44))

#define FLASH_ACR        (*(volatile uint32_t *)0x40023C00)


void SystemInit(void); // clocky stuff

void delay_ms(uint32_t ms);

void pulse_speaker();

void playTone(const uint32_t hz, const uint32_t duration_ms);

void pinout();
#endif

