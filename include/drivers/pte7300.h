#ifndef DRIVERS_PTE7300_H
#define DRIVERS_PTE7300_H

#include <Arduino.h>
#include <PTE7300_I2C.h>
#include "i2c_mux.h"

typedef struct pte7300_config_t {
    I2CMux* mux;
    uint8_t port;
} pte7300_config_t;

typedef struct pte7300_sample_t {
    float pressure;
    float temperature;
} pte7300_sample_t;

typedef struct pte7300_reading_t {
    pte7300_sample_t sample;
    bool pressure_valid;
    bool temperature_valid;
} pte7300_reading_t;

class PTE7300 {
private:
    I2CMux* m_mux;
    uint8_t m_port;
    PTE7300_I2C m_instance;
public:
    explicit PTE7300(const pte7300_config_t& config): m_mux{config.mux}, m_port{config.port}, m_instance{config.mux->get_i2c()} {}
    void init();
    pte7300_reading_t sample();
};

#endif // DRIVERS_PTE7300_H