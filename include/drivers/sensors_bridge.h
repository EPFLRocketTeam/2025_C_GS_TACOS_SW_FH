#ifndef SENSORS_BRIDGE_H
#define SENSORS_BRIDGE_H

#include <inttypes.h>
#include "sensors/pte.h"
#include "sensors/sensors_layout.h"

typedef struct sensors_t {
    pte_sample_t ptes[PTES_AMOUNT];
} sensors_t;

class SensorsBridge {
private:
    PTESensor ptes[PTES_AMOUNT];
public:
    void init();
    void tick_all();
    sensors_t get_all();
    void calibrate_all();
};

static SensorsBridge sensorsInstance{};

#endif // SENSORS_BRIDGE_H