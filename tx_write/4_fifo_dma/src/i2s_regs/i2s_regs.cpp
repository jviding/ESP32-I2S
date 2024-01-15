#include "i2s_regs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/i2s_reg.h"

void I2S_Regs::print_I2S_FIFO_registers() {
  printf("\n*** I2S FIFO REGISTERS ***\n");
  printf("I2S_FIFO_WR_REG ........ (WO)\n");
  printf("I2S_FIFO_RD_REG ........ 0x%lx\n\n", READ_PERI_REG(REG_I2S_BASE(0) + 0x0004));
};

void I2S_Regs::print_configuration_registers() {
  printf("\n*** CONFIGURATION REGISTERS ***\n");
  printf("I2S_CONF_REG ........... 0x%lx\n", READ_PERI_REG(I2S_CONF_REG(0)));
  printf("I2S_CONF1_REG .......... 0x%lx\n", READ_PERI_REG(I2S_CONF1_REG(0)));
  printf("I2S_CONF2_REG .......... 0x%lx\n", READ_PERI_REG(I2S_CONF2_REG(0)));
  printf("I2S_TIMING_REG ......... 0x%lx\n", READ_PERI_REG(I2S_TIMING_REG(0)));
  printf("I2S_FIFO_CONF_REG ...... 0x%lx\n", READ_PERI_REG(I2S_FIFO_CONF_REG(0)));
  printf("I2S_CONF_SINGLE_DATA_REG 0x%lx\n", READ_PERI_REG(I2S_CONF_SIGLE_DATA_REG(0)));
  printf("I2S_CONF_CHAN_REG ...... 0x%lx\n", READ_PERI_REG(I2S_CONF_CHAN_REG(0)));
  printf("I2S_LC_HUNG_CONF_REG ... 0x%lx\n", READ_PERI_REG(I2S_LC_HUNG_CONF_REG(0)));
  printf("I2S_CLKM_CONF_REG ...... 0x%lx\n", READ_PERI_REG(I2S_CLKM_CONF_REG(0)));
  printf("I2S_SAMPLE_RATE_CONF_REG 0x%lx\n", READ_PERI_REG(I2S_SAMPLE_RATE_CONF_REG(0)));
  printf("I2S_PD_CONF_REG ........ 0x%lx\n", READ_PERI_REG(I2S_PD_CONF_REG(0)));
  printf("I2S_STATE_REG .......... 0x%lx\n\n", READ_PERI_REG(I2S_STATE_REG(0)));
};

void I2S_Regs::print_DMA_registers() {
  printf("\n*** DMA REGISTERS ***\n");
  printf("Not implemented\n\n");
};

void I2S_Regs::print_PDM_registers() {
  printf("\n*** PULSE DENSITY (DE) MODULATION REGISTERS ***\n");
  printf("Not implemented\n\n");
};

void I2S_Regs::print_interrupt_registers() {
  printf("\n*** INTERRUPT REGISTERS ***\n");
  printf("I2S_INT_RAW_REG ........ 0x%lx\n", READ_PERI_REG(I2S_INT_RAW_REG(0)));
  printf("I2S_INT_ST_REG ......... 0x%lx\n", READ_PERI_REG(I2S_INT_ST_REG(0)));
  printf("I2S_INT_ENA_REG ........ 0x%lx\n", READ_PERI_REG(I2S_INT_ENA_REG(0)));
  printf("I2S_INT_CLR_REG ........ (WO)\n\n");
};

void I2S_Regs::print_all() {
  print_I2S_FIFO_registers();
  print_configuration_registers();
  print_DMA_registers();
  print_PDM_registers();
  print_interrupt_registers();
};