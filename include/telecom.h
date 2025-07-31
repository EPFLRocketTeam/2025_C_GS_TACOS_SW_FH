#ifndef TELECOM_H
#define TELECOM_H

#include <Arduino.h>
#include <cppQueue.h>
#include <SPI.h>
#include "Protocol.h"

#define RX_COMMANDS_BUFFER              10
#define LORA_INIT_MAX                   10

#define LORA_DOWNLINK_PORT              SPI
#define LORA_DOWNLINK_SCK               27
#define LORA_DOWNLINK_MOSI              26
#define LORA_DOWNLINK_MISO              39
#define LORA_DOWNLINK_CS                38
#define LORA_DOWNLINK_INT0              40
#define LORA_DOWNLINK_RST               37

#define LORA_UPLINK_PORT                SPI1
#define LORA_UPLINK_SCK                 13   
#define LORA_UPLINK_MOSI                11
#define LORA_UPLINK_MISO                12
#define LORA_UPLINK_CS                  10
#define LORA_UPLINK_INT0                9
#define LORA_UPLINK_RST                 14

class TelecomBridge {
private:
    void on_receive(uint8_t capsuleId, uint8_t* data, uint32_t len);
    cppQueue rxBuffer;
    gse_uplink_t uplink;
    
public:

    TelecomBridge() : rxBuffer(gse_uplink_size, RX_COMMANDS_BUFFER, FIFO, true) {}

    void init();
    void tick();
    void send(uint8_t packetId, uint8_t *data, uint32_t len);    

    gse_uplink_t get_uplink();

    static void lora_uplink(int packetSize);
    static void capsule_uplink(uint8_t capsuleId, uint8_t* data, uint32_t len);
};

static TelecomBridge telecomInstance{};

#endif