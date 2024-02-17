#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


class DMA {
public:
  // Functions
  static uintptr_t init(size_t rows, size_t cols, uint16_t samples[]);
  static void deinit();

private:
  // Types
  struct dw0_t;
  union dma_desc_t;

  // Variables
  static uint32_t** buffers;
  static uint32_t num_buffers;
  static dma_desc_t* descriptors;

  // Functions
  static void fill_buffers(size_t rows, size_t cols, uint16_t samples[]);
  static void create_buffers(size_t rows, size_t cols, uint16_t samples[]);
  static dma_desc_t create_descriptor(size_t row, size_t cols, bool eof);
  static void link_descriptors(size_t rows);
  static void create_descriptors(size_t rows, size_t cols);
};