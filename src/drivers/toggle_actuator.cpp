#include "drivers/toggle_actuator.h"

void ToggleActuator::init() {
    pinMode(m_pin, OUTPUT);
    soft_reset();
}

void ToggleActuator::set_position(const toggle_position position) {
    digitalWrite(m_pin, position ^ m_type);
}

void ToggleActuator::soft_reset() {
    digitalWrite(m_pin, 0); // Set in default position
}