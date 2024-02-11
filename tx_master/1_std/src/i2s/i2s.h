#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"

#define PIN_BCLK  GPIO_NUM_4
#define PIN_WS    GPIO_NUM_5
#define PIN_DOUT  GPIO_NUM_18


class I2S {
public:
  // Functions
  static void init();
  static void write();

private:
  // Variables
  static i2s_chan_handle_t tx_handle;
  static const i2s_std_config_t std_conf;
};