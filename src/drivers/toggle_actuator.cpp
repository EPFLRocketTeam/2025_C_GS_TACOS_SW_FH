#include "drivers/toggle_actuator.h"

void ToggleActuator::init() {
    pinMode(m_pin, OUTPUT);
    soft_reset();
}

void ToggleActuator::set_position(const toggle_position position) {
    digitalWrite(m_pin, position ^ m_type);
    m_current_position = position;
}

toggle_position ToggleActuator::get_current_position() {
    return (toggle_position) m_current_position;
}

void ToggleActuator::soft_reset() {
    digitalWrite(m_pin, 0); // Set in default position
    m_current_position = (toggle_position) m_type;
}