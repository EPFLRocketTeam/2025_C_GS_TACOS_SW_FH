#include "telecom.h"

#include <Arduino.h>
#include <LoRa.h>
#include <LoopbackStream.h>
#include <capsule.h>
#include "logger.h"

#define loraUplink LoRa
static LoRaClass loraDownlink;

static LoopbackStream buffer(1024);
static Capsule<TelecomBridge> capsule{TelecomBridge::capsule_uplink};


// INSTANCE DEPENDANT

void TelecomBridge::init() {
    log("Initializing Telecom Bridge...\n");
    rxBuffer.flush();

    log("Intializing Downlink LoRa...\n");

    LORA_DOWNLINK_PORT.setMISO(LORA_DOWNLINK_MISO);
    LORA_DOWNLINK_PORT.setMOSI(LORA_DOWNLINK_MOSI);
    LORA_DOWNLINK_PORT.setSCK(LORA_DOWNLINK_SCK);

    loraDownlink.setPins(LORA_DOWNLINK_CS, LORA_DOWNLINK_RST, LORA_DOWNLINK_INT0);
    loraDownlink.setSPI(LORA_DOWNLINK_PORT);

    bool status = false;
    for(int i = 0; i < LORA_INIT_MAX && !status; i++) {
        status = loraDownlink.begin(GSE_DOWNLINK_FREQUENCY);
        log("Attempting Downlink LoRa Conenction (%d/%d)\n", i+1, LORA_INIT_MAX);
    }

    if(!status) log("Downlink LoRa could not connect!\n");
    else log("Downlink LoRa connected!\n");

    log("Configuring Downlink LoRa...\n");
    loraDownlink.setTxPower(GSE_DOWNLINK_POWER);
    loraDownlink.setSpreadingFactor(GSE_DOWNLINK_SF);
    loraDownlink.setSignalBandwidth(GSE_DOWNLINK_BW);
    loraDownlink.setCodingRate4(GSE_DOWNLINK_CR);
    loraDownlink.setPreambleLength(GSE_DOWNLINK_PREAMBLE_LEN);
    #if (GSE_DOWNLINK_CRC)
    loraDownlink.enableCrc();  // not necessary to work with miaou, even if miaou enbale it...:-|
    #else
    loraDownlink.disableCrc();
    #endif
    #if (GSE_DOWNLINK_INVERSE_IQ)
    loraDownlink.enableInvertIQ();
    #else
    loraDownlink.disableInvertIQ();
    #endif
    log("Downlink LoRa configured.\n");

    log("Intializing Uplink link LoRa...\n");
    LORA_UPLINK_PORT.setMISO(LORA_UPLINK_MISO);
    LORA_UPLINK_PORT.setMOSI(LORA_UPLINK_MOSI);
    LORA_UPLINK_PORT.setSCK(LORA_UPLINK_SCK);

    loraUplink.setPins(LORA_UPLINK_CS, LORA_UPLINK_RST, LORA_UPLINK_INT0);
    loraUplink.setSPI(LORA_UPLINK_PORT);

    status = false;
    for(int i = 0; i < LORA_INIT_MAX && !status; i++) {
        status = loraUplink.begin(UPLINK_FREQUENCY);
        log("Attempting Uplink LoRa Conenction (%d/%d)", i+1, LORA_INIT_MAX);
    }

    if(!status) log("Uplink LoRa could not connect!");
    else log("Uplink LoRa connected!");

    log("Configuring Uplink LoRa...\n");
    loraUplink.setTxPower(UPLINK_POWER);
    loraUplink.setSpreadingFactor(UPLINK_SF);
    loraUplink.setSignalBandwidth(UPLINK_BW);
    loraUplink.setCodingRate4(UPLINK_CR);
    loraUplink.setPreambleLength(UPLINK_PREAMBLE_LEN);
    #if (UPLINK_CRC)
    loraUplink.enableCrc();  // not necessary to work with miaou, even if miaou enbale it...:-|
    #else
    loraUplink.disableCrc();
    #endif
    #if (UPLINK_INVERSE_IQ)
    loraUplink.enableInvertIQ();
    #else
    loraUplink.disableInvertIQ();
    #endif

    // Setup Rx Interrupt
    loraUplink.receive(); 
    loraUplink.onReceive(TelecomBridge::lora_uplink); 
    log("Uplink LoRa configured!\n");
    log("Telecoms Initialized\n");

}

void TelecomBridge::tick() {
    while (buffer.available())
        capsule.decode(buffer.read());

    if(!rxBuffer.isEmpty()) { // Pop or no packet
        rxBuffer.pop(&uplink);
    } else {
        uplink.order_id = NO_PACKET;
        uplink.order_value = 0;
    }
}

void TelecomBridge::send(uint8_t packetId, uint8_t *data, uint32_t len) {
    log("Sending a new packet (%d)\n", packetId);

    uint8_t *codedBuffer = capsule.encode(packetId, data, len);
    size_t codedLen = capsule.getCodedLen(len);

    loraDownlink.beginPacket();
    loraDownlink.write(codedBuffer, codedLen);
    loraDownlink.endPacket(true);

    log("Packet Sent\n");

    delete[] codedBuffer;
}

gse_uplink_t TelecomBridge::get_uplink() {
    return uplink;     
}

void TelecomBridge::on_receive(uint8_t capsuleId, uint8_t* data, uint32_t len) {
    if(capsuleId != CAPSULE_ID::GSC_CMD)
        return;
    
    log("Received new packet from GSC\n");

    // We have a de facto timestamp-based ordering since packets are transmitted sequentially
    // No end-of-scope issues since queue.push has an internal memcpy
    gse_uplink_t packet;
    memcpy(&packet, data, len);
    rxBuffer.push(&packet);
}

// STATIC, LORA/CAPSULE

void TelecomBridge::lora_uplink(int packetSize) {
    for (int i = 0; i < packetSize; i++)
        buffer.write(loraUplink.read());
} 

void TelecomBridge::capsule_uplink(uint8_t capsuleId, uint8_t* data, uint32_t len) {
    telecomInstance.on_receive(capsuleId, data, len);
}