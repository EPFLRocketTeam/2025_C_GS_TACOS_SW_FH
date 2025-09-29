#include "drivers/toggle_actuator.h"

void ToggleActuator::init() {
    pinMode(m_pin, OUTPUT);
    soft_reset();
}

void ToggleActuator::set_position(const toggle_position position) {
    digitalWrite(m_pin, position ^ m_type);
    m_current_position = static_cast<uint8_t>(digitalRead(m_pin) ^ m_type);
}

toggle_position ToggleActuator::get_current_position() {
    uint8_t pin_state = digitalRead(m_pin);
    m_current_position = static_cast<uint8_t>(pin_state ^ m_type);
    return static_cast<toggle_position>(m_current_position);
}

void ToggleActuator::soft_reset() {
    toggle_position default_position = (m_type == TOGGLE_TYPE_NO) ? TOGGLE_POSITION_OPEN : TOGGLE_POSITION_CLOSE;
    set_position(default_position);
}

toggle_position ToggleActuator::toggle() {
    toggle_position current = get_current_position();
    toggle_position next = (current == TOGGLE_POSITION_OPEN) ? TOGGLE_POSITION_CLOSE : TOGGLE_POSITION_OPEN;
    set_position(next);
    return next;
}