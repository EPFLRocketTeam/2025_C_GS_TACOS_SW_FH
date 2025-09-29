#include "telecom.h"

#include <SPI.h>
#include <LoRa.h>
#include <LoopbackStream.h>

// #define LORA_UPLINK_PORT              SPI
// #define LORA_UPLINK_SCK               13
// #define LORA_UPLINK_MOSI              11
// #define LORA_UPLINK_MISO              12
// #define LORA_UPLINK_CS                37
// #define LORA_UPLINK_INT0              36
// #define LORA_UPLINK_RST               35

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
    // Remove or comment out lora_uplink initialization code
    
    // Initialize only the working lora_downlink module
    LORA_DOWNLINK_PORT.setMISO(LORA_DOWNLINK_MISO);
    LORA_DOWNLINK_PORT.setMOSI(LORA_DOWNLINK_MOSI);
    LORA_DOWNLINK_PORT.setCS(LORA_DOWNLINK_CS);
    LORA_DOWNLINK_PORT.setSCK(LORA_DOWNLINK_SCK);
    LORA_DOWNLINK_PORT.begin();
    lora_downlink.setPins(LORA_DOWNLINK_CS, LORA_DOWNLINK_RST, LORA_DOWNLINK_INT0);
    lora_downlink.setSPI(LORA_DOWNLINK_PORT);
    
    // Use GSE_DOWNLINK_FREQUENCY for both sending and receiving
    if (!lora_downlink.begin(GSE_DOWNLINK_FREQUENCY)) {
        Serial.println("Could not setup radio");     
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
    
    // Start in receive mode
    lora_downlink.onReceive(lora_handle_uplink);
    lora_downlink.receive();
}


#define TELEMETRY_TX_INTERVAL_MS 1000  // 1Hz transmission rate

void Telecom::set_telemetry_packet(const gse_downlink_t& packet) {
    memcpy(&m_current_telemetry_packet, &packet, sizeof(gse_downlink_t));
}

// Modify update() to use the stored packet:
void Telecom::update() {
    static unsigned long lastTxTime = 0;
    unsigned long currentTime = millis();
    
    // Check if it's time to send telemetry data
    if (currentTime - lastTxTime >= TELEMETRY_TX_INTERVAL_MS) {
        // Send the stored telemetry packet
        send_packet(m_current_telemetry_packet);
        lastTxTime = currentTime;
        
        // Return to receive mode immediately after sending
        lora_downlink.receive();
    }
    
    // Check for received packets (when not transmitting)
    int packet_size = lora_downlink.parsePacket();
    if (packet_size) {
        // Handle received packet
        Serial.println("Packet Received");
        for (int i = 0; i < packet_size; i++) {
            uplink_buffer.write(lora_downlink.read());
        }
    }
    
    // Process any buffered packets
    while (uplink_buffer.available()) {
        capsule_uplink.decode(uplink_buffer.read());
    }
}

void Telecom::reset() {
    lora_downlink.end();
    delay(20);
    if (!lora_downlink.begin(GSE_DOWNLINK_FREQUENCY)) {
        Serial.println("Could not setup radio");     
    }
    
    lora_downlink.receive();
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
        Serial.println("Failed to begin LoRa packet");
        delete[] encoded;
        lora_downlink.receive(); // Ensure we return to receive mode
        return;
    }

    size_t bytes_written = lora_downlink.write(encoded, gse_downlink_size + ADDITIONAL_BYTES);
    bool success = lora_downlink.endPacket(false);
    delete[] encoded;
    
    if (success) {
        Serial.print("Telemetry packet sent successfully - ");
        Serial.print("Packet size: ");
        Serial.print(gse_downlink_size);
        Serial.print(" bytes + ");
        Serial.print(ADDITIONAL_BYTES);
        Serial.print(" header = ");
        Serial.print(gse_downlink_size + ADDITIONAL_BYTES);
        Serial.print(" total bytes, wrote: ");
        Serial.println(bytes_written);
    } else {
        Serial.println("Failed to send telemetry packet");
    }
    
    // Always return to receive mode
    lora_downlink.receive();
}

void Telecom::lora_handle_uplink(int packet_size) {
    Serial.println("Packet Received");
    for (int i = 0; i < packet_size; i++) {
        // Change from lora_uplink to lora_downlink since we only have one module now
        uplink_buffer.write(lora_downlink.read());
    }
}

void Telecom::capsule_uplink_callback(uint8_t packet_id, uint8_t* data_in, uint32_t len) {
    memcpy(&m_last_packet_received, data_in, len);
    m_time_last_packet_received = millis();
}

void Telecom::capsule_downlink_callback(uint8_t packet_id, uint8_t* data_in, uint32_t len) {
    // Nothing should occur here
}