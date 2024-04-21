#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#ifndef LCD_H
#define LCD_H

class Lcd
{
public:
    Lcd();

    void attach();
    void clear();

    void lcd_adometr(int, int);
    void lcd_tachometr(int, int);
    void lcd_speedometr(int, int);
    void lcd_way_forward();
    void lcd_way_back();
    void lcd_way_left();
    void lcd_way_right();
    void lcd_sensor();
};

#endif