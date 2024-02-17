#include "i2s.h"
#include "hal/gpio_hal.h"
#include "rom/gpio.h"


void I2S::configure_pin(uint16_t gpio_num, gpio_mode_t gpio_mode) {
  gpio_config_t gpio_conf = {
    .pin_bit_mask = (1ULL << gpio_num),
    .mode = gpio_mode,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE,
  };
  gpio_config(&gpio_conf);
};

void I2S::enable_gpio() {
  // Input - BCLK
  configure_pin(PIN_BCLK, GPIO_MODE_INPUT);
  gpio_matrix_in(PIN_BCLK, I2S0O_BCK_IN_IDX, 0);
  // Input - WS
  configure_pin(PIN_WS, GPIO_MODE_INPUT);
  gpio_matrix_in(PIN_WS, I2S0O_WS_IN_IDX, 0);
  // Output - DOUT
  // Matrix signals [23:23-N+1], where N is bit width
  configure_pin(PIN_DOUT, GPIO_MODE_OUTPUT);
  gpio_matrix_out(PIN_DOUT, I2S0O_DATA_OUT23_IDX, 0, 0); 
};
