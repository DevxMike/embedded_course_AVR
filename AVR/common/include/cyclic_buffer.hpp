#ifndef cyclic_buffer_h
#define cyclic_buffer_h

#include "utils.hpp"

template<typename T, uint16_t size>
class CyclicBuffer {
private:
    T values[size];
    volatile uint16_t write_idx;
    volatile uint16_t read_idx;

public:
    CyclicBuffer():
        write_idx{ 0 }, read_idx{ 0 } { }
    
    bool empty() const {
        return write_idx == read_idx;
    }

    void push(const T& val) {
        values[write_idx] = val;

        write_idx = (write_idx + 1) % size;
    }

    Custom::Optional<T> pop() {
        if(empty()) {
            return { };
        }
        else {
            Custom::Optional<T> retval{ values[read_idx] };

            read_idx = (read_idx + 1) % size;
            return retval;
        }
    }

    Custom::Optional<T> peek() const {
        if(empty()) {
            return { };
        }
        else {
            return Custom::Optional<T>{ values[read_idx] };
        }
    }
};

#endif