# ESP32-I2S

Code snippets to dig into the internals of the ESP32 I2S.
The objective is to gain a hardware level understanding of it.

The folders are structured per I2S mode of operation.
Each folder holds a set of projects, each project working on a lower level.

The recommended order of browsing them:

1. TX Master mode
Easiest kick-off is with trying to produce an output.

2. TX Slave mode
Next step is to try produce the output with an external bit clock.

3. RX Slave mode
Final step is to try read the output with an external bit clock.

RX Master mode hasn't been implemented.
But that should be rather straightforward using the examples here.

It's recommended to use a scope, to be able to see and debug the signals.
Otherwise it would be like a sight-seeing tour with eyes closed.


# TX Master

All pins should produce output.

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

Both BCK and WS should be taken as input from the I2S Master.

## 6_std

Kick-off with a minimalistic configuration, using the I2S high-level API.
Verify the I2S TX transmitter produces output in Slave mode.

## 7_std_gpio

Switch Gpio configuration to a low-level API.
Verify the GPIO Mux configuration for BCK and WS.

## 8_std_dma

Switch transmit to a low-level API.
FIFO DMA mode, with interrupt routine to handle TX EOF.

## 9_raw_regs

Configure also Clock and Channel with the low-level API.
Otherwise, similar mode of operation.


# RX

All pins should be read as input.

## 10_std

Kick-off with a minimalistic configuration, using the I2S high-level API.
Verify data can be received in RX Slave mode.

## 11_std_gpio

Switch Gpio configuration to a low-level API.
Verify the GPIO Mux configuration for all pins.

## 12_raw_dma

Configure clock, channel, and DMA with the low-level API.
FIFO DMA mode, with interrupt routine to handle RX EOF.


# Platformio

To flash the built project to ESP32: 
\> esptool.py --chip esp32 --port /dev/ttyUSB0 write_flash -z \
\>  0x1000 bootloader.bin \
\>  0x8000 partitions.bin \
\>  0x10000 firmware.bin 