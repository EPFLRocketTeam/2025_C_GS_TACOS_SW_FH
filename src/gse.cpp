#include "gse.h"
#include "logger.h"

void GSE::init() {
    log("Welcome. Initializing GSE...\n");
    currentTime = millis();
    fsm.init();
    telecom->init();
    actuators->init();
    sensors->init();
    log("Initalization complete...\nStarting GSE main loop.\n");
}

void GSE::tick() {
    currentTime = millis();

    // Input collections
    telecom->tick();        // Process & Expose a new command 
    sensors->tick_all();
    
    //(in, state) -> out
    compute_output();       // Execute output sequences
    
    //(in, state) -> next
    state_update();         // Compute the next state
}

void GSE::compute_output() {
    
    telemetry_downlink();

    // OPERATIONS ONLY ALLOWED IN ARMED STATE
    if(fsm.get().current == STATE_PASSIVATE) {
        actuators->config_vent();
        return;
    }

    if(fsm.get().current == STATE_CALIBRATION) {
        sensors->calibrate_all();
        actuators->calibrate_all();
        // DO NOT RETURN (use "not armed" which follows to safe and return)
    }

    if(fsm.get().current != STATE_ARMED){
        actuators->config_safe();
        return;
    }

    // FOR ACTUATORS MOVEMENT, USE CMD DEF TO OUR ADVANTAGE
    // Case: servo
    if(telecom->get_uplink().order_id >= CMD_ID::GSE_CMD_SERVO_1 && telecom->get_uplink().order_id < CMD_ID::GSE_CMD_SERVO_2){
        actuators->write_servo(telecom->get_uplink().order_id -  GSE_CMD_SERVO_1, telecom->get_uplink().order_value);
    }

    // FOR ACTUATORS MOVEMENT, USE CMD DEF TO OUR ADVANTAGE
    // Case: toggle
    if(telecom->get_uplink().order_id >= CMD_ID::GSE_CMD_TOGGLE_11 && telecom->get_uplink().order_id < CMD_ID::GSE_CMD_TOGGLE_28){
        actuators->write_toggle(telecom->get_uplink().order_id -  GSE_CMD_TOGGLE_11, telecom->get_uplink().order_value);
    }

}

void GSE::telemetry_downlink() {

    sensors_t measurements = sensors->get_all();
    actuators_t positions = actuators->get_all();

    // TODO populate downlink packet
    gse_downlink_t packet = {};
    
    uint8_t* stream = new uint8_t[gse_downlink_size];
    memcpy(stream, &packet, gse_downlink_size);

    telecom->send(GSE_TELEMETRY, stream, gse_downlink_size);

    delete[] stream;
}

void GSE::state_update(){
    state_t next = fsm.get().current;

    // Perform FSM switch
    switch (fsm.get().current) {
    case STATE_IDLE:
        next = idle_state();
        break;
    case STATE_ARMED:
        next = armed_state();
        break;
    case STATE_CALIBRATION:
        next = calibration_state();
        break;
    case STATE_PASSIVATE:
        next = passivate_state();
        break;
    default:
        break;
    }

    // Exit here if on-place transition
    if(!fsm.set_state(next)) return;

    // Use for internal states reset and logging
    switch (next) {
    case STATE_IDLE:
        on_idle();
        break;
    case STATE_ARMED:
        on_armed();
        break;
    case STATE_CALIBRATION:
        on_calibration();
        break;
    case STATE_PASSIVATE:
        on_passivate();
        break;
    default:
        break;
    }

}

state_t GSE::idle_state() {

    if(telecom->get_uplink().order_id == CMD_ID::GSE_CMD_ARM)
        return STATE_ARMED;

    if(telecom->get_uplink().order_id == CMD_ID::GSE_CMD_CALIBRATE)
        return STATE_CALIBRATION;

    return STATE_IDLE;
}

void GSE::on_idle() {
    log("Transition into IDLE state\n");
}

state_t GSE::armed_state() {
    if(telecom->get_uplink().order_id == CMD_ID::GSE_CMD_IDLE)
        return STATE_IDLE;

    if(telecom->get_uplink().order_id == CMD_ID::GSE_CMD_PASSIVATE)
        return STATE_PASSIVATE;

    return STATE_ARMED;
}

void GSE::on_armed() {
    log("Transition into ARMED state\n");

}

state_t GSE::calibration_state() {

    // Stay for a signle cycle (use calibration as trigger)
    return STATE_IDLE;
}

void GSE::on_calibration() {
    log("Transition into CALIBRATION state\n");
}

state_t GSE::passivate_state() {
    
    if(telecom->get_uplink().order_id == CMD_ID::GSE_CMD_IDLE)
        return STATE_IDLE;

    return STATE_PASSIVATE;
}

void GSE::on_passivate() {
    log("Transition into PASSIVATE state\n");
}