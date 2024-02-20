#include "i2s.h"
#include "../dma/dma.h"
#include "../i2s_regs/i2s_regs.h"


SemaphoreHandle_t I2S::isr_handle;
intr_handle_t I2S::intr_handle;
uint32_t I2S::inlink_addr;


void I2S::start_inlink() {
  // Reset
  SET_PERI_REG_MASK(I2S_IN_LINK_REG(0), I2S_INLINK_RESTART);
  // Then start
  CLEAR_PERI_REG_MASK(I2S_IN_LINK_REG(0), I2S_INLINK_STOP);
  SET_PERI_REG_MASK(I2S_IN_LINK_REG(0), I2S_INLINK_START);
};

void I2S::stop_inlink() {
  SET_PERI_REG_MASK(I2S_IN_LINK_REG(0), I2S_INLINK_STOP);
  CLEAR_PERI_REG_MASK(I2S_IN_LINK_REG(0), I2S_INLINK_START);
};

void I2S::enable_fifo_dma_inlink() {
  // Allocate descritors & DMA buffers
  size_t num_buffers = 2;
  size_t len_buffers = 8;
  inlink_addr = (uint32_t)DMA::init(num_buffers, len_buffers);
  // Ensure inlink is stopped
  stop_inlink();
  // Set inlink address
  SET_PERI_REG_BITS(I2S_IN_LINK_REG(0), I2S_INLINK_ADDR, inlink_addr, I2S_INLINK_ADDR_S);
};

void I2S::enable_fifo_dma_interrupt() {
  // Set expected receive size, for RX EOF
  uint32_t num_words = 16;
  WRITE_PERI_REG(I2S_RXEOF_NUM_REG(0), num_words);
  // Enable RX EOF interrupt
  SET_PERI_REG_MASK(I2S_INT_ENA_REG(0), I2S_IN_SUC_EOF_INT_ENA); 
  SET_PERI_REG_MASK(I2S_INT_CLR_REG(0), I2S_IN_SUC_EOF_INT_CLR); 
  // Create ISR handle
  isr_handle = xSemaphoreCreateBinary();
};

void I2S::enable_fifo_dma() {
  // Enable FIFO DMA mode
  SET_PERI_REG_MASK(I2S_FIFO_CONF_REG(0), I2S_RX_FIFO_MOD_FORCE_EN);
  SET_PERI_REG_MASK(I2S_FIFO_CONF_REG(0), I2S_DSCR_EN);
  // Use DMA burst mode, for fast memory access
  // Occupies system bus during RX write, blocking CPU
  SET_PERI_REG_MASK(I2S_LC_CONF_REG(0), I2S_INDSCR_BURST_EN);
  SET_PERI_REG_MASK(I2S_LC_CONF_REG(0), I2S_OUT_DATA_BURST_EN);
  // Follow descriptors (linked list) without CPU
  SET_PERI_REG_MASK(I2S_RXEOF_NUM_REG(0), I2S_OUT_AUTO_WRBACK);
  // Enable inlink and RX interrupt
  enable_fifo_dma_inlink();
  enable_fifo_dma_interrupt();
};

void IRAM_ATTR I2S::fifo_dma_stop_ISR(void* arg) {
  // Stop inlink
  stop_inlink();
  // Clear interrupt
  SET_PERI_REG_MASK(I2S_INT_CLR_REG(0), I2S_IN_SUC_EOF_INT_CLR);
  // Signal ISR ready
  xSemaphoreGiveFromISR(isr_handle, NULL);
};

void I2S::fifo_dma_read() {
  // Enable ISR
  esp_intr_alloc(ETS_I2S0_INTR_SOURCE, 0, (intr_handler_t)I2S::fifo_dma_stop_ISR, nullptr, &intr_handle);
  // Start RX
  start_inlink();
  // Wait until RX EOF
  while (1) {
    // CPU non-blocking wait
    if (xSemaphoreTake(isr_handle, pdMS_TO_TICKS(10)) == pdTRUE) {
      // Release the ISR upon complete
      esp_intr_free(intr_handle);
      break;
    }
  };
  // Print descriptors & buffered data
  I2S_Regs::print_DMA_inlink(inlink_addr, true);
};
