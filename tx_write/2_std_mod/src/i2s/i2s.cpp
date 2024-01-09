#include "i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "driver/i2s_types.h"
#include "driver/i2s_common.h"

i2s_chan_handle_t I2S::tx_handle;

const i2s_std_clk_config_t I2S::clk_conf = {
  .sample_rate_hz = 48000,
  .clk_src = I2S_CLK_SRC_PLL_160M,
  .mclk_multiple = I2S_MCLK_MULTIPLE_256,
};

const i2s_std_slot_config_t I2S::slot_conf = {
  .data_bit_width = I2S_DATA_BIT_WIDTH_8BIT,  // Sample size 8 bits
  .slot_bit_width = I2S_SLOT_BIT_WIDTH_8BIT,
  .slot_mode = I2S_SLOT_MODE_MONO,            // Use one channel
  .slot_mask = I2S_STD_SLOT_LEFT,             // DOUT when WS low
  .ws_width = I2S_DATA_BIT_WIDTH_8BIT,
  .ws_pol = false,
  .bit_shift = false,
  .msb_right = false,
};

const i2s_std_gpio_config_t I2S::gpio_conf = {
  .mclk = I2S_GPIO_UNUSED,
  .bclk = PIN_BCLK,
  .ws   = PIN_WS,
  .dout = PIN_DOUT,
  .din  = I2S_GPIO_UNUSED,
  .invert_flags = {
    .mclk_inv = false,
    .bclk_inv = false,
    .ws_inv   = false,
  }
};

void I2S::init() {
  i2s_chan_config_t chan_conf = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
  i2s_std_config_t std_conf = {
    .clk_cfg = clk_conf,
    .slot_cfg = slot_conf,
    .gpio_cfg = gpio_conf,
  };
  i2s_new_channel(&chan_conf, &tx_handle, NULL);
  i2s_channel_init_std_mode(tx_handle, &std_conf);
  i2s_channel_enable(tx_handle);
};

void I2S::write() {
  size_t buffer_size = 4;
  size_t bytes_written = 0;
  uint16_t *buffer = (uint16_t *)malloc(buffer_size);

  // Second byte for i2s buffer alignment
  // Last 8 bits are dropped
  buffer[0] = 0x0100;
  buffer[1] = 0x0300;
  buffer[2] = 0x0500;
  buffer[3] = 0x0700;

  // Transferred as:
  // ..., 0x03, 0x01, 0x07, 0x05, ...
  // Where 1 byte transferred each WS low

  // i2s reads as bytes (uint8)
  size_t i2s_buffer_size = buffer_size * 2;

  i2s_channel_write(tx_handle, buffer, i2s_buffer_size, &bytes_written, portMAX_DELAY);
  free(buffer);
};