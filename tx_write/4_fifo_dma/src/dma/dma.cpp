#include "dma.h"
#include "soc/i2s_reg.h"

/* Note:
 *  There's a mistake in the ESP32 datasheet.
 *  DW0 size and length have been flipped.
 *  Length is buffer size, and vice versa.
*/

// Size must be word-aligned
struct DMA::dw0_t {
  uint32_t size     : 12; // Bytes per DMA transfer
  uint32_t length   : 12; // Buffer size (bytes)
  uint32_t RESERVED : 6;  // Reserved bits
  uint32_t eof      : 1;  // 0: No EOF char - 1: At the end of the linked list
  uint32_t owner    : 1;  // 0: CPU         - 1: DMA Controller
};

// Buffer address (DW1) must be word-aligned
union DMA::dma_desc_t {
  uint32_t bytes[3];
  struct {
    DMA::dw0_t dw0;          // dw0 - Header
    uint32_t buff_addr_ptr;  // dw1 - Pointer to buffer address, must be word-aligned
    uint32_t next_desc_addr; // dw2 - Pointer to next descriptor address
  } values;
};


uint32_t* DMA::buffer;
DMA::dma_desc_t DMA::descriptor;


void DMA::create_buffer(size_t num_samples, uint16_t samples[]) {
  // Allocate from DMA capable memory, word-aligned (32 bits)
  buffer = (uint32_t*)heap_caps_malloc(num_samples, MALLOC_CAP_DMA | MALLOC_CAP_32BIT);
  // Write samples to buffer
  for (size_t i = 0; i < num_samples; i++) {
    buffer[i] = samples[i];
  }
};

void DMA::create_descriptor(size_t num_samples) {
  descriptor.values.dw0 = {
    .size   = 4 * num_samples,  // Bytes per DMA transfer
    .length = 4 * num_samples,  // Buffer size (bytes)
    .eof    = 1,                // End of linked list
    .owner  = 1,
  };
  descriptor.values.buff_addr_ptr  = (uint32_t)buffer;
  descriptor.values.next_desc_addr = 0; // No next descriptor
};

uintptr_t DMA::init(size_t num_samples, uint16_t samples[]) {
  create_buffer(num_samples, samples);
  create_descriptor(num_samples);
  // Return descriptor address
  return (uintptr_t)&descriptor;
};

void DMA::deinit() {
  heap_caps_free(buffer);
};
