#include "drivers/actuators_bridge.h"
#include "logger.h"

void ActuatorsBridge::init() {
    for(uint8_t i = 0; i < SERVOS_AMNT; i++) servos[i].init(SERVOS_CONFIG[i]);
    for(uint8_t i = 0; i < TOGGLES_AMNT; i++) toggles[i].init(TOGGLES_CONFIG[i]);
    log("Actuators Initialized\n");
}

void ActuatorsBridge::calibrate_all() {
    for(uint8_t i = 0; i < SERVOS_AMNT; i++) servos[i].calibrate();
    for(uint8_t i = 0; i < TOGGLES_AMNT; i++) toggles[i].calibrate();
}

actuators_t ActuatorsBridge::get_all() {
    actuators_t buffer;
    for (uint8_t i = 0; i < SERVOS_AMNT; i++) buffer.servos[i] = servos[i].get();
    for (uint8_t i = 0; i < TOGGLES_AMNT; i++) buffer.toggles[i] = toggles[i].get();
    return buffer;
}

void ActuatorsBridge::write_servo(const uint8_t idx, const int32_t value) {
    log("Writing to servo #%d: %d", idx, value);
    servos[idx].write(value);
}

void ActuatorsBridge::write_toggle(const uint8_t idx, const uint8_t value) {
    log("Writing to toggle #%d: %d", idx, value);
    toggles[idx].write(value);
}

void ActuatorsBridge::config_safe() {
    // Everything in a safe position
    for(uint8_t i = 0; i < SERVOS_AMNT; i++) servos[i].write(SERVO_CLOSED);
    for(uint8_t i = 0; i < TOGGLES_AMNT; i++) toggles[i].write(TOGGLE_CLOSED);
}

void ActuatorsBridge::config_vent() {
    // Everything in a venting position
    // TODO
}