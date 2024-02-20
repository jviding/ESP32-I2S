#include "i2s.h"


void I2S::init() {
  enable_peripheral();
  enable_channel();
  enable_gpio();
  enable_fifo_dma();
};

void I2S::read() {
  fifo_dma_read();
};
