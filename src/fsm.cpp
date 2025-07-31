#include "fsm.h"
#include "logger.h"

void FSM::init() {
    log("Initializing FSM...\n");
    current = STATE_IDLE;
    lastTransition = millis();
    log("FSM Initialized\n");

}

bool FSM::set_state(const state_t state) {
    bool s = (current != state);
    if(s) lastTransition = millis();
    current = state;
    return s;

}

fsm_t FSM::get() {
    return {
        .current = current,
        .lastTransition = lastTransition
    };
}