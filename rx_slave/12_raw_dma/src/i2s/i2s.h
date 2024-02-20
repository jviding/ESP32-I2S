#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/i2s.h"
#include "soc/i2s_reg.h"


#define PIN_BCLK  GPIO_NUM_4
#define PIN_WS    GPIO_NUM_5
#define PIN_DIN   GPIO_NUM_18


class I2S {
public:
  // Functions
  static void init();
  static void read();

private:
  // Functions - Periph
  static void reset();
  static void clear_defaults();
  static void enable_peripheral();

  // Functions - Chan
  static void configure_clock();
  static void configure_channel();
  static void enable_channel();

  // Functions - Gpio
  static void configure_pin(uint16_t gpio_num, gpio_mode_t gpio_mode);
  static void enable_gpio();

  // Variables - Fifo
  static SemaphoreHandle_t isr_handle;
  static intr_handle_t intr_handle;
  static uint32_t inlink_addr;
  // Functions - Fifo
  static void start_inlink();
  static void stop_inlink();
  static void enable_fifo_dma_inlink();
  static void enable_fifo_dma_interrupt();
  static void enable_fifo_dma();
  static void IRAM_ATTR fifo_dma_stop_ISR(void* arg);
  static void fifo_dma_read();
};