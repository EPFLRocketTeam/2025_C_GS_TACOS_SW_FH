#include "telecom.h"

#include <SPI.h>
#include <LoRa.h>
#include <LoopbackStream.h>

#define LORA_UPLINK_PORT                SPI
#define LORA_UPLINK_SCK                 13
#define LORA_UPLINK_MOSI                11
#define LORA_UPLINK_MISO                12
#define LORA_UPLINK_CS                  37
#define LORA_UPLINK_INT0                36
#define LORA_UPLINK_RST                 35

#define LORA_DOWNLINK_PORT                SPI1
#define LORA_DOWNLINK_SCK                 27  
#define LORA_DOWNLINK_MOSI                26
#define LORA_DOWNLINK_MISO                39
#define LORA_DOWNLINK_CS                  38
#define LORA_DOWNLINK_INT0                31
#define LORA_DOWNLINK_RST                 32

#define lora_uplink LoRa
LoRaClass lora_downlink;

LoopbackStream uplink_buffer(MAX_BUFFER_SIZE);

// Static instance pointer for callback access
static Telecom* telecom_instance = nullptr;

static bool start_radio(
    LoRaClass& radio,
    double frequency,
    uint8_t tx_power,
    double bandwidth,
    uint8_t spreading_factor,
    uint8_t coding_rate,
    uint16_t preamble_length,
    bool crc_enabled,
    bool invert_iq,
    const char* label) {

    if (!radio.begin(static_cast<long>(frequency))) {
        Serial.print("[");
        Serial.print(label);
        Serial.println("] Could not setup radio");
        return false;
    }

    radio.setTxPower(tx_power);
    radio.setSignalBandwidth(static_cast<long>(bandwidth));
    radio.setSpreadingFactor(spreading_factor);
    radio.setCodingRate4(coding_rate);
    radio.setPreambleLength(preamble_length);

    if (crc_enabled) {
        radio.enableCrc();
    } else {
        radio.disableCrc();
    }

    if (invert_iq) {
        radio.enableInvertIQ();
    } else {
        radio.disableInvertIQ();
    }

    Serial.print("[");
    Serial.print(label);
    Serial.print("] Radio ready at ");
    Serial.print(frequency / 1E6, 3);
    Serial.println(" MHz");

    return true;
}

static void handle_radio_receive(LoRaClass& radio, const char* radio_name, int packet_size, bool push_to_uplink_buffer) {
    if (packet_size <= 0) {
        radio.receive();
        return;
    }

    Serial.print("\n=== ");
    Serial.print(radio_name);
    Serial.println(" LORA PACKET RECEIVED ===");
    Serial.print("Size: ");
    Serial.print(packet_size);
    Serial.print(" bytes, RSSI: ");
    Serial.print(radio.packetRssi());
    Serial.print(" dBm, SNR: ");
    Serial.print(radio.packetSnr());
    Serial.println(" dB");

    Serial.print("Raw bytes: ");
    for (int i = 0; i < packet_size; i++) {
        uint8_t byte_received = radio.read();
        if (push_to_uplink_buffer) {
            uplink_buffer.write(byte_received);
        }

        Serial.print("0x");
        if (byte_received < 0x10) Serial.print("0");
        Serial.print(byte_received, HEX);
        Serial.print(" ");

        if ((i + 1) % 8 == 0) Serial.println();
    }
    Serial.println();
    if (push_to_uplink_buffer) {
        Serial.print("Packet data added to buffer for Capsule decoding (source: ");
        Serial.print(radio_name);
        Serial.println(")");
    } else {
        Serial.print("Packet captured on ");
        Serial.print(radio_name);
        Serial.println(" radio (monitoring only)");
    }
    Serial.println("==========================\n");

    radio.receive();
}

void Telecom::init() {
    // Store instance for static callback
    telecom_instance = this;
    memset(&m_current_telemetry_packet, 0, sizeof(m_current_telemetry_packet));
    memset(&m_last_packet_received, 0, sizeof(m_last_packet_received));
    m_hasTelemetryPacket = false;
    m_last_downlink_tx_ms = 0;
    m_uplink_ready = false;
    m_downlink_ready = false;
    
    // Initialize uplink radio on SPI
    LORA_UPLINK_PORT.setMISO(LORA_UPLINK_MISO);
    LORA_UPLINK_PORT.setMOSI(LORA_UPLINK_MOSI);
    LORA_UPLINK_PORT.setCS(LORA_UPLINK_CS);
    LORA_UPLINK_PORT.setSCK(LORA_UPLINK_SCK);
    LORA_UPLINK_PORT.begin();
    lora_uplink.setPins(LORA_UPLINK_CS, LORA_UPLINK_RST, LORA_UPLINK_INT0);
    lora_uplink.setSPI(LORA_UPLINK_PORT);

    bool uplink_ready = start_radio(
        lora_uplink,
        UPLINK_FREQUENCY,
        UPLINK_POWER,
        UPLINK_BW,
        UPLINK_SF,
        UPLINK_CR,
        UPLINK_PREAMBLE_LEN,
        static_cast<bool>(UPLINK_CRC),
        static_cast<bool>(UPLINK_INVERSE_IQ),
        "UPLINK"
    );

    m_uplink_ready = uplink_ready;
    if (m_uplink_ready) {
        lora_uplink.onReceive(lora_handle_uplink);
        lora_uplink.receive();
    }

    // Initialize downlink radio on SPI1
    LORA_DOWNLINK_PORT.setMISO(LORA_DOWNLINK_MISO);
    LORA_DOWNLINK_PORT.setMOSI(LORA_DOWNLINK_MOSI);
    LORA_DOWNLINK_PORT.setCS(LORA_DOWNLINK_CS);
    LORA_DOWNLINK_PORT.setSCK(LORA_DOWNLINK_SCK);
    LORA_DOWNLINK_PORT.begin();
    lora_downlink.setPins(LORA_DOWNLINK_CS, LORA_DOWNLINK_RST, LORA_DOWNLINK_INT0);
    lora_downlink.setSPI(LORA_DOWNLINK_PORT);

    bool downlink_ready = start_radio(
        lora_downlink,
        GSE_DOWNLINK_FREQUENCY,
        GSE_DOWNLINK_POWER,
        GSE_DOWNLINK_BW,
        GSE_DOWNLINK_SF,
        GSE_DOWNLINK_CR,
        GSE_DOWNLINK_PREAMBLE_LEN,
        static_cast<bool>(GSE_DOWNLINK_CRC),
        static_cast<bool>(GSE_DOWNLINK_INVERSE_IQ),
        "DOWNLINK"
    );

    m_downlink_ready = downlink_ready;
    if (m_downlink_ready) {
        lora_downlink.onReceive(lora_handle_downlink);
        lora_downlink.receive();
    }
    
    Serial.println("=== TELECOM INITIALIZED ===");
    Serial.print("[UPLINK] ");
    Serial.println(m_uplink_ready ? "Listening for ground station commands" : "FAILED to initialize");
    Serial.print("[DOWNLINK] ");
    Serial.println(m_downlink_ready ? "Ready to transmit telemetry" : "FAILED to initialize");
    Serial.println("================================");
}


#define TELEMETRY_TX_INTERVAL_MS 1000  // 1Hz transmission rate

void Telecom::set_telemetry_packet(const gse_downlink_t& packet) {
    memcpy(&m_current_telemetry_packet, &packet, sizeof(gse_downlink_t));
    m_hasTelemetryPacket = true;
}

// Modify update() to use the stored packet:
void Telecom::update() {
    unsigned long currentTime = millis();
    
    // Check if it's time to send telemetry data
    if (m_downlink_ready && m_hasTelemetryPacket && (currentTime - m_last_downlink_tx_ms >= TELEMETRY_TX_INTERVAL_MS)) {
        if (send_packet(m_current_telemetry_packet)) {
            m_last_downlink_tx_ms = currentTime;
        }
    }
    
    // Process any buffered packets received via callback
    static int decode_count = 0;
    while (uplink_buffer.available()) {
        uint8_t byte_to_decode = uplink_buffer.read();
        decode_count++;
        Serial.print("[CAPSULE] Decoding byte #");
        Serial.print(decode_count);
        Serial.print(": 0x");
        Serial.println(byte_to_decode, HEX);
        capsule_uplink.decode(byte_to_decode);
    }
}

void Telecom::reset() {
    lora_downlink.end();
    delay(20);
    bool downlink_ready = start_radio(
        lora_downlink,
        GSE_DOWNLINK_FREQUENCY,
        GSE_DOWNLINK_POWER,
        GSE_DOWNLINK_BW,
        GSE_DOWNLINK_SF,
        GSE_DOWNLINK_CR,
        GSE_DOWNLINK_PREAMBLE_LEN,
        static_cast<bool>(GSE_DOWNLINK_CRC),
        static_cast<bool>(GSE_DOWNLINK_INVERSE_IQ),
        "DOWNLINK"
    );

    m_downlink_ready = downlink_ready;
    if (m_downlink_ready) {
        lora_downlink.onReceive(lora_handle_downlink);
        lora_downlink.receive();
    }

    lora_uplink.end();
    delay(20);
    bool uplink_ready = start_radio(
        lora_uplink,
        UPLINK_FREQUENCY,
        UPLINK_POWER,
        UPLINK_BW,
        UPLINK_SF,
        UPLINK_CR,
        UPLINK_PREAMBLE_LEN,
        static_cast<bool>(UPLINK_CRC),
        static_cast<bool>(UPLINK_INVERSE_IQ),
        "UPLINK"
    );

    m_uplink_ready = uplink_ready;
    if (m_uplink_ready) {
        lora_uplink.onReceive(lora_handle_uplink);
        lora_uplink.receive();
    }

    uplink_buffer.clear();
}

gse_uplink_t Telecom::get_last_packet_received(bool consume) {

    gse_uplink_t copy = m_last_packet_received;

    if(consume)
        memset(&m_last_packet_received, 0, gse_uplink_size);

    return copy;
    
}

bool Telecom::send_packet(const gse_downlink_t& packet) {
    if (!m_downlink_ready) {
        Serial.println("[DOWNLINK] Radio not ready, skipping telemetry transmission");
        return false;
    }

    Serial.println("=== SENDING PACKET ===");
    Serial.print("GP1: "); Serial.println(packet.GP1);
    Serial.print("GP2: "); Serial.println(packet.GP2);
    Serial.print("GP3: "); Serial.println(packet.GP3);
    Serial.print("GP4: "); Serial.println(packet.GP4);
    Serial.print("GP5: "); Serial.println(packet.GP5);
    Serial.print("GQN_NC1: "); Serial.println(packet.GQN_NC1);
    Serial.print("GQN_NC2: "); Serial.println(packet.GQN_NC2);
    
    uint8_t* encoded{capsule_downlink.encode(GSE_TELEMETRY, ((uint8_t*) &packet), gse_downlink_size)};

    Serial.print("Encoded packet first 8 bytes: ");
    for (int i = 0; i < 8 && i < (gse_downlink_size + ADDITIONAL_BYTES); i++) {
        Serial.print("0x");
        Serial.print(encoded[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    size_t encoded_len = gse_downlink_size + ADDITIONAL_BYTES;

    if (!lora_downlink.beginPacket()) {
        Serial.println("Failed to begin LoRa packet");
        delete[] encoded;
        lora_downlink.receive(); // Ensure we return to receive mode
        return false;
    }

    size_t bytes_written = lora_downlink.write(encoded, encoded_len);
    bool success = lora_downlink.endPacket(false);
    delete[] encoded;
    
    if (success) {
        Serial.print("Telemetry packet sent successfully - ");
        Serial.print("Packet size: ");
        Serial.print(gse_downlink_size);
        Serial.print(" bytes + ");
        Serial.print(ADDITIONAL_BYTES);
        Serial.print(" header = ");
        Serial.print(encoded_len);
        Serial.print(" total bytes, wrote: ");
        Serial.println(bytes_written);
        if (bytes_written != encoded_len) {
            Serial.print("[WARN] Expected to write ");
            Serial.print(encoded_len);
            Serial.print(" bytes but actually wrote ");
            Serial.println(bytes_written);
        }
    } else {
        Serial.println("Failed to send telemetry packet");
    }
    
    // Always return to receive mode
    lora_downlink.receive();
    return success;
}

void Telecom::lora_handle_downlink(int packet_size) {
    handle_radio_receive(lora_downlink, "DOWNLINK", packet_size, false);
}

void Telecom::lora_handle_uplink(int packet_size) {
    handle_radio_receive(lora_uplink, "UPLINK", packet_size, true);
}

void Telecom::capsule_uplink_callback(uint8_t packet_id, uint8_t* data_in, uint32_t len) {
    Serial.println("\n*** CAPSULE PACKET SUCCESSFULLY DECODED ***");
    Serial.print("Packet ID: ");
    Serial.print(packet_id);
    
    // Check what type of packet we received
    if (packet_id == GSE_TELEMETRY) {
        Serial.println(" -> GSE_TELEMETRY (télémétrie)");
    } else {
        Serial.println(" -> GSE COMMAND (commande)");
    }
    
    Serial.print("Data Length: ");
    Serial.println(len);
    
    // Print ALL the decoded data (up to 64 bytes)
    Serial.print("Decoded data: ");
    for (uint32_t i = 0; i < len && i < 64; i++) {
        Serial.print("0x");
        if (data_in[i] < 0x10) Serial.print("0");
        Serial.print(data_in[i], HEX);
        Serial.print(" ");
        if ((i + 1) % 8 == 0) Serial.println();
    }
    Serial.println();
    
    // Handle both types of packets
    if (packet_id == GSE_TELEMETRY && len == sizeof(gse_downlink_t)) {
        Serial.println("✓ This is a telemetry packet (same as what we send)");
        gse_downlink_t* telemetry = (gse_downlink_t*)data_in;
        Serial.print("GP1: "); Serial.println(telemetry->GP1);
        Serial.print("GP2: "); Serial.println(telemetry->GP2);
        Serial.print("GQN_NC1: "); Serial.println(telemetry->GQN_NC1);
    } else if (len <= sizeof(gse_uplink_t)) {
        Serial.println("✓ This is a command packet");
        memcpy(&m_last_packet_received, data_in, len);
        m_time_last_packet_received = millis();
        
        if (len >= 2) {
            Serial.print("Command ID: ");
            Serial.print(m_last_packet_received.order_id);
            Serial.print(", Value: ");
            Serial.println(m_last_packet_received.order_value);
        }
    } else {
        Serial.println("✗ Unknown packet type or size");
    }
    
    Serial.println("*******************************************\n");
}

void Telecom::capsule_downlink_callback(uint8_t packet_id, uint8_t* data_in, uint32_t len) {
    // Nothing should occur here
}