# AVR Course – Examples Repository

This repository contains examples presented throughout the AVR programming course. The content is divided into two main parts, showing how to implement similar functionalities using both low-level C code and the Arduino framework.



## Repository Structure
- `ArduinoAVR/`  
  Contains Arduino-based equivalents of the AVR C examples. These sketches use the Arduino API and simplify development using prebuilt functions. They are ideal for beginners or for rapid prototyping, while still reflecting the same core ideas as in the low-level code.

- `AVR_Code/`  
  Contains pure C code for AVR microcontrollers. These examples demonstrate direct register-level programming without relying on any external libraries. They are suitable for learning how AVR works under the hood and are focused on low-level control and understanding of microcontroller internals.

Additionally, this folder includes a set of C++ header files that abstract and simplify hardware control using modern, class-based design. These files allow writing more readable and reusable code for common embedded tasks:

  - `gpio.hpp`  
    Defines the `GPIO_t` structure to describe an I/O port and a `Digital_IO` class to configure any digital pin as an input or output with simple methods.
 
 - `LEDDisplay.hpp`
    Defines the `LEDDisplay` class template for controlling multi-digit 7-segment LED displays with optional decimal points. It supports both common `anode` and `cathode` configurations and handles digit multiplexing through the next() method. The display content is configured using an array of `SignleLEDSegment` structures, which represent each digit and optional dot state. Designed for efficient and modular use in embedded systems.

  - `button.hpp` 
    Defines the `PushButton` class that abstracts button handling, including debouncing and detecting button presses/releases. It allows the user to assign custom callback functions for button events and includes a timestamp generator for accurate debouncing timing. This class simplifies button interaction in embedded systems by ensuring stable button state detection.

  - `utils.hpp`  
    A lightweight implementation of an `Optional` type, useful for safe value handling in embedded systems without dynamic allocation.

  - `timer8_t.hpp`  
    Defines the `timer8_t` structure, which holds pointers to timer registers and interrupt callback functions. This allows easy configuration and use of 8-bit timers in a modular way.

  - `interrupts.hpp`  
    Centralized file for defining and handling interrupt service routines used throughout the AVR examples.

  - `communication.hpp`  
    Implements a circular buffer for communication tasks using `Optional`.  
    Includes the `UART_t` structure that groups UART register pointers, callback pointers for interrupts, and RX/TX buffers.  
    The `UART_comm` class uses `UART_t` to provide easy-to-use methods for UART reading and writing.

  - `logger.hpp`  
    Implements a simple, generic logging class template `Logger<CommIface, Timestamp_t>`.  
    It takes any communication interface (e.g., `UART_comm`) and provides a `log()` method that supports `printf`-style formatted logging.  
    This allows structured debugging or runtime status reporting over a serial connection or any custom interface.  
    The class is lightweight and designed for embedded systems with limited resources.

  - `common_defs.h`  
    Contains hardware abstraction macros that define base instances for timers, UART, GPIO ports etc.  
    It centralizes hardware mappings to simplify peripheral initialization and usage in user code.

## Requirements

- `avr-gcc`, `avr-g++` and `avrdude` installed (for compiling and uploading C code)
- Arduino IDE (for Arduino examples)
- AVR development board (e.g., ATmega328P, ATmega32)
- ISP programmer (e.g., USBasp, USBtinyISP) for flashing C code, if not using Arduino bootloader

## How to Use

Compilation and flashing for atmega328p
- avr-g++ -mmcu=atmega328p -DF_CPU=16000000UL -Os -std=c++11 -o main.elf main.cpp
- avr-objcopy -O ihex -R .eeprom main.elf main.hex
- avrdude -c usbasp -p m328p -U flash:w:main.hex

### AVR C Code
1. Navigate to the `AVR_Code/` directory.
2. Compile the code using `avr-gcc` (or a provided Makefile).
3. Upload the compiled HEX file using `avrdude`.

### Arduino Code
1. Open the corresponding `.ino` file in the Arduino IDE from the `ArduinoAVR/` directory.
2. Select the correct board and port.
3. Upload the sketch directly using the IDE.

## Course Objectives

By following this course and exploring the examples, you will learn:
- The basics of AVR architecture and hardware control
- How to write low-level code using C and direct register access
- The differences between bare-metal C programming and Arduino-based development
- Practical applications of AVR microcontrollers in embedded systems
