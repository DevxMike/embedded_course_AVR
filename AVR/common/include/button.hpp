#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <stdint.h>
#include "../include/interfaces.hpp"

class PushButton {
public:
    static constexpr uint32_t debouncing_period = 50;

    typedef uint32_t (*timestamp_generator)();  
    typedef void (*on_button_action_cb)(GPIO_interface&);  

    struct callback_set {
        on_button_action_cb on_press_cb;
        on_button_action_cb on_release_cb;

        callback_set(
            on_button_action_cb on_press = nullptr, 
            on_button_action_cb on_release = nullptr
        );
    };

    struct button_setup {
        GPIO_interface& btn_io;
        timestamp_generator g;
        const callback_set action_cb;
    };

    PushButton(button_setup& s);
    PushButton(GPIO_interface& io, timestamp_generator g, const callback_set& cs);

    void init();
    void poll();

private:
    enum State : uint8_t {
        Idle,
        Debouncing,
        Action
    };

    uint8_t button_state;
    uint32_t button_timer;
    bool old_reading;
    timestamp_generator gen_fn;
    GPIO_interface& input;
    callback_set cbacks;
};

#endif