#include "../include/spi.hpp"
#include "../include/gpio.hpp"

// ========================
// Singleton
// ========================

SPI_wrapper& SPI_wrapper::instance() {
    static SPI_wrapper inst;
    return inst;
}

// ========================
// ctor
// ========================

SPI_wrapper::SPI_wrapper()
    : spi0{ }, Communication<SPI_t>{ spi0 }
{
    spi0.flush_tx = flush_tx_;
}

// ========================
// helpers
// ========================

uint8_t SPI_wrapper::prescaler_to_idx(prescaler p) {
    return static_cast<uint8_t>(p);
}

// ========================
// TX logic
// ========================

void SPI_wrapper::flush_tx_(SPI_t&) {
    auto& inst = instance();
    auto& spi = inst.spi0;

    auto next = spi.tx_buffer.pop();

    if (next) {
        if (inst.ss_pins) {
            inst.ss_pins[inst.active].set_output(GPIO_interface::Output::LOW);
        }
        *spi.spdr = next.get();
    }
}

void SPI_wrapper::tx_cplt_cb() {
    auto& inst = instance();
    auto& spi = inst.spi0;

    auto next = spi.tx_buffer.pop();

    if (next) {
        *spi.spdr = next.get();
    }
    else {
        spi.busy = false;
        if (inst.ss_pins) {
            inst.ss_pins[inst.active].set_output(GPIO_interface::Output::HIGH);
        }
    }
}
void SPI_wrapper::rx_cplt_cb() {
    volatile uint8_t dummy = *spi0.spdr;
    (void)dummy;
}

// ========================
// config
// ========================

void SPI_wrapper::spi_int_enable(bool en) {
    if (en) {
        *spi0.spcr |= (1 << SPIE);
    }
    else {
        *spi0.spcr &= ~(1 << SPIE);
    }
}

void SPI_wrapper::spi_enable(bool en) {
    if (en) {
        *spi0.spcr |= (1 << SPE);
    }
    else {
        *spi0.spcr &= ~(1 << SPE);
    }
}

void SPI_wrapper::set_data_order(bool is_lsb_first) {
    if (is_lsb_first) {
        *spi0.spcr |= (1 << DORD);
    }
    else {
        *spi0.spcr &= ~(1 << DORD);
    }
}

void SPI_wrapper::master_slave_select(bool is_master) {
    if (is_master) {
        *spi0.spcr |= (1 << MSTR);
    }
    else {
        *spi0.spcr &= ~(1 << MSTR);
    }
}

void SPI_wrapper::set_clock_polarity(bool is_falling) {
    if (is_falling) {
        *spi0.spcr |= (1 << CPOL);
    }
    else {
        *spi0.spcr &= ~(1 << CPOL);
    }
}

void SPI_wrapper::set_clock_phase(bool sample_on_trailing) {
    if (sample_on_trailing) {
        *spi0.spcr |= (1 << CPHA);
    }
    else {
        *spi0.spcr &= ~(1 << CPHA);
    }
}

void SPI_wrapper::set_prescaler(prescaler p, bool is_spi_speed_x2) {
    auto idx = prescaler_to_idx(p);
    const auto num = prescaler_to_idx(prescaler::kNumPrescalers);

    if (idx < num) {
        constexpr uint8_t mask = (1 << SPR0) | (1 << SPR1);

        *spi0.spcr = (*spi0.spcr & ~mask) | (idx << SPR0);

        if (is_spi_speed_x2) {
            *spi0.spsr |= (1 << SPI2X);
        }
        else {
            *spi0.spsr &= ~(1 << SPI2X);
        }
    }
}

void SPI_wrapper::set_ss_pins(GPIO_interface* ss, uint8_t num) {
    if (ss) {
        ss_pins = ss;
        num_pins = num;
        active = 0;

        for (uint8_t i = 0; i < num; ++i) {
            ss[i].init(GPIO_interface::Direction::OUTPUT);
            ss[i].set_output(GPIO_interface::Output::HIGH);
        }
    }
}
void SPI_wrapper::set_active_ss_pin(uint8_t idx) {
    if (idx < num_pins) {
        active = idx;
    }
}