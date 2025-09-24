#include "drivers/i2c_mux.h"

void I2CMux::init() {
    m_i2c->begin();
    delay(10);
    reset();
}

void I2CMux::reset() {
    digitalWrite(m_pin_reset, LOW);
    delay(50);
    digitalWrite(m_pin_reset, HIGH);
}

bool I2CMux::select(uint8_t port) {
    m_i2c->beginTransmission(m_address);
    m_i2c->write(port);
    return (m_i2c->endTransmission() == 0);
}