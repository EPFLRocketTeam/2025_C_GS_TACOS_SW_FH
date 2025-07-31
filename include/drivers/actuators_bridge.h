#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <inttypes.h>
#include "actuators/servo.h"
#include "actuators/toggle.h"
#include "actuators/actuators_layout.h"

typedef struct actuators_t {
    int32_t servos[SERVOS_AMNT];
    uint8_t toggles[TOGGLES_AMNT];
} actuators_t;

class ActuatorsBridge {
private:
    ServoActuator servos[SERVOS_AMNT];
    ToggleActuator toggles[TOGGLES_AMNT];
    
public:
    void init();
    actuators_t get_all();

    // Calibration
    void calibrate_all();

    // Manual commands
    void write_servo(const uint8_t idx, const int32_t value);
    void write_toggle(const uint8_t idx, const uint8_t value);

    // Pre-designed sequences
    void config_safe();
    void config_vent();

};

static ActuatorsBridge actuatorsInstance{};

#endif