#include "../include/button.hpp"

// ========================
// callback_set
// ========================

PushButton::callback_set::callback_set(
    on_button_action_cb on_press,
    on_button_action_cb on_release
)
    : on_press_cb{ on_press }, on_release_cb{ on_release } {}


// ========================
// constructors
// ========================

PushButton::PushButton(button_setup& s)
    : button_state{ Idle },
      gen_fn{ s.g },
      old_reading{ true },
      input{ s.btn_io },
      cbacks{ s.action_cb } {}

PushButton::PushButton(GPIO_interface& io, timestamp_generator g, const callback_set& cs)
    : button_state{ Idle },
      gen_fn{ g },
      old_reading{ true },
      input{ io },
      cbacks{ cs } {}


// ========================
// public API
// ========================

void PushButton::init() {
    input.init(GPIO_interface::Direction::INPUT_PULLUP);
}

void PushButton::poll() {
    bool current_reading = input.read_input();

    constexpr bool Released = true;
    constexpr bool Pushed   = false;

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