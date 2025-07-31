#ifndef PTE_SENSOR_H
#define PTE_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <cppQueue.h>

#define PTE_I2C_ID 0x00
#define PTE_SAMPLES_CALIB_BUFFER 100

typedef struct pte_config_t {
    TwoWire* i2c_channel;
    uint8_t i2c_mux_id;
    time_t min_time;
} pte_config_t;

typedef float pte_sample_t;
#define PTE_SAMPLE_SIZE sizeof(pte_sample_t)

class PTESensor {
private:
    TwoWire* i2c_channel;
    uint8_t i2c_mux_id;
    time_t min_time;
    
    float bias = 0;
    float var = 0;
    
    time_t last_sample = 0;
    pte_sample_t sample = 0;
    cppQueue sample_calib_buffer;

public:
    PTESensor() : sample_calib_buffer(PTE_SAMPLE_SIZE, PTE_SAMPLES_CALIB_BUFFER, FIFO, true) {}
    void init(const pte_config_t& config);
    void tick();
    pte_sample_t get();
    void calibrate();
};

#endif // SENSORS_PTE_SENSOR_H