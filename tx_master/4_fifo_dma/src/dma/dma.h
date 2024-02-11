#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


class DMA {
public:
  // Functions
  static uintptr_t init(size_t num_samples, uint16_t samples[]);
  static void deinit();

private:
  // Types
  struct dw0_t;
  union dma_desc_t;

  // Variables
  static uint32_t* buffer;
  static dma_desc_t descriptor;

  // Functions
  static void create_buffer(size_t num_samples, uint16_t samples[]);
  static void create_descriptor(size_t num_samples);
};