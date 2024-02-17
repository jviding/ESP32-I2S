#include "i2s.h"


const uint32_t I2S::sample_rate = 48000;


/*
*  Uses the TX EOF interrupt internally.
*  Disable interrupts to avoid it blocking transmission.
*/
void I2S::conf_i2s() {
  i2s_config_t i2s_conf = {
    .mode = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_TX),
    .sample_rate = sample_rate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_INTRDISABLED, // Block internal interrupts
    .dma_buf_count = 2,  // Min. >= 2
    .dma_buf_len = 8,    // Min. >= 8
    .use_apll = false,
  };
  i2s_driver_install(I2S_NUM_0, &i2s_conf, 0, NULL);
};
