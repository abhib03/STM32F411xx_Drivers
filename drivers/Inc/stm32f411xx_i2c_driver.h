/*
 * stm32f411xx_i2c_driver.h
 *
 *  Created on: 27-Apr-2026
 *  Author: Abhishek_Bharadwaj
 */

#ifndef INC_STM32F411XX_I2C_DRIVER_H_
#define INC_STM32F411XX_I2C_DRIVER_H_

#include "stm32f411xx.h"


/*
 * Configuration structure for I2Cx peripheral
 *
 * Filled in by the application and pointed to by I2C_Handle_t before calling
 * I2C_Init(). I2C_SCLSpeed picks SM vs FM (see I2C_SCL_SPEED_* below) and
 * drives the CCR/TRISE math in I2C_Init(); I2C_DeviceAddress only matters if
 * this device will act as an I2C slave.
 */
typedef struct
{
	uint32_t I2C_SCLSpeed;		// SCL clock speed
	uint8_t  I2C_DeviceAddress;	// Own 7-bit slave address
	uint8_t  I2C_AckControl;	//ACK control
	uint8_t  I2C_FMDutyCycle;	// Fast Mode duty cycle

} I2C_Config_t;


/*
 * Handle structure for I2Cx peripheral
 *
 * One of these per I2C instance in use (I2C1/I2C2/I2C3), owned by the
 * application. Beyond pI2Cx/I2C_Config it's really just the "current
 * transfer" context that the interrupt-driven APIs need to survive across
 * multiple ISR calls, since a single I2C transfer spans many interrupts.
 */
typedef struct
{
	I2C_RegDef_t *pI2Cx;
	I2C_Config_t  I2C_Config;

	/* Interrupt-driven TX state */
	uint8_t  *pTxBuffer;
	uint32_t TxLen;

	/* Interrupt-driven RX state */
	uint8_t  *pRxBuffer;
	uint32_t RxLen;		// bytes remaining to receive — counts down to 0
	uint32_t RxSize;	// original requested length — fixed for the whole transfer, never decremented. Needed because ACK must be disabled at different moments depending on
						// whether the *whole* transfer is 1 byte (RxSize==1) or just 2 bytes are left of a longer one (RxLen==2) — see I2C_ClearADDRFlag() and
						// I2C_MasterHandleRXNEInterrupt() in the .c file.

	uint8_t TxRxState;	// I2C_READY / I2C_BUSY_IN_TX / I2C_BUSY_IN_RX — also
						// doubles as the "is a transfer already in flight"
						// guard that MasterSendDataIT/MasterReceiveDataIT
						// check before starting a new one
	uint8_t DevAddr;	// target slave's 7-bit address, stashed here so the
						// EV5 (SB) handler in I2C_EV_IRQHandling can send it
						// once START actually goes out
	uint8_t Sr;			// I2C_DISABLE_SR / I2C_ENABLE_SR — whether to
						// generate STOP at the end of this transfer, or leave
						// the bus held for a repeated START (e.g. a
						// write-then-read register access)

} I2C_Handle_t;


/*
 * I2C application states
 *
 * Values for I2C_Handle_t.TxRxState.
 */
#define I2C_READY			0
#define I2C_BUSY_IN_RX		1
#define I2C_BUSY_IN_TX		2


/*
 * I2C Repeated START control
 *
 * Values for the Sr parameter threaded through the Master*Data(IT) APIs and
 * stored in I2C_Handle_t.Sr. ENABLE_SR skips the STOP condition so a
 * following call can continue the transaction with a repeated START — the
 * usual pattern for "write register pointer, then read back" on a slave.
 */
#define I2C_DISABLE_SR		0
#define I2C_ENABLE_SR		1


/*
 * I2C SCL Speed Macros
 *
 * Standard Mode : 100 kHz
 * Fast Mode     : 400 kHz
 *
 * Passed as I2C_Config_t.I2C_SCLSpeed. I2C_Init() compares against
 * I2C_SCL_SPEED_SM to decide SM vs FM math — anything above that threshold is
 * treated as Fast Mode, so FM2K/FM4K are just convenience values, not magic
 * thresholds themselves.
 */
#define I2C_SCL_SPEED_SM	100000U
#define I2C_SCL_SPEED_FM2K	200000U
#define I2C_SCL_SPEED_FM4K	400000U


/*
 * I2C ACK Control
 *
 * Values for I2C_Config_t.I2C_AckControl (the power-on/restored default) and
 * the EnOrDi parameter of I2C_ManageAcking() (the live, per-byte control used
 * mid-transfer). Note these happen to share values with plain ENABLE/DISABLE,
 * but are named separately since they map onto CR1.ACK specifically.
 */
#define I2C_ACK_DISABLE		0
#define I2C_ACK_ENABLE		1


/*
 * I2C Address Mode
 *
 * Not currently consumed by I2C_Init() — this driver only implements 7-bit
 * addressing (OAR1 bit 15 / ADDMODE is left at its 0 = 7-bit reset value).
 * Defined here for future 10-bit support.
 */
#define I2C_ADDR_MODE_7BIT	0
#define I2C_ADDR_MODE_10BIT	1


/*
 * I2C Fast Mode Duty Cycle
 *
 * Values for I2C_Config_t.I2C_FMDutyCycle, only relevant when I2C_SCLSpeed is
 * in Fast Mode. Controls both CCR.DUTY and which CCR divisor formula
 * I2C_Init() uses (÷3 vs ÷25) — see the CCR bit-position comments in
 * stm32f411xx.h for why the two duty cycles need different math.
 */
#define I2C_FM_DUTY_2		0
#define I2C_FM_DUTY_16_9	1


/*
 * I2C Flag Macros
 *
 * Pre-shifted SR1 bitmasks for use with I2C_GetFlagStatus() — e.g.
 * I2C_GetFlagStatus(pI2Cx, I2C_FLAG_TXE) rather than having every caller
 * re-derive (1 << I2C_SR1_TXE) itself.
 */
#define I2C_FLAG_SB				(1U << I2C_SR1_SB)
#define I2C_FLAG_ADDR			(1U << I2C_SR1_ADDR)
#define I2C_FLAG_BTF			(1U << I2C_SR1_BTF)
#define I2C_FLAG_ADD10			(1U << I2C_SR1_ADD10)
#define I2C_FLAG_STOPF			(1U << I2C_SR1_STOPF)
#define I2C_FLAG_RXNE			(1U << I2C_SR1_RXNE)
#define I2C_FLAG_TXE			(1U << I2C_SR1_TXE)

#define I2C_FLAG_BERR			(1U << I2C_SR1_BERR)
#define I2C_FLAG_ARLO			(1U << I2C_SR1_ARLO)
#define I2C_FLAG_AF				(1U << I2C_SR1_AF)
#define I2C_FLAG_OVR			(1U << I2C_SR1_OVR)
#define I2C_FLAG_TIMEOUT		(1U << I2C_SR1_TIMEOUT)


/*
 * I2C Application Event Codes
 *
 * AppEv values I2C_ApplicationEventCallback() receives for successful,
 * non-error events (IT-driven TX/RX completion, STOP detected in slave mode,
 * and slave being addressed for a read/write by a master). Numbered 0-2 and
 * 8-9, deliberately leaving 3-7 free for the error codes below so the two
 * sets can't be confused by value alone.
 */
#define I2C_EVENT_TX_CMPLT	0
#define I2C_EVENT_RX_CMPLT	1
#define I2C_EVENT_STOP		2

#define I2C_EVENT_DATA_REQ	8	// slave mode: master wants to read from us
#define I2C_EVENT_DATA_RCV	9	// slave mode: master has written a byte to us


/*
 * I2C Error Codes
 *
 * AppEv values I2C_ApplicationEventCallback() receives from I2C_ER_IRQHandling()
 * — see the corresponding SR1 flags (I2C_SR1_BERR etc.) and the "@note" on
 * I2C_ER_IRQHandling() in the .c file for what each condition actually means.
 */
#define I2C_ERROR_BERR		3
#define I2C_ERROR_ARLO		4
#define I2C_ERROR_AF		5
#define I2C_ERROR_OVR		6
#define I2C_ERROR_TIMEOUT	7


/*
 ******************************************************************************************
 *
 * 							APIs supported by this driver
 *
 * 		For more information about the APIs check the function definitions
 *
 ******************************************************************************************
 */


/*
 * Peripheral Clock Setup
 */
void I2C_PeriClkControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);


/*
 * Init and De-init
 */
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);


/*******************************************************************************
 * Data Read and Write
 *******************************************************************************/
/* Master mode - Blocking */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr);
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr);
/* Master mode - Interrupt driven. Both return the handle's TxRxState *before*
 * this call — I2C_READY means the request was accepted and is now running in
 * the background; anything else means it was rejected because a transfer was
 * already in progress. */
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr);
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr);

/* Slave mode - Blocking. No Len parameter — these move exactly one byte at a
 * time and are meant to be called from inside I2C_EVENT_DATA_REQ /
 * I2C_EVENT_DATA_RCV in the application's event callback, one call per
 * TXE/RXNE. */
void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx, uint8_t data);
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2Cx);

/*
 * Transfer Abort APIs
 *
 * Force-close an in-progress IT transfer (e.g. on an application-level
 * timeout) without waiting for it to finish naturally. Also called
 * internally at the natural end of a transfer — see I2C_EV_IRQHandling().
 */
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle);
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle);

/*
 * Slave interrupt callback control
 *
 * Call with ENABLE after I2C_PeripheralControl(ENABLE) to start listening
 * for master-initiated transfers in slave mode.
 */
void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);

/*
 * IRQ Configuration and ISR Handling
 *
 * I2C_IRQInterruptConfig/I2C_IRQPriorityConfig are generic NVIC control (see
 * IRQ_NO_I2Cx_EV/_ER in stm32f411xx.h for the numbers to pass). The two
 * IRQHandling functions are meant to be called directly from this project's
 * I2Cx_EV_IRQHandler/I2Cx_ER_IRQHandler ISRs in the startup/vector file.
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi);
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle);
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);

/*
 * Application callback
 *
 * Declared __weak in the .c file — the application overrides it (same name,
 * no __weak) to react to I2C_EVENT_/I2C_ERROR_ codes. If left
 * un-overridden, events are silently dropped.
 */
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv);


/*
 * Other Peripheral Control APIs
 */
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName);
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);

#endif /* INC_STM32F411XX_I2C_DRIVER_H_ */
