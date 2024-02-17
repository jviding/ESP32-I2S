#include "i2s.h"
#include "driver/periph_ctrl.h"


void I2S::reset() {
  // Reset LC_CONF
  uint32_t lc_conf_reset_bits = I2S_AHBM_RST | I2S_AHBM_FIFO_RST | I2S_OUT_RST | I2S_IN_RST;
  SET_PERI_REG_MASK(I2S_LC_CONF_REG(0), lc_conf_reset_bits);
  CLEAR_PERI_REG_MASK(I2S_LC_CONF_REG(0), lc_conf_reset_bits);
  // Reset CONF
  uint32_t conf_reset_bits = I2S_RX_RESET | I2S_TX_RESET | I2S_RX_FIFO_RESET | I2S_TX_FIFO_RESET;
  SET_PERI_REG_MASK(I2S_CONF_REG(0), conf_reset_bits);
  CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), conf_reset_bits);
  // Reset outlink
  CLEAR_PERI_REG_MASK(I2S_OUT_LINK_REG(0), I2S_OUTLINK_RESTART);
  // Wait for reset complete
  uint32_t reset_status_bits = I2S_RX_FIFO_RESET_BACK_S | I2S_TX_FIFO_RESET_BACK_S;
  while ((READ_PERI_REG(I2S_FIFO_CONF_REG(0)) & reset_status_bits) != 0) {
    vTaskDelay(pdMS_TO_TICKS(10));
  }
};

void I2S::clear_defaults() {
  CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_RX_MONO | I2S_TX_MONO | I2S_RX_SHORT_SYNC | I2S_TX_SHORT_SYNC);
  CLEAR_PERI_REG_MASK(I2S_CONF2_REG(0), I2S_LCD_EN);
  CLEAR_PERI_REG_MASK(I2S_PDM_CONF_REG(0), I2S_PDM2PCM_CONV_EN | I2S_PCM2PDM_CONV_EN);
  CLEAR_PERI_REG_MASK(I2S_FIFO_CONF_REG(0), I2S_DSCR_EN);
};

void I2S::enable_peripheral() {
  // Enable the I2S hardware peripheral
  periph_module_enable(PERIPH_I2S0_MODULE);
  // Reset and clear
  reset();
  clear_defaults();
};
