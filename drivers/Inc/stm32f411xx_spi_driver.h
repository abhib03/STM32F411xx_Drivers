/*
 * stm32f411xx_spi_driver.h
 *
 *  Created on: 27-Apr-2026
 *      Author: ASUS
 */

#ifndef INC_STM32F411XX_SPI_DRIVER_H_
#define INC_STM32F411XX_SPI_DRIVER_H_

#include "stm32f411xx.h"

/*
 * Configuration structure for SPIx peripheral
 */
typedef struct
{
	uint8_t	SPI_DeviceMode;
	uint8_t SPI_BusConfig;
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_CPHA;
	uint8_t SPI_CPOL;
	uint8_t SPI_SSM;
}SPI_Config_t;

/*
 * Handle structure for SPIx peripheral
 */
typedef struct
{
	SPI_RegDef_t 	*pSPIx;	 /*! < This holds the base address of SPIx(x:0,1,2,3,4) peripheral> */
	SPI_Config_t  	SPIConfig;
	uint8_t		 	*pTxBuffer; /*! < This store the app. Tx buffer address > */
	uint8_t		 	*pRxBuffer; /*! < This store the app. Rx buffer address > */
	uint32_t 		TxLen;		/*! < To store Tx len > */
	uint32_t 		RxLen;		/*! < To store Rx len > */
	uint8_t 		TxState;	/*! < To store Tx state > */
	uint8_t 		RxState;	/*! < To store Rx state > */
}SPI_Handle_t;


/*
 * @SPI_DeviceMode
 * possible SPI device modes
 */
#define SPI_DEVICE_MODE_SLAVE			0
#define SPI_DEVICE_MODE_MASTER			1


/*
 * @SPI_BusConfig
 * SPI bus cofiguration : Full-duplex, Half-duplex, Simplex(Transmission-only/Reception-only)
 */
#define SPI_BUS_CONFIG_FD				1
#define SPI_BUS_CONFIG_HD				2
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY 	3


/*
 * @SPI_SclkSpeed
 * possible SPI clock speed or baud rate
 */
#define  SPI_SCLK_SPEED_DIV2			0
#define  SPI_SCLK_SPEED_DIV4			1
#define  SPI_SCLK_SPEED_DIV8			2
#define  SPI_SCLK_SPEED_DIV16			3
#define  SPI_SCLK_SPEED_DIV32			4
#define  SPI_SCLK_SPEED_DIV64			5
#define  SPI_SCLK_SPEED_DIV128			6
#define  SPI_SCLK_SPEED_DIV256			7


/*
 * @SPI_DFF
 * SPI data frame format
 */
#define SPI_DFF_8BITS 		0
#define SPI_DFF_16BITS		1


/*
 * @SPI_CPOL (Clock Polarity): Defines idle state of clock
 * CPOL = 0 → Clock idle LOW
 * CPOL = 1 → Clock idle HIGH
 */
#define SPI_CPOL_LOW 		0
#define SPI_CPOL_HIGH 		1


/*
 * @SPI_CPHA (Clock Phase): Defines sampling edge
 * CPHA = 0 → Sample on first edge
 * CPHA = 1 → Sample on second edge
 */
#define SPI_CPHA_LOW 		0
#define SPI_CPHA_HIGH 		1


/*
 * @SPI_SSM (Slave Select Management
 */
#define SPI_SSM_DI			0
#define SPI_SSM_EN			1

/*
 * SPI related status flags definition
 */
#define SPI_TXE_FLAG 	(1 << SPI_SR_TXE)
#define SPI_RXNE_FLAG 	(1 << SPI_SR_RXNE)
#define SPI_BUSY_FLAG 	(1 << SPI_SR_BSY)
#define SPI_OVR_FLAG	(1 << SPI_SR_OVR)


/*
 * Possible SPI application states
 */
#define SPI_READY		0
#define SPI_BUSY_IN_RX	1
#define SPI_BUSY_IN_TX	2


/*
 * Possible SPI application events
 */
#define SPI_EVENT_TX_CMPLT	1
#define SPI_EVENT_RX_CMPLT	2
#define SPI_EVENT_OVR_ERR	3
#define SPI_EVENT_CRC_ERR	4


/*************************************************************************************
 * 					APIs supported by this driver
 * 	For more information about the APIs check the function definitions
 *************************************************************************************/
/*
 * Peripheral Clock Setup
 */
void SPI_PeriClkControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);

/*
 * Init and De-init
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);

/*
 * Data Read and Write
*/
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len);
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len);

/*
 * IRQ Configuration and ISR Handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pHandle);

/*
 * Other Peripheral Control APIs
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName);

#endif /* INC_STM32F411XX_SPI_DRIVER_H_ */
