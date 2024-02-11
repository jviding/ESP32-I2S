#include "gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "hal/gpio_hal.h"
#include "rom/gpio.h"


void Gpio::enable() {
  // Enable pins
  gpio_config_t gpio_conf = {
    .pin_bit_mask = 
      (1ULL << PIN_BCLK) | 
      (1ULL << PIN_WS) | 
      (1ULL << PIN_DOUT),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE,
  };
  gpio_config(&gpio_conf);
  // Enable GPIO Matrix
  gpio_matrix_out(PIN_BCLK, I2S0O_BCK_OUT_IDX,    0, 0);
  gpio_matrix_out(PIN_WS,   I2S0O_WS_OUT_IDX,     0, 0);
  // DOUT, signals [23:23-N+1], where N is bit width
  gpio_matrix_out(PIN_DOUT, I2S0O_DATA_OUT23_IDX, 0, 0);
};
