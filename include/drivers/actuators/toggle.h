#ifndef TOGGLE_ACTUATOR_H
#define TOGGLE_ACTUATOR_H

#include <Arduino.h>

#define TOGGLE_CLOSED 0
#define TOGGLE_OPEN 1

#define TOGGLE_NC 0
#define TOGGLE_NO 1

typedef struct toggle_config_t {
    uint8_t pin;
    uint8_t type;
} toggle_config_t;

class ToggleActuator{
private:
    uint8_t pin;
    uint8_t type;
    
    uint8_t position = 0;
public:
    void init(const toggle_config_t& config);
    void calibrate();
    void write(const uint8_t _position);
    uint8_t get();
};

// Your code here

#endif // TOGGLE_ACTUATOR_H