#ifndef button_hpp
#define button_hpp

#include "gpio.hpp"

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
        ):
            on_press_cb { on_press }, on_release_cb { on_release } {};
    };

 
    
    PushButton(GPIO_interface& io, timestamp_generator g, const callback_set& cs):
        button_state { Idle }, gen_fn{ g }, old_reading { true }, input { io }, cbacks { cs } {}

    void poll() {
        bool current_reading = input.read_input();

        enum {
            Released = true,
            Pushed = false
        } ReadingMapping;

        switch(button_state) {
            case Idle:
                if(old_reading != current_reading) {
                    old_reading = current_reading;
                    button_timer = gen_fn();
                    button_state = Debouncing;
                }
                break;

            case Debouncing:
                if(gen_fn() - button_timer >= debouncing_period) {
                    if(old_reading == current_reading) {
                        button_state = Action;
                    }
                    else {
                        old_reading = current_reading;
                        button_state = Idle;
                    }
                }
                break;

            case Action:
                if(old_reading == Released && cbacks.on_release_cb != nullptr) {
                    cbacks.on_release_cb(input);
                }
                else if(old_reading == Pushed && cbacks.on_press_cb != nullptr) {
                    cbacks.on_press_cb(input);
                }

                old_reading = current_reading;
                button_state = Idle;
                break;

            default:
                button_state = Idle;
                break;
        }
    }

private:
    enum {
        Idle,
        Debouncing,
        Action
    } ButtonStates;

    uint8_t button_state;
    uint32_t button_timer;
    bool old_reading;
    timestamp_generator gen_fn;
    GPIO_interface& input;
    callback_set cbacks;
};

#endif