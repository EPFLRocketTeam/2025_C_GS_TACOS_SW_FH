#ifndef FSM_H
#define FSM_H

#include <Arduino.h>

typedef enum state_t {
    STATE_IDLE = 0x00,
    STATE_ARMED,
    STATE_CALIBRATION,
    STATE_PASSIVATE
} state_t;

typedef struct fsm_t {
    state_t current;
    time_t lastTransition;
} fsm_t;

class FSM {
private:
    state_t current = STATE_IDLE;
    time_t lastTransition = 0;
public:
    void init();
    bool set_state(const state_t state);
    fsm_t get();
};

#endif