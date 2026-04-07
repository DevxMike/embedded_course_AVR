#ifndef spi_h
#define spi_h

#include <avr/io.h>
#include <stdint.h>

#include "main.hpp"

#include "communication.hpp"
#include "cyclic_buffer.hpp"
#include "gpio.hpp"

struct SPI_t {
    static constexpr uint16_t buf_size = 126;

    typedef void (*cback_type)(SPI_t&);
    
    volatile uint8_t* const spcr { &SPCR };
    volatile uint8_t* const spsr { &SPSR };
    volatile uint8_t* const spdr { &SPDR };

    // required by Communication class implementation
    cback_type flush_tx { nullptr };
    volatile bool busy;
    
    CyclicBuffer<char, buf_size> rx_buffer;
    CyclicBuffer<char, buf_size> tx_buffer;
};


class SPI_wrapper : public Communication<SPI_t> {
    public:
        enum class prescaler : uint8_t {
            kPresc1 = 0, 
            kPresc2,
            kPresc3,
            kPresc4,
            kNumPrescalers
        };
    
        static SPI_wrapper& instance();
    
        void tx_cplt_cb();
        void rx_cplt_cb();
    
        void spi_int_enable(bool en);
        void spi_enable(bool en);
        void set_data_order(bool is_lsb_first);
        void master_slave_select(bool is_master);
        void set_clock_polarity(bool is_falling);
        void set_clock_phase(bool sample_on_trailing);
        void set_prescaler(prescaler p, bool is_spi_speed_x2);
        void set_ss_pins(GPIO_interface* ss, uint8_t num);
        void set_active_ss_pin(uint8_t idx);

    private:
        SPI_t spi0 {};
        GPIO_interface* ss_pins { nullptr };
        uint8_t num_pins;
        uint8_t active;
    
        SPI_wrapper();
        ~SPI_wrapper() = default;
    
        SPI_wrapper(const SPI_wrapper&) = delete;
        SPI_wrapper& operator=(const SPI_wrapper&) = delete;
    
        static void flush_tx_(SPI_t&);
        static uint8_t prescaler_to_idx(prescaler p);
    };
    

#endif