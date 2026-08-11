# STM32F411 GPIO Driver (Register Level)

## 📌 Overview
This repository contains a **bare-metal, register-level GPIO driver** developed for the **STM32F411CE** microcontroller.  
The driver is written in **Embedded C**, without using **STM32 HAL** or **CubeMX**, to gain a deep understanding of MCU internals and peripheral programming.

The project follows a **CMSIS-style architecture** with clear separation between:
- Device header
- Driver interface
- Driver implementation
- Application examples

---

## 🎯 Objectives
- Understand STM32F411 GPIO hardware at register level
- Learn memory-mapped peripheral programming
- Implement safe read–modify–write operations
- Design reusable, scalable embedded drivers
- Gain practical experience suitable for embedded interviews

---

## 🧠 Concepts Covered
- Memory-mapped I/O
- AHB/APB bus architecture
- GPIO registers:
  - MODER
  - OTYPER
  - OSPEEDR
  - PUPDR
  - IDR
  - ODR
  - BSRR
  - AFRL / AFRH
- RCC clock enable and reset control
- GPIO pin configuration and control
- CMSIS-style handle and configuration structures
- Bare-metal driver design (no HAL)

---

## 📁 Project Structure

STM32F411CE_GPIO_Driver/ <br>
├── inc/ <br>
│ ├── stm32f411xx.h <br>
│ └── stm32f411xx_gpio_driver.h <br>
├── src/ <br>
│ └── stm32f411xx_gpio_driver.c <br>
├── examples/ <br>
│ └── led_button_exti.c <br>
├── startup/ <br>
│ └── startup_stm32f411xx.s <br>
├── linker/ <br>
│ └── stm32f411.ld <br>
├── README.md <br>
└── LICENSE <br>


---

## 📄 File Description

### 🔹 `stm32f411xx.h`
- Device header file defining:
  - Memory map and peripheral base addresses
  - AHB1 / APB1 / APB2 bus mapping
  - GPIO and RCC register definition structures
  - Clock enable/disable macros
  - Peripheral reset macros
- Designed to be minimal and expandable

---

### 🔹 `stm32f411xx_gpio_driver.h`
- Public GPIO driver interface
- GPIO configuration and handle structures
- API declarations for:
  - GPIO initialization and de-initialization
  - Input/output operations
  - GPIO toggling

---

### 🔹 `stm32f411xx_gpio_driver.c`
- Register-level implementation of GPIO driver
- Implements:
  - GPIO pin mode configuration
  - Speed, pull-up/pull-down, output type setup
  - Atomic GPIO operations using read–modify–write
  - GPIO port reset using RCC

---

### 🔹 `examples/`
- Contains example applications demonstrating driver usage
- `led_button_exti.c` shows GPIO input/output usage

---

## ⚙️ GPIO Initialization Flow

Enable GPIO clock (RCC)
↓
Configure pin mode (MODER)
↓
Configure output type (OTYPER)
↓
Configure speed (OSPEEDR)
↓
Configure pull-up / pull-down (PUPDR)
↓
Configure alternate function (AFRL / AFRH)


---

## 🧪 Example Usage

### LED Output Configuration

```c
GPIO_Handle_t led;

led.pGPIOx = GPIOA;
led.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_5;
led.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUTPUT;
led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_MEDIUM;
led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

GPIO_Init(&led);
```
---
✅ Features Implemented

Register-level GPIO configuration

Read–modify–write safe register access

Input and output pin control

GPIO port reset via RCC

CMSIS-style modular driver architecture

No HAL or CubeMX dependency

🔮 Future Enhancements

EXTI (GPIO interrupts) support

UART, SPI, and I2C drivers using same architecture

GPIO interrupt callback mechanism

RTOS-friendly driver abstraction

---

🛠️ Tools & References

STM32F411CE (Black Pill)

ARM GCC toolchain

STM32F411 Reference Manual (RM0383)

Embedded C

Git & GitHub

---

📄 License

This project is licensed under the MIT License.

---
👤 Author

Abhishek Bharadwaj <br>
Embedded Systems Enthusiast
