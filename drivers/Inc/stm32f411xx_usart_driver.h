/*
 * stm32f411xx_usart_driver.h
 *
 *  Created on: 11-Apr-2026
 *  Author: Abhishek Bharadwaj
 */

#ifndef INC_STM32F411XX_USART_H_
#define INC_STM32F411XX_USART_H_

#include "stm32f411xx.h"


/*==============================================================================
 * USART PERIPHERAL NOTES — STM32F411CE
 *
 * Available instances : USART1 (APB2), USART2 (APB1), USART6 (APB2)
 * Base addresses      : defined in stm32f411xx.h
 * Register struct     : USART_RegDef_t defined in stm32f411xx.h
 * Bit positions       : USART_SR_*, USART_CR1_*, etc. defined in stm32f411xx.h
 *============================================================================*/


/*==============================================================================
 * CONFIGURATION STRUCTURE
 *============================================================================*/

typedef struct
{
    uint8_t  USART_Mode;            // TX only / RX only / TX+RX        @USART_Mode
    uint32_t USART_Baud;            // Baud rate                        @USART_Baud
    uint8_t  USART_NoOfStopBits;    // Stop bit count                   @USART_NoOfStopBits
    uint8_t  USART_WordLength;      // 8-bit or 9-bit data frame        @USART_WordLength
    uint8_t  USART_ParityControl;   // None / Even / Odd                @USART_ParityControl
    uint8_t  USART_HWFlowControl;   // None / CTS / RTS / CTS+RTS      @USART_HWFlowControl
} USART_Config_t;


/*==============================================================================
 * HANDLE STRUCTURE
 *============================================================================*/

typedef struct
{
    USART_RegDef_t *pUSARTx;        // Base address of the USART peripheral

    USART_Config_t  USART_Config;   // User configuration

    uint8_t        *pTxBuffer;      // Application TX buffer pointer
    uint8_t        *pRxBuffer;      // Application RX buffer pointer
    uint32_t        TxLen;          // Bytes remaining to transmit
    uint32_t        RxLen;          // Bytes remaining to receive
    uint8_t         TxBusyState;    // Current TX state                 @USART_States
    uint8_t         RxBusyState;    // Current RX state                 @USART_States
} USART_Handle_t;


/*==============================================================================
 * @USART_Mode
 * Possible values for USART_Config_t.USART_Mode
 * Controls CR1 TE and RE bits
 *============================================================================*/
#define USART_MODE_ONLY_TX      0   // CR1 TE=1, RE=0
#define USART_MODE_ONLY_RX      1   // CR1 TE=0, RE=1
#define USART_MODE_TXRX         2   // CR1 TE=1, RE=1


/*==============================================================================
 * @USART_Baud
 * Standard baud rate values for USART_Config_t.USART_Baud
 *============================================================================*/
#define USART_STD_BAUD_1200         1200
#define USART_STD_BAUD_2400         2400
#define USART_STD_BAUD_9600         9600
#define USART_STD_BAUD_19200        19200
#define USART_STD_BAUD_38400        38400
#define USART_STD_BAUD_57600        57600
#define USART_STD_BAUD_115200       115200
#define USART_STD_BAUD_230400       230400
#define USART_STD_BAUD_460800       460800
#define USART_STD_BAUD_921600       921600


/*==============================================================================
 * @USART_NoOfStopBits
 * Possible values for USART_Config_t.USART_NoOfStopBits
 * Map directly to CR2 STOP[13:12] field values
 * Note: 0.5 and 1.5 stop bits are not available in asynchronous (UART) mode
 *============================================================================*/
#define USART_STOPBITS_1            0   // CR2 STOP = 00
#define USART_STOPBITS_0_5          1   // CR2 STOP = 01
#define USART_STOPBITS_2            2   // CR2 STOP = 10
#define USART_STOPBITS_1_5          3   // CR2 STOP = 11


/*==============================================================================
 * @USART_WordLength
 * Possible values for USART_Config_t.USART_WordLength
 * Maps directly to CR1 M bit
 * Note: if parity is enabled, one bit of the word is the parity bit,
 *       so effective data bits = WordLength - 1
 *============================================================================*/
#define USART_WORDLEN_8BITS         0   // CR1 M = 0
#define USART_WORDLEN_9BITS         1   // CR1 M = 1


/*==============================================================================
 * @USART_ParityControl
 * Possible values for USART_Config_t.USART_ParityControl
 * Controls CR1 PCE and PS bits
 *============================================================================*/
#define USART_PARITY_DISABLE        0   // CR1 PCE = 0
#define USART_PARITY_EN_EVEN        1   // CR1 PCE = 1, PS = 0
#define USART_PARITY_EN_ODD         2   // CR1 PCE = 1, PS = 1


/*==============================================================================
 * @USART_HWFlowControl
 * Possible values for USART_Config_t.USART_HWFlowControl
 * Controls CR3 RTSE and CTSE bits
 *============================================================================*/
#define USART_HW_FLOW_CTRL_NONE     0   // CR3 CTSE=0, RTSE=0
#define USART_HW_FLOW_CTRL_CTS      1   // CR3 CTSE=1
#define USART_HW_FLOW_CTRL_RTS      2   // CR3 RTSE=1
#define USART_HW_FLOW_CTRL_CTS_RTS  3   // CR3 CTSE=1, RTSE=1


/*==============================================================================
 * @USART_States
 * Peripheral transfer states — used in TxBusyState and RxBusyState fields
 *============================================================================*/
#define USART_READY                 0
#define USART_BUSY_IN_TX            1
#define USART_BUSY_IN_RX            2


/*==============================================================================
 * STATUS FLAG MASKS
 * Pre-shifted masks for use with USART_GetFlagStatus() and USART_ClearFlag()
 * Derived from bit position defines in stm32f411xx.h
 *============================================================================*/
#define USART_FLAG_PE               (1 << USART_SR_PE)
#define USART_FLAG_FE               (1 << USART_SR_FE)
#define USART_FLAG_NF               (1 << USART_SR_NF)
#define USART_FLAG_ORE              (1 << USART_SR_ORE)
#define USART_FLAG_IDLE             (1 << USART_SR_IDLE)
#define USART_FLAG_RXNE             (1 << USART_SR_RXNE)
#define USART_FLAG_TC               (1 << USART_SR_TC)
#define USART_FLAG_TXE              (1 << USART_SR_TXE)
#define USART_FLAG_LBD              (1 << USART_SR_LBD)
#define USART_FLAG_CTS              (1 << USART_SR_CTS)


/*==============================================================================
 * APPLICATION EVENT CODES
 * Passed to USART_ApplicationEventCallback() to identify the event or error.
 * The callback is implemented by the application, called by the driver ISR.
 *============================================================================*/
#define USART_EVENT_TX_CMPLT        0   // Transmission complete (TC flag)
#define USART_EVENT_RX_CMPLT        1   // Reception complete (all requested bytes received)
#define USART_EVENT_CTS             2   // CTS line toggled
#define USART_EVENT_IDLE            3   // IDLE line detected
#define USART_EVENT_ERR_FE          4   // Framing error
#define USART_EVENT_ERR_NE          5   // Noise error
#define USART_EVENT_ERR_ORE         6   // Overrun error


/*==============================================================================
 * API FUNCTION PROTOTYPES
 *============================================================================*/

/*
 * Peripheral clock control
 */
void    USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnOrDi);


/*
 * Init and De-Init
 * Note: USART_Init() configures all registers but does NOT set CR1 UE.
 *       Call USART_PeripheralControl(ENABLE) explicitly after Init()
 *       because BRR must be written while UE = 0.
 */
void    USART_Init(USART_Handle_t *pUSARTHandle);
void    USART_DeInit(USART_RegDef_t *pUSARTx);


/*
 * Peripheral enable / disable
 * Toggles CR1 UE bit — call after USART_Init()
 */
void    USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnOrDi);


/*
 * Data send and receive — blocking (polling)
 * Function returns only after all Len bytes are transferred.
 * Handle is required (not raw pointer) because word length and parity
 * config affect how DR is written and read.
 */
void    USART_SendData(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len);
void    USART_ReceiveData(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len);


/*
 * Data send and receive — non-blocking (interrupt driven)
 * Returns USART_BUSY_IN_TX / USART_BUSY_IN_RX if peripheral is already busy
 * (call is a no-op in that case). Returns USART_READY if transfer was started.
 */
uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len);
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len);


/*
 * IRQ configuration and ISR handling
 */
void    USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi);
void    USART_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void    USART_IRQHandling(USART_Handle_t *pUSARTHandle);


/*
 * Status flag utilities
 * USART_GetFlagStatus : returns SET if flag is set, RESET otherwise
 * USART_ClearFlag     : clears the specified flag (where software-clearable)
 */
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx, uint32_t FlagName);
void    USART_ClearFlag(USART_RegDef_t *pUSARTx, uint32_t FlagName);


/*
 * Application callback
 * Must be implemented by the application — NOT by the driver.
 * Called from USART_IRQHandling() on transfer completion and errors.
 * AppEvent values are defined in the @USART application event codes above.
 */
void    USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle, uint8_t AppEvent);


#endif /* INC_STM32F411XX_USART_H_ */
