#include "drivers/pte7300.h"

void PTE7300::init(){ 

}

pte7300_reading_t PTE7300::sample() {
    pte7300_reading_t reading{0};

    if(!m_mux->select(m_port)){
        Serial.printf("COULD NOT MUX %d\n", m_port + 1);
        return reading;
    }

    if(!m_instance.isConnected()){
        Serial.printf("COULD NOT CONNECT %d\n", m_port + 1);
        return reading;
    }

    int16_t raw_p = m_instance.readDSP_S();
    int16_t raw_t = m_instance.readDSP_T();

    reading.pressure_valid = (raw_t != NAN);
    reading.temperature_valid = (raw_p != NAN);
    reading.sample.temperature = raw_t * 82.5 / 16000 + 42.5; // in °C
    reading.sample.pressure = (raw_p - (-16000.0)) * (100.0) / (16000.0 - (-16000.0)); // in bar

    return reading;
}
