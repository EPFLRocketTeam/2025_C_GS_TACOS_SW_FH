// Last update: 05/09/2025
#include <Arduino.h>
#include "TACOSComputer.h"
#include <SPI.h>
#include <LoRa.h>

#define LORA_DOWNLINK_PORT              SPI
#define LORA_DOWNLINK_SCK               13
#define LORA_DOWNLINK_MOSI              11
#define LORA_DOWNLINK_MISO              12
#define LORA_DOWNLINK_CS                37
#define LORA_DOWNLINK_INT0              36
#define LORA_DOWNLINK_RST               35

#define LORA_UPLINK_PORT                SPI1
#define LORA_UPLINK_SCK                 27  
#define LORA_UPLINK_MOSI                26
#define LORA_UPLINK_MISO                39
#define LORA_UPLINK_CS                  38
#define LORA_UPLINK_INT0                31
#define LORA_UPLINK_RST                 32

// #define LORA_DOWNLINK_PORT              SPI1
// #define LORA_DOWNLINK_SCK               19
// #define LORA_DOWNLINK_MOSI              18
// #define LORA_DOWNLINK_MISO              31
// #define LORA_DOWNLINK_CS                38
// #define LORA_DOWNLINK_INT0              31
// #define LORA_DOWNLINK_RST               32

TACOSComputer computer;

void setup() {
    delay(2000);
    Serial.begin(115200);
    Serial.println("Starting");
    computer.init();


    // LORA_UPLINK_PORT.setMISO(LORA_UPLINK_MISO);
    // LORA_UPLINK_PORT.setMOSI(LORA_UPLINK_MOSI);
    // LORA_UPLINK_PORT.setCS(LORA_UPLINK_CS);
    // LORA_UPLINK_PORT.setSCK(LORA_UPLINK_SCK);
    // LORA_UPLINK_PORT.begin();
	// LoRa.setPins(LORA_UPLINK_CS, LORA_UPLINK_RST, LORA_UPLINK_INT0);
	// LoRa.setSPI(LORA_UPLINK_PORT);
    
	// if (!LoRa.begin(GSE_DOWNLINK_FREQUENCY)) Serial.println("Starting LoRa failed!");

	// // Set LoRa parameters
	// LoRa.setTxPower(GSE_DOWNLINK_POWER);
	// LoRa.setSpreadingFactor(GSE_DOWNLINK_SF);
	// LoRa.setSignalBandwidth(GSE_DOWNLINK_BW);
	// LoRa.setCodingRate4(GSE_DOWNLINK_CR);
	// LoRa.setPreambleLength(GSE_DOWNLINK_PREAMBLE_LEN);

	// #if (GSE_DOWNLINK_CRC)
	// LoRa.enableCrc(); // not necessary to work with miaou, even if miaou enbale it...:-|
	// #else
	// LoRa.disableCrc();
	// #endif


	// // ! \\ Ne fonctionne que en disableInvertIQ
	// #if (GSE_DOWNLINK_INVERSE_IQ)	
	// 	LoRa.enableInvertIQ();
	// #else
	// 	LoRa.disableInvertIQ();
	// #endif
}

void loop() {

    computer.update(millis());
    // Serial.println("In the loop");
    // delay(1000);
    // uint8_t  packetToSend[4] = {0, 1, 2, 3};
    // if(LoRa.beginPacket()) {
    //     Serial.println("Transmitted a packet");
	// 	LoRa.write(packetToSend,4);
	// 	LoRa.endPacket(false);
    // } else {
    //     Serial.println("Could not transmit");
    // }

}