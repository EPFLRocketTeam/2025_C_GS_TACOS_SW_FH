#ifndef I2C_MUX_H
#define I2C_MUX_H

#include <Arduino.h>
#include <Wire.h>

#define MUX_ADDR 0
#define MUX_nRST 0

typedef struct i2c_mux_config_t {
    TwoWire* i2c;
    uint8_t address;
    uint8_t pin_reset;
} i2c_mux_config_t;

class I2CMux {
private:
    TwoWire* m_i2c;
    uint8_t m_address;
    uint8_t m_pin_reset;
public:
    explicit I2CMux(const i2c_mux_config_t& config): m_i2c{config.i2c}, m_address{config.address}, m_pin_reset{config.pin_reset} {}
    void init();
    void reset();
    bool select(uint8_t port);

    inline TwoWire* get_i2c() {return m_i2c;}
};

#endif // I2C_MUX_H