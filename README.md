Here is your **updated README with USART marked as completed and cleaned up consistently**:

# STM32F411xx Bare-Metal Drivers

A beginner-friendly collection of **bare-metal peripheral drivers for the STM32F411CE**.

This project is mainly for learning how STM32 peripherals work at the **register level**, without using STM32 HAL or LL drivers.

I am building these drivers step by step while learning embedded C, STM32 peripherals, interrupts, and low-level firmware development.

## Target MCU

- **MCU:** STM32F411CE
- **Board:** STM32F411CE Blackpill
- **Core:** ARM Cortex-M4
- **IDE:** STM32CubeIDE
- **Compiler:** ARM GCC
- **Language:** C

## Drivers

The following drivers are currently implemented or under development:

| Driver | Status        |
| ------ | ------------- |
| GPIO   | ✅ Implemented |
| RCC    | ✅ Implemented |
| SPI    | ✅ Implemented |
| I2C    | ✅ Implemented |
| USART  | ✅ Implemented |
| EXTI   | 🔲 Planned    |
| ADC    | 🔲 Planned    |
| Timer  | 🔲 Planned    |

## Project Structure

```text
STM32F411xx_Drivers/
│
├── drivers/
│   ├── Inc/
│   │   ├── stm32f411xx.h
│   │   ├── stm32f411xx_gpio_driver.h
│   │   ├── stm32f411xx_rcc_driver.h
│   │   ├── stm32f411xx_spi_driver.h
│   │   ├── stm32f411xx_i2c_driver.h
│   │   └── stm32f411xx_usart_driver.h
│   │
│   └── Src/
│       ├── stm32f411xx_gpio_driver.c
│       ├── stm32f411xx_rcc_driver.c
│       ├── stm32f411xx_spi_driver.c
│       ├── stm32f411xx_i2c_driver.c
│       └── stm32f411xx_usart_driver.c
│
├── Startup/
│   └── startup_stm32f411ceux.s
│
├── examples/
│   └── ...
│
└── README.md
```

## Why I am building this

The goal of this project is to understand what happens underneath high-level embedded libraries.

Instead of directly using functions such as:

```c
HAL_GPIO_WritePin();
HAL_UART_Transmit();
HAL_SPI_Transmit();
```

I am learning to configure and control the STM32 peripherals by working directly with their registers.

For example:

```text
Application
     ↓
Driver API
     ↓
Peripheral registers
     ↓
STM32F411 hardware
```

This project is also helping me practice:

- Embedded C
- Memory-mapped registers
- Bit manipulation
- STM32 reference manuals
- Peripheral configuration
- Interrupts
- NVIC
- Clock configuration
- Driver/API design
- Bare-metal firmware development

## Current Drivers

### GPIO

The GPIO driver supports basic GPIO configuration and operations such as:

- Input/output configuration
- Output type
- Output speed
- Pull-up/pull-down
- Reading input pins
- Writing output pins
- Toggling output pins

### RCC

The RCC driver is used for peripheral clock control and clock-related calculations.

It includes support for:

- AHB1 peripheral clocks
- APB1 peripheral clocks
- APB2 peripheral clocks
- PCLK1 calculation
- PCLK2 calculation

### SPI

The SPI driver is being developed for the STM32F411 SPI peripherals.

Current areas include:

- Master mode
- Slave mode
- Full duplex
- Half duplex
- Clock polarity
- Clock phase
- Baud-rate prescaler
- 8-bit and 16-bit data frames
- Software NSS
- Polling-based communication
- Interrupt-based communication

### I2C

The I2C driver supports the basic STM32 I2C master communication flow.

Current areas include:

- I2C initialization
- Clock configuration
- ACK configuration
- Start/Stop conditions
- Master transmit
- Master receive
- Polling-based communication
- Interrupt-based communication

### USART

The USART driver is now **fully implemented**.

It supports:

- USART1
- USART2
- USART6
- Baud-rate configuration
- Word length configuration
- Stop bits configuration
- Parity configuration
- Hardware flow control
- TX/RX communication
- Polling mode
- Interrupt mode
- Application callbacks
- USART error handling

## Examples

I plan to add simple examples for each driver as I test them on the STM32F411CE Blackpill.

Planned examples include:

```text
GPIO
├── LED blink
├── Button input
└── EXTI interrupt

USART
├── Polling TX/RX
└── Interrupt echo

SPI
└── SPI loopback

I2C
├── I2C scanner
└── Sensor communication

ADC
└── Analog sensor reading

Timer
└── Periodic interrupt / delay generation
```

The examples are intended to be simple and easy to understand rather than being complete applications.

## Hardware

The main development board used for this project is:

**STM32F411CE Blackpill**

The drivers are written specifically with the STM32F411xx peripheral set in mind.

## How to Use

Clone the repository and open the project in STM32CubeIDE.

```bash
git clone https://github.com/abhib03/STM32F411xx_Drivers.git
```

Build the project and flash it to an STM32F411CE board.

The driver source and header files can also be included in other STM32F411 bare-metal projects.

## Learning Approach

I am developing this project incrementally.

The general workflow is:

```text
Read Reference Manual
        ↓
Understand peripheral registers
        ↓
Create register definitions
        ↓
Create configuration structures
        ↓
Implement driver functions
        ↓
Build
        ↓
Test on hardware
        ↓
Fix bugs / improve driver
```

This repository is therefore a **learning project**, and the implementation may change as I improve my understanding of STM32 firmware development.

## Future Goals

The next goal is to expand the driver library while keeping the implementation simple and understandable.

### Planned Drivers

- [ ] EXTI driver
- [ ] ADC driver
- [ ] Timer driver
- [ ] SysTick driver
- [ ] Watchdog driver
- [ ] DMA driver

### Planned Improvements

- [ ] Add more hardware examples
- [ ] Add USART interrupt example
- [ ] Add SPI loopback example
- [ ] Add I2C scanner
- [ ] Test drivers with real sensors/peripherals
- [ ] Add better error handling
- [ ] Add timeout handling
- [ ] Improve documentation
- [ ] Add more comments and usage examples

### Long-Term Goal

The long-term goal is to build a small **STM32 bare-metal driver library** that I understand from the register level up, and eventually use these drivers in larger firmware projects.

## Disclaimer

This is a personal learning project.

The drivers are developed for educational purposes and may still contain bugs or limitations. They are being improved and tested as I continue learning STM32 bare-metal firmware development.

## References

- STM32F411 Reference Manual
- STM32F411 Datasheet
- STM32F4 Programming Manual
- ARM Cortex-M4 documentation
- STM32F411CE Blackpill hardware documentation

## Author

**Abhishek Bharadwaj**
Embedded Software Intern
Electronics & Instrumentation Engineering Graduate

Interested in:

- Embedded Systems
- Bare-Metal Firmware
- Embedded C
- RTOS
- Low-Level Driver Development
