#include "i2s.h"
#include "driver/i2s.h"


const uint32_t I2S::sample_rate = 48000;


void I2S::conf_i2s() {
  i2s_config_t i2s_conf = {
    .mode = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_TX),
    .sample_rate = sample_rate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 2,
    .dma_buf_len = 8,
    .use_apll = false,
  };
  i2s_driver_install(I2S_NUM_0, &i2s_conf, 0, NULL);
};

void I2S::init() {
  conf_i2s();
  enable_gpio();  
};

void I2S::write() {
  // Create data
  size_t num_bytes = 2 * 8;
  int16_t audio_data[num_bytes];
  for (int i = 0; i < num_bytes; i++) {
    audio_data[i] = 0x5A; // 0x0101 1010
  }
  // Send data
  size_t bytes_written = 0;
  i2s_write(I2S_NUM_0, audio_data, sizeof(audio_data), &bytes_written, portMAX_DELAY);
};
