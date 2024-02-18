#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"


#define PIN_BCLK  GPIO_NUM_4
#define PIN_WS    GPIO_NUM_5
#define PIN_DIN   GPIO_NUM_18


class I2S {
public:
  // Functions
  static void init();
  static void read();

private:
  // Functions
  static void enable_channel();
  static void enable_gpio();
};