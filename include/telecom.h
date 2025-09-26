#ifndef TELECOM_H
#define TELECOM_H

#include <Arduino.h>
#include <Protocol.h>
#include <capsule.h>

// Add your declarations here
class Telecom {
private:
    gse_uplink_t m_last_packet_received{};
    time_t m_time_last_packet_received = 0;
    time_t m_last_reset_time = 0;

    Capsule<Telecom> capsule_uplink;
    Capsule<Telecom> capsule_downlink;
public:
    explicit Telecom(): capsule_uplink{&Telecom::capsule_uplink_callback, this}, capsule_downlink{&Telecom::capsule_downlink_callback, this} {}

    void init();
    void update();
    void reset();
    
    gse_uplink_t get_last_packet_received(bool consume = true);
    void send_packet(const gse_downlink_t& packet);

    static void lora_handle_uplink1(int packet_size);
    static void lora_handle_uplink2(int packet_size);
    void capsule_uplink_callback(uint8_t packet_id, uint8_t* data_in, uint32_t len);
    void capsule_downlink_callback(uint8_t packet_id, uint8_t* data_in, uint32_t len);
};

#endif // TELECOM_H