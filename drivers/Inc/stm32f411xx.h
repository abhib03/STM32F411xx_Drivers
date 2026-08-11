/*
 * stm32f411xx.h
 *
 *  Created on: 05-Jan-2026
 *  Author: Abhishek_Bharadwaj
 */

#ifndef INC_STM32F411XX_H_
#define INC_STM32F411XX_H_

#include <stddef.h>
#include <stdint.h>

/* Compiler attribute aliases — define if CMSIS is not present */
#ifndef __weak
#define __weak    __attribute__((weak))
#endif

#ifndef __vo
#define __vo      volatile
#endif


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
#define SPI5_BASEADDR				(APB2PERIPH_BASEADDR + 0x5000U)  //Base address of SPI5   -	0x40015000U


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
 * Peripheral register definition structure for RCC ( Reset and clock control ) for STM32F411CE.
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
 * Peripheral register definition structure for EXTI
 */
typedef struct
{
	__vo uint32_t IMR; 			// Interrupt mask register, 			Address offset : 0x00
	__vo uint32_t EMR;			// Event mask register,					Address offset : 0x04
	__vo uint32_t RTSR;			// Risiing trigger selection register,  Address offset : 0x08
	__vo uint32_t FTSR;			// Falling trigger selection register,	Address offset : 0x0C
	__vo uint32_t SWIER;		// Software interrupt event register,	Address offset : 0x10
	__vo uint32_t PR;			// Pending register,					Address offset : 0x14
} EXTI_RegDef_t;


/*
 * Peripheral register definition structure for SYSCFG
 */
typedef struct
{
	__vo uint32_t MEMRMP; 		// Memory remap register,				Address offset : 0x00
	__vo uint32_t PMC;			// Peripheral mode config. register,	Address offset : 0x04
	__vo uint32_t EXTICR[4];	// External interrupt config. register, Address offset : 0x08 - 0x14
	uint32_t RESERVED1[2];		// Reserved, 							0x18 - 0x1C
	__vo uint32_t CMPCR;		// Compensation cell control register, 	Address offset : 0x20
	uint32_t RESERVED2[2];		// Reserved,							0x24 - 0x28
	__vo uint32_t CFGR; 		// Configuration register,				Address offset : 0x2C
} SYSCFG_RegDef_t;


/*
 * peripheral register definition structure for USART
 */
typedef struct
{
	__vo uint32_t SR;			// Status register,						Address offset : 0x00
	__vo uint32_t DR;			// Data register,						Address offset : 0x04
	__vo uint32_t BRR;			// Baud rate register, 					Address offset : 0x08
	__vo uint32_t CR1;			// Control register 1,					Address offset : 0x0C
	__vo uint32_t CR2;			// Control register 2,					Address offset : 0x10
	__vo uint32_t CR3;			// Control register 3, 					Address offset : 0x14
	__vo uint32_t GTPR;			// Guard time and prescaler register,	Address offset : 0x18
} USART_RegDef_t;


/*
 * peripheral register definition structure for SPI
 */
typedef struct
{
	__vo uint32_t CR1;			// Control register 1,					Address offset : 0x00
	__vo uint32_t CR2;			// Control register 2,					Address offset : 0x04
	__vo uint32_t SR;			// Status register,						Address offset : 0x08
	__vo uint32_t DR;			// Data register,						Address offset : 0x0C
	__vo uint32_t CRCPR;		// CRC polynomial register,				Address offset : 0x10
	__vo uint32_t RXCRCR;		// RX CRC register,						Address offset : 0x14
	__vo uint32_t TXCRCR;		// TX CRC register,						Address offset : 0x18
	__vo uint32_t I2SCFGR;		// I2S configuration register,			Address offset : 0x1C
	__vo uint32_t I2SPR;		// I2S prescalar register,				Address offset : 0x20
}SPI_RegDef_t;


/*
 * peripheral register definition structure for I2C
 */
typedef struct
{
	__vo uint32_t CR1;			// Control register 1,					Address offset : 0x00
	__vo uint32_t CR2;			// Control register 2,					Address offset : 0x04
	__vo uint32_t OAR1;			// Own address register 1,				Address offset : 0x08
	__vo uint32_t OAR2;			// Own address register 2,				Address offset : 0x0C
	__vo uint32_t DR;			// Data register,						Address offset : 0x10
	__vo uint32_t SR1;			// Status register 1,					Address offset : 0x14
	__vo uint32_t SR2;			// Status register 2,					Address offset : 0x18
	__vo uint32_t CCR;			// Clock control register,				Address offset : 0x1C
	__vo uint32_t TRISE;		// TRISE register,						Address offset : 0x20
	__vo uint32_t FLTR;			// FLTR register,						Address offset : 0x24
}I2C_RegDef_t;


/*
 * GPIO peripheral definitions  (base addresses type-casted to GPIOx_RegDef_t)
 * USART peripheral definitions (base addresses type-casted to USART_RegDef_t)
 * SPI peripheral definitions (base addresses type-casted to SPI_RegDef_t)
 * I2C peripheral definitions (base addresses type-casted to I2C_RegDef_t
 */
#define GPIOA   		((GPIO_RegDef_t*)GPIOA_BASEADDR)
#define GPIOB   		((GPIO_RegDef_t*)GPIOB_BASEADDR)
#define GPIOC   		((GPIO_RegDef_t*)GPIOC_BASEADDR)
#define GPIOD   		((GPIO_RegDef_t*)GPIOD_BASEADDR)
#define GPIOE   		((GPIO_RegDef_t*)GPIOE_BASEADDR)
#define GPIOH			((GPIO_RegDef_t*)GPIOH_BASEADDR)

#define RCC             ((RCC_RegDef_t*)RCC_BASEADDR)
#define EXTI 			((EXTI_RegDef_t*)EXTI_BASEADDR)
#define SYSCFG			((SYSCFG_RegDef_t*)SYSCFG_BASEADDR)

#define USART1 			((USART_RegDef_t*)USART1_BASEADDR)
#define USART2 			((USART_RegDef_t*)USART2_BASEADDR)
#define USART6 			((USART_RegDef_t*)USART6_BASEADDR)

#define SPI1			((SPI_RegDef_t*)SPI1_BASEADDR)
#define SPI2			((SPI_RegDef_t*)SPI2_BASEADDR)
#define SPI3			((SPI_RegDef_t*)SPI3_BASEADDR)
#define SPI4			((SPI_RegDef_t*)SPI4_BASEADDR)
#define SPI5			((SPI_RegDef_t*)SPI5_BASEADDR)

#define I2C1			((I2C_RegDef_t*)I2C1_BASEADDR)
#define I2C2			((I2C_RegDef_t*)I2C2_BASEADDR)
#define I2C3			((I2C_RegDef_t*)I2C3_BASEADDR)

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
#define SPI5_PCLK_EN()		(RCC->APB2ENR |= (1 << 20)) //SPI5 is present in APB2 clock enable register at bit position 13


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
#define SPI5_PCLK_DI()		(RCC->APB2ENR &= ~(1 << 20)) //SPI5 is present in APB2 clock enable register at bit position 20


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
 *
 * The reset is performed by setting the corresponding bit in the RCC AHB1
 * peripheral reset register (AHB1RSTR) and then clearing it.
 * The reset bit must not remain set, otherwise the peripheral will stay
 * in reset state.
 */
#define GPIOA_REG_RESET()   do{ (RCC->AHB1RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0)); } while(0)
#define GPIOB_REG_RESET()   do{ (RCC->AHB1RSTR |= (1 << 1)); (RCC->AHB1RSTR &= ~(1 << 1)); } while(0)
#define GPIOC_REG_RESET()   do{ (RCC->AHB1RSTR |= (1 << 2)); (RCC->AHB1RSTR &= ~(1 << 2)); } while(0)
#define GPIOD_REG_RESET()   do{ (RCC->AHB1RSTR |= (1 << 3)); (RCC->AHB1RSTR &= ~(1 << 3)); } while(0)
#define GPIOE_REG_RESET()   do{ (RCC->AHB1RSTR |= (1 << 4)); (RCC->AHB1RSTR &= ~(1 << 4)); } while(0)
#define GPIOH_REG_RESET()   do{ (RCC->AHB1RSTR |= (1 << 7)); (RCC->AHB1RSTR &= ~(1 << 7)); } while(0)


/*
 * Macros to reset USARTX peripherals
 * USART1 and USART6 are on APB2; USART2 is on APB1.
 */
#define USART1_REG_RESET() do{(RCC -> APB2RSTR |= (1 << 4)); (RCC -> APB2RSTR &= ~(1 << 4)); } while(0)
#define USART2_REG_RESET() do{(RCC -> APB1RSTR |= (1 << 17)); (RCC -> APB1RSTR &= ~(1 << 17)); } while(0)
#define USART6_REG_RESET() do{(RCC -> APB2RSTR |= (1 << 5)); (RCC -> APB2RSTR &= ~(1 << 5)); } while(0)


/*
 * Macros to reset SPIx periphrals
 * SPI1, SPI4, and SPI5 are on APB2; SPI2 and SPI3 are on APB1.
 */
#define SPI1_REG_RESET() do{(RCC -> APB2RSTR |= (1 << 12)); (RCC -> APB2RSTR &= ~(1 << 12)); } while(0)
#define SPI2_REG_RESET() do{(RCC -> APB1RSTR |= (1 << 14)); (RCC -> APB1RSTR &= ~(1 << 14)); } while(0)
#define SPI3_REG_RESET() do{(RCC -> APB1RSTR |= (1 << 15)); (RCC -> APB1RSTR &= ~(1 << 15)); } while(0)
#define SPI4_REG_RESET() do{(RCC -> APB2RSTR |= (1 << 13)); (RCC -> APB2RSTR &= ~(1 << 13)); } while(0)
#define SPI5_REG_RESET() do{(RCC -> APB2RSTR |= (1 << 20)); (RCC -> APB2RSTR &= ~(1 << 20)); } while(0)


/*
 * Macros to reset I2CX peripherals
 * I2C1, I2C2, and I2C3 are on APB1.
 */
#define I2C1_REG_RESET() do{(RCC -> APB1RSTR |= (1 << 21)); (RCC -> APB1RSTR &= ~(1 << 21)); } while(0)
#define I2C2_REG_RESET() do{(RCC -> APB1RSTR |= (1 << 22)); (RCC -> APB1RSTR &= ~(1 << 22)); } while(0)
#define I2C3_REG_RESET() do{(RCC -> APB1RSTR |= (1 << 23)); (RCC -> APB1RSTR &= ~(1 << 23)); } while(0)


/*
 * Macro to convert GPIO base address to port code for SYSCFG_EXTICR
 * Returns: 0 for GPIOA, 1 for GPIOB, 2 for GPIOC, 3 for GPIOD, 4 for GPIOE, 7 for GPIOH
 */
#define GPIO_BASEADDR_TO_CODE(x)  ( (x == GPIOA) ? 0 :\
									(x == GPIOB) ? 1 :\
									(x == GPIOC) ? 2 :\
									(x == GPIOD) ? 3 :\
									(x == GPIOE) ? 4 :\
									(x == GPIOH) ? 7 : -1 )

/*
 * ARM Cortex-M4 Processor NVIC ISERx (Interrupt Set-Enable Register) addresses
 */
#define NVIC_ISER0		((__vo uint32_t*)0xE000E100)
#define NVIC_ISER1		((__vo uint32_t*)0xE000E104)
#define NVIC_ISER2		((__vo uint32_t*)0xE000E108)
#define NVIC_ISER3		((__vo uint32_t*)0xE000E10C)

/*
 * ARM Cortex-M4 Processor NVIC ICERx (Interrupt Clear-Enable Register) addresses
 */
#define NVIC_ICER0		((__vo uint32_t*)0xE000E180)
#define NVIC_ICER1		((__vo uint32_t*)0xE000E184)
#define NVIC_ICER2		((__vo uint32_t*)0xE000E188)
#define NVIC_ICER3		((__vo uint32_t*)0xE000E18C)

/*
 * ARM Cortex-M4 Processor NVIC Priority register base address
 */
#define NVIC_PR_BASE_ADDR	((__vo uint32_t*)0xE000E400)

/*
 * ARM Cortex-M4 Processor number of priority bits implemented in Priority register
 */
#define NO_PR_BITS_IMPLEMENTED	4

/*
 * EXTIx IRQ (Interrupt Request) Numbers for STM32F411xx
 */
#define IRQ_NO_EXTI0		6
#define IRQ_NO_EXTI1		7
#define IRQ_NO_EXTI2		8
#define IRQ_NO_EXTI3		9
#define IRQ_NO_EXTI4		10
#define IRQ_NO_EXTI9_5		23
#define IRQ_NO_EXTI15_10	40


/*
 * USARTx IRQ (Interrupt Request) Numbers for STM32F411xx
 */
#define IRQ_NO_USART1		37
#define IRQ_NO_USART2		38
#define IRQ_NO_USART6		71


/*
 * SPIx IRQ (Interrupt Request ) Numbers for STM32F411xx
 */
#define IRQ_NO_SPI1			35
#define IRQ_NO_SPI2			36
#define IRQ_NO_SPI3			51
#define IRQ_NO_SPI4			84
#define IRQ_NO_SPI5			85


/*
 * I2Cx IRQ (Interrupt Request) Numbers for STM32F411xx
 * Each I2C instance has two IRQ lines:
 *   EV = event interrupt (SB, ADDR, BTF, TXE, RXNE, STOPF, etc.)
 *   ER = error interrupt (BERR, ARLO, AF, OVR, TIMEOUT, etc.)
 */
#define IRQ_NO_I2C1_EV      31
#define IRQ_NO_I2C1_ER      32
#define IRQ_NO_I2C2_EV      33
#define IRQ_NO_I2C2_ER      34
#define IRQ_NO_I2C3_EV      72
#define IRQ_NO_I2C3_ER      73


/*
 * IRQ Priority levels
 */
#define NVIC_IRQ_PRI0		0
#define NVIC_IRQ_PRI1		1
#define NVIC_IRQ_PRI2		2
#define NVIC_IRQ_PRI3		3
#define NVIC_IRQ_PRI4		4
#define NVIC_IRQ_PRI5		5
#define NVIC_IRQ_PRI6		6
#define NVIC_IRQ_PRI7		7
#define NVIC_IRQ_PRI8		8
#define NVIC_IRQ_PRI9		9
#define NVIC_IRQ_PRI10		10
#define NVIC_IRQ_PRI11		11
#define NVIC_IRQ_PRI12		12
#define NVIC_IRQ_PRI13		13
#define NVIC_IRQ_PRI14		14
#define NVIC_IRQ_PRI15		15

/*************************************************************************************
 * Bit position definition of UART/USART peripheral
 *************************************************************************************/

/*
 * Bit position definition for USART_SR (Status Register)
 */
#define USART_SR_PE			0	// Parity error
#define USART_SR_FE			1	// Framing error
#define USART_SR_NF			2	// Noise detected flag
#define USART_SR_ORE		3	// Overrun error
#define USART_SR_IDLE		4	// Idle line detected
#define USART_SR_RXNE		5	// Read data register not empty
#define USART_SR_TC         6   // Transmission complete
#define USART_SR_TXE        7   // Transmit data register empty
#define USART_SR_LBD        8   // LIN break detection flag
#define USART_SR_CTS        9   // CTS flag


/*
 * Bit position definition for USART_BRR (Baud Rate Register)
 * USARTDIV = BRR[15:4] (mantissa) + BRR[3:0] (fraction)
 * When OVER8=1, BRR[2:0] holds fraction;
 * BRR[3] must be kept 0
 */
#define USART_BRR_DIV_FRACTION 0 	// Bits [3:0] - fraction of USARTDIV
#define USART_BRR_DIV_MANTISSA 4    // Bits [15:4] - mantissa of USARTDIV


/*
 * Bit position definition for USART_CR1 (USART_Control Register 1)
 */
#define USART_CR1_SBK		0	//Send break
#define USART_CR1_RWU		1	//Receiver wakeup
#define USART_CR1_RE		2	//Receiver enable
#define USART_CR1_TE		3	//Transmitter enable
#define USART_CR1_IDLEIE	4	//IDLE interrupt enable
#define USART_CR1_RXNEIE	5	//RXNE interrupt enable
#define USART_CR1_TCIE		6	//Transmission complete interrupt enable
#define USART_CR1_TXEIE		7	//TXE interrupt enable
#define USART_CR1_PEIE		8	//PE interrupt enable
#define USART_CR1_PS        9   // Parity selection (0=even, 1=odd)
#define USART_CR1_PCE       10  // Parity control enable
#define USART_CR1_WAKE      11  // Wakeup method
#define USART_CR1_M         12  // Word length (0=8-bit, 1=9-bit)
#define USART_CR1_UE        13  // USART enable
#define USART_CR1_OVER8     15  // Oversampling mode (0=16x, 1=8x)


/*
 * Bit position definitions for USART_CR2 (USART_Control Register 2)
 */
#define USART_CR2_ADD       0   // Bits [3:0] - Address of the USART node
#define USART_CR2_LBDL      5   // LIN break detection length
#define USART_CR2_LBDIE     6   // LIN break detection interrupt enable
#define USART_CR2_LBCL      8   // Last bit clock pulse
#define USART_CR2_CPHA      9   // Clock phase
#define USART_CR2_CPOL      10  // Clock polarity
#define USART_CR2_CLKEN     11  // Clock enable (synchronous mode)
#define USART_CR2_STOP      12  // Bits [13:12] - STOP bits (00=1, 01=0.5, 10=2, 11=1.5)
#define USART_CR2_LINEN     14  // LIN mode enable


/*
 * Bit position definitions for USART_CR3 (USART_Control Register 3)
 */
#define USART_CR3_EIE       0   // Error interrupt enable (for FE, NF, ORE in DMA mode)
#define USART_CR3_IREN      1   // IrDA mode enable
#define USART_CR3_IRLP      2   // IrDA low-power mode
#define USART_CR3_HDSEL     3   // Half-duplex selection (single-wire mode)
#define USART_CR3_NACK      4   // Smartcard NACK enable
#define USART_CR3_SCEN      5   // Smartcard mode enable
#define USART_CR3_DMAR      6   // DMA enable receiver
#define USART_CR3_DMAT      7   // DMA enable transmitter
#define USART_CR3_RTSE      8   // RTS enable
#define USART_CR3_CTSE      9   // CTS enable
#define USART_CR3_CTSIE     10  // CTS interrupt enable
#define USART_CR3_ONEBIT    11  // One sample bit method enable


/*************************************************************************************
 * Bit position definition of SPI peripheral
 *************************************************************************************/

/*
 * Bit position definition for SPI_CR1 (SPI_Control Register 1)
 */
#define SPI_CR1_CPHA		0	// Clock phase
#define SPI_CR1_CPOL		1	// Clock polarity
#define SPI_CR1_MSTR		2	// Master select (0: Slave configuration, 1: Master configuration)
#define SPI_CR1_BR			3	// Baud rate control
#define SPI_CR1_SPE			6	// SPI enable
#define SPI_CR1_LSBFIRST	7	// Frame format (0: MSB transmitted first, 1: LSB transmitted first)
#define SPI_CR1_SSI			8 	// Internal slave select
#define SPI_CR1_SSM			9	// Software slave management
#define SPI_CR1_RXONLY		10	// Revive only
#define SPI_CR1_DFF			11  // Data frame format
#define SPI_CR1_CRCNEXT		12	// CRC transfer next
#define SPI_CR1_CRCEN		13  // Hardware CRC calculation enable
#define SPI_CR1_BIDIOE		14	// Output enable in bidirectional mode
#define SPI_CR1_BIDIMODE	15  // Bidirectional data mode enable


/*
 * Bit position definition for SPI_CR2 (SPI_Control Register 2)
 */
#define SPI_CR2_RXDMAEN		0	// Rx buffer DMA enable
#define SPI_CR2_TXDMAEN		1 	// Tx buffer DMA enable
#define SPI_CR2_SSOE		2	// SS output enable
#define SPI_CR2_FRF			4	// Frame format
#define SPI_CR2_ERRIE		5	// Error interrupt enable
#define SPI_CR2_RXNEIE		6	// RX buffer not empty interrupt enable
#define SPI_CR2_TXEIE		7 	// TX buffer empty interrupt enable


/*
 * Bit position definition for SPI_SR (Status Register)
 */
#define SPI_SR_RXNE 		0	// Receive buffer not empty
#define SPI_SR_TXE			1	// Transmit buffer empty
#define SPI_SR_CHSIDE		2	// Channel side
#define SPI_SR_UDR			3	// Underrun flag
#define SPI_SR_CRCERR		4	// CRC error flag
#define SPI_SR_MODF			5	// Mode fault
#define SPI_SR_OVR			6	// Overrun flag
#define SPI_SR_BSY			7	// Busy flag
#define SPI_SR_FRE			8	// Frame format error


/*************************************************************************************
 * Bit position definition of I2C peripheral
 *************************************************************************************/

/*
 * Bit position definition for I2C_CR1 (I2C_Control Register 1)
 */
#define I2C_CR1_PE			0	//Peripheral enable
#define I2C_CR1_SMBUS		1	//SMBus mode
#define I2C_CR1_SMBTYPE		3	//SMBus type
#define I2C_CR1_ENARP		4	//ARP enable
#define I2C_CR1_ENPEC		5	//PEC enable
#define I2C_CR1_ENGC		6	//General call enable
#define I2C_CR1_NOSTRETCH	7	//Clock stretching disable(Target mode)
#define I2C_CR1_START		8	//Start generation
#define I2C_CR1_STOP		9	//Stop generation
#define I2C_CR1_ACK			10	//Acknowledge enable
#define I2C_CR1_POS			11	//Acknowledge/PEC position
#define I2C_CR1_PEC			12 	//Packet error checking
#define I2C_CR1_ALERT		13	//SMBus alert
#define I2C_CR1_SWRST		15	//Software reset


/*
 * Bit position definition for I2C_CR2 (I2C_Control Register 2)
 */
#define I2C_CR2_FREQ		0	//Peripheral clock frequency [5:0]
#define I2C_CR2_ITERREN		8	//Error interrupt enable
#define I2C_CR2_ITEVTEN		9	//Event interrupt enable
#define I2C_CR2_ITBUFEN		10	//Buffer interrupt enable
#define I2C_CR2_DMAEN		11	//DMA requests enable
#define I2C_CR2_LAST		12	//DMA last trasnfer


/*
 * Bit position definition for I2C_OAR1 (I2C_Own address register 1)
 */
#define I2C_OAR1_ADD0		0	//Interface address
#define I2C_OAR1_ADD71		1	//Interface address [7:1]
#define I2C_OAR1_ADD98		8	//Interface address [9:8]
#define I2C_OAR1_ADDMODE	15	//Addressing mode (target mode)


/*
 * Bit position definition for I2C_OAR2 (I2C_Own address register 2)
 */
#define I2C_OAR2_ENDUAL		0	//Dual addressing mode enable
#define I2C_OSR2_ADD2		1	//Interface address


/*
 * Bit position definition for I2C_SR1 (I2C_Status Register)
 */
#define I2C_SR1_SB			0	//Start bit (Controller mode)
#define I2C_SR1_ADDR		1	//Address sent (Controller mode)/matched(target mode)
#define I2C_SR1_BTF			2	//Byte transfer finished
#define I2C_SR1_ADD10		3	//10-bit header
#define I2C_SR1_STOPF		4	//Stop detection(target mode)
#define I2C_SR1_RXNE		6	//Data register not empty
#define I2C_SR1_TXE			7	//Data register empty (transmitters)
#define I2C_SR1_BERR		8	//Bus error
#define I2C_SR1_ARLO		9	//Arbitration lost(controller mode)
#define I2C_SR1_AF			10	//Acknowledge failure
#define I2C_SR1_OVR			11	//Overrun/Underrun
#define I2C_SR1_PECERR		12	//PEC Error in reception
#define I2C_SR1_TIMEOUT     14  // Timeout or Tlow error (SMBus; SCL held low too long)

/*
 * Bit position definition for I2C_SR2 (I2C_Status Register 2)
 * SR2 is always read after SR1 — reading SR1+SR2 together clears the ADDR flag.
 * SR2 is read-only; no flags are cleared by writing to it.
 */
#define I2C_SR2_MSL         0   // Master/Slave        : 1=Master, 0=Slave
#define I2C_SR2_BUSY        1   // Bus busy            : 1=communication ongoing on the bus
#define I2C_SR2_TRA         2   // Transmitter/Receiver: 1=Transmitting, 0=Receiving
#define I2C_SR2_GENCALL     4   // General call address: 1=General call received (slave mode)
#define I2C_SR2_SMBDEFAULT  5   // SMBus device default address (SMBus mode only)
#define I2C_SR2_SMBHOST     6   // SMBus host header   (SMBus mode only)
#define I2C_SR2_DUALF       7   // Dual flag           : 1=Address matched OAR2, 0=matched OAR1
#define I2C_SR2_PEC         8   // Bits [15:8] — Packet error checking register value


/*
 * Bit position definition for I2C_CCR (I2C_Clock Control Register)
 * CCR must be configured while PE=0 (peripheral disabled).
 *
 * CCR[11:0] — Clock control value:
 *   SM : T_high = T_low = CCR × T_PCLK1   → CCR = F_PCLK1 / (2 × F_SCL)
 *   FM DUTY=0 : T_low=2×CCR×T_PCLK1, T_high=CCR×T_PCLK1  → CCR = F_PCLK1 / (3 × F_SCL)
 *   FM DUTY=1 : T_low=16×CCR×T_PCLK1, T_high=9×CCR×T_PCLK1 → CCR = F_PCLK1 / (25 × F_SCL)
 */
#define I2C_CCR_CCR         0   // Bits [11:0] — Clock control divider value
#define I2C_CCR_DUTY        14  // Fast Mode duty cycle: 0 = t_low/t_high=2, 1 = t_low/t_high=16/9
#define I2C_CCR_FS          15  // Mode select: 0 = Standard Mode (≤100kHz), 1 = Fast Mode (≤400kHz)


/*
 * Some Generic Macros
 */
#define ENABLE		 	1
#define DISABLE 		0
#define SET  			ENABLE
#define RESET 			DISABLE
#define GPIO_PIN_SET	1//SET
#define GPIO_PIN_RESET	0//RESET
#define FLAG_SET		1//SET
#define FLAG_RESET 		0//RESET

#include "stm32f411xx_gpio_driver.h"
#include "stm32f411xx_usart_driver.h"
#include "stm32f411xx_spi_driver.h"
#include "stm32f411xx_i2c_driver.h"
#include "stm32f411xx_rcc_driver.h"

#endif /* INC_STM32F411XX_H_ */
