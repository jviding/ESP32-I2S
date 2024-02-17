#include "i2s.h"
#include "soc/i2s_reg.h"
#include "../dma/dma.h"


SemaphoreHandle_t I2S::isr_handle;
intr_handle_t I2S::intr_handle;


void I2S::enable_FIFO_DMA() {
  /*
  * Already enabled and started by the i2s library.
  * Sets also the DMA TX EOF interrupt.
  *  
  * So, instead, stop outlink and clear the interrupt.
  * This will release it for the use here.
  */
  SET_PERI_REG_MASK(I2S_OUT_LINK_REG(0), I2S_OUTLINK_STOP);
  SET_PERI_REG_MASK(I2S_INT_CLR_REG(0), I2S_OUT_EOF_INT_ENA);
  // Create ISR handle
  isr_handle = xSemaphoreCreateBinary();
};

void IRAM_ATTR I2S::fifo_dma_stop_ISR(void* arg) {
  // Stop DMA
  SET_PERI_REG_MASK(I2S_OUT_LINK_REG(0), I2S_OUTLINK_STOP);
  CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_OUTLINK_START);
  // Clear interrupt
  SET_PERI_REG_MASK(I2S_INT_CLR_REG(0), I2S_OUT_EOF_INT_ENA);
  // Signal ISR ready
  xSemaphoreGiveFromISR(isr_handle, NULL);
};

void I2S::fifo_dma_send_data(size_t rows, size_t cols, uint16_t samples[]) {
  // Allocate buffers and descriptors, set the outlink address
  uint32_t outlink_addr = (uint32_t)DMA::init(rows, cols, samples);
  SET_PERI_REG_BITS(I2S_OUT_LINK_REG(0), I2S_OUTLINK_ADDR, outlink_addr, I2S_OUTLINK_ADDR_S);
  // Set interrupt handler
  esp_intr_alloc(ETS_I2S0_INTR_SOURCE, 0, (intr_handler_t)I2S::fifo_dma_stop_ISR, nullptr, &intr_handle);
  // Start outlink for transmit
  CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_OUTLINK_STOP);
  SET_PERI_REG_MASK(I2S_OUT_LINK_REG(0), I2S_OUTLINK_START);
  while (1) {
    // CPU non-blocking wait
    // Until ISR signals completetion
    if (xSemaphoreTake(isr_handle, pdMS_TO_TICKS(10)) == pdTRUE) {
      // Release the ISR upon complete
      esp_intr_free(intr_handle);
      break;
    }
  };
  // Free the allocated buffers
  DMA::deinit();
};
