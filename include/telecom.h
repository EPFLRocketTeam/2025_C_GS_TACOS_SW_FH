#ifndef TELECOM_H
#define TELECOM_H

#include <Arduino.h>
#include <Protocol.h>
#include "capsule.h"

// Add your declarations here
class Telecom {
private:
    gse_uplink_t m_last_packet_received{};
    time_t m_time_last_packet_received = 0;
    time_t m_last_reset_time = 0;

    Capsule<Telecom> capsule_uplink;
    Capsule<Telecom> capsule_downlink;
    void change_frequency(long new_freq);
    gse_downlink_t m_current_telemetry_packet; // Store the current telemetry packet
    bool m_hasTelemetryPacket = false;
    bool m_uplink_ready = false;
    bool m_downlink_ready = false;
    unsigned long m_last_downlink_tx_ms = 0;
    
public:
    explicit Telecom(): capsule_uplink{&Telecom::capsule_uplink_callback, this}, capsule_downlink{&Telecom::capsule_downlink_callback, this} {}

    void init();
    void update();
    void reset();
    
    gse_uplink_t get_last_packet_received(bool consume = true);
    bool send_packet(const gse_downlink_t& packet);

    static void lora_handle_downlink(int packet_size);
    static void lora_handle_uplink(int packet_size);
    void capsule_uplink_callback(uint8_t packet_id, uint8_t* data_in, uint32_t len);
    void capsule_downlink_callback(uint8_t packet_id, uint8_t* data_in, uint32_t len);

    void set_telemetry_packet(const gse_downlink_t& packet);
};

#endif // TELECOM_H