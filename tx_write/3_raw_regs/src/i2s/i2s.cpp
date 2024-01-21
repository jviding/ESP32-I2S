#include "i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "hal/gpio_hal.h"
#include "rom/gpio.h"
#include "soc/i2s_reg.h"
#include "driver/periph_ctrl.h"


SemaphoreHandle_t I2S::tx_handle;

void I2S::enable() {
  // Enable the i2s peripheral
  periph_module_enable(PERIPH_I2S0_MODULE);
  // Create semaphore for TX
  tx_handle = xSemaphoreCreateBinary();
  // Enable the semaphore for use
  xSemaphoreGive(tx_handle);
};

void I2S::reset_and_clear_defaults() {
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
  // Clear some defaults
  CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_RX_MONO | I2S_TX_MONO | I2S_RX_SHORT_SYNC | I2S_TX_SHORT_SYNC);
  CLEAR_PERI_REG_MASK(I2S_CONF2_REG(0), I2S_LCD_EN);
  CLEAR_PERI_REG_MASK(I2S_PDM_CONF_REG(0), I2S_PDM2PCM_CONV_EN | I2S_PCM2PDM_CONV_EN);
  CLEAR_PERI_REG_MASK(I2S_FIFO_CONF_REG(0), I2S_DSCR_EN);
};

void I2S::configure_clock() {
  // I2S PERIPHERAL CLOCK
  // Base Clock = (ESP32 System Clock / (C + A/C)) * B
  uint16_t div_a = 1;     // A = div_a
  SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(0), I2S_CLKM_DIV_A, div_a, I2S_CLKM_DIV_A_S);
  uint16_t div_b = 0;     // B = (1 << div_b)
  SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(0), I2S_CLKM_DIV_B, div_b, I2S_CLKM_DIV_B_S);
  uint16_t div_num = 79;  // C = (div_num + 8)
  SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(0), I2S_CLKM_DIV_NUM, div_num, I2S_CLKM_DIV_NUM_S);
  // BIT CLOCK
  // Sample Rate = Base Clock / div_clk
  uint16_t div_clk = 2;   // In Master mode >=2 because 1 BCLK tick takes 2 Base Clock ticks
  SET_PERI_REG_BITS(I2S_SAMPLE_RATE_CONF_REG(0), I2S_TX_BCK_DIV_NUM, div_clk, I2S_TX_BCK_DIV_NUM_S);
};

void I2S::configure_channel() {
  /*
  * Data is sent as samples of size uint8_t.      -> 0xDD
  * But written to the FIFO buffer as uint16_t.   -> 0xDD00
  * The internal TX buffer drops the low 8 bits.  -> 0xDD--
  */
  uint16_t sample_size = 8;
  SET_PERI_REG_BITS(I2S_SAMPLE_RATE_CONF_REG(0), I2S_TX_BITS_MOD, sample_size, I2S_TX_BITS_MOD_S);
  /*
  * FIFO buffer is 32 bits.
  * Single channel data: Each write is copied twice to the TX buffer.
  * 1) Bits [32:16] are copied as Left-channel data, where [15:0] are zero.
  * 2) Bits [15:0] are copied as Right-channel data, where [32:16] are zero.
  * 
  * Dual channel data: Each write is copied only once.
  * Writing uint16_t sets bits [15:0], while [32:16] will remain zero.
  * This means all the data will now be copied as Right-channel data.
  */
  uint16_t fifo_mode = 0;  // 16-bit dual channel data
  SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(0), I2S_TX_FIFO_MOD, fifo_mode, I2S_TX_FIFO_MOD_S); 
  uint16_t frame_size = 1; // Send 1 data sample per frame (WS cycle)
  SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(0), I2S_TX_DATA_NUM, frame_size, I2S_TX_DATA_NUM_S);
  // Enable FIFO
  SET_PERI_REG_MASK(I2S_FIFO_CONF_REG(0), I2S_TX_FIFO_MOD_FORCE_EN); 
  /*
  * Channel mode: Stereo - Data samples are sent during both WS low and high.
  * Channel mode: Mono - Data samples are sent during either WS low or high.
  */
  uint16_t channel_mode = 4; // Mono, choose Left-channel (WS low)
  SET_PERI_REG_BITS(I2S_CONF_CHAN_REG(0), I2S_TX_CHAN_MOD, channel_mode, I2S_TX_CHAN_MOD_S);
  /*
  * There's now Right-channel data, but transmission uses the Left-channel.
  * The channels can be swapped, so that Right-channel data is read from the Left.
  */
  SET_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_MSB_RIGHT); 
};

void I2S::configure_gpio() {
  // Enable pins
  gpio_config_t gpio_conf = {
    .pin_bit_mask = (1ULL << PIN_BCLK) | (1ULL << PIN_WS) | (1ULL << PIN_DOUT),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE,
  };
  gpio_config(&gpio_conf);
  // Enable GPIO Matrix
  gpio_matrix_out(PIN_BCLK, I2S0O_BCK_OUT_IDX, 0, 0);
  gpio_matrix_out(PIN_WS,   I2S0O_WS_OUT_IDX, 0, 0);
  // DOUT, signals [23:23-N+1], where N is bit width
  gpio_matrix_out(PIN_DOUT, I2S0O_DATA_OUT23_IDX, 0, 0);
};

void I2S::configure_interrupts() {
  // Enable TX interrupts
  uint32_t tx_interrupts = I2S_TX_REMPTY_INT_ENA | I2S_TX_WFULL_INT_ENA;
  SET_PERI_REG_MASK(I2S_INT_ENA_REG(0), tx_interrupts);
};

void I2S::init() {
  enable();
  reset_and_clear_defaults();
  configure_clock();
  configure_channel();
  configure_gpio();
  configure_interrupts();
};

void I2S::fifo_set_data() {
  // Allocate buffer
  size_t buffer_size = 4;
  uint16_t *buffer = (uint16_t *)malloc(buffer_size);
  // Fill with data
  buffer[0] = 0x0100; // Second byte for alignment
  buffer[1] = 0x0300; // Dropped by the TX buffer
  buffer[2] = 0x0500;
  buffer[3] = 0x0700;
  // Copy to FIFO TX buffer
  for (size_t i = 0; i < buffer_size; i++) {
    WRITE_PERI_REG(REG_I2S_BASE(0), buffer[i]);
  } 
  free(buffer);  
};

void I2S::fifo_send_data() {
  // TX start
  SET_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_START);
  // Wait until TX empty
  while (REG_GET_FIELD(I2S_INT_ST_REG(0), I2S_TX_REMPTY_INT_ST) == 0) {
    ; // Interrupt not raised yet
  }
  // TX stop
  CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_START);
  // Clear interrupt
  SET_PERI_REG_MASK(I2S_INT_CLR_REG(0), I2S_TX_REMPTY_INT_CLR);
}

esp_err_t I2S::write() {
  // Acquire lock
  if (xSemaphoreTake(tx_handle, pdMS_TO_TICKS(200)) != pdTRUE) {
    return ESP_ERR_TIMEOUT;
  }
  // Transmit data
  fifo_set_data();
  fifo_send_data();
  // Transmits byte per WS low:
  // ..., 0x05, 0x07, 0x01, 0x03, ...
  // Release lock
  xSemaphoreGive(tx_handle);
  return ESP_OK;
};