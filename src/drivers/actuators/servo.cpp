#include "drivers/actuators/servo.h"
#include "logger.h"
void ServoActuator::init(const servo_config_t& config) {
    pin = config.pin;
    instance.attach(pin);
}

void ServoActuator::calibrate() {
    //TODO
}

void ServoActuator::write(const int32_t _position) {
    position = _position;
    instance.write(_position);
    
}

int32_t ServoActuator::get() {
    return position;
}
