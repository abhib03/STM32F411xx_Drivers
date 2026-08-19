/*
 * stm32f411xx_usart_driver.c
 *
 *  Created on: 18-Aug-2026
 *  Author: Abhishek_Bharadwaj
 *
 *  USART driver implementation for STM32F411xx
 */

#include "stm32f411xx.h"
#include "stm32f411xx_usart_driver.h"
#include "stm32f411xx_rcc_driver.h"

/*********************************************************************
 * @fn              - USART_SetBaudRate
 *
 * @brief           - Configures the USART baud rate
 *
 * @param[in]       - pUSARTx : Pointer to USART peripheral
 * @param[in]       - BaudRate : Desired baud rate
 *
 * @return          - None
 *
 * @Note            - Configures the USART_BRR register based on
 *                    APB clock and OVER8 configuration.
 *
 *********************************************************************/
void USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t BaudRate)
{
    uint32_t PCLKx;
    uint32_t usartdiv;

    uint32_t M_part;
    uint32_t F_part;

    uint32_t tempreg = 0;


    /*
     * USART1 and USART6 are connected to APB2.
     * USART2, USART3, UART4 and UART5 are connected to APB1.
     */
    if(pUSARTx == USART1 || pUSARTx == USART6)
    {
        PCLKx = RCC_GetPCLK2Value();
    }
    else
    {
        PCLKx = RCC_GetPCLK1Value();
    }


    /*
     * Check OVER8 configuration bit.
     *
     * OVER8 = 1 -> Oversampling by 8
     * OVER8 = 0 -> Oversampling by 16
     */
    if(pUSARTx->CR1 & (1 << USART_CR1_OVER8))
    {
        /*
         * Oversampling by 8
         */
        usartdiv = ((25 * PCLKx) / (2 * BaudRate));
    }
    else
    {
        /*
         * Oversampling by 16
         */
        usartdiv = ((25 * PCLKx) / (4 * BaudRate));
    }


    /*
     * Calculate Mantissa.
     */
    M_part = usartdiv / 100;


    /*
     * Place Mantissa in BRR[15:4].
     */
    tempreg |= M_part << 4;


    /*
     * Extract fractional part.
     */
    F_part = usartdiv - (M_part * 100);


    /*
     * Calculate final fractional value.
     */
    if(pUSARTx->CR1 & (1 << USART_CR1_OVER8))
    {
        /*
         * Oversampling by 8.
         * Fraction is 3 bits.
         */
        F_part = (((F_part * 8) + 50) / 100);

        F_part &= (uint8_t)0x07;
    }
    else
    {
        /*
         * Oversampling by 16.
         * Fraction is 4 bits.
         */
        F_part = (((F_part * 16) + 50) / 100);

        F_part &= (uint8_t)0x0F;
    }


    /*
     * Place Fraction in BRR.
     */
    tempreg |= F_part;


    /*
     * Write final value to BRR.
     */
    pUSARTx->BRR = tempreg;
}


/*********************************************************************
 * @fn              - USART_Init
 *
 * @brief           - Initializes the USART peripheral according to
 *                    the configuration specified in the handle.
 *
 * @param[in]       - pUSARTHandle : Pointer to USART handle
 *
 * @return          - None
 *
 * @Note            - USART peripheral must be enabled separately using
 *                    USART_PeripheralControl().
 *
 *********************************************************************/
void USART_Init(USART_Handle_t *pUSARTHandle)
{
    uint32_t tempreg = 0;


    /******************************** Configuration of CR1 ********************************/

    /*
     * Enable clock for USART peripheral.
     */
    USART_PeriClockControl(pUSARTHandle->pUSARTx, ENABLE);

    /*
     * Configure USART mode.
     */
    if(pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_RX)
    {
        /*
         * Enable Receiver.
         */
        tempreg |= (1 << USART_CR1_RE);
    }
    else if(pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_TX)
    {
        /*
         * Enable Transmitter.
         */
        tempreg |= (1 << USART_CR1_TE);
    }
    else if(pUSARTHandle->USART_Config.USART_Mode == USART_MODE_TXRX)
    {
        /*
         * Enable both Transmitter and Receiver.
         */
        tempreg |= ((1 << USART_CR1_RE) |(1 << USART_CR1_TE)
        );
    }

    /*
     * Configure Word Length.
     */
    tempreg |=pUSARTHandle->USART_Config.USART_WordLength << USART_CR1_M;

    /*
     * Configure Parity.
     */
    if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_EVEN)
    {
        /*
         * Enable parity control.
         *
         * PS = 0 by default -> Even parity.
         */
        tempreg |= (1 << USART_CR1_PCE);
    }
    else if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_ODD)
    {
        /*
         * Enable parity control.
         */
        tempreg |= (1 << USART_CR1_PCE);

        /*
         * Select odd parity.
         */
        tempreg |= (1 << USART_CR1_PS);
    }


    /*
     * Program CR1.
     */
    pUSARTHandle->pUSARTx->CR1 = tempreg;

    /******************************** Configuration of CR2 ********************************/

    tempreg = 0;
    /*
     * Configure number of stop bits.
     */
    tempreg |= pUSARTHandle->USART_Config.USART_NoOfStopBits << USART_CR2_STOP;
    /*
     * Program CR2.
     */
    pUSARTHandle->pUSARTx->CR2 = tempreg;

    /******************************** Configuration of CR3 ********************************/

    tempreg = 0;
    /*
     * Configure hardware flow control.
     */
    if(pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS)
    {
        /*
         * Enable CTS flow control.
         */
        tempreg |= (1 << USART_CR3_CTSE);
    }
    else if(pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_RTS)
    {
        /*
         * Enable RTS flow control.
         */
        tempreg |= (1 << USART_CR3_RTSE);
    }
    else if(pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS_RTS)
    {
        /*
         * Enable CTS and RTS flow control.
         */
        tempreg |= ((1 << USART_CR3_CTSE) | (1 << USART_CR3_RTSE));
    }

    /*
     * Program CR3.
     */
    pUSARTHandle->pUSARTx->CR3 = tempreg;


    /******************************** Configuration of BRR ********************************/

    /*
     * Configure baud rate.
     */
    USART_SetBaudRate(pUSARTHandle->pUSARTx, pUSARTHandle->USART_Config.USART_Baud);
}


/*********************************************************************
 * @fn              - USART_PeripheralControl
 *
 * @brief           - Enables or disables USART peripheral.
 *
 * @param[in]       - pUSARTx : Pointer to USART peripheral
 * @param[in]       - Cmd     : ENABLE or DISABLE
 *
 * @return          - None
 *
 * @Note            - Controls the UE bit in USART_CR1.
 *
 *********************************************************************/
void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t Cmd)
{
    if(Cmd == ENABLE)
    {
        /*
         * Enable USART peripheral.
         */
        pUSARTx->CR1 |= (1 << USART_CR1_UE);
    }
    else
    {
        /*
         * Disable USART peripheral.
         */
        pUSARTx->CR1 &= ~(1 << USART_CR1_UE);
    }
}


/*********************************************************************
 * @fn              - USART_PeriClockControl
 *
 * @brief           - Enables or disables the peripheral clock for
 *                    the selected USART/UART peripheral.
 *
 * @param[in]       - pUSARTx : Pointer to USART peripheral
 * @param[in]       - EnorDi  : ENABLE or DISABLE
 *
 * @return          - None
 *
 * @Note            - STM32F411 USART mapping:
 *                    USART1 -> APB2
 *                    USART2 -> APB1
 *                    USART6 -> APB2
 *
 *********************************************************************/
void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        if(pUSARTx == USART1)
        {
            USART1_PCLK_EN();
        }
        else if(pUSARTx == USART2)
        {
            USART2_PCLK_EN();
        }
        else if(pUSARTx == USART6)
        {
            USART6_PCLK_EN();
        }
    }
    else
    {
        if(pUSARTx == USART1)
        {
            USART1_PCLK_DI();
        }
        else if(pUSARTx == USART2)
        {
            USART2_PCLK_DI();
        }
        else if(pUSARTx == USART6)
        {
            USART6_PCLK_DI();
        }
    }
}

/*********************************************************************
 * @fn              - USART_GetFlagStatus
 *
 * @brief           - Returns the status of a USART status flag.
 *
 * @param[in]       - pUSARTx          : Pointer to USART peripheral
 * @param[in]       - StatusFlagName   : USART status flag
 *
 * @return          - SET if flag is set
 *                    RESET if flag is not set
 *
 *********************************************************************/
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx, uint16_t StatusFlagName)
{
    if(pUSARTx->SR & StatusFlagName)
    {
        return SET;
    }
    return RESET;
}


/*********************************************************************
 * @fn              - USART_SendData
 *
 * @brief           - Sends data using polling method.
 *
 * @param[in]       - pUSARTHandle : Pointer to USART handle
 * @param[in]       - pTxBuffer    : Pointer to transmit buffer
 * @param[in]       - Len          : Length of data
 *
 * @return          - None
 *
 * @Note            - Supports 8-bit and 9-bit USART data.
 *
 *********************************************************************/
void USART_SendData(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len)
{
    uint16_t *pdata;

    for(uint32_t i = 0; i < Len; i++)
    {
        /*
         * Wait until TXE is set.
         */
        while(!USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TXE));
        /*
         * Check word length.
         */
        if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
        {
            /*
             * 9-bit data.
             */
            pdata = (uint16_t *)pTxBuffer;

            pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x01FF);

            if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
            {
                /*
                 * No parity.
                 * All 9 bits are user data.
                 */
                pTxBuffer += 2;
            }
            else
            {
                /*
                 * Parity enabled.
                 * 8 bits are user data.
                 */
                pTxBuffer++;
            }
        }
        else
        {
            /*
             * 8-bit data.
             */
            pUSARTHandle->pUSARTx->DR = (*pTxBuffer & (uint8_t)0xFF);

            pTxBuffer++;
        }
    }

    /*
     * Wait until transmission is complete.
     */
    while(!USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TC));
}


/*********************************************************************
 * @fn              - USART_ReceiveData
 *
 * @brief           - Receives data using polling method.
 *
 * @param[in]       - pUSARTHandle : Pointer to USART handle
 * @param[in]       - pRxBuffer    : Pointer to receive buffer
 * @param[in]       - Len          : Length of data
 *
 * @return          - None
 *
 * @Note            - Supports 8-bit and 9-bit USART data.
 *
 *********************************************************************/
void USART_ReceiveData(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len)
{
    for(uint32_t i = 0; i < Len; i++)
    {
        /*
         * Wait until RXNE is set.
         */
        while(!USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_RXNE));

        /*
         * Check word length.
         */
        if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
        {
            if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
            {
                /*
                 * 9-bit data.
                 */
                *((uint16_t *)pRxBuffer) = (pUSARTHandle->pUSARTx->DR & (uint16_t)0x01FF);

                pRxBuffer += 2;
            }
            else
            {
                /*
                 * 8-bit data + parity.
                 */
                *pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR & (uint8_t)0xFF);

                pRxBuffer++;
            }
        }
        else
        {
            if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
            {
                /*
                 * 8-bit data.
                 */
                *pRxBuffer =(uint8_t)(pUSARTHandle->pUSARTx->DR & (uint8_t)0xFF );
            }
            else
            {
                /*
                 * 7-bit data + parity.
                 */
                *pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR & (uint8_t)0x7F);
            }
            pRxBuffer++;
        }
    }
}


/*********************************************************************
 * @fn              - USART_SendDataIT
 *
 * @brief           - Sends data using interrupt method.
 *
 * @param[in]       - pUSARTHandle : Pointer to USART handle
 * @param[in]       - pTxBuffer    : Pointer to transmit buffer
 * @param[in]       - Len          : Length of data
 *
 * @return          - Previous TxBusyState
 *
 *********************************************************************/
uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len)
{
    uint8_t txstate = pUSARTHandle->TxBusyState;

    if(txstate != USART_BUSY_IN_TX)
    {
        /*
         * Save transmission information.
         */
        pUSARTHandle->TxLen = Len;
        pUSARTHandle->pTxBuffer = pTxBuffer;

        pUSARTHandle->TxBusyState = USART_BUSY_IN_TX;

        /*
         * Enable TXE interrupt.
         */
        pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_TXEIE);

        /*
         * Enable TC interrupt.
         */
        pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_TCIE);
    }


    return txstate;
}


/*********************************************************************
 * @fn              - USART_ReceiveDataIT
 *
 * @brief           - Receives data using interrupt method.
 *
 * @param[in]       - pUSARTHandle : Pointer to USART handle
 * @param[in]       - pRxBuffer    : Pointer to receive buffer
 * @param[in]       - Len          : Length of data
 *
 * @return          - Previous RxBusyState
 *
 *********************************************************************/
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len)
{
    uint8_t rxstate = pUSARTHandle->RxBusyState;

    if(rxstate != USART_BUSY_IN_RX)
    {
        /*
         * Save reception information.
         */
        pUSARTHandle->RxLen = Len;
        pUSARTHandle->pRxBuffer = pRxBuffer;

        pUSARTHandle->RxBusyState = USART_BUSY_IN_RX;
        /*
         * Clear any stale RXNE condition by reading DR.
         */
        (void)pUSARTHandle->pUSARTx->DR;

        /*
         * Enable RXNE interrupt.
         */
        pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_RXNEIE);
    }
    return rxstate;
}


/*********************************************************************
 * @fn              - USART_ClearFlag
 *
 * @brief           - Clears USART status flag.
 *
 * @param[in]       - pUSARTx          : Pointer to USART peripheral
 * @param[in]       - StatusFlagName   : Status flag
 *
 * @return          - None
 *
 * @Note            - Application should use this API carefully because
 *                    some USART flags require specific clear sequences.
 *
 *********************************************************************/
void USART_ClearFlag(USART_RegDef_t *pUSARTx, uint16_t StatusFlagName)
{
    pUSARTx->SR &= ~(StatusFlagName);
}


/*********************************************************************
 * @fn              - USART_IRQInterruptConfig
 *
 * @brief           - Enables or disables USART interrupt in NVIC.
 *
 * @param[in]       - IRQNumber : USART IRQ number
 * @param[in]       - EnorDi    : ENABLE or DISABLE
 *
 * @return          - None
 *
 *********************************************************************/
void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        if(IRQNumber <= 31)
        {
            /*
             * ISER0 : IRQ 0 - 31
             */
            *NVIC_ISER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber > 31 && IRQNumber < 64)
        {
            /*
             * ISER1 : IRQ 32 - 63
             */
            *NVIC_ISER1 |= (1 << (IRQNumber % 32));
        }
        else if(IRQNumber >= 64 && IRQNumber < 96)
        {
            /*
             * ISER2 : IRQ 64 - 95
             */
            *NVIC_ISER2 |= (1 << (IRQNumber % 32));
        }
    }
    else
    {
        if(IRQNumber <= 31)
        {
            /*
             * ICER0 : IRQ 0 - 31
             */
            *NVIC_ICER0 = (1 << IRQNumber);
        }
        else if(IRQNumber > 31 && IRQNumber < 64)
        {
            /*
             * ICER1 : IRQ 32 - 63
             */
            *NVIC_ICER1 = (1 << (IRQNumber % 32));
        }
        else if(IRQNumber >= 64 && IRQNumber < 96)
        {
            /*
             * ICER2 : IRQ 64 - 95
             */
            *NVIC_ICER2 = (1 << (IRQNumber % 32));
        }
    }
}


/*********************************************************************
 * @fn              - USART_IRQPriorityConfig
 *
 * @brief           - Configures USART interrupt priority.
 *
 * @param[in]       - IRQNumber   : USART IRQ number
 * @param[in]       - IRQPriority : Interrupt priority
 *
 * @return          - None
 *
 *********************************************************************/
void USART_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
    uint8_t iprx;
    uint8_t iprx_section;
    uint8_t shift_amount;

    /*
     * Find IPR register.
     */
    iprx = IRQNumber / 4;

    /*
     * Find section within IPR register.
     */
    iprx_section = IRQNumber % 4;

    /*
     * Calculate shift amount.
     */
    shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

    /*
     * Configure priority.
     */
    *(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}


/*********************************************************************
 * @fn              - USART_IRQHandling
 *
 * @brief           - Handles USART interrupts.
 *
 * @param[in]       - pUSARTHandle : Pointer to USART handle
 *
 * @return          - None
 *
 * @Note            - Handles:
 *                    TXE
 *                    TC
 *                    RXNE
 *                    CTS
 *                    IDLE
 *                    ORE
 *                    FE
 *                    NE
 *
 *********************************************************************/
void USART_IRQHandling(USART_Handle_t *pUSARTHandle)
{
    uint32_t temp1;
    uint32_t temp2;

    uint16_t *pdata;


    /************************* Check for TC flag *************************/

    temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_TC);
    temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_TCIE);

    if(temp1 && temp2)
    {
        if(pUSARTHandle->TxBusyState == USART_BUSY_IN_TX)
        {
            if(pUSARTHandle->TxLen == 0)
            {
                /*
                 * Disable TC interrupt.
                 */
                pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_TCIE);

                /*
                 * Clear TC flag.
                 *
                 * TC is cleared by reading SR followed
                 * by writing DR. Since transmission is
                 * already complete, writing zero to SR
                 * is also used here for this driver.
                 */
                pUSARTHandle->pUSARTx->SR &= ~(1 << USART_SR_TC);

                /*
                 * Reset transmission state.
                 */
                pUSARTHandle->TxBusyState = USART_READY;
                pUSARTHandle->pTxBuffer = NULL;
                pUSARTHandle->TxLen = 0;

                /*
                 * Notify application.
                 */
                USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_TX_CMPLT);
            }
        }
    }


    /************************* Check for TXE flag *************************/
    temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_TXE);
    temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_TXEIE);

    if(temp1 && temp2)
    {
        if(pUSARTHandle->TxBusyState == USART_BUSY_IN_TX)
        {
            if(pUSARTHandle->TxLen > 0)
            {
                /*
                 * 9-bit transfer.
                 */
                if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
                {
                    pdata = (uint16_t *)pUSARTHandle->pTxBuffer;
                    pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x01FF);

                    if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
                    {
                        /*
                         * 9-bit user data.
                         */
                        pUSARTHandle->pTxBuffer += 2;
                        pUSARTHandle->TxLen -= 2;
                    }
                    else
                    {
                        /*
                         * 8-bit data + parity.
                         */
                        pUSARTHandle->pTxBuffer++;
                        pUSARTHandle->TxLen--;
                    }
                }
                else
                {
                    /*
                     * 8-bit transfer.
                     */
                    pUSARTHandle->pUSARTx->DR = (*pUSARTHandle->pTxBuffer & (uint8_t)0xFF);

                    pUSARTHandle->pTxBuffer++;
                    pUSARTHandle->TxLen--;
                }
            }


            /*
             * Disable TXE interrupt once all data
             * has been loaded into DR.
             *
             * TC interrupt remains enabled so that
             * final transmission completion can be
             * detected.
             */
            if(pUSARTHandle->TxLen == 0)
            {
                pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_TXEIE);
            }
        }
    }


    /************************* Check for RXNE flag *************************/

    temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_RXNE);
    temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_RXNEIE);

    if(temp1 && temp2)
    {
        if(pUSARTHandle->RxBusyState == USART_BUSY_IN_RX)
        {
            if(pUSARTHandle->RxLen > 0)
            {
                /*
                 * 9-bit transfer.
                 */
                if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
                {
                    if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
                    {
                        /*
                         * 9-bit user data.
                         */
                        *((uint16_t *)pUSARTHandle->pRxBuffer) = (pUSARTHandle->pUSARTx->DR & (uint16_t)0x01FF);

                        pUSARTHandle->pRxBuffer += 2;
                        pUSARTHandle->RxLen -= 2;
                    }
                    else
                    {
                        /*
                         * 8-bit data + parity.
                         */
                        *pUSARTHandle->pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR & (uint8_t)0xFF);
                        pUSARTHandle->pRxBuffer++;
                        pUSARTHandle->RxLen--;
                    }
                }
                else
                {
                    /*
                     * 8-bit transfer.
                     */
                    if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
                    {
                        /*
                         * 8-bit user data.
                         */
                        *pUSARTHandle->pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR &(uint8_t)0xFF );
                    }
                    else
                    {
                        /*
                         * 7-bit user data + parity.
                         */
                        *pUSARTHandle->pRxBuffer =(uint8_t)(pUSARTHandle->pUSARTx->DR & (uint8_t)0x7F);
                    }

                    pUSARTHandle->pRxBuffer++;
                    pUSARTHandle->RxLen--;
                }
            }


            /*
             * Reception complete.
             */
            if(pUSARTHandle->RxLen == 0)
            {
                /*
                 * Disable RXNE interrupt.
                 */
                pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_RXNEIE);

                /*
                 * Reset receive state.
                 */
                pUSARTHandle->RxBusyState = USART_READY;

                /*
                 * Notify application.
                 */
                USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_RX_CMPLT);
            }
        }
    }

    /************************* Check for CTS flag *************************/

    /*
     * CTS interrupt is not applicable to UART4/UART5.
     */

    temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_CTS);
    temp2 = pUSARTHandle->pUSARTx->CR3 & (1 << USART_CR3_CTSE);

    if(temp1 && temp2)
    {
        /*
         * Clear CTS flag.
         */
        pUSARTHandle->pUSARTx->SR &= ~(1 << USART_SR_CTS);

        /*
         * Notify application.
         */
        USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_CTS);
    }

    /************************* Check for IDLE flag *************************/

    temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_IDLE);
    temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_IDLEIE);

    if(temp1 && temp2)
    {
        /*
         * Clear IDLE flag.
         *
         * Required sequence:
         * 1. Read SR
         * 2. Read DR
         */
        (void)pUSARTHandle->pUSARTx->SR;
        (void)pUSARTHandle->pUSARTx->DR;


        /*
         * Notify application.
         */
        USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_IDLE);
    }


    /************************* Check for ORE flag *************************/

    temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_ORE);
    temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_RXNEIE);

    if(temp1 && temp2)
    {
        /*
         * Notify application about overrun.
         */
        USART_ApplicationEventCallback(pUSARTHandle, USART_ERR_ORE );

        /*
         * Clear ORE.
         *
         * Required sequence:
         * 1. Read SR
         * 2. Read DR
         */
        (void)pUSARTHandle->pUSARTx->SR;
        (void)pUSARTHandle->pUSARTx->DR;
    }


    /************************* Check for Error flags *************************/

    temp2 = pUSARTHandle->pUSARTx->CR3 & (1 << USART_CR3_EIE);
    if(temp2)
    {
        temp1 = pUSARTHandle->pUSARTx->SR;
        /*
         * Framing Error.
         */
        if(temp1 & (1 << USART_SR_FE))
        {
            USART_ApplicationEventCallback(pUSARTHandle, USART_ERR_FE);
        }

        /*
         * Noise Error.
         */
        if(temp1 & (1 << USART_SR_NE))
        {
            USART_ApplicationEventCallback(pUSARTHandle, USART_ERR_NE);
        }

        /*
         * Overrun Error.
         */
        if(temp1 & (1 << USART_SR_ORE))
        {
            USART_ApplicationEventCallback(pUSARTHandle,USART_ERR_ORE);
        }

        /*
         * Clear error flags.
         *
         * Required sequence:
         * 1. Read SR
         * 2. Read DR
         */
        (void)pUSARTHandle->pUSARTx->DR;
    }
}


/*********************************************************************
 * @fn              - USART_ApplicationEventCallback
 *
 * @brief           - Weak callback function called by USART driver
 *                    when an interrupt-driven USART event occurs.
 *
 * @param[in]       - pUSARTHandle : Pointer to USART handle
 * @param[in]       - event        : USART event/error
 *
 * @return          - None
 *
 * @Note            - Application can override this weak function.
 *
 *********************************************************************/
__weak void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle, uint8_t event)
{
    /*
     * User application can override this function.
     */
}
