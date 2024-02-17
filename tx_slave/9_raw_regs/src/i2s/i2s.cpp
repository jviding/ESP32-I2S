#include "i2s.h"
#include "../i2s_regs/i2s_regs.h"


void I2S::init() {
  enable_peripheral();
  enable_channel();
  enable_gpio();
  enable_FIFO_DMA();
};

void I2S::write() {
  // Data to send
  // - Written out per WS low
  // - Data value at BCK falling edge
  uint16_t samples[] = {
    0x5A00, 0x5A00, 0x5A00, 0x5A00, 0x5A00, 0x5A00, 0x5A00, 0x5A00,
    0x5A00, 0x5A00, 0x5A00, 0x5A00, 0x5A00, 0x5A00, 0x5A00, 0x5A00,
  };
  size_t rows = 2;
  size_t cols = 8;
  // Transmit data
  fifo_dma_send(rows, cols, samples);
  // Print registers
  I2S_Regs::print_all();
};
