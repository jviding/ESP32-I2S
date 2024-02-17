#pragma once

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/i2s.h"


#define PIN_BCLK  GPIO_NUM_4
#define PIN_WS    GPIO_NUM_5
#define PIN_DOUT  GPIO_NUM_18


class I2S {
public:
  // Functions
  static void init();
  static void write();

private:
  // Variables - Conf
  static const uint32_t sample_rate;
  // Functions - Conf
  static void conf_i2s();

  // Functions - Gpio
  static void enable_pin(uint16_t gpio_num, gpio_mode_t gpio_mode);
  static void enable_gpio();

  // Variables
  static SemaphoreHandle_t isr_handle;
  static intr_handle_t intr_handle;
  // Functions
  static void enable_FIFO_DMA();
  static void IRAM_ATTR fifo_dma_stop_ISR(void* arg);
  static void fifo_dma_send_data(size_t rows, size_t cols, uint16_t samples[]);
};
