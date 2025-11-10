# STM32F407VGTX RTOS

This is a very lightweight bare-metal OS for the STM32F407VGT6(no libraries or external headers, only C) that includes a simple font renderer and LCD driver(Elegoo 2.8in TFT LCD), ps-2 keyboard drivers, a simple file system(128kb of flash accessible for now)

## Features
- TFT LCD interface
- PS-2 keyboard interface
- Simple flash controller/file system
- Text file viewer/editor
- Simple tone playback via small piezoelectric element
- Partially implemented assembler to assemble/run code written in text files(work in progress!)

## Wiring/Setup
  ### Requirements
    - 2.7 inch Elegoo TFT LCD
    - PS-2 full ASCII keyboard with keyboard socket adapter
    - STM32F407VGTX Dev Board(VGT6-DISC1 preferred)
    - Female to female pin connectors
    - Piezoelectric speaker
  
  ### Wiring Guide
    Connect the LCD via this guide:
    - GPIOB_1 register select(RS)
    - GPIOB_0 write pulse(WR)
    - GPIOC_5 chip select(CS)
    - GPIOC_4 reset pin(RST)
    - GPIOA is data bus(Connect Data_0 to GPIOA_0, Data_1 to GPIOA_1, etc.)
    
    Connect the PS-2 keyboard via this guide:
    - GPIOB_6 clk
    - GPIOB_7 data
    
    Connect the piezoelectric speaker using GPIOD_0
  ## Build
    The project requires ZERO external headers. Everything from the startup code 
    to specific io register memory locations is in the project.
    You can either:
    a.) Use STM32CubeIDE to open the project, and then simply clone the repo and press upload/run
    b.) Build using CMake, and upload via custom upload commands(still working on generating them, will be here soon)

  # Issues
    The project still requires the debugger to be flashed onto the cortex. 
    I have some custom upload commands but am unsure if they work on other
    systems.
  # Fun Demos!
  Everything here is just fun little demos I recorded:


![WIN_20250930_23_55_41_Pro](https://github.com/user-attachments/assets/8e84e555-251b-45b0-9cb3-1df81f105cd6)



https://github.com/user-attachments/assets/f92bb7cc-9c32-4bc3-8326-d45dd07e4bf8

![IMG_E0008](https://github.com/user-attachments/assets/43f05f6a-9949-431c-b0b6-e4a1d51086c7)