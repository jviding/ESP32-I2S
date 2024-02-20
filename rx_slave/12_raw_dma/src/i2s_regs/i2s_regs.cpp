#include "i2s_regs.h"
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
  printf("I2S_LC_CONF_REG ........ 0x%lx\n", READ_PERI_REG(I2S_LC_CONF_REG(0)));
  printf("I2S_RXEOF_NUM_REG ...... 0x%lx\n", READ_PERI_REG(I2S_RXEOF_NUM_REG(0)));
  printf("I2S_OUT_LINK_REG ....... 0x%lx\n", READ_PERI_REG(I2S_OUT_LINK_REG(0)));
  printf("I2S_IN_LINK_REG ........ 0x%lx\n", READ_PERI_REG(I2S_IN_LINK_REG(0)));
  printf("Not implemented\n\n");
};

void I2S_Regs::print_PDM_registers() {
  printf("\n*** PULSE DENSITY (DE) MODULATION REGISTERS ***\n");
  printf("I2S_PDM_CONF_REG ....... 0x%lx\n", READ_PERI_REG(I2S_PDM_CONF_REG(0)));
  printf("I2S_PDM_FREQ_CONF_REG .. 0x%lx\n\n", READ_PERI_REG(I2S_PDM_FREQ_CONF_REG(0)));
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

uint16_t I2S_Regs::get_DW0_size(uint32_t dw0) {
  uint32_t mask = 0x00000FFF; // [11:0]
  return dw0 & mask;
};

uint16_t I2S_Regs::get_DW0_length(uint32_t dw0) {
  uint32_t mask = 0x00FFF000; // [23:11]
  return (dw0 & mask) >> 12;
};

void I2S_Regs::print_DMA_descriptor(uint32_t* ptr, uint16_t index) {
  printf("\n## DMA Descriptor [%d] ##\n", index);
  printf("DW0 ... 0x%lx\n", *ptr);
  printf("DW1 ... 0x%lx\n", *(ptr+1));
  printf("DW2 ... 0x%lx\n", *(ptr+2));
  printf("\n## DMA Descriptor - DW0 ##\n");
  uint16_t size = get_DW0_size(*ptr);
  uint16_t length = get_DW0_length(*ptr);
  printf("Size [11:0] ...... 0x%x = %d bytes\n", size, size);
  printf("Length [23:11] ... 0x%x = %d bytes\n\n", length, length);
};

void I2S_Regs::print_DMA_buffer(uint32_t* ptr, uint16_t index) {
  printf("\n## DMA Buffer [%d] ##\n", index);
  uint16_t length = get_DW0_length(*ptr);
  uint32_t* buff_ptr = (uint32_t*)(*(ptr + 1));
  for (uint16_t a = 0; a < length/4; a ++) {
    printf("Buffer[%d] ... 0x%08lx\n", a, *(buff_ptr + a));
  }
  printf("\n"); 
}

void I2S_Regs::print_DMA_inlink(uintptr_t outlink_addr, bool follow_links) {
  uint32_t* desc_ptr = (uint32_t*)outlink_addr;
  uint16_t index = 1;
  while (1) {
    print_DMA_descriptor(desc_ptr, index);
    print_DMA_buffer(desc_ptr, index);
    if (!!follow_links && (uint32_t)(*(desc_ptr+2)) != 0) {
      desc_ptr = (uint32_t*)((uint32_t)(*(desc_ptr+2)));
      index++;
    } else {
      break;
    }
  }
};
