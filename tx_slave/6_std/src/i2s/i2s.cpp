#include "i2s.h"
#include "driver/i2s.h"
#include <stdio.h>


const uint32_t I2S::sample_rate = 48000;


void I2S::conf_i2s() {
  i2s_config_t i2s_conf = {
    .mode = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_TX),
    .sample_rate = sample_rate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
  };
  i2s_driver_install(I2S_NUM_0, &i2s_conf, 0, NULL);
};

void I2S::conf_gpio() {
  i2s_pin_config_t pin_conf = {
    .bck_io_num = PIN_BCLK,
    .ws_io_num = PIN_WS,
    .data_out_num = PIN_DOUT,
    .data_in_num = -1,
  };
  i2s_set_pin(I2S_NUM_0, &pin_conf);
};

void I2S::init() {
  conf_i2s();
  conf_gpio();  
};

void I2S::write() {
  // Create data
  int16_t audio_data[512];
  for (int i = 0; i < 512; i++) {
    audio_data[i] = i;
  }
  // Send data
  size_t bytes_written = 0;
  i2s_write(I2S_NUM_0, audio_data, sizeof(audio_data), &bytes_written, portMAX_DELAY);
};
