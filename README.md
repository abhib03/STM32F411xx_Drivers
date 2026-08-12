# STM32F411 Bare-Metal Device Drivers

A reusable **bare-metal device driver library for the STM32F411** microcontroller, developed by programming peripheral registers directly from the STM32F411 reference manual and datasheet.

The project focuses on understanding how MCU peripherals work at the register level and implementing clean, reusable driver APIs without relying on STM32 HAL or LL libraries.

## Features

* Register-level peripheral programming
* Modular driver architecture
* Reusable configuration structures and APIs
* Peripheral clock management through RCC
* GPIO configuration and control
* I²C master communication
* SPI communication
* USART communication
* Hardware-oriented driver design
* Designed for STM32F411 Cortex-M4 microcontrollers

## Supported Drivers

| Driver | Description                                | Status         |
| ------ | ------------------------------------------ | -------------- |
| RCC    | Peripheral clock and reset control         | ✅ Complete     |
| GPIO   | Digital input/output and pin configuration | ✅ Complete     |
| I²C    | I²C master communication                   | ✅ Complete     |
| SPI    | SPI communication                          | ✅ Complete     |
| USART  | Serial communication                       | 🔄 In Progress |

## Project Structure

```text
STM32F411xx_Drivers/
│
├── drivers/
│   ├── Inc/
│   │   ├── stm32f411xx.h
│   │   ├── stm32f411xx_gpio_driver.h
│   │   ├── stm32f411xx_rcc_driver.h
│   │   ├── stm32f411xx_i2c_driver.h
│   │   ├── stm32f411xx_spi_driver.h
│   │   └── stm32f411xx_usart_driver.h
│   │
│   └── Src/
│       ├── stm32f411xx_gpio_driver.c
│       ├── stm32f411xx_rcc_driver.c
│       ├── stm32f411xx_i2c_driver.c
│       ├── stm32f411xx_spi_driver.c
│       └── stm32f411xx_usart_driver.c
│
├── Examples/(Will be added soon)
│   ├── GPIO/
│   ├── I2C/
│   ├── SPI/
│   └── USART/
│
├── README.md
└── LICENSE
```

## Hardware

The drivers are developed and tested using the **STM32F411CE** microcontroller, including the STM32F411CE Black Pill development board.

### Main MCU

* **MCU:** STM32F411CEU6
* **Core:** ARM Cortex-M4
* **Architecture:** 32-bit
* **Maximum CPU frequency:** 100 MHz
* **Flash:** 512 KB
* **SRAM:** 128 KB
* **Debug interface:** SWD

## Development Environment

* STM32CubeIDE
* ARM GNU Toolchain
* STM32F411CE development board
* ST-Link debugger/programmer
* Logic analyzer / oscilloscope for peripheral validation

## Driver Architecture

The drivers are organized into separate header and source files.

```text
Application
     │
     ▼
Driver API
     │
     ├── GPIO
     ├── RCC
     ├── I²C
     ├── SPI
     └── USART
     │
     ▼
STM32F411 Peripheral Registers
     │
     ▼
Hardware
```

The application interacts with the peripheral through the driver API instead of directly manipulating peripheral registers.

## GPIO Driver

The GPIO driver provides configuration and control of STM32F411 GPIO peripherals.

Typical functionality includes:

* Input/output configuration
* Pull-up/pull-down configuration
* Output speed configuration
* Push-pull/open-drain configuration
* Alternate-function configuration
* GPIO output control
* GPIO input reading
* GPIO toggle
* GPIO interrupt configuration support

Example:

```c
GPIO_Handle_t GpioLed;

GpioLed.pGPIOx = GPIOA;
GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

GPIO_Init(&GpioLed);
GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
```

## RCC Driver

The RCC driver manages peripheral clock configuration and reset operations.

Responsibilities include:

* GPIO peripheral clock control
* I²C peripheral clock control
* SPI peripheral clock control
* USART peripheral clock control
* Peripheral reset control
* Clock-related configuration required by peripherals

The RCC driver provides the clock infrastructure required by the other peripheral drivers.

## I²C Driver

The I²C driver implements register-level I²C master communication.

Supported functionality includes:

* I²C peripheral initialization
* Master transmit
* Master receive
* START condition generation
* STOP condition generation
* ACK/NACK handling
* Address transmission
* Repeated START handling
* Status flag handling
* Bus/error condition handling

The driver was developed by studying the STM32F411 I²C peripheral registers and communication sequence described in the reference manual.

## SPI Driver

The SPI driver provides register-level SPI communication.

Supported functionality includes:

* SPI initialization
* Master configuration
* Slave configuration
* Full-duplex communication
* SPI data transmission
* SPI data reception
* Clock polarity configuration
* Clock phase configuration
* Baud-rate configuration
* Data frame configuration
* Software/hardware slave-management configuration
* SPI status handling

The driver is designed to provide a reusable interface for communicating with SPI peripherals such as sensors, displays, memory devices, and other external ICs.

## USART Driver

The USART driver provides serial communication through the STM32F411 USART peripherals.

Planned/supported functionality includes:

* USART initialization
* Baud-rate configuration
* Word-length configuration
* Stop-bit configuration
* Parity configuration
* Transmit
* Receive
* USART status handling
* TX/RX configuration

USART can also be used as a debugging interface for applications using the other peripheral drivers.

## Design Approach

The drivers follow a modular design based on configuration structures and peripheral handles.

Example:

```c
GPIO_Handle_t GPIO_Handle;
I2C_Handle_t I2C_Handle;
SPI_Handle_t SPI_Handle;
USART_Handle_t USART_Handle;
```

This approach separates:

1. Peripheral configuration
2. Driver initialization
3. Hardware register access
4. Application-level functionality

The goal is to make the drivers reusable across multiple STM32F411 projects.

## Validation

The drivers are validated using actual STM32F411 hardware rather than only software compilation.

Validation methods include:

* LED/GPIO functional testing
* Serial communication with a PC
* I²C communication with external sensors
* SPI communication with external peripherals
* Register/status verification
* Logic analyzer measurements
* Oscilloscope measurements where required

For communication peripherals, electrical signals and protocol transactions are inspected to verify correct hardware behavior.

## Learning Objectives

This project was developed to gain practical understanding of:

* STM32 memory-mapped peripherals
* MCU register architecture
* Peripheral initialization sequences
* ARM Cortex-M4 microcontrollers
* Clock and reset architecture
* Serial communication protocols
* Embedded C
* Hardware/software interaction
* Driver abstraction
* Datasheet and reference-manual interpretation

## Why Bare-Metal?

Unlike STM32 HAL-based applications, this project directly configures the STM32F411 peripheral registers.

This provides a deeper understanding of:

* What happens inside peripheral initialization
* How registers control MCU hardware
* How communication peripherals operate
* How status flags and control bits are used
* How higher-level driver APIs are built

The project is intended primarily as a **learning and firmware-engineering project**, while maintaining a reusable driver structure suitable for future embedded applications.

## Future Improvements

The core driver set focuses on:

* GPIO
* RCC
* I²C
* SPI
* USART

Future work may include:

* Interrupt-driven communication
* DMA support
* More extensive error handling
* Additional STM32 peripherals
* RTOS integration
* Sensor/device examples using the drivers

## References

* STM32F411 Reference Manual
* STM32F411 Datasheet
* ARM Cortex-M4 Technical Reference Manual
* ARM Cortex-M4 Devices Generic User Guide

## Author

**Abhishek Bharadwaj**

Embedded Software Intern - Aktivolt Celtek
Electronics & Instrumentation Engineering Graduate

GitHub: [@abhib03](https://github.com/abhib03)

---

> **Note:** This project is intended for educational purposes and to demonstrate low-level embedded firmware and device-driver development on the STM32F411.
