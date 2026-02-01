# =========================
# Toolchain
# =========================
CXX        := avr-g++
OBJCOPY    := avr-objcopy
AVRDUDE    := avrdude

# =========================
# Default configuration
# =========================
MCU        ?= atmega328p
F_CPU      ?= 16000000
PROGRAMMER ?= usbasp
DEVSIG     ?= m328p

# =========================
# Targets
# =========================
TARGET     := main
ELF        := $(TARGET).elf
HEX        := $(TARGET).hex

# =========================
# Sources
# =========================
SRC_MAIN   := main.cpp
SRC_COMMON := $(wildcard ../common/src/*.cpp)

SRCS       := $(SRC_MAIN) $(SRC_COMMON)
OBJS       := $(SRCS:.cpp=.o)

# =========================
# Flags
# =========================
CXXFLAGS := -mmcu=$(MCU) \
            -DF_CPU=$(F_CPU) \
            -Os \
            -std=c++11 \
            -Wall \
            -Wextra

LDFLAGS  := -mmcu=$(MCU)

# =========================
# Rules
# =========================
all: $(HEX)

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(ELF):
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@

flash: $(HEX)
	$(AVRDUDE) -c $(PROGRAMMER) -p $(DEVSIG) -U flash:w:$(HEX)

clean:
	rm -f $(OBJS) $(ELF) $(HEX)

help:
	@echo "Available targets:"
	@echo "  all        Build HEX (default)"
	@echo "  flash      Flash HEX to MCU"
	@echo "  clean      Remove build artifacts"
	@echo
	@echo "Configurable variables:"
	@echo "  MCU=atmega328p"
	@echo "  F_CPU=16000000"
	@echo "  PROGRAMMER=usbasp"
	@echo "  DEVSIG=m328p"

