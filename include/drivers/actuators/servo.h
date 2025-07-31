#ifndef SERVO_ACTUATOR_H
#define SERVO_ACTUATOR_H

#include <inttypes.h>
#include <PWMServo.h>

#define SERVO_CLOSED 0
#define SERVO_OPEN 110

typedef struct servo_config_t {
    uint8_t pin;
} servo_config_t;

class ServoActuator{
private:
    uint8_t pin;

    PWMServo instance;
    int32_t position = 0;
public:
    void init(const servo_config_t& config);
    void calibrate();
    void write(const int32_t _position);
    int32_t get();
};

#endif // SERVO_ACTUATOR_H