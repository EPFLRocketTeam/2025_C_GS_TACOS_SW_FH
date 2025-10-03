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

	//computer.force_all_solenoids_open(); // Pour test de continuité électrique de la board.
    


}

void loop() {
 	computer.update(millis());

}