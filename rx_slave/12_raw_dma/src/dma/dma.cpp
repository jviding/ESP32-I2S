#include "dma.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
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


uint32_t** DMA::buffers;
uint32_t DMA::num_buffers = 0;
DMA::dma_desc_t* DMA::descriptors;


void DMA::fill_buffers(size_t rows, size_t cols) {
  for (size_t a = 0; a < rows; a++) {
    uint32_t* buffer = buffers[a];
    for (size_t b = 0; b < cols; b++ ) {
      buffer[b] = 0;
    }
  }
};

void DMA::create_buffers(size_t rows, size_t cols) {
  // Allocate memory for buffer pointers
  buffers = (uint32_t**)malloc(rows * sizeof(uint32_t));
  // Allocate buffers from DMA capable memory, word aligned (32 bits)
  for (size_t i = 0; i < rows; i++) {
    uint32_t* buffer = (uint32_t*)heap_caps_malloc(cols * sizeof(uint32_t), MALLOC_CAP_DMA | MALLOC_CAP_32BIT);
    buffers[i] = buffer;
    num_buffers++;
  }
  fill_buffers(rows, cols);
};

DMA::dma_desc_t DMA::create_descriptor(size_t row, size_t cols, bool eof) {
  dma_desc_t desc;
  desc.values.dw0 = {
    .size   = cols * 4, // 4 bytes per sample
    .length = cols * 4, // 4 bytes per sample
    .eof    = eof,
    .owner  = 1,
  };
  desc.values.buff_addr_ptr = (uint32_t)buffers[row];
  desc.values.next_desc_addr = 0; // Linked later
  return desc;
};

void DMA::link_descriptors(size_t rows) {
  // Last one is not linked (= rows - 1)
  for (size_t i = 0; i < rows - 1; i++) {
    uint32_t next_desc_addr = (uint32_t)&descriptors[i + 1];
    descriptors[i].values.next_desc_addr = next_desc_addr;
  }
};

void DMA::create_descriptors(size_t rows, size_t cols) {
  // Allocate memory from DMA capable memory, word aligned (32 bits)
  descriptors = (dma_desc_t*)heap_caps_malloc(rows * sizeof(dma_desc_t), MALLOC_CAP_DMA | MALLOC_CAP_32BIT);
  // Create and prepopulate descriptors
  for (size_t a = 0; a < rows; a++) {
    // EOF = 1, if last
    uint32_t eof = (a == rows - 1 ? 1 : 0);
    // Create descriptor
    dma_desc_t desc = create_descriptor(a, cols, eof);
    // Add to buffer
    descriptors[a] = desc;    
  }
  link_descriptors(rows);
};

uintptr_t DMA::init(size_t rows, size_t cols) {
  create_buffers(rows, cols);
  create_descriptors(rows, cols);
  // Return descriptor address
  return (uintptr_t)&descriptors[0];
};

void DMA::deinit() {
  for (uint16_t a = 0; a < num_buffers; a++) {
    heap_caps_free(buffers[a]);
  }
  free(buffers);
  num_buffers = 0;
  heap_caps_free(descriptors);
};
