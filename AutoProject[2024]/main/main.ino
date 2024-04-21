#include "Wheels.h"
#include "Lcd.h"
#include "PinChangeInterrupt.h"
#include <LiquidCrystal_I2C.h>
#include <IRremote.hpp>
#include <Servo.h>

#define DECODE_NEC

// pin silnika sensora (kontroli serwo (musi być PWM))
#define SERVO 9

// pin pilota
#define IR_RECEIVE_PIN 2

// piny dla liczników obrotu
#define INTINPUT0 A0
#define INTINPUT1 A1

// piny dla sonaru (HC-SR04)
#define TRIG A2
#define ECHO A3

Wheels w;
Lcd lcd;
Servo serwo;

volatile char cmd;
volatile int cnt0, cnt1;
volatile int counter;
volatile int lks = 200; // predkość lewe koło
volatile int rks = 200; // predkość prawe koło
volatile int ct;
volatile int tp1;
volatile int tp2;

void setup()
{
    // 2/4 - prawy przod | 4/7 - prawy tyl | 3/5 - prawy zasilanie | 7/13 - lewy tyl | 8/12 - lewy przod | 5/3 - lewy zasilanie
    w.attach(4, 7, 6, 12, 8, 5); // startuj kola
    lcd.attach();               // startuj lcd
    serwo.attach(SERVO);        // startuj silnik sensora

    Serial.begin(9600);
    // Serial.setTimeout(200);

    w.setSpeedLeft(lks);
    w.setSpeedRight(rks);

    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    pinMode(TRIG, OUTPUT); // TRIG startuje sonar
    pinMode(ECHO, INPUT);  // ECHO odbiera powracający impuls

    pinMode(INTINPUT0, INPUT);
    pinMode(INTINPUT1, INPUT);

    cnt0 = 0; // licznik lewy
    cnt1 = 0; // licznik prawy

    tp1 = tp2 = ct = 0; // delay procedury 1,2 | ct = curent time

    attachPCINT(digitalPinToPCINT(INTINPUT0), increment, CHANGE); // startuj lewy licznik obrotów
    attachPCINT(digitalPinToPCINT(INTINPUT1), increment, CHANGE); // startuj prawy licznik obrotów

    serwo.write(90); // ustaw kąt sensora 90
}

void loop()
{
    //controler();
    //GoForwardAndLook();
    //  pilot();
    /*
    ct = millis();
    if (ct >= tp1 + 500)
    {
        tp1 = ct;
        lcd.lcd_tachometr(cnt0, cnt1);
        lcd.lcd_adometr(cnt0, cnt1);
        Serial.println("obroty: ");
        Serial.print(cnt0);
        Serial.print(" ");
        Serial.println(cnt1);
    }
    if (ct >= tp2 + 100)
    {
        tp2 = ct;
        sync_wheels(cnt0, cnt1);
    }
    */

    // BA45FF00 - 1 - 0x45
    // B946FF00 - 2 - 0x46
    // B847FF00 - 3 - 0x47
    // BB44FF00 - 4 - 0x44
    // BF40FF00 - 5 - 0x40
    // BC43FF00 - 6 - 0x43
    // F807FF00 - 7 - 0x07
    // EA15FF00 - 8 - 0x15
    // F609FF00 - 9 - 0x9
    // E619FF00 - 0 - 0x19
    // gora - 0x18 - 24
    // dol - 0x52 - 82
    // lewo - 0x8 - 8
    // prawo - 0x5A - 90
    // ok - 0x1C - 28
    
    if (IrReceiver.decode())
    {
        //Serial.println(IrReceiver.decodedIRData.command);
        //Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // Print "old" raw data
        //IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
        //IrReceiver.printIRSendUsage(&Serial);   // Print the statement required to send this data
        //IrReceiver.resume(); // Enable receiving of the next value
        //cmd = IrReceiver.decodedIRData.command;
        switch (IrReceiver.decodedIRData.command)
        {
        case 0x18:
            Serial.println("Forward");
            w.forward();
            lcd.lcd_way_forward();
            break;
        case 0x52:
            Serial.println("Back");
            w.back();
            lcd.lcd_way_back();
            break;
        case 0x8:
            Serial.println("Left");
            w.stopLeft();
            lcd.lcd_way_left();
            break;
         case 0x5A:
            Serial.println("Right");
            w.stopRight();
            lcd.lcd_way_right();
            break;
        case 0x1C:
            w.stop();
            lcd.clear();
            rks = lks = 200;
            w.setSpeedLeft(lks);
            w.setSpeedRight(rks);
            break;
        }
        IrReceiver.resume();
    }
    
}

void GoForwardAndLook()
{
    unsigned long tot; // czas powrotu (time-of-travel)
    unsigned int distance;

    //w.forward();
    serwo.write(90); // patrzy prosto

    /* uruchamia sonar (puls 10 ms na `TRIGGER')
     * oczekuje na powrotny sygnał i aktualizuje
     */
    digitalWrite(TRIG, HIGH);
    delay(10);
    digitalWrite(TRIG, LOW);
    tot = pulseIn(ECHO, HIGH);
    /* prędkość dźwięku = 340m/s => 1 cm w 29 mikrosekund
     * droga tam i z powrotem, zatem:
     */
    distance = tot / 58;

    if (distance <= 30)
    {
        w.stop();
        Serial.println("Przeszkoda z przodu");
        for (byte angle = 0; angle < 180; angle += 20)
        {
            lookAndTellDistance(angle);
            delay(500);
        }
        w.back();
        delay(600);
        w.stop();
    }
}

void lookAndTellDistance(byte angle)
{

    unsigned long tot; // czas powrotu (time-of-travel)
    unsigned int distance;

    Serial.print("Patrzę w kącie ");
    Serial.print(angle);
    serwo.write(angle);

    /* uruchamia sonar (puls 10 ms na `TRIGGER')
     * oczekuje na powrotny sygnał i aktualizuje
     */
    digitalWrite(TRIG, HIGH);
    delay(10);
    digitalWrite(TRIG, LOW);
    tot = pulseIn(ECHO, HIGH);

    /* prędkość dźwięku = 340m/s => 1 cm w 29 mikrosekund
     * droga tam i z powrotem, zatem:
     */
    distance = tot / 58;

    Serial.print(": widzę coś w odległości ");
    Serial.println(distance);
}

void increment()
{
    if (digitalRead(INTINPUT0))
        cnt0++;
    if (digitalRead(INTINPUT1))
        cnt1++;
}

void sync_wheels(int ll, int rl)
{
    int s = (lks + rks) / 2;

    lcd.lcd_speedometr(s);

    if (ll < rl - 10)
    {
        lks += 8;
        rks -= 8;
        w.setSpeedLeft(lks);
        w.setSpeedRight(rks);
    }

    if (rl < ll - 10)
    {
        rks += 8;
        lks -= 8;
        w.setSpeedLeft(lks);
        w.setSpeedRight(rks);
    }
    Serial.println("speed: ");
    Serial.print(lks);
    Serial.print(" ");
    Serial.println(rks);
}

void goForward(int cm)
{
    // 0.5 cm - 1 counter
    // 40 counter = 20cm
    counter = (cnt0 + cnt1) / 4;
    w.forward();
    if (cm < counter)
    {
        w.stop();
    }
    // if (cm < counter)
    //     w.forward();
}

void pilot()
{
    // BA45FF00 - 1 - 0x45
    // B946FF00 - 2 - 0x46
    // B847FF00 - 3 - 0x47
    // BB44FF00 - 4 - 0x44
    // BF40FF00 - 5 - 0x40
    // BC43FF00 - 6 - 0x43
    // F807FF00 - 7 - 0x07
    // EA15FF00 - 8 - 0x15
    // F609FF00 - 9 - 0x9
    // E619FF00 - 0 - 0x19
    // gora - 0x18
    // dol - 0x52
    // lewo - 0x8
    // prawo - 0x5A
    // ok - 0x1C
    while (IrReceiver.decode())
    {
        cmd = IrReceiver.decodedIRData.command;
        switch (cmd)
        {
        case '24':
            w.forward();
            lcd.lcd_way_forward();
            break;
        case '0x52':
            w.back();
            lcd.lcd_way_back();
            break;
        case 'a':
            w.forwardLeft();
            break;
        case 'd':
            w.forwardRight();
            break;
        case 'z':
            w.backLeft();
            break;
        case 'c':
            w.backRight();
            break;
        case '0x1C':
            w.stop();
            lcd.clear();
            rks = lks = 200;
            w.setSpeedLeft(lks);
            w.setSpeedRight(rks);
            break;
        case 'g':
            goForward(10);
            // w.goForward_delay(10);
            lcd.lcd_way_forward();
            break;
        case 'b':
            w.goBack_delay(10);
            lcd.lcd_way_back();
            break;
        case 'v':
            w.setSpeed(200);
            lcd.lcd_speedometr(200);
            break;
        }
    }
    IrReceiver.resume();
}

void controler()
{
    while (Serial.available())
    {
        cmd = Serial.read();
        switch (cmd)
        {
        case 'w':
            w.forward();
            lcd.lcd_way_forward();
            break;
        case 'x':
            w.back();
            lcd.lcd_way_back();
            break;
        case 'a':
            w.forwardLeft();
            break;
        case 'd':
            w.forwardRight();
            break;
        case 'z':
            w.backLeft();
            break;
        case 'c':
            w.backRight();
            break;
        case 's':
            w.stop();
            lcd.clear();
            rks = lks = 200;
            w.setSpeedLeft(lks);
            w.setSpeedRight(rks);
            break;
        case 'g':
            goForward(10);
            // w.goForward_delay(10);
            lcd.lcd_way_forward();
            break;
        case 'b':
            w.goBack_delay(10);
            lcd.lcd_way_back();
            break;
        case 'v':
            w.setSpeed(0);
            w.setSpeed(200);
            lcd.lcd_speedometr(200);
            break;
        }
    }
}