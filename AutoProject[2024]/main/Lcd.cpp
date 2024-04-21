#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "Lcd.h"

byte LCDAddress = 0x27;

LiquidCrystal_I2C lcd_i2c(LCDAddress, 16, 2);

Lcd::Lcd()
{
}

void Lcd::attach()
{
    lcd_i2c.init();
    lcd_i2c.backlight();
}

void Lcd::lcd_adometr(int l, int r)
{
    int s = (l + r) / 4;
    lcd_i2c.setCursor(0, 0);
    lcd_i2c.print(s);
    lcd_i2c.print("cm");
}

void Lcd::lcd_tachometr(int l, int r)
{
    lcd_i2c.setCursor(5, 1);
    int a = (l + r) / 2;
    lcd_i2c.print(a);
}

void Lcd::lcd_sensor()
{
    lcd_i2c.setCursor(11, 1);
    // lcd_i2c.print(angle);
    // lcd_i2c.print("^!");
    // lcd_i2c.print("v!");
    // lcd_i2c.print(">!");
    // lcd_i2c.print("<!");
}

void Lcd::lcd_speedometr(int l, int r)
{
    lcd_i2c.setCursor(9, 0);
    int s = (l + r) / 4;
    int t = millis();
    int v = s / t;
    lcd_i2c.print(v);
    lcd_i2c.print("cm/s");
}

void Lcd::lcd_way_forward()
{
    lcd_i2c.setCursor(0, 1); //(0 - pozycja od 0 do 15, 1 rzad od 0 do 1)
    lcd_i2c.print("^");
}

void Lcd::lcd_way_back()
{
    lcd_i2c.setCursor(0, 1);
    lcd_i2c.print("v");
}

void Lcd::lcd_way_right()
{
    lcd_i2c.setCursor(1, 1);
    lcd_i2c.print(">");
}

void Lcd::lcd_way_left()
{
    lcd_i2c.setCursor(1, 1);
    lcd_i2c.print("<");
}

void Lcd::clear()
{
    lcd_i2c.clear();
}