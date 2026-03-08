#pragma once

#include <stdint.h>
#include "../common/include/main.hpp"

class Menu {
public:
    enum class State : uint8_t {
        Idle,
        Settings,
        Info,
        RGBSet,
        State_Num
    };

    enum class MenuEntry : int8_t {
        Settings,
        Info,
        Menu_Num
    };

    enum class SettingEntry : int8_t {
        Red, 
        Green,
        Blue,
        Back,
        Settings_Num
    };

    struct RGB {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };

private:

    static constexpr uint8_t ENCODER_CENTER = 128;
    static constexpr uint8_t ENCODER_THRESHOLD = 5;

    static constexpr uint8_t MAIN_MENU_ITEMS = 2;
    static constexpr uint8_t RGB_MENU_ITEMS  = 4;

    static const char* main_menu[MAIN_MENU_ITEMS];
    static const char* rgb_menu[RGB_MENU_ITEMS];

private:
    RotaryEncoderBtn<uint8_t,0,255>& encoder;
    LiquidCrystal& lcd;

    State state { State::Idle };
    int8_t menu_idx { 0 };
    int8_t setting_idx { 0 }; 

    volatile RGB settings {255,255,255};

    uint8_t last_encoder_value {ENCODER_CENTER};

    static Menu* instance;

private:

    Menu(RotaryEncoderBtn<uint8_t,0,255>& enc_h,
         LiquidCrystal& lcd_h)
         : encoder{ enc_h }, lcd{ lcd_h } {}

public:

    Menu(const Menu&) = delete;
    Menu& operator=(const Menu&) = delete;

    static void init(RotaryEncoderBtn<uint8_t,0,255>& enc,
                      LiquidCrystal& lcd);

    static Menu& get();

    const volatile RGB& get_settings() const {
        return settings;
    }

public:

    void poll();

    void render();

private:

    void handle_encoder_change(int8_t direction);
    void adjust_rgb(int8_t value);

private:

    void render_main_menu();
    void render_rgb_menu();
    void render_rgb_set(SettingEntry e, uint8_t value);
    void render_info();

public:

    static void button_handler(GPIO_interface& btn);
};