#include "../include/communication.hpp"
#include <util/delay.h>

LiquidCrystal::LiquidCrystal(GPIO_interface& rs, GPIO_interface& enable,
                             GPIO_interface& d0, GPIO_interface& d1, GPIO_interface& d2, GPIO_interface& d3,
                             GPIO_interface& d4, GPIO_interface& d5, GPIO_interface& d6, GPIO_interface& d7)
{
    _rs_pin = &rs;
    _rw_pin = nullptr;
    _enable_pin = &enable;

    _data_pins[0] = &d0; _data_pins[1] = &d1;
    _data_pins[2] = &d2; _data_pins[3] = &d3;
    _data_pins[4] = &d4; _data_pins[5] = &d5;
    _data_pins[6] = &d6; _data_pins[7] = &d7;

    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
    begin(16, 1);
}

LiquidCrystal::LiquidCrystal(GPIO_interface& rs, GPIO_interface& rw, GPIO_interface& enable,
                             GPIO_interface& d0, GPIO_interface& d1, GPIO_interface& d2, GPIO_interface& d3,
                             GPIO_interface& d4, GPIO_interface& d5, GPIO_interface& d6, GPIO_interface& d7)
{
    _rs_pin = &rs;
    _rw_pin = &rw;
    _enable_pin = &enable;

    _data_pins[0] = &d0; _data_pins[1] = &d1;
    _data_pins[2] = &d2; _data_pins[3] = &d3;
    _data_pins[4] = &d4; _data_pins[5] = &d5;
    _data_pins[6] = &d6; _data_pins[7] = &d7;

    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
    begin(16, 1);
}

LiquidCrystal::LiquidCrystal(GPIO_interface& rs, GPIO_interface& enable,
                             GPIO_interface& d0, GPIO_interface& d1, GPIO_interface& d2, GPIO_interface& d3)
{
    _rs_pin = &rs;
    _rw_pin = nullptr;
    _enable_pin = &enable;

    _data_pins[0] = &d0; _data_pins[1] = &d1;
    _data_pins[2] = &d2; _data_pins[3] = &d3;
    for(int i = 4; i < 8; i++) _data_pins[i] = nullptr;

    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    begin(16, 1);
}

LiquidCrystal::LiquidCrystal(GPIO_interface& rs, GPIO_interface& rw, GPIO_interface& enable,
                             GPIO_interface& d0, GPIO_interface& d1, GPIO_interface& d2, GPIO_interface& d3)
{
    _rs_pin = &rs;
    _rw_pin = &rw;
    _enable_pin = &enable;

    _data_pins[0] = &d0; _data_pins[1] = &d1;
    _data_pins[2] = &d2; _data_pins[3] = &d3;
    for(int i = 4; i < 8; i++) _data_pins[i] = nullptr;

    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    begin(16, 1);
}

void LiquidCrystal::begin(uint8_t cols, uint8_t lines, uint8_t dotsize)
{
    if(lines > 1) _displayfunction |= LCD_2LINE;
    _numlines = lines;

    setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);

    if((dotsize != LCD_5x8DOTS) && (lines == 1))
        _displayfunction |= LCD_5x10DOTS;

    _rs_pin->init(GPIO_interface::OUTPUT);
    if(_rw_pin) _rw_pin->init(GPIO_interface::OUTPUT);
    _enable_pin->init(GPIO_interface::OUTPUT);

    for(int i = 0; i < ((_displayfunction & LCD_8BITMODE) ? 8 : 4); ++i)
        if(_data_pins[i]) _data_pins[i]->init(GPIO_interface::OUTPUT);

    _delay_us(50000);

    _rs_pin->set_output(GPIO_interface::LOW);
    _enable_pin->set_output(GPIO_interface::LOW);
    if(_rw_pin) _rw_pin->set_output(GPIO_interface::LOW);

    if(!(_displayfunction & LCD_8BITMODE)) {
        write4bits(0x03);
        _delay_us(4500);
        write4bits(0x03);
        _delay_us(4500);
        write4bits(0x03);
        _delay_us(150);
        write4bits(0x02);
    } else {
        command(LCD_FUNCTIONSET | _displayfunction);
        _delay_us(4500);
        command(LCD_FUNCTIONSET | _displayfunction);
        _delay_us(150);
        command(LCD_FUNCTIONSET | _displayfunction);
    }

    command(LCD_FUNCTIONSET | _displayfunction);
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();
    clear();
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal::clear() {
    command(LCD_CLEARDISPLAY);
    _delay_us(2000);
}

void LiquidCrystal::home() {
    command(LCD_RETURNHOME);
    _delay_us(2000);
}

void LiquidCrystal::setCursor(uint8_t col, uint8_t row) {
    if(row >= _numlines) row = _numlines - 1;
    command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
}

// -----------------------------
// Display control
// -----------------------------
void LiquidCrystal::noDisplay() { _displaycontrol &= ~LCD_DISPLAYON; command(LCD_DISPLAYCONTROL | _displaycontrol); }
void LiquidCrystal::display() { _displaycontrol |= LCD_DISPLAYON; command(LCD_DISPLAYCONTROL | _displaycontrol); }
void LiquidCrystal::noCursor() { _displaycontrol &= ~LCD_CURSORON; command(LCD_DISPLAYCONTROL | _displaycontrol); }
void LiquidCrystal::cursor() { _displaycontrol |= LCD_CURSORON; command(LCD_DISPLAYCONTROL | _displaycontrol); }
void LiquidCrystal::noBlink() { _displaycontrol &= ~LCD_BLINKON; command(LCD_DISPLAYCONTROL | _displaycontrol); }
void LiquidCrystal::blink() { _displaycontrol |= LCD_BLINKON; command(LCD_DISPLAYCONTROL | _displaycontrol); }
void LiquidCrystal::scrollDisplayLeft() { command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT); }
void LiquidCrystal::scrollDisplayRight() { command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT); }
void LiquidCrystal::leftToRight() { _displaymode |= LCD_ENTRYLEFT; command(LCD_ENTRYMODESET | _displaymode); }
void LiquidCrystal::rightToLeft() { _displaymode &= ~LCD_ENTRYLEFT; command(LCD_ENTRYMODESET | _displaymode); }
void LiquidCrystal::autoscroll() { _displaymode |= LCD_ENTRYSHIFTINCREMENT; command(LCD_ENTRYMODESET | _displaymode); }
void LiquidCrystal::noAutoscroll() { _displaymode &= ~LCD_ENTRYSHIFTINCREMENT; command(LCD_ENTRYMODESET | _displaymode); }
void LiquidCrystal::createChar(uint8_t location, uint8_t charmap[]) {
    location &= 0x7;
    command(LCD_SETCGRAMADDR | (location << 3));
    for(uint8_t i=0;i<8;i++) putc(charmap[i]);
}

char LiquidCrystal::putc(char c) {
    send(static_cast<uint8_t>(c), true);
    return c;
}

uint16_t LiquidCrystal::puts(const char* s) {
    uint16_t count = 0;
    while(*s) { putc(*s++); ++count; }
    return count;
}

uint16_t LiquidCrystal::put_buffer(const char* b, uint16_t len) {
    for(uint16_t i = 0; i < len; ++i) putc(b[i]);
    return len;
}

void LiquidCrystal::flush() {
}

void LiquidCrystal::send(uint8_t value, bool mode) {
    _rs_pin->set_output(mode ? GPIO_interface::HIGH : GPIO_interface::LOW);
    if(_rw_pin) _rw_pin->set_output(GPIO_interface::LOW);

    if(_displayfunction & LCD_8BITMODE) write8bits(value);
    else { write4bits(value >> 4); write4bits(value); }
}

void LiquidCrystal::command(uint8_t value) {
  send(value, GPIO_interface::LOW);
}

void LiquidCrystal::setRowOffsets(int row0, int row1, int row2, int row3)
{
  _row_offsets[0] = row0;
  _row_offsets[1] = row1;
  _row_offsets[2] = row2;
  _row_offsets[3] = row3;
}

void LiquidCrystal::pulseEnable() {
  _enable_pin->set_output(GPIO_interface::LOW);
  _delay_us(1);
  _enable_pin->set_output(GPIO_interface::HIGH);
  _delay_us(1);
  _enable_pin->set_output(GPIO_interface::LOW);
  _delay_us(50);
}

void LiquidCrystal::write4bits(uint8_t value) {
  for(int i=0;i<4;i++)
      if(_data_pins[i])
          _data_pins[i]->set_output((value >> i) & 0x01 ? GPIO_interface::HIGH : GPIO_interface::LOW);
  pulseEnable();
}

void LiquidCrystal::write8bits(uint8_t value) {
    for(int i=0;i<8;i++) if(_data_pins[i]) _data_pins[i]->set_output((value>>i) & 1 ? GPIO_interface::HIGH : GPIO_interface::LOW);
    pulseEnable();
}
