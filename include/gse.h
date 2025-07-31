#ifndef GSE_H
#define GSE_H

#include <Arduino.h>
#include "fsm.h"
#include "telecom.h"
#include "drivers/actuators_bridge.h"
#include "drivers/sensors_bridge.h"

class GSE {
private:
    time_t currentTime;
    FSM fsm;
    TelecomBridge* telecom;
    ActuatorsBridge* actuators;
    SensorsBridge* sensors;

    void state_update();
    void compute_output();
    void telemetry_downlink();

    state_t idle_state();
    void on_idle();
    state_t armed_state();
    void on_armed();
    state_t calibration_state();
    void on_calibration();
    state_t passivate_state();
    void on_passivate();
public:
    GSE() : telecom(&telecomInstance), actuators(&actuatorsInstance), sensors(&sensorsInstance) {}
    void init();
    void tick();
};

static GSE gseInstance{};

#endif