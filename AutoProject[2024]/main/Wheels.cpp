#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "Wheels.h"

#define SET_MOVEMENT(side, f, b) \
    digitalWrite(side[0], f);    \
    digitalWrite(side[1], b)

Wheels::Wheels()
{
}

void Wheels::attachRight(int pF, int pB, int pS)
{
    pinMode(pF, OUTPUT);
    pinMode(pB, OUTPUT);
    pinMode(pS, OUTPUT);
    this->pinsRight[0] = pF;
    this->pinsRight[1] = pB;
    this->pinsRight[2] = pS;
}

void Wheels::attachLeft(int pF, int pB, int pS)
{
    pinMode(pF, OUTPUT);
    pinMode(pB, OUTPUT);
    pinMode(pS, OUTPUT);
    this->pinsLeft[0] = pF;
    this->pinsLeft[1] = pB;
    this->pinsLeft[2] = pS;
}

void Wheels::setSpeedRight(uint8_t s)
{
    analogWrite(this->pinsRight[2], s);
}

void Wheels::setSpeedLeft(uint8_t s)
{
    analogWrite(this->pinsLeft[2], s);
}

void Wheels::setSpeed(uint8_t s)
{
    setSpeedLeft(s);
    setSpeedRight(s);
}

void Wheels::attach(int pRF, int pRB, int pRS, int pLF, int pLB, int pLS)
{
    this->attachRight(pRF, pRB, pRS);
    this->attachLeft(pLF, pLB, pLS);
}

void Wheels::forwardLeft()
{
    SET_MOVEMENT(pinsLeft, HIGH, LOW);
}

void Wheels::forwardRight()
{
    SET_MOVEMENT(pinsRight, HIGH, LOW);
}

void Wheels::backLeft()
{
    SET_MOVEMENT(pinsLeft, LOW, HIGH);
}

void Wheels::backRight()
{
    SET_MOVEMENT(pinsRight, LOW, HIGH);
}

void Wheels::forward()
{
    this->forwardLeft();
    this->forwardRight();
}

void Wheels::back()
{
    this->backLeft();
    this->backRight();
}

void Wheels::stopLeft()
{
    SET_MOVEMENT(pinsLeft, LOW, LOW);
}

void Wheels::stopRight()
{
    SET_MOVEMENT(pinsRight, LOW, LOW);
}

void Wheels::stop()
{
    this->stopLeft();
    this->stopRight();
}

void Wheels::goForward(int cm, int cntl, int cntr)
{
    // 0.5 cm - 1 counter
    // 40 counter = 20cm
    int counter = (cntl + cntr) / 4;
    while (cm < counter)
    {
        this->forward();
    }
    this->stop();
}

void Wheels::goBack(int cm)
{
    this->setSpeed(200);
    this->back();
    delay(cm * 18.6);
    this->stop();
}

void Wheels::goForward_delay(int cm)
{
    // 18,518ms = 1cm; v = 200;
    // 1000ms = 54cm;
    // S = v * t
    this->setSpeed(200);
    this->forward();
    delay(cm * 18.6);
    this->stop();
}

void Wheels::goBack_delay(int cm)
{
    this->setSpeed(200);
    this->back();
    delay(cm * 18.6);
    this->stop();
}
