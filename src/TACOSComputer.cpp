#include "TACOSComputer.h"
#include <Servo.h>

I2CMux MUX_1{{&Wire1, MUX_ADDR, MUX_nRST}};
I2CMux MUX_2{{&Wire2, MUX_ADDR, MUX_nRST}};

PTE7300 GP1{{&MUX_1, 0}};
PTE7300 GP2{{&MUX_1, 1}};
PTE7300 GP3{{&MUX_1, 2}};
PTE7300 GP4{{&MUX_1, 3}};
//PTE7300 SENSATA_5{{&MUX_2, 0}};
//PTE7300 SENSATA_6{{&MUX_2, 1}};

ToggleActuator GQN1{{toggle_type::TOGGLE_TYPE_NC, 2}};
ToggleActuator GQN2{{toggle_type::TOGGLE_TYPE_NC, 3}};
ToggleActuator GQN3{{toggle_type::TOGGLE_TYPE_NC, 4}};
ToggleActuator GQN4{{toggle_type::TOGGLE_TYPE_NC, 5}};
ToggleActuator GQN5{{toggle_type::TOGGLE_TYPE_NC, 6}};
ToggleActuator GQN6{{toggle_type::TOGGLE_TYPE_NC, 7}};

ToggleActuator GPN{{toggle_type::TOGGLE_TYPE_NC, 8}};
ToggleActuator GPA{{toggle_type::TOGGLE_TYPE_NC, 9}};
ToggleActuator GVN{{toggle_type::TOGGLE_TYPE_NC, 10}};
ToggleActuator GFE{{toggle_type::TOGGLE_TYPE_NC, 11}};
ToggleActuator GFO{{toggle_type::TOGGLE_TYPE_NC, 12}};
ToggleActuator GDO{{toggle_type::TOGGLE_TYPE_NC, 16}};
ToggleActuator PC{{toggle_type::TOGGLE_TYPE_NC, 17}};
ToggleActuator PUMP{{toggle_type::TOGGLE_TYPE_NC, 20}};
// ToggleActuator PR{{toggle_type::TOGGLE_TYPE_NC, 0}};
//ToggleActuator TOGGLE_28{{toggle_type::TOGGLE_TYPE_NC, 0}};

#define SERVO_1_PIN 21
Servo SERVO_1{};

#define SERVO_2_PIN 25
Servo SERVO_2{};

#define SERVO_OPEN 180
#define SERVO_CLOSE 0
#define SERVO_DELAY 2000

struct ScheduledTask {
    Servo* servo;
    time_t close_time;
    bool active;
    int close_position;
};

// Array to hold scheduled tasks
#define MAX_SCHEDULED_TASKS 5
ScheduledTask scheduled_tasks[MAX_SCHEDULED_TASKS];



void TACOSComputer::init() {
    
    m_telecom.init();

    SERVO_1.attach(SERVO_1_PIN);
    SERVO_2.attach(SERVO_2_PIN);

    GP1.init();
    GP2.init();
    GP3.init();
    GP4.init();

    GQN1.init();
    GQN2.init();
    GQN3.init();
    GQN4.init();
    GQN5.init();
    GQN6.init();

    GPN.init();
    GPA.init();
    GVN.init();
    GFE.init();
    GFO.init();
    GDO.init();
    PC.init();
    PUMP.init();

    // Initialize scheduled tasks array
    for (int i = 0; i < MAX_SCHEDULED_TASKS; i++) {
        scheduled_tasks[i].active = false;
    }

    soft_reset();


}

void TACOSComputer::check_pte7300_sample(pte7300_reading_t reading, pte7300_sample_t& reg) {
    reg.pressure = reading.pressure_valid ? reading.sample.pressure : reg.pressure;
    reg.temperature = reading.temperature_valid ? reading.sample.temperature : reg.temperature;
}



void async_schedule_close(Servo& servo, time_t current, int delay_ms = SERVO_DELAY, int close_pos = SERVO_CLOSE) {
    // Find an empty slot in the task array
    for (int i = 0; i < MAX_SCHEDULED_TASKS; i++) {
        if (!scheduled_tasks[i].active) {
            scheduled_tasks[i].servo = &servo;
            scheduled_tasks[i].close_time = current + delay_ms;
            scheduled_tasks[i].active = true;
            scheduled_tasks[i].close_position = close_pos;
            return;
        }
    }
}

void TACOSComputer::process_scheduled_tasks(time_t current) {
    for (int i = 0; i < MAX_SCHEDULED_TASKS; i++) {
        if (scheduled_tasks[i].active && current >= scheduled_tasks[i].close_time) {
            // Close the servo
            scheduled_tasks[i].servo->write(scheduled_tasks[i].close_position);
            // Mark task as inactive
            scheduled_tasks[i].active = false;
        }
    }
}


void TACOSComputer::process_telecom_command(const gse_uplink_t& packet) {
    switch (packet.order_id)
    {
    case CMD_ID::GSE_CMD_TOGGLE_GQN1: {
        GQN1.toggle();
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GQN2: {
        GQN2.toggle();
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GQN3: {
        GQN3.toggle();
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GQN4: {
        GQN4.toggle();
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GQN5: {
        GQN5.toggle();
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GQN6: {
        GQN6.toggle();
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GPN: {
        GPN.toggle();
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GPA: {
        GPA.toggle();
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GVN: {
        GVN.toggle();
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GFE: {
        GFE.toggle();
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GFO: {
        GFO.toggle();
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GDO: {
        GDO.toggle();
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_PC: {
        PC.toggle();
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_PUMP: {
        PUMP.toggle();
        break;
    }    
    case CMD_ID::GSE_CMD_SERVO_1: {
        SERVO_1.write(SERVO_OPEN);
        async_schedule_close(SERVO_1, millis());
        break;
    }    
    case CMD_ID::GSE_CMD_SERVO_2: {
        SERVO_2.write(SERVO_OPEN);
    async_schedule_close(SERVO_2, millis());
        break;
    }    

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
    packet.GPN_NC2 = GPA.get_current_position();
    return packet;
}

void TACOSComputer::update(time_t current) {

    // Fetch commands
    m_telecom.update();
    process_scheduled_tasks(current);
    // Process sensors and actuate
    process_telecom_command(m_telecom.get_last_packet_received(true));
    // Update status
    check_status();

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

    m_telecom.send_packet(build_downlink());

}

void TACOSComputer::soft_reset() {

    //m_telecom.reset();

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

    GPN.soft_reset();
    GPA.soft_reset();
    GVN.soft_reset();
    GFO.soft_reset();
    GDO.soft_reset();
    PC.soft_reset();
    
}