#include <Arduino.h>
#include "gse.h"
#include "logger.h"

#define NRST_PIN -1

void setup() {
    delay(200);

    pinMode(NRST_PIN, OUTPUT);
    digitalWrite(NRST_PIN, 1);

    DEBUG_SERIAL.begin(DEBUG_SERIAL_BAUD);
    DEBUG_SERIAL.println("[OFFLOG] Serial connection OK");
    DEBUG_SERIAL.println("[OFFLOG] Attempting to connect to SD card");

    bool status = false;
    for (int i = 0; i < SD_INIT_MAX && !status; i++) {
        status = SD.begin(SD_CARD_NSS);
        DEBUG_SERIAL.print(".");
        delay(200);
    }
    
    if(!status){
        DEBUG_SERIAL.println("[OFFLOG] Unable to connect to SD card, BRICKING");
        while(1) delay(1000);
    }
    
    DEBUG_SERIAL.println("[OFFLOG] Logger OK, let's swing!");
    gseInstance.init();
}

void loop() {
    gseInstance.tick();
}