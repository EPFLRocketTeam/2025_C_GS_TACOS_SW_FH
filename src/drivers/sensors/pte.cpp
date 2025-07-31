#include "drivers/sensors/pte.h"

void PTESensor::init(const pte_config_t& config) {
    i2c_channel = config.i2c_channel;
    i2c_mux_id = config.i2c_mux_id;
    min_time = config.min_time;

    i2c_channel->begin();
    sample_calib_buffer.flush();
}

void PTESensor::tick() {
    if(millis() - last_sample > min_time) {
        last_sample = millis();
        // TODO read from I2C into sample
        sample_calib_buffer.push(&sample);
    }
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
