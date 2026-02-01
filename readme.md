# AVR Course – Examples Repository

This repository contains examples presented throughout the AVR programming course.
The course shows how to work with AVR microcontrollers using both low-level AVR C/C++
(register-level programming) and the Arduino framework for comparison.

The repository evolves together with the course – later lessons introduce refactoring,
code reuse, and more structured project layouts, similar to real embedded projects.



## Repository Structure

### `ArduinoAVR/`
Contains Arduino-based equivalents of the AVR examples.
These sketches use the Arduino API and are intended to:
- show the same ideas in a higher-level environment
- help beginners understand the concepts faster
- allow quick testing and prototyping

---

### `AVR/`
Contains bare-metal AVR C/C++ examples using `avr-gcc` / `avr-g++`.

From lesson 5 onward, the project structure changes to better reflect
real-world embedded development practices.

#### `AVR/common/`
Shared code used across multiple AVR examples.

- `include/` – reusable headers (drivers, abstractions, utilities)
- `src/` – source files with implementations (interrupts, timers, etc.)

This folder contains reusable components such as:
- GPIO abstraction
- UART communication
- timers and interrupts
- button handling with debouncing
- LED display drivers
- logging utilities

---

### ⚠️ Important note about earlier lessons (1–4)

Lessons **1–4 were refactored** to introduce a shared `common/` directory.
Because of this change:

- Older projects **may not compile without modification**
- Header paths and file locations have changed

To preserve full backward compatibility, **a complete backup of lessons 1–4**
is available in: AVR/lesson1_to_4_backup/


This backup contains:
- the original project layout
- original headers and source files
- Arduino and AVR examples exactly as shown in the early lessons

👉 If you want to build or follow lessons 1–4 **exactly as presented in the course videos**,
use the projects from `lesson1_to_4_backup`.

---

## Requirements

- `avr-gcc`, `avr-g++`, `avr-objcopy`
- `avrdude`
- AVR MCU (e.g. ATmega328P)
- ISP programmer (e.g. USBasp) or Arduino bootloader
- Arduino IDE (for Arduino examples)

---

## Building AVR examples (manual)


Example for ATmega328P:
avr-g++ -mmcu=atmega328p -DF_CPU=16000000UL -Os -std=c++11 -o main.elf main.cpp
avr-objcopy -O ihex -R .eeprom main.elf main.hex
avrdude -c usbasp -p m328p -U flash:w:main.hex

### Build and flash script for atmega
Using the included Makefile

Each AVR project (starting from project no. 5) comes with a ready-to-use Makefile, already configured
for that project. You can build and flash the project directly with:

  make        # builds the HEX
  make flash  # flashes the HEX to your MCU
  make clean  # cleans build artifacts

The Makefile automatically handles:

- AVR MCU type (default: atmega328p)

- CPU frequency (F_CPU)

- compiler flags and source files

- building and flashing workflow

This allows you to compile and program each project quickly without
modifying any paths or flags.

This repository includes a bash script that allows you to conveniently build and flash your project to an ATmega microcontroller.
By default, the script targets the atmega328p controller.

To see all available options and usage instructions, run:
  ./avr_compile_and_flash.sh -h

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


