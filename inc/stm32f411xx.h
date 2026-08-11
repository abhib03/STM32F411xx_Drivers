/*
 * stm32f411cexx.h
 *
 *  Created on: 05-Jan-2026
 *  Author: Abhishek_Bharadwaj
 */

#ifndef INC_STM32F411XX_H_
#define INC_STM32F411XX_H_

#include <stdint.h>
#define __vo volatile

/*
 * Base addresses of Flash and SRAM memories
 */
#define FLASH_BASEADDR 				0x08000000U // 512kB of Main memory. Flash memory base address.
#define SRAM1_BASEADDR 				0x20000000U
#define SRAM2_BASEADDR 				0x2001C000U
#define SRAM_BASEADDR				SRAM1_BASEADDR //SRAM contains both SRAM1 and SRAM2, so base address of SRAM starts from SRAM1.
#define ROM_BASEADDR  				0x1FFF0000U // 30kB of System memory. System memory/ROM base address.

/*
 * AHBx and APBx Bus peripheral base addresses
 */
#define PERIPH_BASEADDR				0x40000000U
#define APB1PERIPH_BASEADDR 		PERIPH_BASEADDR
#define APB2PERIPH_BASEADDR			0x40010000U
#define AHB1PERIPH_BASEADDR			0x40020000U
#define AHB2PERIPH_BASEADDR			0x50000000U

/*
 * Base addresses of peripherals hanging on AHB1 bus
 * Note: STM32F411CE has GPIOH but with limited pins (only PH0 and PH1 typically used for external oscillator)
 */
#define RCC_BASEADDR                (AHB1PERIPH_BASEADDR + 0x3800U)  //Base address of RCC   - 0x40023800U (Reset and Clock Control)
#define GPIOA_BASEADDR 				(AHB1PERIPH_BASEADDR + 0x0000U)  //Base address of GPIOA - 0x40020000U
#define GPIOB_BASEADDR				(AHB1PERIPH_BASEADDR + 0x0400U)  //Base address of GPIOB - 0x40020400U
#define GPIOC_BASEADDR				(AHB1PERIPH_BASEADDR + 0x0800U)  //Base address of GPIOC - 0x40020800U
#define GPIOD_BASEADDR				(AHB1PERIPH_BASEADDR + 0x0C00U)  //Base address of GPIOD - 0x40020C00U
#define GPIOE_BASEADDR				(AHB1PERIPH_BASEADDR + 0x1000U)  //Base address of GPIOE - 0x40021000U
#define GPIOH_BASEADDR				(AHB1PERIPH_BASEADDR + 0x1C00U)  //Base address of GPIOH - 0x40021C00U


/*
 * Base addresses of peripherals hanging on AHB2 bus
 * NOTE:
 * STM32F411 has only one AHB2 peripheral:
 * USB OTG Full-Speed controller
 * Not using it right now.
#define USBOTG_FS_BASEADDR			(AHB2PERIPH_BASEADDR + 0x0000U)  //Base address of USB OTG FS peripheral - 0x50000000U
*/


/*
 * Base addresses of peripherals hanging on APB1 bus
 * Timer peripherals are not mentioned
 */
#define SPI2_BASEADDR				(APB1PERIPH_BASEADDR + 0x3800U)	 //Base address of SPI2   - 0x40003800U
#define SPI3_BASEADDR				(APB1PERIPH_BASEADDR + 0x3C00U)	 //Base address of SPI3   - 0x40003C00U
#define USART2_BASEADDR 			(APB1PERIPH_BASEADDR + 0x4400U)  //Base address of USART2 - 0x40004400U
#define I2C1_BASEADDR				(APB1PERIPH_BASEADDR + 0x5400U)  //Base address of I2C1   - 0x40005400U
#define I2C2_BASEADDR				(APB1PERIPH_BASEADDR + 0x5800U)	 //Base address of I2C2   - 0x40005800U
#define I2C3_BASEADDR 				(APB1PERIPH_BASEADDR + 0x5C00U)  //Base address of I2C3   - 0x40005C00U

/*
 *  Base addresses of peripherals hanging on APB2 bus
 */
#define USART1_BASEADDR				(APB2PERIPH_BASEADDR + 0x1000U)	 //Base address of USART1 - 0x40011000U
#define USART6_BASEADDR				(APB2PERIPH_BASEADDR + 0x1400U)	 //Base address of USART6 - 0x40011400U
#define SPI1_BASEADDR				(APB2PERIPH_BASEADDR + 0x3000U)  //Base address of SPI1   - 0x40013000U
#define SPI4_BASEADDR 				(APB2PERIPH_BASEADDR + 0x3400U)  //Base address of SPI4   - 0x40013400U
#define SYSCFG_BASEADDR 			(APB2PERIPH_BASEADDR + 0x3800U)	 //Base address of SYSCFG - 0x40013800U
#define EXTI_BASEADDR 				(APB2PERIPH_BASEADDR + 0x3C00U)  //Base address of EXTI   - 0x40013C00U

/*
 *  Peripheral register definition structure for GPIO
 */
typedef struct
{
	__vo uint32_t MODER;	// GPIO port mode register, 											Address offset : 0x00
	__vo uint32_t OTYPER;	// GPIO port output type register, 										Address offset : 0x04
	__vo uint32_t OSPEEDR; 	// GPIO port output speed register,										Address offset : 0x08
	__vo uint32_t PUPDR;	// GPIO port pull-up/pull-down register, 								Address offset : 0x0C
	__vo uint32_t IDR; 		// GPIO port input data register, 										Address offset : 0x10
	__vo uint32_t ODR;		// GPIO port output data register, 										Address offset : 0x14
	__vo uint32_t BSRR;		// GPIO port bit set/reset register, 									Address offset : 0x18
	__vo uint32_t LCKR;		// GPIO port configuration lock register,								Address offset : 0x1C
	__vo uint32_t AFR[2];   // GPIO port alternate function register, 							 	Address offset : 0x20 - 0x24
	/* AFR[0] = AFRL, AFR[1] = AFRH
	 * uint32_t AFRL; 	//Address offset is 0x20
	 * uint32_t AFRH;	//Address offset is 0x24
	*/
}GPIO_RegDef_t;


/*
 * Peripheral register definition for RCC ( Reset and clock control ) for STM32F411CE.
 */
typedef struct
{
    __vo uint32_t CR;            // Address offset: 0x00
    __vo uint32_t PLLCFGR;       // Address offset: 0x04
    __vo uint32_t CFGR;          // Address offset: 0x08
    __vo uint32_t CIR;           // Address offset: 0x0C
    __vo uint32_t AHB1RSTR;      // Address offset: 0x10
    __vo uint32_t AHB2RSTR;      // Address offset: 0x14
    uint32_t RESERVED0[2];       // Reserved, 0x18-0x1C
    __vo uint32_t APB1RSTR;      // Address offset: 0x20
    __vo uint32_t APB2RSTR;      // Address offset: 0x24
    uint32_t RESERVED1[2];       // Reserved, 0x28-0x2C
    __vo uint32_t AHB1ENR;       // Address offset: 0x30
    __vo uint32_t AHB2ENR;       // Address offset: 0x34
    uint32_t RESERVED2[2];       // Reserved, 0x38-0x3C
    __vo uint32_t APB1ENR;       // Address offset: 0x40
    __vo uint32_t APB2ENR;       // Address offset: 0x44
    uint32_t RESERVED3[2];       // Reserved, 0x48-0x4C
    __vo uint32_t AHB1LPENR;     // Address offset: 0x50
    __vo uint32_t AHB2LPENR;     // Address offset: 0x54
    uint32_t RESERVED4[2];       // Reserved, 0x58-0x5C
    __vo uint32_t APB1LPENR;     // Address offset: 0x60
    __vo uint32_t APB2LPENR;     // Address offset: 0x64
    uint32_t RESERVED5[2];       // Reserved, 0x68-0x6C
    __vo uint32_t BDCR;          // Address offset: 0x70
    __vo uint32_t CSR;           // Address offset: 0x74
    uint32_t RESERVED6[2];       // Reserved, 0x78-0x7C
    __vo uint32_t SSCGR;         // Address offset: 0x80
    __vo uint32_t PLLI2SCFGR;    // Address offset: 0x84
    uint32_t RESERVED7;          // Reserved, 0x88
    __vo uint32_t DCKCFGR;       // Address offset: 0x8C
} RCC_RegDef_t;


/*
 * Peripheral definitions ( peripheral base addresses type-casted to GPIOx_RegDef_t)
 */
#define GPIOA   		((GPIO_RegDef_t*)GPIOA_BASEADDR)
#define GPIOB   		((GPIO_RegDef_t*)GPIOB_BASEADDR)
#define GPIOC   		((GPIO_RegDef_t*)GPIOC_BASEADDR)
#define GPIOD   		((GPIO_RegDef_t*)GPIOD_BASEADDR)
#define GPIOE   		((GPIO_RegDef_t*)GPIOE_BASEADDR)
#define GPIOH			((GPIO_RegDef_t*)GPIOH_BASEADDR)

#define RCC             ((RCC_RegDef_t*)RCC_BASEADDR)


/*
 * Clock Enable Macros for GPIOx peripherals
 */
#define GPIOA_PCLK_EN()		(RCC->AHB1ENR |= (1 << 0))	//GPIOA bit position is 0
#define GPIOB_PCLK_EN()		(RCC->AHB1ENR |= (1 << 1))	//GPIOB bit position is 1
#define GPIOC_PCLK_EN()		(RCC->AHB1ENR |= (1 << 2))	//GPIOC bit position is 2
#define GPIOD_PCLK_EN()		(RCC->AHB1ENR |= (1 << 3))	//GPIOD bit position is 3
#define GPIOE_PCLK_EN()		(RCC->AHB1ENR |= (1 << 4))	//GPIOE bit position is 4
#define GPIOH_PCLK_EN()		(RCC->AHB1ENR |= (1 << 7))  //Note: GPIOH is at bit position 7 in the RCC_AHB1ENR register (not bit 5!)


/*
 * Clock Enable Macros for I2Cx peripherals
 */
#define I2C1_PCLK_EN()		(RCC->APB1ENR |= (1 << 21)) //I2C1 clock enable is at bit position 21
#define I2C2_PCLK_EN()		(RCC->APB1ENR |= (1 << 22)) //I2C2 clock enable is at bit position 22
#define I2C3_PCLK_EN()		(RCC->APB1ENR |= (1 << 23)) //I2C3 clock enable is at bit position 23


/*
 * Clock Enable Macros for SPIx peripherals
 */
#define SPI1_PCLK_EN()		(RCC->APB2ENR |= (1 << 12)) //SPI1 is present in APB2 clock enable register at bit position 12
#define SPI2_PCLK_EN()		(RCC->APB1ENR |= (1 << 14)) //SPI2 is present in APB1 clock enable register at bit position 14
#define SPI3_PCLK_EN()		(RCC->APB1ENR |= (1 << 15)) //SPI3 is present in APB1 clock enable register at bit position 15
#define SPI4_PCLK_EN()		(RCC->APB2ENR |= (1 << 13)) //SPI4 is present in APB2 clock enable register at bit position 13


/*
 * Clock Enable Macros for USARTx peripherals
 */
#define USART1_PCLK_EN()	(RCC->APB2ENR |= (1 << 4))  //USART1 is present in APB2 clock enable register at bit position 4
#define USART2_PCLK_EN()	(RCC->APB1ENR |= (1 << 17)) //USART2 is present in APB1 clock enable register at bit position 17
#define USART6_PCLK_EN()	(RCC->APB2ENR |= (1 << 5))  //USART6 is present in APB2 clock enable register at bit position 5


/*
 * Clock Enable Macro for SYSCFG peripheral
 */
#define SYSCFG_PCLK_EN()	(RCC->APB2ENR |= (1 << 14))  //SYSCFG is present in APB2 at bit position 14


/*
 * Clock Disable Macros for GPIOx peripherals
 */
#define GPIOA_PCLK_DI()		(RCC->AHB1ENR &= ~(1 << 0))	//GPIOA bit position is 0
#define GPIOB_PCLK_DI()		(RCC->AHB1ENR &= ~(1 << 1))	//GPIOB bit position is 1
#define GPIOC_PCLK_DI()		(RCC->AHB1ENR &= ~(1 << 2))	//GPIOC bit position is 2
#define GPIOD_PCLK_DI()		(RCC->AHB1ENR &= ~(1 << 3))	//GPIOD bit position is 3
#define GPIOE_PCLK_DI()		(RCC->AHB1ENR &= ~(1 << 4))	//GPIOE bit position is 4
#define GPIOH_PCLK_DI()		(RCC->AHB1ENR &= ~(1 << 7)) //Note: GPIOH is at bit position 7 in the RCC_AHB1ENR register (not bit 5!)


/*
 * Clock Disable Macros for I2Cx peripherals
 */
#define I2C1_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 21)) //I2C1 clock enable is at bit position 21
#define I2C2_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 22)) //I2C2 clock enable is at bit position 22
#define I2C3_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 23)) //I2C3 clock enable is at bit position 23


/*
 * Clock Disable Macros for SPIx peripherals
 */
#define SPI1_PCLK_DI()		(RCC->APB2ENR &= ~(1 << 12)) //SPI1 is present in APB2 clock enable register at bit position 12
#define SPI2_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 14)) //SPI2 is present in APB1 clock enable register at bit position 14
#define SPI3_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 15)) //SPI3 is present in APB1 clock enable register at bit position 15
#define SPI4_PCLK_DI()		(RCC->APB2ENR &= ~(1 << 13)) //SPI4 is present in APB2 clock enable register at bit position 13


/*
 * Clock Disable Macros for USARTx peripherals
 */
#define USART1_PCLK_DI()	(RCC->APB2ENR &= ~(1 << 4))  //USART1 is present in APB2 clock enable register at bit position 4
#define USART2_PCLK_DI()	(RCC->APB1ENR &= ~(1 << 17)) //USART2 is present in APB1 clock enable register at bit position 17
#define USART6_PCLK_DI()	(RCC->APB2ENR &= ~(1 << 5))  //USART6 is present in APB2 clock enable register at bit position 5


/*
 * Clock Disable Macro for SYSCFG peripheral
 */
#define SYSCFG_PCLK_DI()	(RCC->APB2ENR &= ~(1 << 14))


/*
 * Macros to reset GPIOx peripherals
 */
#define GPIOA_REG_RESET()   do{ (RCC->AHB1RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0)); } while(0)
#define GPIOB_REG_RESET()   do{ (RCC->AHB1RSTR |= (1 << 1)); (RCC->AHB1RSTR &= ~(1 << 1)); } while(0)
#define GPIOC_REG_RESET()   do{ (RCC->AHB1RSTR |= (1 << 2)); (RCC->AHB1RSTR &= ~(1 << 2)); } while(0)
#define GPIOD_REG_RESET()   do{ (RCC->AHB1RSTR |= (1 << 3)); (RCC->AHB1RSTR &= ~(1 << 3)); } while(0)
#define GPIOE_REG_RESET()   do{ (RCC->AHB1RSTR |= (1 << 4)); (RCC->AHB1RSTR &= ~(1 << 4)); } while(0)
#define GPIOH_REG_RESET()   do{ (RCC->AHB1RSTR |= (1 << 7)); (RCC->AHB1RSTR &= ~(1 << 7)); } while(0)

/*
 * Some Generic Macros
 */
#define ENABLE		 	1
#define DISABLE 		0
#define SET  			ENABLE
#define RESET 			DISABLE
#define GPIO_PIN_SET	SET
#define GPIO_PIN_RESET	RESET


#endif /* INC_STM32F411XX_H_ */
