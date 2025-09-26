#include "telecom.h"

#include <SPI.h>
#include <LoRa.h>
#include <LoopbackStream.h>

#define LORA_UPLINK_PORT              SPI
#define LORA_UPLINK_SCK               13
#define LORA_UPLINK_MOSI              11
#define LORA_UPLINK_MISO              12
#define LORA_UPLINK_CS                37
#define LORA_UPLINK_INT0              36
#define LORA_UPLINK_RST               35

#define LORA_DOWNLINK_PORT                SPI1
#define LORA_DOWNLINK_SCK                 27  
#define LORA_DOWNLINK_MOSI                26
#define LORA_DOWNLINK_MISO                39
#define LORA_DOWNLINK_CS                  38
#define LORA_DOWNLINK_INT0                31
#define LORA_DOWNLINK_RST                 32

#define lora_uplink LoRa
// LoRaClass lora_uplink;
LoRaClass lora_downlink;

LoopbackStream uplink_buffer(MAX_BUFFER_SIZE);

void Telecom::init() {
    LORA_UPLINK_PORT.begin();
    lora_uplink.setPins(LORA_UPLINK_CS, LORA_UPLINK_RST, LORA_DOWNLINK_INT0);
    lora_uplink.setSPI(LORA_UPLINK_PORT);
    
    if (!lora_uplink.begin(UPLINK_FREQUENCY)) {
        Serial.println("Could not setup uplink radio");     
    }
  
    lora_uplink.setTxPower(UPLINK_POWER);
    lora_uplink.setSignalBandwidth(UPLINK_BW);
    lora_uplink.setSpreadingFactor(UPLINK_SF);
    lora_uplink.setCodingRate4(UPLINK_CR);
    lora_uplink.setPreambleLength(UPLINK_PREAMBLE_LEN);
    

    #if (UPLINK_CRC)
    lora_uplink.enableCrc();
    #else
    lora_uplink.disableCrc();
    #endif


    // Set uplink radio as a continuous receiver
    lora_uplink.onReceive(lora_handle_uplink);
    lora_uplink.receive();
    lora_uplink.receive();

    
    LORA_DOWNLINK_PORT.setMISO(LORA_DOWNLINK_MISO);
    LORA_DOWNLINK_PORT.setMOSI(LORA_DOWNLINK_MOSI);
    LORA_DOWNLINK_PORT.setCS(LORA_DOWNLINK_CS);
    LORA_DOWNLINK_PORT.setSCK(LORA_DOWNLINK_SCK);
    LORA_DOWNLINK_PORT.begin();
    lora_downlink.setPins(LORA_DOWNLINK_CS, LORA_DOWNLINK_RST, LORA_DOWNLINK_INT0);
    lora_downlink.setSPI(LORA_DOWNLINK_PORT);
    if (!lora_downlink.begin(GSE_DOWNLINK_FREQUENCY)) {
        Serial.println("Could not setup downlink radio");     
    }

    lora_downlink.setTxPower(GSE_DOWNLINK_POWER);
    lora_downlink.setSignalBandwidth(GSE_DOWNLINK_BW);
    lora_downlink.setSpreadingFactor(GSE_DOWNLINK_SF);
    lora_downlink.setCodingRate4(GSE_DOWNLINK_CR);
    lora_downlink.setPreambleLength(GSE_DOWNLINK_PREAMBLE_LEN);

    #if (DOWNLINK_CRC)
    lora_downlink.enableCrc();
    #else
    lora_downlink.disableCrc();
    #endif
}

void Telecom::update() {
    int packet_size = lora_uplink.parsePacket();
    if (packet_size) {
        lora_handle_uplink(packet_size);
        Serial.println("Packet handled");
    }

    while (uplink_buffer.available()) {
        Serial.println("Decoding packet");
        capsule_uplink.decode(uplink_buffer.read());
    }
}

void Telecom::reset() {
    lora_downlink.end();
    delay(50);
    if (!lora_downlink.begin(GSE_DOWNLINK_FREQUENCY)) {
    Serial.println("Could not setup downlink radio");     
    }

    lora_uplink.end();
    delay(50);
    if (!lora_uplink.begin(UPLINK_FREQUENCY)) {
    Serial.println("Could not setup uplink radio");     
    }
    
    lora_uplink.receive();
    lora_uplink.receive();

    uplink_buffer.clear();
}

gse_uplink_t Telecom::get_last_packet_received(bool consume) {

    gse_uplink_t copy = m_last_packet_received;

    if(consume)
        memset(&m_last_packet_received, 0, gse_uplink_size);

    return copy;
    
}

void Telecom::send_packet(const gse_downlink_t& packet) {
    uint8_t* encoded{capsule_downlink.encode(GSE_TELEMETRY, ((uint8_t*) &packet), gse_downlink_size)};

    if (!lora_downlink.beginPacket()) {
        return;
    }

    lora_downlink.write(encoded, gse_downlink_size + ADDITIONAL_BYTES);
    lora_downlink.endPacket(false);
    delete[] encoded;

}

void Telecom::lora_handle_uplink(int packet_size) {
    Serial.println("Packet Received");
    for (int i = 0; i < packet_size; i++) {
        uplink_buffer.write(lora_uplink.read());
    }
}

void Telecom::capsule_uplink_callback(uint8_t packet_id, uint8_t* data_in, uint32_t len) {
    memcpy(&m_last_packet_received, data_in, len);
    m_time_last_packet_received = millis();
}

void Telecom::capsule_downlink_callback(uint8_t packet_id, uint8_t* data_in, uint32_t len) {
    // Nothing should occur here
}