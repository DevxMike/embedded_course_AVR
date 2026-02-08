#ifndef relative_encoder
#define relative_encoder

#include "button.hpp"
#include "gpio.hpp"

struct base_encoder_setup_t {
    GPIO_interface& pinA;
    GPIO_interface& pinB;
    GPIO_t::pcint_callback_t encoder_change_cb { nullptr };

    base_encoder_setup_t(GPIO_interface& A, GPIO_interface& B, GPIO_t::pcint_callback_t cb):
        pinA{ A }, pinB{ B }, encoder_change_cb{ cb } {} 
};

struct encoder_with_btn_setup_t {
    base_encoder_setup_t base;
    PushButton::button_setup button;
};

template <typename val_t = uint8_t, val_t min = 0, val_t max = 255>
class RotaryEncoder {
public:
    RotaryEncoder(base_encoder_setup_t& setup):
        enc_setup{ setup } {}

    virtual void init() final {
        enc_setup.pinA.init(GPIO_interface::Direction::INPUT_PULLUP);
        enc_setup.pinB.init(GPIO_interface::Direction::INPUT_PULLUP);
        enc_setup.pinA.attach_pcint(enc_setup.encoder_change_cb, (void*)this);
        enc_setup.pinB.attach_pcint(enc_setup.encoder_change_cb, (void*)this);
    }

    void update() {
        uint8_t currState = (enc_setup.pinA.read_input() << 1) | enc_setup.pinB.read_input();

        static const int8_t table[4][4] = {
            { 0, -1, 1, 0 },
            { 1, 0, 0, -1 },
            { -1, 0, 0, 1 },
            { 0, 1, -1, 0 }
        };

        auto diff = table[prevState][currState];

        if (diff > 0 && value < max) {
            ++value;
        }
        else if (diff < 0 && value > min) {
            --value;
        }

        prevState = currState;
    }

    int32_t getValue() const { return value; }

private:
    base_encoder_setup_t& enc_setup;
    volatile uint8_t prevState;
    volatile int32_t value;
};

template <typename val_t = uint8_t, val_t min = 0, val_t max = 255>
class RotaryEncoderBtn : public RotaryEncoder <val_t, min, max>{
public:
    RotaryEncoderBtn(encoder_with_btn_setup_t& setup):
        RotaryEncoder<val_t, min, max>{ setup.base }, enc_btn{ setup.button } {}

    void handleBtn() {
        return enc_btn.poll();
    }

private:
    PushButton enc_btn;
};

#endif