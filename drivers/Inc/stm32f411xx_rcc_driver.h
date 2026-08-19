/*
 * stm32f411xx_rcc_driver.h
 *
 *  Created on: Aug 10, 2026
 *  Author: Abhishek_Bharadwaj
 */

#ifndef INC_STM32F411XX_RCC_DRIVER_H_
#define INC_STM32F411XX_RCC_DRIVER_H_

#include "stm32f411xx.h"

#define HSI_VALUE   16000000U
#define HSE_VALUE   25000000U

//This returns the APB1 clock value
uint32_t RCC_GetPCLK1Value(void);

//This returns the APB2 clock value
uint32_t RCC_GetPCLK2Value(void);


uint32_t  RCC_GetPLLOutputClock(void);

#endif /* INC_STM32F411XX_RCC_DRIVER_H_ */
