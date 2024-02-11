#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class I2S_Regs {
public:
  // Functions
  static void print_all();
  static void print_I2S_FIFO_registers();
  static void print_configuration_registers();
  static void print_DMA_registers();
  static void print_PDM_registers();
  static void print_interrupt_registers();

  static void print_DMA_outlink(uintptr_t outlink_addr);

private:
  // Functions
  static uint16_t get_DW0_size(uint32_t dw0);
  static uint16_t get_DW0_length(uint32_t dw0);
  static void print_DMA_descriptor(uint32_t* ptr);
  static void print_DMA_buffer(uint32_t* ptr);
};
