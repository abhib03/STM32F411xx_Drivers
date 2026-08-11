/*
 * stm32f411xx_spi_driver.c
 *
 *  Created on: 27-Apr-2026
 *      Author: Abhishek Bharadwaj
 *
 *  Reviewed & corrected — all bugs from initial draft fixed.
 */

#include "stm32f411xx.h"
#include "stm32f411xx_spi_driver.h"

/* =========================================================================
 * Private helper prototypes
 * ========================================================================= */

static void SPI_TXE_Interrupt_Handle(SPI_Handle_t *pSPIHandle);
static void SPI_RXNE_Interrupt_Handle(SPI_Handle_t *pSPIHandle);
static void SPI_OVR_ERR_Interrupt_Handle(SPI_Handle_t *pSPIHandle);
void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv);
/* =========================================================================
 * Peripheral Clock Control
 * ========================================================================= */

/*****************************************************************
 * @fn          - SPI_PeriClkControl
 *
 * @brief       - Enables or disables the peripheral clock for
 *                the given SPI peripheral.
 *
 * @param[in]   - pSPIx   : Base address of the SPI peripheral
 * @param[in]   - EnOrDi  : ENABLE or DISABLE
 *
 * @return      - None
 *****************************************************************/
void SPI_PeriClkControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
    if (EnOrDi == ENABLE)
    {
        if      (pSPIx == SPI1) { SPI1_PCLK_EN(); }
        else if (pSPIx == SPI2) { SPI2_PCLK_EN(); }
        else if (pSPIx == SPI3) { SPI3_PCLK_EN(); }
        else if (pSPIx == SPI4) { SPI4_PCLK_EN(); }
        else if (pSPIx == SPI5) { SPI5_PCLK_EN(); }
    }
    else
    {
        if      (pSPIx == SPI1) { SPI1_PCLK_DI(); }
        else if (pSPIx == SPI2) { SPI2_PCLK_DI(); }
        else if (pSPIx == SPI3) { SPI3_PCLK_DI(); }
        else if (pSPIx == SPI4) { SPI4_PCLK_DI(); }
        else if (pSPIx == SPI5) { SPI5_PCLK_DI(); }
    }
}

/* =========================================================================
 * Initialization / De-initialization
 * ========================================================================= */

/*****************************************************************
 * @fn          - SPI_Init
 *
 * @brief       - Initializes the SPI peripheral according to the
 *                parameters in pSPIHandle->SPIConfig.
 *
 *                IMPORTANT: SPE is NOT set here. Call
 *                SPI_PeripheralControl(pSPIx, ENABLE) only after
 *                all configuration is complete, because certain
 *                bits (e.g. BR, DFF) must not be changed while
 *                SPE = 1.
 *
 *                When SSM = 1 (software NSS), SSI is set to 1
 *                automatically to prevent a spurious MODF fault.
 *                MODF is triggered when the hardware sees NSS low
 *                while MSTR = 1; SSI acts as the internal NSS
 *                level in software-NSS mode.
 *
 * @param[in]   - pSPIHandle : Pointer to SPI handle structure
 *
 * @return      - None
 *****************************************************************/
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
    /* Enable peripheral clock */
    SPI_PeriClkControl(pSPIHandle->pSPIx, ENABLE);

    uint32_t tempreg = 0;

    /* 1. Device mode (MSTR bit) */
    tempreg |= (pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR);

    /* 2. Bus configuration */
    if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
    {
        /* Full-duplex: BIDIMODE = 0 */
        tempreg &= ~(1U << SPI_CR1_BIDIMODE);
    }
    else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
    {
        /* Half-duplex: BIDIMODE = 1 */
        tempreg |= (1U << SPI_CR1_BIDIMODE);
    }
    else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
    {
        /* Simplex RX: BIDIMODE = 0, RXONLY = 1 */
        tempreg &= ~(1U << SPI_CR1_BIDIMODE);
        tempreg |=  (1U << SPI_CR1_RXONLY);
    }

    /* 3. Baud rate prescaler (BR[2:0]) */
    tempreg |= (pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR);

    /* 4. Data frame format (DFF) */
    tempreg |= (pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF);

    /* 5. Clock polarity (CPOL) */
    tempreg |= (pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL);

    /* 6. Clock phase (CPHA) */
    tempreg |= (pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA);

    /* 7. Software slave management (SSM) */
    tempreg |= (pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM);

    pSPIHandle->pSPIx->CR1 = tempreg;

    /*
     * FIX: When SSM = 1 and MSTR = 1, SSI must be 1.
     *
     * In software-NSS mode, SSI is the bit the hardware reads as the NSS
     * level. If SSI = 0 with MSTR = 1, the hardware thinks another master
     * is asserting NSS, raises MODF, and automatically clears SPE —
     * silently killing the peripheral.
     */
    if (pSPIHandle->SPIConfig.SPI_SSM == SPI_SSM_EN)
    {
        SPI_SSIConfig(pSPIHandle->pSPIx, ENABLE);
    }
}

/*****************************************************************
 * @fn          - SPI_DeInit
 *
 * @brief       - Resets all registers of the given SPI peripheral
 *                via the RCC reset register.
 *
 * @param[in]   - pSPIx : Base address of the SPI peripheral
 *
 * @return      - None
 *****************************************************************/
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
    if      (pSPIx == SPI1) { SPI1_REG_RESET(); }
    else if (pSPIx == SPI2) { SPI2_REG_RESET(); }
    else if (pSPIx == SPI3) { SPI3_REG_RESET(); }
    else if (pSPIx == SPI4) { SPI4_REG_RESET(); }
    else if (pSPIx == SPI5) { SPI5_REG_RESET(); }
}

/* =========================================================================
 * Flag helper
 * ========================================================================= */

/*****************************************************************
 * @fn          - SPI_GetFlagStatus
 *
 * @brief       - Returns the status of a requested SR flag.
 *
 * @param[in]   - pSPIx    : Base address of the SPI peripheral
 * @param[in]   - FlagName : Flag bitmask (e.g. SPI_TXE_FLAG)
 *
 * @return      - FLAG_SET or FLAG_RESET
 *****************************************************************/
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
    return (pSPIx->SR & FlagName) ? FLAG_SET : FLAG_RESET;
}

/* =========================================================================
 * Blocking (polling) data transfer
 * ========================================================================= */

/*****************************************************************
 * @fn          - SPI_SendData
 *
 * @brief       - Transmits Len bytes over SPI in blocking mode.
 *
 *                The caller must assert/de-assert the CS GPIO
 *                around this call. This function waits for BSY=0
 *                before returning so that CS is never deasserted
 *                while the shift register is still clocking out
 *                the last byte.
 *
 * @param[in]   - pSPIx      : Base address of the SPI peripheral
 * @param[in]   - pTxBuffer  : Pointer to transmit data buffer
 * @param[in]   - Len        : Number of bytes to send
 *
 * @return      - None
 *
 * @Note        - Blocking call.
 *****************************************************************/
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
    while (Len > 0)
    {
        /* Wait until TXE = 1 (transmit buffer empty) */
        while (SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);

        if (pSPIx->CR1 & (1U << SPI_CR1_DFF))
        {
            /* 16-bit DFF */
            pSPIx->DR = *((uint16_t *)pTxBuffer);
            Len -= 2;

            /*
             * FIX: (uint16_t*)pTxBuffer++ is a precedence trap.
             * The cast applies to the result of pTxBuffer++ (the old
             * address), not to pTxBuffer itself. The cast is then
             * silently discarded, so pTxBuffer advances by only 1 byte.
             * Correct form: advance the pointer by 2 bytes explicitly.
             */
            pTxBuffer += 2;
        }
        else
        {
            /* 8-bit DFF */
            pSPIx->DR = *pTxBuffer;
            Len--;
            pTxBuffer++;
        }
    }

    /*
     * FIX: Wait for BSY = 0 before returning.
     *
     * TXE goes high as soon as DR is loaded into the shift register,
     * but the shift register is still physically clocking out bits.
     * Returning (and de-asserting CS) before BSY clears will truncate
     * the last byte on the wire.
     *
     * After the transfer, read DR then SR to clear the OVR flag that
     * accumulates from the MISO bytes clocked in during TX-only mode.
     */
    while (SPI_GetFlagStatus(pSPIx, SPI_BUSY_FLAG) == FLAG_SET);

    /* Flush OVR: read DR, then SR — order is mandatory (RM0383 §20.3.10) */
    (void)pSPIx->DR;
    (void)pSPIx->SR;
}

/*****************************************************************
 * @fn          - SPI_ReceiveData
 *
 * @brief       - Receives Len bytes over SPI in blocking mode.
 *                In master mode, clocking is driven by dummy 0xFF
 *                bytes written to DR.
 *
 * @param[in]   - pSPIx      : Base address of the SPI peripheral
 * @param[in]   - pRxBuffer  : Pointer to receive data buffer
 * @param[in]   - Len        : Number of bytes to receive
 *
 * @return      - None
 *
 * @Note        - Blocking call.
 *****************************************************************/
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{
    while (Len > 0)
    {
        /* Wait until RXNE = 1 (receive buffer not empty) */
        while (SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);

        if (pSPIx->CR1 & (1U << SPI_CR1_DFF))
        {
            /* 16-bit DFF */
            *((uint16_t *)pRxBuffer) = (uint16_t)pSPIx->DR;
            Len -= 2;

            /* FIX: same pointer-advance fix as in SPI_SendData */
            pRxBuffer += 2;
        }
        else
        {
            /* 8-bit DFF */
            *pRxBuffer = (uint8_t)pSPIx->DR;
            Len--;
            pRxBuffer++;
        }
    }
}

/* =========================================================================
 * Interrupt-driven data transfer
 * ========================================================================= */

/*****************************************************************
 * @fn          - SPI_SendDataIT
 *
 * @brief       - Initiates a non-blocking SPI transmit.
 *                Returns immediately; completion is signalled via
 *                SPI_ApplicationEventCallback(SPI_EVENT_TX_CMPLT).
 *
 * @param[in]   - pSPIHandle : Pointer to SPI handle
 * @param[in]   - pTxBuffer  : Source buffer (must stay valid until callback)
 * @param[in]   - Len        : Number of bytes to send
 *
 * @return      - Current Tx state (SPI_READY or SPI_BUSY_IN_TX)
 *****************************************************************/
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len)
{
    uint8_t state = pSPIHandle->TxState;

    if (state != SPI_BUSY_IN_TX)
    {
        /* Save buffer info in the handle */
        pSPIHandle->pTxBuffer = pTxBuffer;
        pSPIHandle->TxLen     = Len;

        /* Mark peripheral as busy so no other caller can take it */
        pSPIHandle->TxState = SPI_BUSY_IN_TX;

        /* Enable TXEIE — ISR fires as soon as TXE = 1 */
        pSPIHandle->pSPIx->CR2 |= (1U << SPI_CR2_TXEIE);
    }

    return state;
}

/*****************************************************************
 * @fn          - SPI_ReceiveDataIT
 *
 * @brief       - Initiates a non-blocking SPI receive.
 *                Returns immediately; completion is signalled via
 *                SPI_ApplicationEventCallback(SPI_EVENT_RX_CMPLT).
 *
 * @param[in]   - pSPIHandle : Pointer to SPI handle
 * @param[in]   - pRxBuffer  : Destination buffer (must stay valid until callback)
 * @param[in]   - Len        : Number of bytes to receive
 *
 * @return      - Current Rx state (SPI_READY or SPI_BUSY_IN_RX)
 *****************************************************************/
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len)
{
    uint8_t state = pSPIHandle->RxState;

    if (state != SPI_BUSY_IN_RX)
    {
        /* Save buffer info in the handle */
        pSPIHandle->pRxBuffer = pRxBuffer;
        pSPIHandle->RxLen     = Len;

        /* Mark peripheral as busy */
        pSPIHandle->RxState = SPI_BUSY_IN_RX;

        /* Enable RXNEIE and ERRIE */
        pSPIHandle->pSPIx->CR2 |= (1U << SPI_CR2_RXNEIE);
        pSPIHandle->pSPIx->CR2 |= (1U << SPI_CR2_ERRIE);
    }

    return state;
}

/* =========================================================================
 * IRQ configuration
 * ========================================================================= */

/*****************************************************************
 * @fn          - SPI_IRQInterruptConfig
 *
 * @brief       - Enables or disables a specific IRQ in the NVIC.
 *
 * @param[in]   - IRQNumber : IRQ number (0–95)
 * @param[in]   - EnOrDi   : ENABLE or DISABLE
 *
 * @return      - None
 *****************************************************************/
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi)
{
    if (EnOrDi == ENABLE)
    {
        if      (IRQNumber <= 31)                          { *NVIC_ISER0 |= (1U << IRQNumber);          }
        else if (IRQNumber > 31 && IRQNumber < 64)         { *NVIC_ISER1 |= (1U << (IRQNumber % 32));   }
        else if (IRQNumber >= 64 && IRQNumber < 96)        { *NVIC_ISER2 |= (1U << (IRQNumber % 64));   }
    }
    else
    {
        if      (IRQNumber <= 31)                          { *NVIC_ICER0 |= (1U << IRQNumber);          }
        else if (IRQNumber > 31 && IRQNumber < 64)         { *NVIC_ICER1 |= (1U << (IRQNumber % 32));   }
        else if (IRQNumber >= 64 && IRQNumber < 96)        { *NVIC_ICER2 |= (1U << (IRQNumber % 64));   }
    }
}

/*****************************************************************
 * @fn          - SPI_IRQPriorityConfig
 *
 * @brief       - Sets the priority for a given IRQ number in the
 *                NVIC IPR registers.
 *
 * @param[in]   - IRQNumber   : IRQ number
 * @param[in]   - IRQPriority : Priority value
 *
 * @return      - None
 *****************************************************************/
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
    uint8_t iprx         = IRQNumber / 4;
    uint8_t iprx_section = IRQNumber % 4;
    uint8_t shift_amount = (8U * iprx_section) + (8U - NO_PR_BITS_IMPLEMENTED);

    *(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}

/* =========================================================================
 * Unified IRQ handler (call from SPIx_IRQHandler in stm32f4xx_it.c)
 * ========================================================================= */

/*****************************************************************
 * @fn          - SPI_IRQHandling
 *
 * @brief       - Handles TXE, RXNE, and OVR interrupts for the
 *                SPI peripheral associated with pHandle.
 *
 *                Place a call to this inside your vector handler:
 *                  void SPI1_IRQHandler(void) {
 *                      SPI_IRQHandling(&hspi1);
 *                  }
 *
 * @param[in]   - pHandle : Pointer to SPI handle
 *
 * @return      - None
 *****************************************************************/
void SPI_IRQHandling(SPI_Handle_t *pHandle)
{
    uint8_t temp1, temp2;

    /* --- TXE ------------------------------------------------------------ */
    temp1 = pHandle->pSPIx->SR  & (1U << SPI_SR_TXE);
    temp2 = pHandle->pSPIx->CR2 & (1U << SPI_CR2_TXEIE);

    if (temp1 && temp2)
    {
        /* FIX: pass pHandle — the function takes SPI_Handle_t* */
        SPI_TXE_Interrupt_Handle(pHandle);
    }

    /* --- RXNE ----------------------------------------------------------- */
    temp1 = pHandle->pSPIx->SR  & (1U << SPI_SR_RXNE);
    temp2 = pHandle->pSPIx->CR2 & (1U << SPI_CR2_RXNEIE);

    if (temp1 && temp2)
    {
        /* FIX: pass pHandle */
        SPI_RXNE_Interrupt_Handle(pHandle);
    }

    /* --- OVR ------------------------------------------------------------ */
    temp1 = pHandle->pSPIx->SR  & (1U << SPI_SR_OVR);
    temp2 = pHandle->pSPIx->CR2 & (1U << SPI_CR2_ERRIE);

    if (temp1 && temp2)
    {
        /* FIX: pass pHandle */
        SPI_OVR_ERR_Interrupt_Handle(pHandle);
    }
}

/* =========================================================================
 * Private interrupt helpers
 * ========================================================================= */

/*
 * TXE handler — writes the next byte/half-word to DR.
 *
 * When TxLen reaches zero:
 *   - TXEIE is disabled immediately to prevent an interrupt storm.
 *     (TXE stays set once the shift register absorbs the last byte;
 *      leaving TXEIE armed with nothing left to send causes continuous
 *      re-entry into the ISR.)
 *   - State is reset to READY and the application is notified.
 */
static void SPI_TXE_Interrupt_Handle(SPI_Handle_t *pSPIHandle)
{
    if (pSPIHandle->pSPIx->CR1 & (1U << SPI_CR1_DFF))
    {
        /* 16-bit DFF */
        /* FIX 1: was reading pRxBuffer — must read pTxBuffer             */
        /* FIX 2: pointer advance was (uint16_t*)pTxBuffer++ which only   */
        /*        moves by 1 byte due to precedence; use += 2 instead.    */
        pSPIHandle->pSPIx->DR = *((uint16_t *)pSPIHandle->pTxBuffer);
        pSPIHandle->TxLen    -= 2;
        pSPIHandle->pTxBuffer += 2;
    }
    else
    {
        /* 8-bit DFF */
        /* FIX: was decrementing pSPIHandle->Len (nonexistent field)      */
        pSPIHandle->pSPIx->DR = *pSPIHandle->pTxBuffer;
        pSPIHandle->TxLen--;
        pSPIHandle->pTxBuffer++;
    }

    if (pSPIHandle->TxLen == 0)
    {
        /* Disable TXEIE first — stops the interrupt storm */
        pSPIHandle->pSPIx->CR2 &= ~(1U << SPI_CR2_TXEIE);
        pSPIHandle->pTxBuffer    = NULL;
        pSPIHandle->TxLen        = 0;
        pSPIHandle->TxState      = SPI_READY;
        SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
    }
}

/*
 * RXNE handler — reads the next byte/half-word from DR.
 *
 * When RxLen reaches zero:
 *   - RXNEIE and ERRIE are disabled.
 *   - State is reset to READY and the application is notified.
 *
 * FIX: The original version used undeclared bare variables (pSPIx,
 * pRxBuffer, Len) instead of handle members — it would not compile.
 * Also, the close-out block (disable RXNEIE, reset state, callback)
 * was entirely missing.
 */
static void SPI_RXNE_Interrupt_Handle(SPI_Handle_t *pSPIHandle)
{
    if (pSPIHandle->pSPIx->CR1 & (1U << SPI_CR1_DFF))
    {
        /* 16-bit DFF */
        *((uint16_t *)pSPIHandle->pRxBuffer) = (uint16_t)pSPIHandle->pSPIx->DR;
        pSPIHandle->RxLen    -= 2;
        pSPIHandle->pRxBuffer += 2;
    }
    else
    {
        /* 8-bit DFF */
        *pSPIHandle->pRxBuffer = (uint8_t)pSPIHandle->pSPIx->DR;
        pSPIHandle->RxLen--;
        pSPIHandle->pRxBuffer++;
    }

    if (pSPIHandle->RxLen == 0)
    {
        /* Disable RXNEIE and ERRIE */
        pSPIHandle->pSPIx->CR2 &= ~(1U << SPI_CR2_RXNEIE);
        pSPIHandle->pSPIx->CR2 &= ~(1U << SPI_CR2_ERRIE);
        pSPIHandle->pRxBuffer    = NULL;
        pSPIHandle->RxLen        = 0;
        pSPIHandle->RxState      = SPI_READY;
        SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);
    }
}

/*
 * OVR error handler.
 *
 * Overrun clear sequence — RM0383 §20.3.10:
 *   Step 1: Read DR   (clears the stale received byte)
 *   Step 2: Read SR   (completes the clear sequence)
 * Order is mandatory. Reading SR before DR does NOT clear OVR.
 *
 * FIX: The original had a stray semicolon after the signature, which
 * turned the definition into a forward-declaration followed by an
 * orphaned block — a compile error. The body was also empty, so OVR
 * would never have been cleared.
 */
static void SPI_OVR_ERR_Interrupt_Handle(SPI_Handle_t *pSPIHandle)
{
    volatile uint32_t temp;

    /* Only clear OVR if the peripheral is not currently transmitting.
     * If TX is in progress the application owns DR; clearing here would
     * corrupt the ongoing transfer. Let the application sort it out via
     * the callback. */
    if (pSPIHandle->TxState != SPI_BUSY_IN_TX)
    {
        /* Read DR then SR to clear OVR — order is mandatory */
        temp = pSPIHandle->pSPIx->DR;
        temp = pSPIHandle->pSPIx->SR;
        (void)temp;
    }

    SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);
}

/* =========================================================================
 * Other peripheral control APIs
 * ========================================================================= */

/*****************************************************************
 * @fn          - SPI_PeripheralControl
 *
 * @brief       - Enables or disables the SPI peripheral (SPE bit).
 *                Call with ENABLE only after SPI_Init() is complete.
 *
 * @param[in]   - pSPIx   : Base address of the SPI peripheral
 * @param[in]   - EnOrDi  : ENABLE or DISABLE
 *
 * @return      - None
 *****************************************************************/
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
    if (EnOrDi == ENABLE)
        pSPIx->CR1 |=  (1U << SPI_CR1_SPE);
    else
        pSPIx->CR1 &= ~(1U << SPI_CR1_SPE);
}

/*****************************************************************
 * @fn          - SPI_SSIConfig
 *
 * @brief       - Sets or clears the SSI bit (CR1).
 *
 *                In software-NSS mode (SSM = 1), SSI acts as the
 *                internal NSS level seen by the hardware. SSI must
 *                be 1 in master mode to avoid a spurious MODF fault
 *                that would silently clear SPE.
 *
 * @param[in]   - pSPIx   : Base address of the SPI peripheral
 * @param[in]   - EnOrDi  : ENABLE (SSI=1) or DISABLE (SSI=0)
 *
 * @return      - None
 *****************************************************************/
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
    if (EnOrDi == ENABLE)
        pSPIx->CR1 |=  (1U << SPI_CR1_SSI);
    else
        pSPIx->CR1 &= ~(1U << SPI_CR1_SSI);
}

/*****************************************************************
 * @fn          - SPI_SSOEConfig
 *
 * @brief       - Sets or clears the SSOE bit (CR2).
 *
 *                SSOE = 1: NSS output enabled. In master mode the
 *                hardware drives NSS low while SPE = 1 and raises
 *                it high after each transfer (single-master setups).
 *
 * @param[in]   - pSPIx   : Base address of the SPI peripheral
 * @param[in]   - EnOrDi  : ENABLE or DISABLE
 *
 * @return      - None
 *****************************************************************/
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
    if (EnOrDi == ENABLE)
        pSPIx->CR2 |=  (1U << SPI_CR2_SSOE);
    else
        pSPIx->CR2 &= ~(1U << SPI_CR2_SSOE);
}

/* =========================================================================
 * Application callback — weak default, override in application code
 *
 * The driver calls this at the end of every interrupt-driven transfer and
 * on error. Define your own version in the application; the __weak
 * attribute ensures the linker prefers your definition over this stub.
 * ========================================================================= */

__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{
    /* Default: do nothing. Override in application. */
    (void)pSPIHandle;
    (void)AppEv;
}
