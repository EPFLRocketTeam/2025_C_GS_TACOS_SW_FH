#ifndef TACOSCOMPUTER_H
#define TACOSCOMPUTER_H

#include <Arduino.h>

#include "drivers/i2c_mux.h"
#include "drivers/pte7300.h"
#include "drivers/toggle_actuator.h"
#include "telecom.h"

class TACOSComputer {
private:
    bool m_lox_disconnecetd = false;
    bool m_ambient_disconnecetd = false;
    bool m_isolation_disconnecetd = false;

    pte7300_sample_t m_gp1{};
    pte7300_sample_t m_gp2{};
    pte7300_sample_t m_gp3{};
    pte7300_sample_t m_gp4{};
    // pte7300_sample_t m_sensata_5{};
    // pte7300_sample_t m_sensata_6{};
    time_t m_last_sensors_polling;

    Telecom m_telecom;

    void check_pte7300_sample(pte7300_reading_t reading, pte7300_sample_t& reg);
    void process_telecom_command(const gse_uplink_t& packet);
    void check_status();
    void process_scheduled_tasks(time_t current);
    
    gse_downlink_t build_downlink();
public:
    explicit TACOSComputer() {}
    void init();
    void update(time_t current);
    void soft_reset();

};

#endif // TACOSCOMPUTER_H