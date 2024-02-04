#include "i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "hal/gpio_hal.h"
#include "rom/gpio.h"
#include "soc/i2s_reg.h"
#include "driver/periph_ctrl.h"
#include "../gpio/gpio.h"
#include "../dma/dma.h"
#include "../i2s_regs/i2s_regs.h"


SemaphoreHandle_t I2S::tx_handle;
SemaphoreHandle_t I2S::isr_handle;
intr_handle_t I2S::intr_handle;


void I2S::reset() {
  // Reset LC_CONF
  uint32_t lc_conf_reset_bits = I2S_AHBM_RST | I2S_AHBM_FIFO_RST | I2S_OUT_RST | I2S_IN_RST;
  SET_PERI_REG_MASK(I2S_LC_CONF_REG(0), lc_conf_reset_bits);
  CLEAR_PERI_REG_MASK(I2S_LC_CONF_REG(0), lc_conf_reset_bits);
  // Reset CONF
  uint32_t conf_reset_bits = I2S_RX_RESET | I2S_TX_RESET | I2S_RX_FIFO_RESET | I2S_TX_FIFO_RESET;
  SET_PERI_REG_MASK(I2S_CONF_REG(0), conf_reset_bits);
  CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), conf_reset_bits);
  // Wait for reset complete
  uint32_t reset_status_bits = I2S_RX_FIFO_RESET_BACK_S | I2S_TX_FIFO_RESET_BACK_S;
  while ((READ_PERI_REG(I2S_FIFO_CONF_REG(0)) & reset_status_bits) != 0) {
    vTaskDelay(pdMS_TO_TICKS(10));
  }
};

void I2S::enable() {
  // Enable the i2s peripheral
  periph_module_enable(PERIPH_I2S0_MODULE);
  // Create semaphores for TX and ISR
  tx_handle = xSemaphoreCreateBinary();
  isr_handle = xSemaphoreCreateBinary();
  // Enable the semaphore for use
  xSemaphoreGive(tx_handle);
  // Reset interfaces
  reset();
};

void I2S::clear_defaults() {
  CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_RX_MONO | I2S_TX_MONO | I2S_RX_SHORT_SYNC | I2S_TX_SHORT_SYNC);
  CLEAR_PERI_REG_MASK(I2S_CONF2_REG(0), I2S_LCD_EN);
  CLEAR_PERI_REG_MASK(I2S_PDM_CONF_REG(0), I2S_PDM2PCM_CONV_EN | I2S_PCM2PDM_CONV_EN);
  CLEAR_PERI_REG_MASK(I2S_FIFO_CONF_REG(0), I2S_DSCR_EN);
};

void I2S::configure_clock() {
  // I2S PERIPHERAL CLOCK
  // Base Clock = ESP32 System Clock / (n + b/a)
  uint16_t a = 1;
  SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(0), I2S_CLKM_DIV_A, a, I2S_CLKM_DIV_A_S);
  uint16_t b = 0; 
  SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(0), I2S_CLKM_DIV_B, b, I2S_CLKM_DIV_B_S);
  uint16_t n = 79;
  SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(0), I2S_CLKM_DIV_NUM, n, I2S_CLKM_DIV_NUM_S);
  // BIT CLOCK
  // Sample Rate = Base Clock / div_clk
  uint16_t div_clk = 2;   // In Master mode >=2 because 1 BCLK tick takes 2 Base Clock ticks
  SET_PERI_REG_BITS(I2S_SAMPLE_RATE_CONF_REG(0), I2S_TX_BCK_DIV_NUM, div_clk, I2S_TX_BCK_DIV_NUM_S);
};

void I2S::configure_channel() {
  // Sample size
  uint16_t sample_size = 8; // 8-bit samples
  SET_PERI_REG_BITS(I2S_SAMPLE_RATE_CONF_REG(0), I2S_TX_BITS_MOD, sample_size, I2S_TX_BITS_MOD_S);
  // FIFO mode
  uint16_t fifo_mode = 0;   // 16-bit dual channel data
  SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(0), I2S_TX_FIFO_MOD, fifo_mode, I2S_TX_FIFO_MOD_S); 
  uint16_t frame_size = 1;  // Send 1 data sample per frame (WS cycle)
  SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(0), I2S_TX_DATA_NUM, frame_size, I2S_TX_DATA_NUM_S);
  // Enable FIFO
  SET_PERI_REG_MASK(I2S_FIFO_CONF_REG(0), I2S_TX_FIFO_MOD_FORCE_EN);  
  // Channel mode
  uint16_t channel_mode = 4; // Mono, choose Left-channel (WS low)
  SET_PERI_REG_BITS(I2S_CONF_CHAN_REG(0), I2S_TX_CHAN_MOD, channel_mode, I2S_TX_CHAN_MOD_S);
  // Swap Right and Left channels
  SET_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_MSB_RIGHT);
};

void I2S::configure_FIFO_DMA() {
  // Enable FIFO DMA mode
  SET_PERI_REG_MASK(I2S_FIFO_CONF_REG(0), I2S_DSCR_EN);
  // Use DMA in burst mode for fast transfer
  // Occupies system bus during TX, blocking CPU
  SET_PERI_REG_MASK(I2S_LC_CONF_REG(0), I2S_OUT_DATA_BURST_EN);
  SET_PERI_REG_MASK(I2S_LC_CONF_REG(0), I2S_OUTDSCR_BURST_EN);
  // Generate I2S_OUT_EOF_INT when DMA has popped all data from the FIFO
  SET_PERI_REG_MASK(I2S_LC_CONF_REG(0), I2S_OUT_EOF_MODE);
  // Follow descriptors (linked list) without CPU
  SET_PERI_REG_MASK(I2S_RXEOF_NUM_REG(0), I2S_OUT_AUTO_WRBACK);
};

void I2S::configure_gpio() {
  Gpio::enable();
};

void I2S::configure_DMA_interrupts() {
  // Enable TX interrupts
  uint32_t tx_interrupts = I2S_OUT_EOF_INT_ENA;
  SET_PERI_REG_MASK(I2S_INT_ENA_REG(0), tx_interrupts);
};

void I2S::init() {
  enable();
  clear_defaults();
  configure_clock();
  configure_channel();
  configure_FIFO_DMA();
  configure_gpio();
  configure_DMA_interrupts();
};

void I2S::fifo_dma_set_data(size_t rows, size_t cols, uint16_t samples[]) {
  // Allocate DMA buffer and create descriptor
  uint32_t outlink_addr = (uint32_t)DMA::init(rows, cols, samples);
  // Set outlink address
  SET_PERI_REG_BITS(I2S_OUT_LINK_REG(0), I2S_OUTLINK_ADDR, outlink_addr, I2S_OUTLINK_ADDR_S);
  // Print descriptors & buffers
  //I2S_Regs::print_DMA_outlink(outlink_addr, true);
};

void I2S::fifo_dma_start() {
  // Start DMA
  CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_OUTLINK_STOP);
  SET_PERI_REG_MASK(I2S_OUT_LINK_REG(0), I2S_OUTLINK_START);
  // Start TX transmitter
  SET_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_START);
};

void IRAM_ATTR I2S::fifo_dma_stop_ISR(void* arg) {
  // Stop TX transmitter
  CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_START);
  // Stop DMA
  SET_PERI_REG_MASK(I2S_OUT_LINK_REG(0), I2S_OUTLINK_STOP);
  CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_OUTLINK_START);
  // Clear interrupt
  SET_PERI_REG_MASK(I2S_INT_CLR_REG(0), I2S_OUT_EOF_INT_ENA);
  // Signal ISR ready
  xSemaphoreGiveFromISR(isr_handle, NULL);
};

void I2S::fifo_dma_send_data() {
  // Stop transmit upon EOF interrupt
  esp_intr_alloc(ETS_I2S0_INTR_SOURCE, 0, (intr_handler_t)I2S::fifo_dma_stop_ISR, nullptr, &intr_handle);
  // Start transmit
  fifo_dma_start();
  // Wait until ISR signals complete
  while (1) {
    // CPU non-blocking wait
    if (xSemaphoreTake(isr_handle, pdMS_TO_TICKS(10)) == pdTRUE) {
      break;
    }
  };
  // Release the ISR
  esp_intr_free(intr_handle);
  // Print INT registers
  //I2S_Regs::print_interrupt_registers();
  // Free DMA buffers
  DMA::deinit();
};

esp_err_t I2S::write() {
  // Acquire lock
  if (xSemaphoreTake(tx_handle, pdMS_TO_TICKS(200)) != pdTRUE) {
    return ESP_ERR_TIMEOUT;
  }
  // Data to send
  uint16_t samples[] = {            // Buffer // Written out per WS low, in order:
    0x0100, 0x0200, 0x0300, 0x0400, //  #1    //  5 -  6 -  7 -  8
    0x0500, 0x0600, 0x0700, 0x0800, //  #2    //  9 - 10 - 11 - 12
    0x0900, 0x0A00, 0x0B00, 0x0C00, //  #3    // 13 - 14 - 15 - 16
    0x0D00, 0x0E00, 0x0F00, 0x1000, //  #4    //  1 -  2 -  3 -  4
  };
  size_t rows = 4;
  size_t cols = 4;
  // Transmit data
  fifo_dma_set_data(rows, cols, samples);
  fifo_dma_send_data();
  // Release lock
  xSemaphoreGive(tx_handle);
  return ESP_OK;
};
