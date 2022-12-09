#include <Light.hpp>
#include <Arduino.h>

bool Light::getOnState()
{
    return _onState;
}

uint8_t Light::getPWM()
{
    return _pwmValue;
}

void Light::on()
{
    _onState = true;
    writeOutput();
}

void Light::off()
{
    _onState = false;
    writeOutput();
}

void Light::pwm(uint8_t value)
{
    _pwmValue = value;
    writeOutput();
}

void Light::writeOutput()
{
    if (_onState) {
        analogWrite(_pin, 255 - _pwmValue);
    } else {
        digitalWrite(_pin, true);
    }    
}