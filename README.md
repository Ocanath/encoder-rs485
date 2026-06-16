# encoder-rs485

STM32G031G6UX firmware for a magnetic rotary encoder node on an RS-485 bus. The device uses a sin/cos analog encoder interface and communicates over RS-485 using the [DARTT protocol](https://github.com/ocanath/dartt-protocol). It is designed to operate on a shared daisy-chain bus alongside other DARTT-addressed nodes.

## Protocol

The firmware exposes two DARTT address classes:

- **Motor address** — a minimal, low-overhead framing mode for high-rate angle polling. Returns a fixed-point angle value with the smallest possible packet size, intended for real-time motion control loops.
- **Misc address** — full DARTT register access for calibration, diagnostics, and device control.

Refer to `Core/Inc/dartt_map.h` for the full register map and command definitions.

## Calibration

Calibration parameters (sin/cos ADC bounds, angular offset, orthogonality error correction) are stored in the last flash page and loaded at startup. They can be updated over the bus and saved at runtime. The angle computation applies gain normalization and error correction before the `atan2` call.

## Action Flags

The DARTT register map includes an action flag field that triggers deferred operations on the device — saving calibration to flash, restarting, entering the bootloader for a firmware update, or toggling the onboard LED. See `Core/Inc/dartt_map.h` for the full list.

## Firmware Update

When flashed alongside the [bootloader](https://github.com/ocanath/dartt_bootloader), this device can recieve firmware updates over RS485. See the top level repo [networked-encoder](https://github.com/ocanath/networked-encoder) for flashing instructions, DFU tools, etc.

## Build Notes

Developed in STM32CubeIDE. Build with `-DNDEBUG` in the preprocessor to strip DARTT assertion code, which has a significant impact on binary size.

## Dependencies

| Submodule | Purpose |
|---|---|
| [`dartt-protocol`](https://github.com/ocanath/dartt-protocol) | DARTT framing and register access |
| [`stm32-dma-cobs-uart`](https://github.com/ocanath/stm32-dma-cobs-uart) | DMA-based UART with COBS framing for STM32 |
