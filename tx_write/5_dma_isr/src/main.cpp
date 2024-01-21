#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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

void app_main() {
  I2S::init();
  while(1) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    I2S::write();
  }
};
