# ESP32-I2S

This library digs deep into the ESP32 I2S peripheral.
The purpose of this is to understand its functions in detail.

Both RX and TX directories hold several, ordered, subprojects.
Each subproject builds on top of the previous, going deeper.

Scope is recommended, to be able to observe the signals.


# TX Master

## 1_std

Kick-off with a minimalistic configuration, using the I2S high-level API.
The purpose is to verify the I2S TX transmitter produces output as Master.

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
TX stop is switched to using an interrupt routine.


# TX Slave

## 6_std

Kick-off with a minimalistic configuration, using the I2S high-level API.
The purpose is to verify the I2S TX transmitter produces output as Slave.
Both BCK and WS are taken as input from the Master.

## 7_std_gpio

Switching Gpio configuration to a low-level API.
The purpose is to verify the Gpio Mux signal configuration for BCK and WS.


