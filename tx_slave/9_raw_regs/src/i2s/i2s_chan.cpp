#include "i2s.h"


void I2S::configure_clock() {
  /*
  * I2S PERIPHERAL CLOCK
  * Base Clock = ESP32 System Clock (80MHz) / (n + b/a)
  * 
  * In Slave mode, should be >= 8 * Master BCLK
  * Here, Master BCLK is assumed 1 MHz
  * 
  * => Set Base Clock to 8 MHz
  */
  uint16_t a = 1;
  SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(0), I2S_CLKM_DIV_A, a, I2S_CLKM_DIV_A_S);
  uint16_t b = 0;
  SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(0), I2S_CLKM_DIV_B, b, I2S_CLKM_DIV_B_S);
  uint16_t n = 10;
  SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(0), I2S_CLKM_DIV_NUM, n, I2S_CLKM_DIV_NUM_S);
  /*
  * BIT CLOCK
  * Sample Rate = Base Clock / div_clk
  * 
  * => Set BCLK to 1 MHz
  */
  uint16_t div_clk = 8;
  SET_PERI_REG_BITS(I2S_SAMPLE_RATE_CONF_REG(0), I2S_TX_BCK_DIV_NUM, div_clk, I2S_TX_BCK_DIV_NUM_S);
};

void I2S::configure_channel() {
  uint16_t sample_size = 8;
  SET_PERI_REG_BITS(I2S_SAMPLE_RATE_CONF_REG(0), I2S_TX_BITS_MOD, sample_size, I2S_TX_BITS_MOD_S);
  uint16_t fifo_mode = 0;    // 16-bit dual channel data
  SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(0), I2S_TX_FIFO_MOD, fifo_mode, I2S_TX_FIFO_MOD_S); 
  uint16_t frame_size = 1;   // Send 1 data sample per frame (WS cycle)
  SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(0), I2S_TX_DATA_NUM, frame_size, I2S_TX_DATA_NUM_S);
  uint16_t channel_mode = 4; // Mono, choose Left-channel (WS low)
  SET_PERI_REG_BITS(I2S_CONF_CHAN_REG(0), I2S_TX_CHAN_MOD, channel_mode, I2S_TX_CHAN_MOD_S);
  // Use right channel
  SET_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_MSB_RIGHT);
};

void I2S::enable_channel() {
  configure_clock();
  configure_channel();
  // Enable TX Slave mode
  SET_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_SLAVE_MOD);
  // Start
  SET_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_START);
};
