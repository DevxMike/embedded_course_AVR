#!/bin/bash

# Default values
controller="atmega328p"
fcpu=16000000
input="main.cpp"
elf_target="main.elf"
hex_target="main.hex"
programmer="usbasp"
dev_signature="m328p"

# Help function
print_help() {
    echo "Usage: ./build.sh [options]"
    echo
    echo "Options:"
    echo "  --controller=MCU       Target microcontroller (default: atmega328p)"
    echo "  --fcpu=FREQ_HZ         CPU frequency in Hz (default: 16000000)"
    echo "  --input=FILE           C++ source file (default: main.cpp)"
    echo "  --programmer=NAME      AVR programmer name (default: usbasp)"
    echo "  --devsignature=SIGN    Device signature (default: m328p)"
    echo "  -h, --help             Show this help message and exit"
    exit 0
}

# Parse arguments
for arg in "$@"; do
    case $arg in
        --controller=*)
            controller="${arg#*=}"
            ;;
        --fcpu=*)
            fcpu="${arg#*=}"
            ;;
        --input=*)
            input="${arg#*=}"
            ;;
        --programmer=*)
            programmer="${arg#*=}"
            ;;
        --devsignature=*)
            dev_signature="${arg#*=}"
            ;;
        -h|--help)
            print_help
            ;;
        *)
            echo "Unknown option: $arg"
            echo "Use -h or --help to see available options."
            exit 1
            ;;
    esac
done

# Compilation
avr-g++ -mmcu=$controller -DF_CPU=$fcpu -Os -std=c++11 -o $elf_target $input
if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

# Convert ELF to HEX
avr-objcopy -O ihex -R .eeprom $elf_target $hex_target

# Clean up
rm $elf_target

# Flash the HEX file to the microcontroller
avrdude -c $programmer -p $dev_signature -U flash:w:$hex_target
