#include "i2s.h"


void I2S::enable_gpio() {
  i2s_pin_config_t pin_conf = {
    .bck_io_num = PIN_BCLK,
    .ws_io_num = PIN_WS,
    .data_out_num = -1,
    .data_in_num = PIN_DIN,
  };
  i2s_set_pin(I2S_NUM_0, &pin_conf);
};
