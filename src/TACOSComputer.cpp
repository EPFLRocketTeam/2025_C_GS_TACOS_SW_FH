#include "TACOSComputer.h"

I2CMux MUX_1{{&Wire1, MUX_ADDR, MUX_nRST}};
I2CMux MUX_2{{&Wire2, MUX_ADDR, MUX_nRST}};

PTE7300 SENSATA_1{{&MUX_1, 0}};
PTE7300 SENSATA_2{{&MUX_1, 1}};
PTE7300 SENSATA_3{{&MUX_1, 2}};
PTE7300 SENSATA_4{{&MUX_1, 3}};
PTE7300 SENSATA_5{{&MUX_2, 0}};
PTE7300 SENSATA_6{{&MUX_2, 1}};

ToggleActuator TOGGLE_11{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator TOGGLE_12{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator TOGGLE_13{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator TOGGLE_14{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator TOGGLE_15{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator TOGGLE_16{{toggle_type::TOGGLE_TYPE_NC, 0}};

ToggleActuator TOGGLE_21{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator TOGGLE_22{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator TOGGLE_23{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator TOGGLE_24{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator TOGGLE_25{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator TOGGLE_26{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator TOGGLE_27{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator TOGGLE_28{{toggle_type::TOGGLE_TYPE_NC, 0}};

void TACOSComputer::init() {
    m_telecom.init();
    soft_reset();

}

void TACOSComputer::check_pte7300_sample(pte7300_reading_t reading, pte7300_sample_t& reg) {
    reg.pressure = reading.pressure_valid ? reading.sample.pressure : reg.pressure;
    reg.temperature = reading.temperature_valid ? reading.sample.temperature : reg.temperature;
}

void TACOSComputer::process_telecom_command(const gse_uplink_t& packet) {
    switch (packet.order_id)
    {
    default:
        break;
    }
}

void TACOSComputer::update(time_t current) {
    /* Exectuion order
     * 1. fetch commands
     * 2. read sensors (with polling rate)
     * 3. move actuators and autosequences
     * 4. update status
     * 5. send downlink
     */

    process_telecom_command(m_telecom.get_last_packet_received(true));

    #ifdef SENSORS_POLLING_RATE_MS
    if(current - m_last_sensors_polling > SENSORS_POLLING_RATE_MS) {
    #endif
    check_pte7300_sample(SENSATA_1.sample(), m_sensata_1);
    check_pte7300_sample(SENSATA_2.sample(), m_sensata_2);
    check_pte7300_sample(SENSATA_3.sample(), m_sensata_3);
    check_pte7300_sample(SENSATA_4.sample(), m_sensata_4);
    check_pte7300_sample(SENSATA_5.sample(), m_sensata_5);
    check_pte7300_sample(SENSATA_6.sample(), m_sensata_6);
    m_last_sensors_polling = current;
    #ifdef SENSORS_POLLING_RATE_MS
    }
    #endif
}

void TACOSComputer::soft_reset() {

    m_telecom.reset();

    /* RESET MUX */
    MUX_1.reset();
    MUX_2.reset();

    /* RESET READINGS */
    m_sensata_1 = {};
    m_sensata_2 = {};
    m_sensata_3 = {};
    m_sensata_4 = {};
    m_sensata_5 = {};
    m_sensata_6 = {};
    m_last_sensors_polling = 0; //Wil force a new polling if SENSORS_POLLING_RATE_MS is defined
    
    /* RESET ACTATOR POSITIONS */
    TOGGLE_11.soft_reset();
    TOGGLE_12.soft_reset();
    TOGGLE_13.soft_reset();
    TOGGLE_14.soft_reset();
    TOGGLE_15.soft_reset();
    TOGGLE_16.soft_reset();

    TOGGLE_21.soft_reset();
    TOGGLE_22.soft_reset();
    TOGGLE_23.soft_reset();
    TOGGLE_24.soft_reset();
    TOGGLE_25.soft_reset();
    TOGGLE_26.soft_reset();
    TOGGLE_27.soft_reset();
    TOGGLE_28.soft_reset();
}