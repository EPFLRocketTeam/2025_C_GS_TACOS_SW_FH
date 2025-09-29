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

ToggleActuator GQN1{{toggle_type::TOGGLE_TYPE_NC, 1}}; //SOLENOID 11
ToggleActuator GQN2{{toggle_type::TOGGLE_TYPE_NC, 0}}; //SOLENOID 12
ToggleActuator GQN3{{toggle_type::TOGGLE_TYPE_NC, 3}}; //SOLENOID 13
ToggleActuator GQN4{{toggle_type::TOGGLE_TYPE_NC, 2}}; //SOLENOID 14
ToggleActuator GQN5{{toggle_type::TOGGLE_TYPE_NC, 5}}; //SOLENOID 15
ToggleActuator GQN6{{toggle_type::TOGGLE_TYPE_NC, 4}}; //SOLENOID 16

ToggleActuator GPN{{toggle_type::TOGGLE_TYPE_NC, 7}}; //SOLENOID 21
ToggleActuator GPA{{toggle_type::TOGGLE_TYPE_NC, 6}}; //SOLENOID 22
ToggleActuator GVN{{toggle_type::TOGGLE_TYPE_NC, 9}}; //SOLENOID 23
ToggleActuator GFE{{toggle_type::TOGGLE_TYPE_NC, 8}}; //SOLENOID 24
ToggleActuator GFO{{toggle_type::TOGGLE_TYPE_NC, 24}}; //SOLENOID 25
ToggleActuator GDO{{toggle_type::TOGGLE_TYPE_NC, 10}}; //SOLENOID 26
ToggleActuator PC{{toggle_type::TOGGLE_TYPE_NC, 20}}; //SOLENOID 27
ToggleActuator PUMP{{toggle_type::TOGGLE_TYPE_NC, 25}}; //SOlENOID 28
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
    
    // Clear packet
    memset(&packet, 0, sizeof(gse_downlink_t));
    
    // Add pressure sensor readings - use the actual fields from the structure
    packet.GP1 = m_gp1.pressure;  // Nitrogen pressure in the filling line
    packet.GP2 = m_gp2.pressure;  // LOX pressure in the deware
    packet.GP3 = m_gp3.pressure;  // Pressure in the low-pressure side of the gas booster
    packet.GP4 = m_gp4.pressure;  // Pressure before the pneumatic valve
    // packet.GP5 = m_gp5.pressure;  // Uncomment if you have this sensor
    
    // Add toggle states using the correct field names
    // GQN_NC1 (Nitrogen and Ethanol disconnect actuation)
    packet.GQN_NC1 = 0;
    
    // GQN_NC2 (LOX disconnect actuation) - combine multiple values into bits
    packet.GQN_NC2 = 0;
    packet.GQN_NC2 |= (GQN1.get_current_position() << 0); // Bit 0
    packet.GQN_NC2 |= (GQN2.get_current_position() << 1); // Bit 1
    packet.GQN_NC2 |= (GQN3.get_current_position() << 2); // Bit 2
    packet.GQN_NC2 |= (GQN4.get_current_position() << 3); // Bit 3
    packet.GQN_NC2 |= (GQN5.get_current_position() << 4); // Bit 4
    packet.GQN_NC2 |= (GQN6.get_current_position() << 5); // Bit 5
    
    // Reserved fields
    packet.GQN_NC3 = 0;  // Reserved
    packet.GQN_NC4 = 0;  // Reserved
    
    // GQN_NC5 (Low mass flow anti-freeze lox disconnect)
    packet.GQN_NC5 = 0;
    
    // GPN_NC1 (Controls the activation of the pressure booster)
    packet.GPN_NC1 = GPA.get_current_position();
    
    // Other toggle states
    packet.GPN_NC2 = GPN.get_current_position();  // Control the opening of the high pressure bottle
    packet.GVN_NC = GVN.get_current_position();   // Vents the tube before disconnect
    packet.GFE_NC = GFE.get_current_position();   // Controls the filling of ethanol along with the pump
    packet.GFO_NCC = GFO.get_current_position();  // Controls LOX filling
    packet.GDO_NCC = GDO.get_current_position();  // Vent the tube before disconnect
    packet.PC_OLC = PC.get_current_position();    // Trigger Lox disconnect and purge the tube of LOX
    
    // For PUMP status, we could use a reserved field if available, or add to a status byte
    // For now, we'll just leave it out as there's no direct field for it
    
    // We can include system status info in unused bits of other fields if needed
    // For example, if GQN_NC1 isn't fully used:
    if (m_lox_disconnecetd) {
        packet.GQN_NC1 |= 0x80;  // Set highest bit
    }
    if (m_ambient_disconnecetd) {
        packet.GQN_NC1 |= 0x40;  // Set second-highest bit
    }
    
    return packet;
}

void TACOSComputer::update(time_t current) {
    // First, build the telemetry packet with current data
    gse_downlink_t telemetry_packet = build_downlink();
    
    // Provide the packet to the Telecom subsystem
    m_telecom.set_telemetry_packet(telemetry_packet);
    
    // Update telecom subsystem (this will send the packet at 5Hz)
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