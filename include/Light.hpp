#ifndef LIGHT_H
#define LIGHT_H

#include <inttypes.h>

class Light 
{   
public:
    Light(uint8_t pin) : 
        _onState(false), 
        _pwmValue(0),
        _pin(pin) {}

    bool getOnState();
    uint8_t getPWM();
    void on();
    void off();
    void pwm(uint8_t value);

protected:
    void writeOutput();

    bool _onState;
    uint8_t _pwmValue;
    uint8_t _pin;    
};

#endif