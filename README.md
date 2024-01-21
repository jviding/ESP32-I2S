# ESP32-I2S

This library digs deep into the ESP32 I2S peripheral.
The purpose of this is to understand its functions in detail.

Both RX and TX directories hold several, ordered, subprojects.
Each subproject builds on top of the previous, going deeper.

Scope is recommended, to be able to observe the signals.


# TX

## 1_std

Kick-off with a minimalistic configuration, using the I2S high-level API.
The purpose is to verify the I2S TX transmitter produces output.

## 2_std_mod

Rewriting some default configurations, still with the high-level API.
The purpose is to observe how the different parameters influence output.

## 3_raw_regs

Switching from the high-level API to low-level API.
Implementation of the previous functionality with direct register access.
FIFO mode, with manual GPIO Mux configuration.

## 4_fifo_dma

Extending from FIFO to FIFO DMA (burst) mode.
Uses a single DMA descriptor and buffer.

## 5_dma_isr

Extending FIFO DMA with linked buffers.


# RX
