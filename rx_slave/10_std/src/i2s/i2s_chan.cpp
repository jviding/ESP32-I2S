#include "i2s.h"

/*
* Master BCLK rate assumed as 1 Mhz
*/
void I2S::enable_channel() {
  i2s_config_t i2s_conf = {
    .mode = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_RX),
    .sample_rate = 500000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_8BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 2,
    .dma_buf_len = 8,
    .use_apll = false,
  };
  i2s_driver_install(I2S_NUM_0, &i2s_conf, 0, NULL);
};
