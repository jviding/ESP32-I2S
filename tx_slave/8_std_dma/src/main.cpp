#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "i2s/i2s.h"
#include "driver/uart.h"

uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};

extern "C" {
  void app_main(void);
};

/*
 * Note: Memory is not properly aligned on the I2S interface.
 * To verify transmission with an oscilloscope, use trigger on the data channel.
 * Because there might be some "empty" transmissions...
*/
void app_main() {
  I2S::init();
  uint32_t i = 1;
  while(1) {
    vTaskDelay(pdMS_TO_TICKS(200));
    I2S::write();
    printf("Wrote %ld\n", i);
    fflush(stdout);
    i++;
  }
};
