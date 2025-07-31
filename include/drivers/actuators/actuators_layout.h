#ifndef ACTUATORS_LAYOUT_H
#define ACTUATORS_LAYOUT_H

#include "servo.h"
#include "toggle.h"

#ifdef HARDWARE_LAYOUT_FIREHORN
#include "actuators_layout_fh.h"
#endif

enum servos_alias_t {
#define DEF(alias, pin) alias,
    SERVOS_LIST
#undef DEF
    SERVOS_AMNT
};

enum toggles_alias_t {
#define DEF(alias, pin, type) alias,
    TOGGLES_LIST
#undef DEF
    TOGGLES_AMNT
};

// Generate toggle_config_t array from TOGGLE_LIST
constexpr servo_config_t SERVOS_CONFIG[] = {
#define DEF(alias, pin) {pin},
    SERVOS_LIST
#undef DEF
};


// Generate toggle_config_t array from TOGGLE_LIST
constexpr toggle_config_t TOGGLES_CONFIG[] = {
#define DEF(alias, pin, type) {pin, type},
    TOGGLES_LIST
#undef DEF
};

#endif // ACTUATORS_LAYOUT_H