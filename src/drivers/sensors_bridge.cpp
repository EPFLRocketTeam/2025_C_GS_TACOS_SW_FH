#include "drivers/sensors_bridge.h"
#include "logger.h"

void SensorsBridge::init() {
    for(uint8_t i = 0; i < PTES_AMOUNT; i++) ptes[i].init(PTES_CONFIG[i]);
    log("Sensors Initialized\n");
}

void SensorsBridge::tick_all(){
    for(uint8_t i = 0; i < PTES_AMOUNT; i++) ptes[i].tick();
}

sensors_t SensorsBridge::get_all() {
    sensors_t buffer;
    for(uint8_t i = 0; i < PTES_AMOUNT; i++) buffer.ptes[i] = ptes[i].get();
    return buffer;
}

void SensorsBridge::calibrate_all() {
    for(uint8_t i = 0; i < PTES_AMOUNT; i++) ptes[i].calibrate();
}
