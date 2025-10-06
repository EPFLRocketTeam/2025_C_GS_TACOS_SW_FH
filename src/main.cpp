// Last update: 05/09/2025
#include <Arduino.h>
#include "TACOSComputer.h"
#include <SPI.h>
#include <LoRa.h>


TACOSComputer computer;

void setup() {
    delay(2000);
    Serial.begin(115200);
    Serial.println("Starting");
    computer.init();
}

void loop() {
 	computer.update(millis());

}