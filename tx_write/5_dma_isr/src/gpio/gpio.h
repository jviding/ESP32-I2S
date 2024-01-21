#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PIN_BCLK  GPIO_NUM_4
#define PIN_WS    GPIO_NUM_5
#define PIN_DOUT  GPIO_NUM_18

class Gpio {
public:
  // Functions
  static void enable();
};