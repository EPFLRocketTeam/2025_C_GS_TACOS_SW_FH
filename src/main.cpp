// Last update: 05/09/2025
#include <Arduino.h>
#include "TACOSComputer.h"

TACOSComputer computer;

void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("Started");
    computer.init();
}

void loop() {

    computer.update(millis());

}