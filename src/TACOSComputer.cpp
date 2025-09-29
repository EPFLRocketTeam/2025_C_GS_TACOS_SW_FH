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

ToggleActuator GQN1{{toggle_type::TOGGLE_TYPE_NC, 0}}; //out 11
ToggleActuator GQN2{{toggle_type::TOGGLE_TYPE_NC, 1}}; //out 12
ToggleActuator GQN3{{toggle_type::TOGGLE_TYPE_NC, 2}}; //out 13
ToggleActuator GQN4{{toggle_type::TOGGLE_TYPE_NC, 3}}; //out 14
ToggleActuator GQN5{{toggle_type::TOGGLE_TYPE_NC, 4}}; //out 15
ToggleActuator GQN6{{toggle_type::TOGGLE_TYPE_NC, 5}}; //out 16

ToggleActuator GPN{{toggle_type::TOGGLE_TYPE_NC, 6}}; //out 21
ToggleActuator GPA{{toggle_type::TOGGLE_TYPE_NC, 7}}; //out 22
ToggleActuator GVN{{toggle_type::TOGGLE_TYPE_NC, 8}}; //out 23
ToggleActuator GFE{{toggle_type::TOGGLE_TYPE_NC, 9}}; //out 24
ToggleActuator GFO{{toggle_type::TOGGLE_TYPE_NC, 24}}; //out 25
ToggleActuator GDO{{toggle_type::TOGGLE_TYPE_NC, 10}}; //out 26
ToggleActuator PC{{toggle_type::TOGGLE_TYPE_NC, 20}}; //out 27
ToggleActuator PUMP{{toggle_type::TOGGLE_TYPE_NC, 25}}; //out 28
// ToggleActuator PR{{toggle_type::TOGGLE_TYPE_NC, 0}};
//ToggleActuator TOGGLE_28{{toggle_type::TOGGLE_TYPE_NC, 0}};

#define SERVO_1_PIN 29
Servo SERVO_1{};

#define SERVO_2_PIN 33
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

namespace {

bool apply_solenoid_command(ToggleActuator& actuator, uint8_t order_value, const char* label) {
    if (order_value == ACTIVE) {
        actuator.set_position(TOGGLE_POSITION_OPEN);
    } else if (order_value == INACTIVE) {
        actuator.set_position(TOGGLE_POSITION_CLOSE);
    } else {
        actuator.toggle();
        Serial.print("[CMD][WARN] Non-explicit order value 0x");
        Serial.print(order_value, HEX);
        Serial.print(" for ");
        Serial.print(label);
        Serial.println(" -> toggling device");
    }

    toggle_position actual = actuator.get_current_position();
    Serial.print("[CMD] ");
    Serial.print(label);
    Serial.print(" -> ");
    Serial.println(actual == TOGGLE_POSITION_OPEN ? "OPEN" : "CLOSE");
    return true;
}

}



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
        apply_solenoid_command(GQN1, packet.order_value, "GQN1");
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GQN2: {
        apply_solenoid_command(GQN2, packet.order_value, "GQN2");
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GQN3: {
        apply_solenoid_command(GQN3, packet.order_value, "GQN3");
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GQN4: {
        apply_solenoid_command(GQN4, packet.order_value, "GQN4");
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GQN5: {
        apply_solenoid_command(GQN5, packet.order_value, "GQN5");
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GQN6: {
        apply_solenoid_command(GQN6, packet.order_value, "GQN6");
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GPN: {
        apply_solenoid_command(GPN, packet.order_value, "GPN");
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GPA: {
        apply_solenoid_command(GPA, packet.order_value, "GPA");
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GVN: {
        apply_solenoid_command(GVN, packet.order_value, "GVN");
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GFE: {
        apply_solenoid_command(GFE, packet.order_value, "GFE");
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GFO: {
        apply_solenoid_command(GFO, packet.order_value, "GFO");
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_GDO: {
        apply_solenoid_command(GDO, packet.order_value, "GDO");
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_PC: {
        apply_solenoid_command(PC, packet.order_value, "PC");
        break;
    }    
    case CMD_ID::GSE_CMD_TOGGLE_PUMP: {
        apply_solenoid_command(PUMP, packet.order_value, "PUMP");
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
    
    // Clear packet
    memset(&packet, 0, sizeof(gse_downlink_t));
    
    // Add pressure sensor readings - use the actual fields from the structure
    packet.GP1 = m_gp1.pressure;  // Nitrogen pressure in the filling line
    packet.GP2 = m_gp2.pressure;  // LOX pressure in the deware
    packet.GP3 = m_gp3.pressure;  // Pressure in the low-pressure side of the gas booster
    packet.GP4 = m_gp4.pressure;  // Pressure before the pneumatic valve
    packet.GP5 = 0.0f;            // Pressure in the ethanol filling line (not available, set to 0)
    
    // Add toggle states - each field is a separate uint8_t per protocol
    // Map physical valves to protocol fields
    packet.GQN_NC1 = GQN1.get_current_position(); // Nitrogen and Ethanol disconnect actuation
    packet.GQN_NC2 = GQN2.get_current_position(); // LOX disconnect actuation
    packet.GQN_NC3 = GQN3.get_current_position(); // Reserved (using GQN3)
    packet.GQN_NC4 = GQN4.get_current_position(); // Reserved (using GQN4)
    
    // GQN_NC5 (Low mass flow anti-freeze lox disconnect)
    packet.GQN_NC5 = GQN5.get_current_position();
    
    // GPN_NC1 (Controls the activation of the pressure booster)
    packet.GPN_NC1 = GPA.get_current_position();
    
    // Other toggle states
    packet.GPN_NC2 = GPN.get_current_position();  // Control the opening of the high pressure bottle
    packet.GVN_NC = GVN.get_current_position();   // Vents the tube before disconnect
    packet.GFE_NC = GFE.get_current_position();   // Controls the filling of ethanol along with the pump
    packet.GFO_NCC = GFO.get_current_position();  // Controls LOX filling
    packet.GDO_NCC = GDO.get_current_position();  // Vent the tube before disconnect
    packet.PC_OLC = PC.get_current_position();    // Trigger Lox disconnect and purge the tube of LOX
    
    // Note: PUMP status and GQN6 are not directly mapped in this protocol version
    // System status (m_lox_disconnecetd, m_ambient_disconnecetd) could be added to
    // a future protocol version if needed
    
    return packet;
}

void TACOSComputer::update(time_t current) {
    // First, build the telemetry packet with current data
    gse_downlink_t telemetry_packet = build_downlink();
    
    // Provide the packet to the Telecom subsystem
    m_telecom.set_telemetry_packet(telemetry_packet);
    
    // Update telecom subsystem (this will send the packet at 1Hz)
    m_telecom.update();
    
    // Process scheduled tasks (servo movements, etc.)
    process_scheduled_tasks(current);
    
    // Check for new sensor readings and update internal state
    #ifdef SENSORS_POLLING_RATE_MS
    if (current - m_last_sensors_polling > SENSORS_POLLING_RATE_MS) {
        pte7300_reading_t reading;

        //ajouter la fonction de lecture des capteurs
        // Check system status
        check_status();
        
        m_last_sensors_polling = current;
    }
    #endif
    
    // Process any incoming commands
    gse_uplink_t packet = m_telecom.get_last_packet_received(true);
    if (packet.order_id != 0) {
        process_telecom_command(packet);
    }
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