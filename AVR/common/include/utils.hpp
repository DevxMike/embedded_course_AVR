#ifndef utils_h
#define utils_h

#include <stdint.h>

namespace Custom {

template <typename T>
class Optional {
private:
    bool has_value;
    T value;

public:
    Optional(): 
        has_value{ false }, value{ } {}
    
    Optional(const T& v):
        has_value{ true }, value{ v } {}

    const T& get() const {
        return value;
    }

    operator bool() const {
        return has_value;
    }

    void reset() {
        has_value = false;
    }

    void set(const T& val) {
        value = val;
        has_value = true;
    }
};

}

#endif