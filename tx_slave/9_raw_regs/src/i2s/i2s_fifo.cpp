#include "i2s.h"
#include "../dma/dma.h"


SemaphoreHandle_t I2S::isr_handle;
intr_handle_t I2S::intr_handle;


void I2S::enable_FIFO_DMA() {
  // Enable FIFO DMA mode
  SET_PERI_REG_MASK(I2S_FIFO_CONF_REG(0), I2S_TX_FIFO_MOD_FORCE_EN);
  SET_PERI_REG_MASK(I2S_FIFO_CONF_REG(0), I2S_DSCR_EN);
  // Use DMA in burst mode for fast transfer
  // Occupies system bus during TX, blocking CPU
  SET_PERI_REG_MASK(I2S_LC_CONF_REG(0), I2S_OUT_DATA_BURST_EN);
  SET_PERI_REG_MASK(I2S_LC_CONF_REG(0), I2S_OUTDSCR_BURST_EN);
  // Follow descriptors (linked list) without CPU
  SET_PERI_REG_MASK(I2S_RXEOF_NUM_REG(0), I2S_OUT_AUTO_WRBACK);
  // EOF when DMA has popped all data from the FIFO
  SET_PERI_REG_MASK(I2S_LC_CONF_REG(0), I2S_OUT_EOF_MODE);
  // Enable the EOF interrupt
  uint32_t tx_interrupts = I2S_OUT_EOF_INT_ENA;
  SET_PERI_REG_MASK(I2S_INT_ENA_REG(0), tx_interrupts);
  // Create ISR handle
  isr_handle = xSemaphoreCreateBinary();
  // Ensure outlink is stopped and clear INT
  SET_PERI_REG_MASK(I2S_OUT_LINK_REG(0), I2S_OUTLINK_STOP);
  SET_PERI_REG_MASK(I2S_INT_CLR_REG(0), I2S_OUT_EOF_INT_ENA);
};

void IRAM_ATTR I2S::fifo_dma_send_ISR(void* arg) {
  // Stop
  SET_PERI_REG_MASK(I2S_OUT_LINK_REG(0), I2S_OUTLINK_STOP);
  CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_OUTLINK_START);
  // Clear interrupt
  SET_PERI_REG_MASK(I2S_INT_CLR_REG(0), I2S_OUT_EOF_INT_ENA);
  // Signal ISR ready
  xSemaphoreGiveFromISR(isr_handle, NULL);
};

void I2S::fifo_dma_send(size_t rows, size_t cols, uint16_t samples[]) {
  // Allocate buffer and descriptors, set outlink address
  uint32_t outlink_addr = (uint32_t)DMA::init(rows, cols, samples);
  SET_PERI_REG_BITS(I2S_OUT_LINK_REG(0), I2S_OUTLINK_ADDR, outlink_addr, I2S_OUTLINK_ADDR_S);
  // Set interrupt handler
  esp_intr_alloc(ETS_I2S0_INTR_SOURCE, 0, (intr_handler_t)I2S::fifo_dma_send_ISR, nullptr, &intr_handle);
  // Start
  CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_OUTLINK_STOP);
  SET_PERI_REG_MASK(I2S_OUT_LINK_REG(0), I2S_OUTLINK_START);
  while (1) {
    // CPU non-blocking wait
    // Wait until send complete (EOF)
    if (xSemaphoreTake(isr_handle, pdMS_TO_TICKS(10)) == pdTRUE) {
      esp_intr_free(intr_handle);
      break;
    }
  };
  // Free allocated memory
  DMA::deinit();
};
