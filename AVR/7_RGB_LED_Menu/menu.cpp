#include "menu.h"
#include <stdio.h>

Menu* Menu::instance { nullptr };

const char* Menu::main_menu[MAIN_MENU_ITEMS] = {
    "Settings",
    "Info"
};

const char* Menu::rgb_menu[RGB_MENU_ITEMS] = { 
    "Red",
    "Green",
    "Blue",
    "Back"
};

void Menu::init(RotaryEncoderBtn<uint8_t,0,255>& enc, LiquidCrystal& lcd) {
    if(instance) while(1);
    static Menu inst(enc, lcd);
    instance = &inst;
}

Menu& Menu::get() {
    if(!instance) while(1);
    return *instance;
}

void Menu::poll() {
    uint8_t value = encoder.getValue();

    int16_t diff = (int16_t)value - (int16_t)last_encoder_value;

    if(diff >= ENCODER_THRESHOLD) {
        handle_encoder_change(+1);
        encoder.resetValue(ENCODER_CENTER);
        last_encoder_value = ENCODER_CENTER;
    }
    else if(diff <= -ENCODER_THRESHOLD) {
        handle_encoder_change(-1);
        encoder.resetValue(ENCODER_CENTER);
        last_encoder_value = ENCODER_CENTER;
    }

    encoder.handleBtn();
}

void Menu::handle_encoder_change(int8_t dir) {
    if(state == State::Idle) {
        menu_idx += dir;
        const int8_t num_menu_entries = static_cast<int8_t>(MenuEntry::Menu_Num);

        if (menu_idx < 0) {
            menu_idx = 0;
        }
        else if (menu_idx >= num_menu_entries) {
            menu_idx = num_menu_entries - 1;
        }
    }

    if(state == State::Settings) {
        setting_idx += dir;
        const int8_t num_settings_entries = static_cast<int8_t>(SettingEntry::Settings_Num);

        if (setting_idx < 0) {
            setting_idx = 0;
        }
        else if (setting_idx >= num_settings_entries) {
            setting_idx = num_settings_entries - 1;
        }
    } 
    else if(state == State::RGBSet) {
        adjust_rgb(dir);
    }
}

void Menu::adjust_rgb(int8_t dir) {
    volatile uint8_t* val = nullptr;
    switch(setting_idx) {
        case static_cast<int8_t>(SettingEntry::Red): val = &settings.red; break;
        case static_cast<int8_t>(SettingEntry::Green): val = &settings.green; break;
        case static_cast<int8_t>(SettingEntry::Blue): val = &settings.blue; break;
        default: return;
    }

    int16_t v = *val + dir;
    if(v < 0) v = 0;
    if(v > 255) v = 255;
    *val = v;
}

void Menu::button_handler(GPIO_interface&) {
    Menu& m = Menu::get();

    switch(m.state) {
        case State::Idle:
            switch (m.menu_idx) {
                case static_cast<int8_t>(MenuEntry::Settings):
                    m.state = State::Settings;
                    m.setting_idx = 0;
                break;

                case static_cast<int8_t>(MenuEntry::Info):
                    m.state = State::Info;
                break;
            }
        break;

        case State::Settings:
            switch (m.setting_idx) {
                case static_cast<int8_t>(SettingEntry::Red):
                case static_cast<int8_t>(SettingEntry::Green):
                case static_cast<int8_t>(SettingEntry::Blue):
                    m.state = State::RGBSet;
                break;

                case static_cast<int8_t>(SettingEntry::Back):
                    m.state = State::Idle;
                    m.setting_idx = 0;
                break;
            }
        break;

        case State::RGBSet:
            m.state = State::Settings;
        break;

        case State::Info:
            m.state = State::Idle;
        break;

        default:
        break;
    }
}

void Menu::render() {
    switch(state) {
        case State::Idle: render_main_menu(); break;

        case State::Settings: render_rgb_menu(); break;

        case State::RGBSet: 
            switch (setting_idx) {
                case static_cast<int8_t>(SettingEntry::Red):
                    render_rgb_set(SettingEntry::Red, settings.red);
                break;

                case static_cast<int8_t>(SettingEntry::Green):
                    render_rgb_set(SettingEntry::Green, settings.green);
                break;

                case static_cast<int8_t>(SettingEntry::Blue):
                    render_rgb_set(SettingEntry::Blue, settings.blue);
                break;

                default: 
                    state = State::Idle;
                    menu_idx = setting_idx = 0;
                break;
            }
        break;

        case State::Info: render_info(); break;

        default: break;
    }
}

void Menu::render_main_menu() {
    int8_t label_idx = static_cast<int8_t>(menu_idx);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.puts("> "); lcd.puts(main_menu[label_idx]);
    lcd.setCursor(0,1);
    lcd.puts("  "); lcd.puts(main_menu[(label_idx+1)%MAIN_MENU_ITEMS]);
}

void Menu::render_rgb_menu() {
    int8_t label_idx = static_cast<int8_t>(setting_idx);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.puts("> "); lcd.puts(rgb_menu[setting_idx]);

    lcd.setCursor(0,1);
    uint8_t next = (setting_idx + 1) % RGB_MENU_ITEMS;
    lcd.puts("  "); lcd.puts(rgb_menu[next]);
}

void Menu::render_rgb_set(SettingEntry e, uint8_t value) {
    const int8_t label_idx = static_cast<int8_t>(e);

    lcd.clear();
    lcd.setCursor(0,0); lcd.puts(rgb_menu[label_idx]);
    lcd.setCursor(0,1);
    char buf[17];
    sprintf(buf,"Value: %3d", value);
    lcd.puts(buf);
}

void Menu::render_info() {
    lcd.clear();
    lcd.setCursor(0,0); lcd.puts("RGB Controller");
    lcd.setCursor(0,1); lcd.puts("v1.0");
}