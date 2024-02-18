#include "i2s.h"


void I2S::init() {
  enable_channel();
  enable_gpio();
};

void I2S::read() {
  // RX read
  size_t bytes_was_read;
  size_t bytes_to_read = 16;
  uint8_t buff[bytes_to_read * 2];
  i2s_read(I2S_NUM_0, buff, bytes_to_read, &bytes_was_read, portMAX_DELAY);
  // Print data
  for (int i = 0; i < 2; i++) {
    for (int a = 0; a < 8; a++) {
      printf("%d ", buff[i*8 + a]);
    }
    printf("\n");
  };
  printf("\n");
};
