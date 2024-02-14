#pragma once

#include <stdint.h>
#include "driver/gpio.h"


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
  static const uint32_t sample_rate;

  // Functions - Gpio
  static void enable_pin(uint16_t gpio_num, gpio_mode_t gpio_mode);
  static void enable_gpio();

  // Functions
  static void conf_i2s();
};