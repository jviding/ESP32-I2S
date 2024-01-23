#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/i2s_std.h"


class I2S {
public:
  // Functions
  static void init();
  static esp_err_t write();

private:
  // Variables
  static SemaphoreHandle_t tx_handle;
  static SemaphoreHandle_t isr_handle;
  static intr_handle_t intr_handle;

  // Functions
  static void reset();
  static void enable();
  static void clear_defaults();
  static void configure_clock();
  static void configure_channel();
  static void configure_FIFO_DMA();
  static void configure_gpio();
  static void configure_DMA_interrupts();

  static void fifo_dma_set_data(size_t rows, size_t cols, uint16_t samples[]);
  static void fifo_dma_start();
  static void IRAM_ATTR fifo_dma_stop_ISR(void* arg);
  static void fifo_dma_send_data();
};