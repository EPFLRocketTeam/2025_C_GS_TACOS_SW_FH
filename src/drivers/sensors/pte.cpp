#include "drivers/sensors/pte.h"

void PTESensor::init(const pte_config_t& config) {
    i2c_channel = config.i2c_channel;
    mux_instance = config.mux_instance;
    i2c_mux_id = config.i2c_mux_id;
    min_time = config.min_time;

    i2c_channel->begin();
    sample_calib_buffer.flush();

    // Reset MUX
    digitalWrite(PTE_MUX_RST, LOW);
    delay(2); // Hold reset low for at least 1 ms
    digitalWrite(PTE_MUX_RST, HIGH);

    // Start MUX
    mux_instance->begin(); 
}

float rawToBar(int16_t rawPressure) {
  // Linear interpolation
  return ((rawPressure - (-16000.0)) * (100.0) / (16000.0 - (-16000.0)));
}

void PTESensor::tick() {
    if(millis() - last_valid_sample < min_time)
        return;

    mux_instance->selectChannel(i2c_mux_id);
    
    i2c_channel->beginTransmission(PTE_I2C_ID);
    i2c_channel->write(PTE_RAM_ADDR_S);
    i2c_channel->endTransmission(false);
    i2c_channel->requestFrom(PTE_I2C_ID, 2);
    
    if (Wire1.available() >= 2) {
        uint8_t lowByte = i2c_channel->read();
        uint8_t highByte = i2c_channel->read();
        sample = rawToBar((highByte << 8) | lowByte);

        last_valid_sample = millis();
        sample_calib_buffer.push(&sample);
        return;
    }

    sample = 0;

}

pte_sample_t PTESensor::get() {
    return sample;
}

void PTESensor::calibrate() {

    // Prevent calibration when buffer is not full
    if(!sample_calib_buffer.isFull())
        return;

    cppQueue temp = sample_calib_buffer;
    float t = 0;

    // Compute the bias
    bias = 0;

    while(!temp.isEmpty()) {
        temp.pop(&t);
        bias += t;
    }

    bias = bias/PTE_SAMPLES_CALIB_BUFFER;

    // Compute the variance
    var = 0;
    while(!sample_calib_buffer.isEmpty()) {
        sample_calib_buffer.pop(&t);
        var += (t - bias);
        var *= var;
    }

    var = var/(PTE_SAMPLES_CALIB_BUFFER - 1);

    //Sanity check
    sample_calib_buffer.flush();
}
