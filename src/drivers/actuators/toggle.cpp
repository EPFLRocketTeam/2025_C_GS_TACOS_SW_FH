#include "drivers/actuators/toggle.h"

void ToggleActuator::init(const toggle_config_t& config) {
    pin = config.pin;
    type = config.type;
    pinMode(pin, OUTPUT);
}

void ToggleActuator::calibrate() {
    //TODO
}

void ToggleActuator::write(const uint8_t _position) {
    position = _position;
    digitalWrite(pin, _position^type);
    delay(TOGGLE_ACT_DELAY_MS);
}

uint8_t ToggleActuator::get() {
    return position;
}
