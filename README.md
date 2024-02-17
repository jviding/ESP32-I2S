# ESP32-I2S

This library digs deep into the ESP32 I2S peripheral.
The purpose of this is to understand its functions in detail.

Both RX and TX directories hold several, ordered, subprojects.
Each subproject builds on top of the previous, going deeper.

Scope is recommended, to be able to observe the signals.


# TX Master

## 1_std

Kick-off with a minimalistic configuration, using the I2S high-level API.
Verify the I2S TX transmitter produces output as Master.

## 2_std_mod

Expanded configurations, with the high-level API.
Observing how different parameters affect the output.

## 3_raw_regs

Switch to a low-level API, using direct register access.
FIFO mode, with also GPIO Mux configured on lower level.

## 4_fifo_dma

Switch to FIFO DMA (burst) mode.
Single DMA descriptor and buffer.

## 5_dma_isr

Use FIFO DMA with linked buffers.
Use an interrupt routine to handle TX EOF.


# TX Slave

## 6_std

Kick-off with a minimalistic configuration, using the I2S high-level API.
Verify the I2S TX transmitter produces output in Slave mode.
Both BCK and WS are taken as input from Master.

## 7_std_gpio

Switch Gpio configuration to a low-level API.
Verify the GPIO Mux configuration for BCK and WS.

## 8_std_dma

Switch transmit to a low-level API.
FIFO DMA mode, with interrupt routine to handle TX EOF.




