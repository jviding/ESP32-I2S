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

private:
  // Functions
};