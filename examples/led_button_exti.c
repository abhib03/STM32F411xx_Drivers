/*
 * led_button_interrupt.c
 *
 * Example: LED toggle using button interrupt
 *
 * Hardware:
 * - LED connected to PC13 (onboard LED)
 * - Button connected to PA0 (external interrupt)
 */

#include "stm32f411xx.h"
#include "stm32f411xx_gpio_driver.h"

void delay(void)
{
	// Simple delay
	for(uint32_t i = 0; i < 500000/2; i++);
}

int main(void)
{
	GPIO_Handle_t GpioLed, GpioBtn;

	// LED Configuration (PC13) - Output
	GpioLed.pGPIOx = GPIOC;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClkControl(GPIOC, ENABLE);
	GPIO_Init(&GpioLed);

	// Button Configuration (PA0) - Interrupt mode
	GpioBtn.pGPIOx = GPIOA;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;  // Falling edge trigger
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;  // Pull-up

	GPIO_PeriClkControl(GPIOA, ENABLE);
	GPIO_Init(&GpioBtn);

	// IRQ configurations
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI0, NVIC_IRQ_PRI15);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI0, ENABLE);

	while(1);  // Wait for interrupt

	return 0;
}

// ISR for EXTI0 interrupt
void EXTI0_IRQHandler(void)
{
	delay(500);  // Debounce delay

	delay();  // Debounce delay

	GPIO_IRQHandling(GPIO_PIN_NO_0);  // Clear the pending bit
	GPIO_ToggleOutputPin(GPIOC, GPIO_PIN_NO_13);  // Toggle LED
}
