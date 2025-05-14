#ifndef matrix_kbd_hpp
#define matrix_kbd_hpp

#include "gpio.hpp"
#include "utils.hpp"

template <uint8_t num_rows, uint8_t num_cols>
class MatrixKBD {
private:
    Digital_IO* cols;
    Digital_IO* rows;

    volatile uint8_t current_col;
    volatile bool button_readings[num_cols * num_rows];

    // NOTE: Passing val >= num_cols means "deactivate all columns"
    void set_active_col(uint8_t col) {
        for(uint8_t i = 0; i < num_cols; ++i) {
            if(i == col) {
                cols[i].init(Digital_IO::OUTPUT);
            }
            else {
                cols[i].init(Digital_IO::INPUT_PULLUP);
            }
        }
    }

    void set_all_cols_input() {
        set_active_col(num_cols);
    }
    
    uint8_t get_index(uint8_t row, uint8_t col) const {
        return row * num_cols + col;
    }

    void update_readings(uint8_t row, uint8_t col, bool state) {
        button_readings[get_index(row, col)] = state;
    }

public:
    MatrixKBD(Digital_IO* cols_io, Digital_IO* rows_io):
        cols { cols_io }, rows { rows_io }, current_col { 0 } {
            for(uint8_t i = 0; i < num_rows * num_cols; ++i) {
                button_readings[i] = false;
            }
        }
    
    void begin() {
        set_all_cols_input();

        for(uint8_t i = 0; i < num_rows; ++i) {
            rows[i].init(Digital_IO::INPUT_PULLUP);
        }
    }

    const volatile bool* get_readings() const {
        return button_readings;
    }

    Custom::Optional<bool> get_button_reading(int row, int col) const {
        if(row >= num_rows || col >= num_cols) {
            return {};
        }
        else {
            bool reading = static_cast<bool>(button_readings[get_index(row, col)]);
            return Custom::Optional<bool>(reading); 
        }
    }
    
    void poll() {
        for(uint8_t i = 0; i < num_rows; ++i) {
            bool row_reading = (rows[i].read_input()? false : true);
            update_readings(i, current_col, row_reading);
        }

        set_all_cols_input();

        current_col = (current_col + 1) % num_cols;
        cols[current_col].init(Digital_IO::OUTPUT);
        cols[current_col].set_output(false);
    }
};

#endif