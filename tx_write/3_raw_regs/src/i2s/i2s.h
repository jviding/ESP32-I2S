#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/i2s_std.h"

#define PIN_BCLK  GPIO_NUM_4
#define PIN_WS    GPIO_NUM_5
#define PIN_DOUT  GPIO_NUM_18

class I2S {
public:
  // Functions
  static void init();
  static esp_err_t write();

private:
  // Variables
  static SemaphoreHandle_t tx_handle;

  // Functions
  static void enable();
  static void reset_and_clear_defaults();
  static void configure_clock();
  static void configure_channel();
  static void configure_gpio();
  static void configure_interrupts();

  static void fifo_set_data();
  static void fifo_send_data();
};