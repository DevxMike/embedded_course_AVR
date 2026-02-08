# AVR Course – Examples Repository

This repository contains all examples presented in the **AVR programming** part of **C++ embedded development** course. The course demonstrates how to program AVR microcontrollers using both **bare-metal C/C++** (register-level programming) and **Arduino sketches** for comparison.  

The repository evolves with the course, introducing refactoring, code reuse, and a structure closer to real embedded projects.

# Course Objectives
By following this course and exploring the examples, you will learn:
- Basics of AVR architecture and hardware control
- Low-level C++ programming using direct register access
- How to structure reusable code for embedded projects
- Practical applications in embedded systems using LEDs, buttons, UART, timers, and displays
- The mini HAL in AVR/common/ enables rapid prototyping while keeping code hardware‑close and modular, preparing you for real-world embedded projects.

---

## Repository Structure

### `ArduinoAVR/`
Arduino-based equivalents of AVR examples, using the Arduino API for easier testing and learning.

**Projects:**
- **1_DigitalOutput/** – basic digital output example (LED blink)
  - `main.ino` – Arduino sketch for blinking an LED
- **2_Uart/** – UART communication example
  - `uart/uart.ino` – Arduino sketch demonstrating serial communication

---

### `AVR/`
Bare-metal AVR C/C++ examples compiled with `avr-gcc` / `avr-g++`. From lesson 5 onward, projects adopt a more structured layout with Makefiles and shared libraries.

**Projects:**
- **1_DigitalOutput/** – basic digital output using registers
  - `main.cpp`
- **2_Uart/** – UART communication using registers
  - `main.cpp`
- **3_LEDDisplay/** – driving LEDs (multiplexed or single)
  - `main.cpp`
- **4_InputKeyboard/** – reading 4x4 matrix keyboard input
  - `main.cpp`
- **5_LCD_Display/** – LCD 16x2 screen example
  - `main.cpp`, `Makefile`
- **(WIP)6_RGB_LED/** – controlling RGB LED with PWM & an encoder introducing PC interrupts
  - `main.cpp`, `Makefile`

**Utilities:**
- `avr_compile_and_flash.sh` – Bash script to build and flash AVR projects
- `Makefile_template` – template Makefile for new projects

**MISC:**
- **PCINT_test** – PCINT configuration example (test case)

---

### `AVR/common/` – Mini HAL & Shared Libraries

Shared code used across multiple AVR examples, providing a **mini HAL** to simplify development.
```text
common/
    include/
        button.hpp
        common_defs.hpp
        communication.hpp
        gpio.hpp
        interfaces.hpp
        lcd_driver.hpp
        LEDDisplay.hpp
        logger.hpp
        main.hpp
        matrix_kbd.hpp
        relative_encoder.hpp
        system_timer.hpp
        timer8_t.hpp
        utils.hpp
    src/
        interrupts.cpp
        lcd_driver.cpp
        system_timer.cpp
```

**Provided components:**
- **GPIO abstraction** – easy pin configuration and control (portable)
- **Communication** – simple TX/RX handling (flexible & portable)
- **Timers & interrupts** – helper functions for system timers
- **Button handling** – debounced button reading (portable)
- **Rotary encoder (relative) handling** - simple and easy in use encoder handler template (portable)
- **LCD and LED display drivers** – for lessons on displays (portable)
- **Logging utilities** – UART-based debugging

### Important notes

- **logger requires more effort & testing**
- **PCINT handling tested and proven working as designed - example of PCINT configuration is available at PCINT_test**

---

This mini HAL allows lessons to **reuse code consistently** and remain close to hardware, while avoiding boilerplate in each project.

---

### `lesson1_to_4_backup/`
Backup of early lessons to maintain original file layout.

- Includes Arduino and AVR projects for lessons 1–4 exactly as in the course videos
- Original headers and source files preserved
- Use if you want to **follow the course exactly** without refactoring

---

## Requirements

- `avr-gcc`, `avr-g++`, `avr-objcopy`
- `avrdude` (for flashing)
- AVR MCU (e.g., ATmega328P)
- ISP programmer (e.g., USBasp) or Arduino bootloader
- Arduino IDE (for Arduino examples)

---

## Building & Flashing AVR Examples

### Using Makefile (recommended for lessons ≥5)

```bash
make        # builds HEX
make flash  # flashes the MCU
make clean  # cleans build artifacts
```

The Makefile automatically handles MCU type, CPU frequency, compiler flags, and source files.

avr_compile_and_flash.sh can also be used for convenience:
```bash
./avr_compile_and_flash.sh -h
```

Manual compilation (example for ATmega328P)

```bash
avr-g++ -mmcu=atmega328p -DF_CPU=16000000UL -Os -std=c++11 -o main.elf main.cpp
avr-objcopy -O ihex -R .eeprom main.elf main.hex
avrdude -c usbasp -p m328p -U flash:w:main.hex
```