// Last update: 05/09/2025
#include <Arduino.h>
#include "TACOSComputer.h"

TACOSComputer computer;

void setup() {
    computer.init();
}

void loop() {
    computer.update(millis());
}