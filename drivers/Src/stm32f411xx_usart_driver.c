/*
 * stm32f411xx_usart_driver.c
 *
 *  Created on: 11-Apr-2026
 *  Author: Abhishek_Bharadwaj
 */


#include "stm32f411xx.h"
#include "stm32f411xx_usart_driver.h"


/*********************************************************************
 * @fn      		  - USART_Init
 *
 * @brief             - Initialises all USART registers (CR1, CR2, CR3, BRR).
 *                      Does NOT set the UE bit — call USART_PeripheralControl(ENABLE)
 *                      after this function because BRR must be written while UE = 0.
 *
 * @param[in]         - pUSARTHandle : pointer to the fully populated handle struct
 *
 * @return            - none
 *
 * @Note              -
 */
void USART_Init(USART_Handle_t *pUSARTHandle)
{

	//Temporary variable
	uint32_t tempreg=0;
/*****************************************************************************
 * Enable peripheral clock
 * USART1/USART6 sit on APB2; USART2 sits on APB1.
 * The clock-enable macros
 */
/******************************** Configuration of CR1******************************************/


	//Enable USART Tx and Rx engines according to the USART_Mode configuration item
	if ( pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_RX)
	{
		//Implement the code to enable the Receiver bit field
		tempreg|= (1 << TODO);
	}else if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_TX)
	{
		//Implement the code to enable the Transmitter bit field
		tempreg |= ( 1 << TODO );

	}else if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_TXRX)
	{
		//Implement the code to enable the both Transmitter and Receiver bit fields
		tempreg |= ( ( 1 << TODO) | ( 1 << TODO) );
	}

    //Implement the code to configure the Word length configuration item
	tempreg |= pUSARTHandle->USART_Config.USART_WordLength << TODO ;


    //Configuration of parity control bit fields
	if ( pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_EVEN)
	{
		//Implement the code to enale the parity control
		tempreg |= ( 1 << TODO);

		//Implement the code to enable EVEN parity
		//Not required because by default EVEN parity will be selected once you enable the parity control

	}else if (pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_ODD )
	{
		//Implement the code to enable the parity control
	    tempreg |= ( 1 << USART_CR1_PCE);

	    //Implement the code to enable ODD parity
	    tempreg |= ( 1 << TODO);

	}

   //Program the CR1 register
	pUSARTHandle->pUSARTx->CR1 = TODO;

/******************************** Configuration of CR2******************************************/

	tempreg=0;

	//Implement the code to configure the number of stop bits inserted during USART frame transmission
	tempreg |= pUSARTHandle->USART_Config.TODO << TODO;

	//Program the CR2 register
	pUSARTHandle->pUSARTx->TODO = tempreg;

/******************************** Configuration of CR3******************************************/

	tempreg=0;

	//Configuration of USART hardware flow control
	if ( pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS)
	{
		//Implement the code to enable CTS flow control
		tempreg |= ( 1 << TODO);


	}else if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_RTS)
	{
		//Implement the code to enable RTS flow control
		tempreg |= TODO

	}else if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS_RTS)
	{
		//Implement the code to enable both CTS and RTS Flow control
		TODO
	}


	pUSARTHandle->pUSARTx->TODO = tempreg;

/******************************** Configuration of BRR(Baudrate register)******************************************/

	//Implement the code to configure the baud rate
	//We will cover this in the lecture. No action required here

}
