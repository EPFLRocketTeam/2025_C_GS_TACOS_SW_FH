#ifndef DRIVERS_TOGGLE_ACTUATOR_H
#define DRIVERS_TOGGLE_ACTUATOR_H

#include <Arduino.h>

enum toggle_type {
    TOGGLE_TYPE_NC = 0,
    TOGGLE_TYPE_NO = 1,
};

enum toggle_position {
    TOGGLE_POSITION_CLOSE = 0,
    TOGGLE_POSITION_OPEN  = 1,
};

typedef struct toggle_acutator_config_t {
    toggle_type type;
    uint8_t pin;
} toggle_acutator_config_t;

typedef uint8_t toggle_acutator_state_t;

class ToggleActuator {
private:
    toggle_type m_type;
    uint8_t m_pin;
public:
    explicit ToggleActuator(const toggle_acutator_config_t& config): m_type{config.type}, m_pin{config.pin} {}
    void init();
    void set_position(const toggle_position position);
    void soft_reset();
};

#endif // DRIVERS_TOGGLE_ACTUATOR_H