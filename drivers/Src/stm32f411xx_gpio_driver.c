/*
 * stm32f411xx_gpio_driver.c
 *
 *  Created on: 12-Jan-2026
 *  Author: Abhishek_Bharadwaj
 */

#include "stm32f411xx.h"
#include "stm32f411xx_gpio_driver.h"


/*
 * Peripheral Clock Setup
 */

/*****************************************************************
 * @fn          - GPIO_PeriClockControl
 *
 * @brief       - This function enables or disables peripheral
 *                clock for the given GPIO port
 *
 * @param[in]   - Base address of the GPIO peripheral
 * @param[in]   - Macros: Enable or Disable
 *
 * @return      - None
 *
 * @Note        - None
 *
 *****************************************************************/
void GPIO_PeriClkControl(GPIO_RegDef_t *pGPIOx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_EN();
		}
		else if(pGPIOx == GPIOB)
		{
			GPIOB_PCLK_EN();
		}
		else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_EN();
		}
		else if (pGPIOx == GPIOD)
		{
			GPIOD_PCLK_EN();
		}
		else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_EN();
		}
		else if(pGPIOx == GPIOH)
		{
			GPIOH_PCLK_EN();
		}
	}else
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_DI();
		}
		else if(pGPIOx == GPIOB)
		{
			GPIOB_PCLK_DI();
		}
		else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_DI();
		}
		else if (pGPIOx == GPIOD)
		{
			GPIOD_PCLK_DI();
		}
		else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_DI();
		}
		else if(pGPIOx == GPIOH)
		{
			GPIOH_PCLK_DI();
		}
	}
}


/*
 * Init and De-init
 */

/*****************************************************************
 * @fn          - GPIO_Init
 *
 * @brief       -This function initializes the GPIO pin according
 * 				 to the configuration parameters
 *
 * @param[in]   -pGPIOHandle: Pointer to GPIO handle structure
 *
 * @return      - None
 *
 * @Note        - None
 *
 *****************************************************************/
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	uint32_t temp = 0; //temporary register

	// Enable Peripheral clock
	GPIO_PeriClkControl(pGPIOHandle->pGPIOx, ENABLE);

	// 1. Configure the mode of GPIO pin (MODER register - 2 bits per pin)
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
	{
		//Non interrupt mode
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber ));
		// Clear the 2 bits for this pin (Critical !)
		pGPIOHandle->pGPIOx->MODER &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		// Set the new mode
		pGPIOHandle->pGPIOx->MODER |= temp;
	}else
	{
		//Interrupt Mode
		//Configure EXTI
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT )
		{
			//1. Configure the FTSR (Falling Trigger Selection Register)
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			// Clear the corresponding RTSR bit
			EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
		{
			//1. Configure the  RTSR (Rising Trigger Selection Register)
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			// Clear corresponding FTSR bit
			EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
		{
			//1. Configure the  RTSR and FTSR
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}

		//2. Configure the GPIO port selection in SYSCFG
		uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4;
		uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4;
		uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
		SYSCFG_PCLK_EN();
		SYSCFG->EXTICR[temp1] = portcode << (temp2 * 4);

		//3. Enable the EXTI interrupt delivery using IMR
		EXTI->IMR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	}

	temp = 0;

	//2. Configure the speed (OSPEEDR register - 2 bits per pin )
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed<<(2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	// Clear the 2 bits (Critical !)
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	// Set the speed
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;

	temp =0;

	//3. Configure the pull-up/pull-down settings (PUPDR register - 2 bits per pin)
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	// Clear the 2 bits
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	// Set pull-up/pull-down
	pGPIOHandle->pGPIOx->PUPDR |= temp;

	temp = 0;

	//4. Configure the output type (OTYPER register - 1 bit per pin)
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	// Clear 1 bit
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	// Set output type
	pGPIOHandle->pGPIOx->OTYPER |= temp;

	temp = 0;

	//5. Configure the alternate functionality (AFR register - 4 bits per pin)
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
	{
		uint8_t temp1, temp2;

		// Determine which AFR register: AFR[0] for pins 0-7, AFR[1] for pins 8-15
		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8;
		// Determine the bit position within the selected AFR register
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;

		// Clear the 4 bits
		pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xF << (4 * temp2));
		// Set alternate function
		pGPIOHandle->pGPIOx->AFR[temp1] |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2));
	}
}

/*****************************************************************
 * @fn          - GPIO_DeInit
 *
 * @brief       - This function resets all the registers of the
 *                given GPIO port
 *
 * @param[in]   - pGPIOx: Base address of the GPIO peripheral
 *
 * @return      - None
 *
 * @Note        - Uses RCC AHB1 peripheral reset register
 *
 *****************************************************************/
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
	if(pGPIOx == GPIOA)
	{
		GPIOA_REG_RESET();
	}
	else if(pGPIOx == GPIOB)
	{
		GPIOB_REG_RESET();
	}
	else if(pGPIOx == GPIOC)
	{
		GPIOC_REG_RESET();
	}
	else if (pGPIOx == GPIOD)
	{
		GPIOD_REG_RESET();
	}
	else if(pGPIOx == GPIOE)
	{
		GPIOE_REG_RESET();
	}
	else if(pGPIOx == GPIOH)
	{
		GPIOH_REG_RESET();
	}
}


/*
 * Data Read and Write
 */

/*****************************************************************
 * @fn          - GPIO_ReadFromInputPin
 *
 * @brief       - This function reads value from a GPIO input pin
 *
 * @param[in]   - pGPIOx: Base address of the GPIO peripheral
 * @param[in]   - PinNumber: Pin number (0-15)
 *
 * @return      - 0 or 1
 *
 * @Note        - None
 *
 *****************************************************************/
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t value;
	value =(uint8_t) ((pGPIOx->IDR >> PinNumber) & 0x00000001);
	return value;
}

/*****************************************************************
 * @fn          - GPIO_ReadFromInputPort
 *
 * @brief       - This function reads value from entire GPIO port
 *
 * @param[in]   - pGPIOx: Base address of the GPIO peripheral
 *
 * @return      - 16-bit port value
 *
 * @Note        - None
 *
 *****************************************************************/
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
	uint16_t value;
	value = (uint16_t)pGPIOx->IDR;
	return value;
}

/*****************************************************************
 * @fn          - GPIO_WriteToOutputPin
 *
 * @brief       - This function writes value to a GPIO output pin
 *
 * @param[in]   - pGPIOx: Base address of the GPIO peripheral
 * @param[in]   - PinNumber: Pin number (0-15)
 * @param[in]   - Value: GPIO_PIN_SET or GPIO_PIN_RESET
 *
 * @return      - None
 *
 * @Note        - None
 *
 *****************************************************************/
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value)
{
	if(Value == GPIO_PIN_SET)
	{
		// Set the pin or write 1 to the output data register at the bit-field corresponding pin-number
		pGPIOx->ODR |= (1 << PinNumber);

	}else
	{
		// Reset the pin or write 0 to the output data register at the bit-field corresponding pin-number
		pGPIOx->ODR &= ~(1 << PinNumber);
	}
}

/*****************************************************************
 * @fn          - GPIO_WriteToOutputPort
 *
 * @brief       - This function writes value to entire GPIO port
 *
 * @param[in]   - pGPIOx: Base address of the GPIO peripheral
 * @param[in]   - Value: 16-bit value to write
 *
 * @return      - None
 *
 * @Note        - None
 *
 *****************************************************************/
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value)
{
	pGPIOx->ODR = Value;
}

/****************************************************************
 * @fn          - GPIO_ToggleOutputPin
 *
 * @brief       - This function toggles a GPIO output pin
 *
 * @param[in]   - pGPIOx: Base address of the GPIO peripheral
 * @param[in]   - PinNumber: Pin number (0-15)
 *
 * @return      - None
 *
 * @Note        - Uses XOR operation on ODR register
 *
 ****************************************************************/
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	pGPIOx->ODR ^= (1 << PinNumber);
}


/*
 * IRQ Configuration and ISR Handling
 */

/*****************************************************************
 * @fn          - GPIO_IRQInterruptConfig
 *
 * @brief       - This function configures interrupt
 *
 * @param[in]   - IRQNumber: IRQ number
 * @param[in]   - EnOrDi: ENABLE or DISABLE
 *
 * @return      - None
 *
 * @Note        - Configures NVIC ISER/ICER registers
 *
 *****************************************************************/
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi)
{
	// Implement NVIC interrupt configuration
	// Configure NVIC ISER/ICER registers
	if(EnOrDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			// Program ISER0 register
			*NVIC_ISER0 |= (1 << IRQNumber);
		}
		else if(IRQNumber > 31 && IRQNumber < 64)
		{
			// Program ISER1 register
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));
		}
		else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			// Program ISER2 register
			*NVIC_ISER2 |= (1 << (IRQNumber % 64));
		}
	}
	else
	{
		if(IRQNumber <= 31)
		{
			// Program ICER0 register
			*NVIC_ICER0 |= (1 << IRQNumber);
		}
		else if(IRQNumber > 31 && IRQNumber < 64)
		{
			// Program ICER1 register
			*NVIC_ICER1 |= (1<< (IRQNumber % 32));
		}
		else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			// Program ICER2 register
			*NVIC_ICER2 |= (1<< (IRQNumber % 64));
		}

	}
}


/*****************************************************************
 * @fn          - GPIO_IRQPriorityConfig
 *
 * @brief       - This function configures IRQ priority
 *
 * @param[in]   - IRQNumber: IRQ number
 * @param[in]   - IRQPriority: IRQ priority value
 *
 * @return      - None
 *
 * @Note        - Configures NVIC IPR registers
 *
 *****************************************************************/
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// 1. Find out the IPR register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}


/*****************************************************************
 * @fn          - GPIO_IRQHandling
 *
 * @brief       - This function handles the interrupt
 *
 * @param[in]   - PinNumber: Pin number that triggered interrupt
 *
 * @return      - None
 *
 * @Note        - Must be called from ISR
 *
 *****************************************************************/
void GPIO_IRQHandling(uint8_t PinNumber)
{
	// Clear the EXTI PR register for the pin number
	if (EXTI->PR & (1 << PinNumber))
	{
		// Clear by writing 1
		EXTI->PR |= (1 << PinNumber);
	}
}

