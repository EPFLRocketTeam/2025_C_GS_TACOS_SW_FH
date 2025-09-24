#include "TACOSComputer.h"

I2CMux MUX_1{{&Wire1, MUX_ADDR, MUX_nRST}};
I2CMux MUX_2{{&Wire2, MUX_ADDR, MUX_nRST}};

PTE7300 GP1{{&MUX_1, 0}};
PTE7300 GP2{{&MUX_1, 1}};
PTE7300 GP3{{&MUX_1, 2}};
PTE7300 GP4{{&MUX_1, 3}};
//PTE7300 SENSATA_5{{&MUX_2, 0}};
//PTE7300 SENSATA_6{{&MUX_2, 1}};

ToggleActuator GQN1{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator GQN2{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator GQN3{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator GQN4{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator GQN5{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator GQN6{{toggle_type::TOGGLE_TYPE_NC, 0}};

ToggleActuator GPN1{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator GPN2{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator GVN{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator GFO{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator GDO{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator PC{{toggle_type::TOGGLE_TYPE_NC, 0}};
ToggleActuator PR{{toggle_type::TOGGLE_TYPE_NC, 0}};
//ToggleActuator TOGGLE_28{{toggle_type::TOGGLE_TYPE_NC, 0}};

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

    case NO_PACKET:
    default:
        break;
    }
}

void TACOSComputer::check_status() {
    m_lox_disconnecetd |= (GQN3.get_current_position() == TOGGLE_POSITION_OPEN);
    m_ambient_disconnecetd |= (GQN1.get_current_position() == TOGGLE_POSITION_OPEN);
}

gse_downlink_t TACOSComputer::build_downlink() {
    gse_downlink_t packet;
    return packet;
}

void TACOSComputer::update(time_t current) {
   
    // Fetch commands
    m_telecom.update();

    // Poll sensors
    #ifdef SENSORS_POLLING_RATE_MS
    if(current - m_last_sensors_polling > SENSORS_POLLING_RATE_MS) {
    #endif
    check_pte7300_sample(GP1.sample(), m_gp1);
    check_pte7300_sample(GP2.sample(), m_gp2);
    check_pte7300_sample(GP3.sample(), m_gp3);
    check_pte7300_sample(GP4.sample(), m_gp4);

    m_last_sensors_polling = current;
    #ifdef SENSORS_POLLING_RATE_MS
    }
    #endif

    // Process sensors and actuate
    process_telecom_command(m_telecom.get_last_packet_received(true));

    // Update status
    check_status();

    // Send downlink
    m_telecom.send_packet(build_downlink());

}

void TACOSComputer::soft_reset() {

    m_telecom.reset();

    /* RESET MUX */
    MUX_1.reset();
    MUX_2.reset();

    /* RESET READINGS */
    m_gp1 = {};
    m_gp2 = {};
    m_gp3 = {};
    m_gp4 = {};
    m_last_sensors_polling = 0; //Wil force a new polling if SENSORS_POLLING_RATE_MS is defined
    
    /* RESET ACTATOR POSITIONS */
    GQN1.soft_reset();
    GQN2.soft_reset();
    GQN3.soft_reset();
    GQN4.soft_reset();
    GQN5.soft_reset();
    GQN6.soft_reset();

    GPN1.soft_reset();
    GPN2.soft_reset();
    GVN.soft_reset();
    GFO.soft_reset();
    GDO.soft_reset();
    PC.soft_reset();
    PR.soft_reset();
}