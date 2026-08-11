/*
 * stm32f411xx_i2c_driver.c
 *
 *  Created on: 27-Apr-2026
 *  Author: Abhishek_Bharadwaj
 */


#include "stm32f411xx.h"
#include "stm32f411xx_i2c_driver.h"
#include "stm32f411xx_rcc_driver.h"


/*
 ******************************************************************************************
 *
 * 							PRIVATE FUNCTION DECLARATIONS
 *
 ******************************************************************************************
 */

static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx);
static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);


/*
 ******************************************************************************************
 * 							PRIVATE FUNCTIONS
 ******************************************************************************************
 */


/*****************************************************************************************
 * @fn			- I2C_GenerateStartCondition
 *
 * @brief		- Generates START condition.
 *
 *****************************************************************************************/
static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1U << I2C_CR1_START);
}


/*****************************************************************************************
 * @fn			- I2C_GenerateStopCondition
 *
 * @brief		- Generates STOP condition.
 *
 *****************************************************************************************/
static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1U << I2C_CR1_STOP);
}


/*****************************************************************************************
 * @fn			- I2C_ExecuteAddressPhaseWrite
 *
 * @brief		- Sends slave address with WRITE bit.
 *
 *****************************************************************************************/
static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = (uint8_t)(SlaveAddr << 1U);

	// Bit 0 = 0 -> WRITE
	SlaveAddr &= ~(1U);
	pI2Cx->DR = SlaveAddr;
}


/*****************************************************************************************
 * @fn			- I2C_ExecuteAddressPhaseRead
 *
 * @brief		- Sends slave address with READ bit.
 *
 *****************************************************************************************/
static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = (uint8_t)(SlaveAddr << 1U);

	/* Bit 0 = 1 -> READ */
	SlaveAddr |= 1U;

	pI2Cx->DR = SlaveAddr;
}


/*****************************************************************************************
 * @fn			- I2C_ClearADDRFlag
 *
 * @brief		- Clears ADDR flag by reading SR1 followed by SR2.
 *
 * @note		- FIX: For interrupt-driven master reception of exactly one byte
 *                (RxSize == 1), ACK must be disabled *before* ADDR is cleared,
 *                otherwise the peripheral ACKs the only byte instead of NACKing
 *                it, and the slave never sees the NACK/STOP it needs to release
 *                the bus correctly. This mirrors the sequence already used in
 *                the blocking I2C_MasterReceiveData() Len==1 path.
 *
 *****************************************************************************************/
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
	uint32_t dummy_read;

	// Only relevant when we are the bus master
	if (pI2CHandle->pI2Cx->SR2 & (1U << I2C_SR2_MSL))
	{
		if (pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if (pI2CHandle->RxSize == 1U)
			{
				// Disable ACK before clearing ADDR so the single byte
				// we're about to receive gets NACKed.
				I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);
			}
		}
	}

	dummy_read = pI2CHandle->pI2Cx->SR1;
	dummy_read = pI2CHandle->pI2Cx->SR2;

	(void)dummy_read;
}


/*****************************************************************************************
 * @fn			- I2C_MasterHandleTXEInterrupt
 *
 * @brief		- Handles TXE interrupt during master transmission.
 *****************************************************************************************/
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if (pI2CHandle->TxLen > 0U)
	{
		pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);

		pI2CHandle->pTxBuffer++;
		pI2CHandle->TxLen--;
	}
}


/*****************************************************************************************
 * @fn			- I2C_MasterHandleRXNEInterrupt
 *
 * @brief		- Handles RXNE interrupt during master reception.
 *
 * @note		- FIX: added RxSize-based ACK management.
 *                - RxSize == 1: single-byte transfer, ADDR-clear already
 *                  disabled ACK (see I2C_ClearADDRFlag), just read the byte.
 *                - RxSize > 1: ACK must be disabled right when RxLen == 2,
 *                  i.e. just before reading the second-to-last byte, so the
 *                  hardware generates a NACK after the final byte is clocked
 *                  in. Previously this function unconditionally left ACK
 *                  enabled, which is wrong for multi-byte IT reception.
 *****************************************************************************************/
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if (pI2CHandle->RxSize == 1U)
	{
		*pI2CHandle->pRxBuffer = (uint8_t)pI2CHandle->pI2Cx->DR;
		pI2CHandle->pRxBuffer++;
		pI2CHandle->RxLen--;
	}
	else if (pI2CHandle->RxSize > 1U)
	{
		if (pI2CHandle->RxLen == 2U)
		{
			// About to read the second-to-last byte: disable ACK now so
			// the last byte gets NACKed.
			I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);
		}

		*pI2CHandle->pRxBuffer = (uint8_t)pI2CHandle->pI2Cx->DR;
		pI2CHandle->pRxBuffer++;
		pI2CHandle->RxLen--;
	}

	if (pI2CHandle->RxLen == 0U)
	{
		if (pI2CHandle->Sr == I2C_DISABLE_SR)
		{
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		}

		I2C_CloseReceiveData(pI2CHandle);

		I2C_ApplicationEventCallback(pI2CHandle, I2C_EVENT_RX_CMPLT);
	}
}


/*
 ******************************************************************************************
 *
 * 							PERIPHERAL CONTROL APIs
 *
 ******************************************************************************************
 */


/*****************************************************************************************
 * @fn			- I2C_PeriClkControl
 *
 * @brief		- Gates the APB1 peripheral clock to the given I2C instance.
 *
 * @note		- I2C1/I2C2/I2C3 all live on APB1ENR (bits 21/22/23). Nothing on
 *                the bus works — not even register writes taking effect in the
 *                way you'd expect — until this clock is enabled, since the
 *                peripheral's internal logic is unclocked until then.
 *****************************************************************************************/
void I2C_PeriClkControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if (EnOrDi == ENABLE)
	{
		if (pI2Cx == I2C1)
		{
			I2C1_PCLK_EN();
		}
		else if (pI2Cx == I2C2)
		{
			I2C2_PCLK_EN();
		}
		else if (pI2Cx == I2C3)
		{
			I2C3_PCLK_EN();
		}
	}
	else
	{
		if (pI2Cx == I2C1)
		{
			I2C1_PCLK_DI();
		}
		else if (pI2Cx == I2C2)
		{
			I2C2_PCLK_DI();
		}
		else if (pI2Cx == I2C3)
		{
			I2C3_PCLK_DI();
		}
	}
}


/*****************************************************************************************
 * @fn			- I2C_PeripheralControl
 *
 * @brief		- Sets/clears PE (bit 0) in CR1 — the master peripheral-enable switch.
 *
 * @note		- CCR and TRISE (and OAR1) must only be written while PE=0; the
 *                reference manual marks them undefined if written with PE=1.
 *                I2C_Init() runs before this is ever called with ENABLE, so the
 *                ordering is: I2C_Init() -> I2C_PeripheralControl(ENABLE) -> transfers.
 *****************************************************************************************/
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if (EnOrDi == ENABLE)
	{
		pI2Cx->CR1 |= (1U << I2C_CR1_PE);
	}
	else
	{
		pI2Cx->CR1 &= ~(1U << I2C_CR1_PE);
	}
}


/*****************************************************************************************
 * @fn	 	- I2C_GetFlagStatus
 *
 * @brief	- Tests a single-bit flag in SR1 (SB, ADDR, BTF, RXNE, TXE, etc.).
 *
 * @note	- FlagName is expected to already be a shifted bitmask (e.g.
 *              (1 << I2C_SR1_SB)), not a bit index — see the I2C_FLAG_* macros
 *              in the header. Only SR1 is read here; SR2-only status (MSL, BUSY,
 *              TRA) is checked directly against SR2 elsewhere in this driver.
 *****************************************************************************************/
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName)
{
	if (pI2Cx->SR1 & FlagName)
	{
		return FLAG_SET;
	}

	return FLAG_RESET;
}


/*****************************************************************************************
 * @fn		- I2C_ManageAcking
 *
 * @brief	- Sets/clears ACK (bit 10) in CR1.
 *
 * @note	- With ACK=1, every received byte is auto-acknowledged by hardware.
 *              Clearing ACK before the final byte of a master-receive sequence
 *              is what tells the slave "stop sending" — the STM32F4 has no
 *              separate NACK-generation bit, ACK=0 at the right moment *is*
 *              the NACK. Timing matters: see the RxSize/RxLen handling in
 *              I2C_MasterReceiveData() and I2C_MasterHandleRXNEInterrupt().
 *****************************************************************************************/
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if (EnOrDi == I2C_ACK_ENABLE)
	{
		pI2Cx->CR1 |= (1U << I2C_CR1_ACK);
	}
	else
	{
		pI2Cx->CR1 &= ~(1U << I2C_CR1_ACK);
	}
}


/*
 ******************************************************************************************
 * 							INIT AND DE-INIT
 ******************************************************************************************
 */

/*****************************************************************************************
 * @fn			- I2C_Init
 *****************************************************************************************/
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
	uint32_t tempreg = 0U;
	uint16_t ccr_value = 0U;

	//Enable I2C peripheral clock
	I2C_PeriClkControl(pI2CHandle->pI2Cx, ENABLE);

	// Configure ACK control
	// CR1.ACK only takes effect once PE=1, but it's safe (and conventional) to
	// pre-load the desired power-on ACK policy here; I2C_CloseReceiveData()
	// restores this same config after each IT-driven receive.
	tempreg = 0U;
	tempreg |= (uint32_t)pI2CHandle->I2C_Config.I2C_AckControl << I2C_CR1_ACK;
	pI2CHandle->pI2Cx->CR1 = tempreg;

	// Configure APB1 clock frequency
	// CR2.FREQ[5:0] tells the peripheral its own input clock in MHz so it can
	// derive internal timing (e.g. the minimum SCL low/high periods). This is
	// NOT the bus speed — that's CCR below — it's just "what clock am I fed".
	tempreg = 0U;
	tempreg =(RCC_GetPCLK1Value() / 1000000U) & 0x3FU;
	pI2CHandle->pI2Cx->CR2 = tempreg;

	// Configure own address
	// OAR1 holds the address this device answers to in slave mode. Address
	// bits sit at [7:1], so the 7-bit address is shifted left by 1.
	tempreg = 0U;
	tempreg |= (uint32_t)pI2CHandle->I2C_Config.I2C_DeviceAddress << 1U;

	// Bit 14 must be set
	// Reference manual: bit 14 of OAR1 has no defined function but must
	// always be kept at 1 by software.
	tempreg |= (1U << 14U);
	pI2CHandle->pI2Cx->OAR1 = tempreg;

	// Configure CCR
	// CCR[11:0] sets how many PCLK1 cycles make up the SCL high/low periods.
	// The divisor differs by mode because SM has a 1:1 high:low ratio while FM
	// is asymmetric (2:1 or 16:9 depending on DUTY) to meet the FM SCL timing
	// spec at the same average frequency — see the CCR bit-position comments
	// in the header for the exact formulas.
	tempreg = 0U;

	if (pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{
		// Standard Mode (<=100kHz): FS=0, symmetric CCR = PCLK1 / (2 * SCL)
		ccr_value = (uint16_t)(RCC_GetPCLK1Value() / (2U * pI2CHandle->I2C_Config.I2C_SCLSpeed));
		tempreg |= ccr_value;
	}
	else
	{
		// Fast Mode: set FS=1 and program the requested duty cycle
		tempreg |= (1U << I2C_CCR_FS);
		tempreg |= (uint32_t) pI2CHandle->I2C_Config.I2C_FMDutyCycle << I2C_CCR_DUTY;

		if (pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2)
		{
			// DUTY=0, Tlow/Thigh = 2 : CCR = PCLK1 / (3 * SCL)
			ccr_value = (uint16_t)(RCC_GetPCLK1Value() / (3U * pI2CHandle->I2C_Config.I2C_SCLSpeed));
		}
		else
		{
			// DUTY=1, Tlow/Thigh = 16/9 : CCR = PCLK1 / (25 * SCL)
			ccr_value = (uint16_t)(RCC_GetPCLK1Value() / (25U * pI2CHandle->I2C_Config.I2C_SCLSpeed));
		}

		tempreg |= ccr_value;
	}

	pI2CHandle->pI2Cx->CCR = tempreg;

	// Configure TRISE
	// TRISE bounds the maximum allowed SCL rise time so the peripheral knows
	// how long it may need to wait for the line to actually go high before
	// sampling/driving it. In SM it's simply (PCLK1_MHz + 1); in FM the max
	// rise time is tighter (300ns spec), hence the extra scaling before +1.
	if (pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{
		tempreg =(RCC_GetPCLK1Value() / 1000000U) + 1U;
	}
	else
	{
		tempreg = ((RCC_GetPCLK1Value() / 1000000U) * 300U) / 1000U + 1U;
	}

	pI2CHandle->pI2Cx->TRISE = tempreg & 0x3FU;
}


/*****************************************************************************************
 * @fn			- I2C_DeInit
 *****************************************************************************************/
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
	if (pI2Cx == I2C1)
	{
		I2C1_REG_RESET();
	}
	else if (pI2Cx == I2C2)
	{
		I2C2_REG_RESET();
	}
	else if (pI2Cx == I2C3)
	{
		I2C3_REG_RESET();
	}
}


/*
 ******************************************************************************************
 * 							BLOCKING DATA TRANSFER
 ******************************************************************************************
 */

/*****************************************************************************************
 * @fn			- I2C_MasterSendData
 *****************************************************************************************/
void I2C_MasterSendData(
		I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	// Generate START
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	// Confirm that start generation is completed by checking the SB flag in the SR1
	// Until SB is cleared SCL will be stretched (pulled to LOW)
	while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB));

	// Send the address of the slave with r/nw bit set to w(0) (total 8 bits )
	// Send address + WRITE
	I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, SlaveAddr);

	// Wait for ADDR : Confirm that address phase is completed by checking the ADDR flag in teh SR1
	while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));

	// Clear ADDR : clear the ADDR flag according to its software sequence
	// Note: Until ADDR is cleared SCL will be stretched (pulled to LOW)
	I2C_ClearADDRFlag(pI2CHandle);

	// Send data
	while (Len > 0U)
	{
		while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));

		pI2CHandle->pI2Cx->DR = *pTxBuffer;
		pTxBuffer++;
		Len--;
	}

	// Wait for final byte transfer : when Len becomes zero wait for TXE=1 and BTF=1 before generating the STOP condition
	//   Note: TXE=1 , BTF=1 , means that both SR and DR are empty and next transmission should begin
	//   when BTF=1 SCL will be stretched (pulled to LOW)
	while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));

	while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF));

	// Generate STOP
	if (Sr == I2C_DISABLE_SR)
	{
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
	}
}


/*****************************************************************************************
 * @fn			- I2C_MasterReceiveData
 *****************************************************************************************/
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	// Generate START
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	// Wait for SB
	while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB));

	// Send address + READ
	I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, SlaveAddr);

	// Wait for ADDR
	while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));

	// 1-byte reception
	if (Len == 1U)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

		I2C_ClearADDRFlag(pI2CHandle);

		while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

		if (Sr == I2C_DISABLE_SR)
		{
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		}

		*pRxBuffer = (uint8_t)pI2CHandle->pI2Cx->DR;
	}


	/*
	 * 2-byte reception
	 *
	 * NOTE:
	 * Proper POS handling is required for the STM32F4 I2C peripheral. With
	 * only 2 bytes, there's no "wait for RXNE, decide ACK" window like the
	 * N>=3 case has — both bytes end up sitting in DR/shift-register at once.
	 * POS=1 redirects the ACK/NACK control to apply to the *next* byte after
	 * the one currently in the shift register (instead of the one in DR),
	 * which is what lets ACK get disabled for the 2nd byte while the 1st is
	 * still being clocked in.
	 */
	else if (Len == 2U)
	{
		// Set POS — ACK control now refers to the byte after the current one
		pI2CHandle->pI2Cx->CR1 |= (1U << I2C_CR1_POS);

		// Disable ACK
		I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

		// Clear ADDR
		I2C_ClearADDRFlag(pI2CHandle);

		// Wait for BTF
		while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF));

		// Generate STOP
		if (Sr == I2C_DISABLE_SR)
		{
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		}

		// Read both bytes
		*pRxBuffer = (uint8_t)pI2CHandle->pI2Cx->DR;
		pRxBuffer++;

		*pRxBuffer = (uint8_t)pI2CHandle->pI2Cx->DR;

		// Clear POS
		pI2CHandle->pI2Cx->CR1 &= ~(1U << I2C_CR1_POS);
	}


	/*****************************************************************************************
	 * 3 or more bytes
	 *****************************************************************************************/
	else
	{
		I2C_ClearADDRFlag(pI2CHandle);

		while (Len > 3U)
		{
			while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

			*pRxBuffer = (uint8_t)pI2CHandle->pI2Cx->DR;
			pRxBuffer++;
			Len--;
		}

		// Wait for BTF for final three bytes.
		while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF));

		// Disable ACK
		I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

		// Read N-2 byte
		*pRxBuffer = (uint8_t)pI2CHandle->pI2Cx->DR;
		pRxBuffer++;

		// Generate STOP
		if (Sr == I2C_DISABLE_SR)
		{
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		}

		// Read N-1 byte
		*pRxBuffer = (uint8_t)pI2CHandle->pI2Cx->DR;
		pRxBuffer++;

		// Wait for RXNE
		while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));


		// Read final byte
		*pRxBuffer = (uint8_t)pI2CHandle->pI2Cx->DR;
	}


	// Restore ACK configuration.
	if (pI2CHandle->I2C_Config.I2C_AckControl == I2C_ACK_ENABLE)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_ENABLE);
	}
}


/*****************************************************************************************
 * @fn			- I2C_SlaveSendData
 *****************************************************************************************/
void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx,uint8_t data)
{
	pI2Cx->DR = data;
}


/*****************************************************************************************
 * @fn			- I2C_SlaveReceiveData
 *****************************************************************************************/
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2Cx)
{
	return (uint8_t)pI2Cx->DR;
}


/*
 ******************************************************************************************
 * 							INTERRUPT-DRIVEN TRANSFER
 ******************************************************************************************
 */


/*
 * @fn			- I2C_MasterSendDataIT
 *
 * @brief		- Kicks off a non-blocking master transmit. Loads the handle
 *                with the transfer context, fires START, then arms the event
 *                (SB/ADDR/BTF/TXE), buffer (TXE/RXNE), and error interrupts so
 *                the rest of the transfer runs entirely from I2C_EV_IRQHandling
 *                / I2C_ER_IRQHandling.
 *
 * @note		- If a transfer is already in flight (TxRxState is BUSY_IN_TX
 *                or BUSY_IN_RX), this call is a no-op and just reports the
 *                current busy state back to the caller — it does NOT queue
 *                the new request.
 */
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	uint8_t busystate = pI2CHandle->TxRxState;

	if ((busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxBuffer = pTxBuffer;
		pI2CHandle->TxLen = Len;

		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;

		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		pI2CHandle->pI2Cx->CR2 |= (1U << I2C_CR2_ITEVTEN);
		pI2CHandle->pI2Cx->CR2 |= (1U << I2C_CR2_ITBUFEN);
		pI2CHandle->pI2Cx->CR2 |= (1U << I2C_CR2_ITERREN);
	}

	return busystate;
}


/*****************************************************************************************
 * @fn			- I2C_MasterReceiveDataIT
 *
 * @brief		- Kicks off a non-blocking master receive. Same pattern as
 *                I2C_MasterSendDataIT(), but also stashes the original
 *                requested length in RxSize (in addition to the countdown
 *                counter RxLen). RxSize is never decremented — it's what lets
 *                I2C_ClearADDRFlag() and I2C_MasterHandleRXNEInterrupt() tell
 *                "is this a 1-byte transfer" and "are we 2 bytes from the end"
 *                apart from "how many bytes are left right now".
 *
 * @note		- Same busy-check caveat as I2C_MasterSendDataIT(): a request
 *                made while another transfer is in flight is dropped, not
 *                queued.
 *****************************************************************************************/
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	uint8_t busystate = pI2CHandle->TxRxState;


	if ((busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pRxBuffer = pRxBuffer;

		pI2CHandle->RxLen = Len;
		pI2CHandle->RxSize = Len;

		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;

		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;


		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		pI2CHandle->pI2Cx->CR2 |= (1U << I2C_CR2_ITEVTEN);
		pI2CHandle->pI2Cx->CR2 |= (1U << I2C_CR2_ITBUFEN);
		pI2CHandle->pI2Cx->CR2 |= (1U << I2C_CR2_ITERREN);
	}

	return busystate;
}


/*
 * @fn		- I2C_CloseSendData
 *
 * @brief	- Tears down an IT-driven transmit once it's complete: disables
 *              the buffer/event interrupts so TXE/BTF stop firing, resets the
 *              handle back to I2C_READY, and clears the buffer pointer/length
 *              so a stale pointer can't be walked by a late interrupt.
 *
 * @note	- ITERREN (error interrupts) is deliberately left enabled — errors
 *              can still occur on the bus after a logical transfer completes
 *              and should still be reported.
 */
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
	pI2CHandle->pI2Cx->CR2 &= ~(1U << I2C_CR2_ITBUFEN);
	pI2CHandle->pI2Cx->CR2 &= ~(1U << I2C_CR2_ITEVTEN);
	pI2CHandle->TxRxState =	I2C_READY;

	pI2CHandle->pTxBuffer = NULL;
	pI2CHandle->TxLen = 0U;
}


/*
 * @fn		- I2C_CloseReceiveData
 *
 * @brief	- Same teardown as I2C_CloseSendData(), for the receive side.
 *
 * @note	- Also restores ACK if the driver's default config wants it
 *              enabled. This matters because ACK was very likely disabled
 *              mid-transfer (single-byte or last-two-bytes NACK handling —
 *              see I2C_ManageAcking()'s note) and needs to be put back before
 *              the peripheral is used for its next master-receive call.
 */
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
{
	pI2CHandle->pI2Cx->CR2 &= ~(1U << I2C_CR2_ITBUFEN);
	pI2CHandle->pI2Cx->CR2 &= ~(1U << I2C_CR2_ITEVTEN);
	pI2CHandle->TxRxState = I2C_READY;

	pI2CHandle->pRxBuffer = NULL;

	pI2CHandle->RxLen = 0U;
	pI2CHandle->RxSize = 0U;

	if (pI2CHandle->I2C_Config.I2C_AckControl == I2C_ACK_ENABLE)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_ENABLE);
	}
}


/*
 ******************************************************************************************
 *
 * 							SLAVE CALLBACK CONTROL
 *
 ******************************************************************************************
 */

/*
 * @fn		- I2C_SlaveEnableDisableCallbackEvents
 *
 * @brief	- Arms/disarms all three I2C interrupt sources (ITEVTEN, ITBUFEN,
 *              ITERREN) at once. Intended for slave mode: unlike the master
 *              IT calls above, a slave has no "start a transfer" API — it
 *              just needs to be listening on the EV/ER IRQ lines so
 *              I2C_EV_IRQHandling() can hand DATA_REQ/DATA_RCV events up to
 *              I2C_ApplicationEventCallback() whenever a master addresses it.
 */
void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if (EnOrDi == ENABLE)
	{
		pI2Cx->CR2 |= (1U << I2C_CR2_ITEVTEN);
		pI2Cx->CR2 |= (1U << I2C_CR2_ITBUFEN);
		pI2Cx->CR2 |= (1U << I2C_CR2_ITERREN);
	}
	else
	{
		pI2Cx->CR2 &= ~(1U << I2C_CR2_ITEVTEN);
		pI2Cx->CR2 &= ~(1U << I2C_CR2_ITBUFEN);
		pI2Cx->CR2 &= ~(1U << I2C_CR2_ITERREN);
	}
}


/*
 ******************************************************************************************
 *
 * 							NVIC CONFIGURATION
 *
 ******************************************************************************************
 */

/*
 * @fn			- I2C_IRQInterruptConfig
 *
 * @brief		- Sets/clears the given IRQ's bit in the Cortex-M4 NVIC
 *              ISERx/ICERx (Set/Clear-Enable) registers. This is generic NVIC
 *              control, not I2C-specific — it lives here because it's what
 *              I2C_EV_IRQHandling/I2C_ER_IRQHandling need enabled before any
 *              I2Cx_EV/I2Cx_ER interrupt (see IRQ_NO_I2Cx_EV/_ER in the
 *              header) can actually reach the CPU.
 *
 * @note		- ISER/ICER are banked in 32-bit groups, hence IRQNumber % 32
 *              (and the corresponding %64 branch, matching this MCU's IRQ
 *              range) to find the bit within the right register.
 */
void I2C_IRQInterruptConfig(
		uint8_t IRQNumber,
		uint8_t EnOrDi)
{
	if (EnOrDi == ENABLE)
	{
		if (IRQNumber <= 31U)
		{
			*NVIC_ISER0 |= (1U << IRQNumber);
		}
		else if (IRQNumber <= 63U)
		{
			*NVIC_ISER1 |= (1U << (IRQNumber % 32U));
		}
		else if (IRQNumber <= 95U)
		{
			*NVIC_ISER2 |= (1U << (IRQNumber % 64U));
		}
	}
	else
	{
		if (IRQNumber <= 31U)
		{
			*NVIC_ICER0 = (1U << IRQNumber);
		}
		else if (IRQNumber <= 63U)
		{
			*NVIC_ICER1 = (1U << (IRQNumber % 32U));
		}
		else if (IRQNumber <= 95U)
		{
			*NVIC_ICER2 = (1U << (IRQNumber % 64U));
		}
	}
}


/***********************************************************************************
 * @fn			- I2C_IRQPriorityConfig
 *
 * @brief		- Writes an interrupt's priority into the NVIC IPRx registers.
 *
 * @note		- Each IPRx register packs 4 IRQs (one byte each), so
 *              iprx = IRQNumber / 4 picks the register and iprx_section
 *              (IRQNumber % 4) picks the byte within it. Cortex-M4 only
 *              implements the top NO_PR_BITS_IMPLEMENTED (4) bits of each
 *              priority byte, so the value is left-shifted within that byte —
 *              that's the "+ (8 - NO_PR_BITS_IMPLEMENTED)" term.
 ***********************************************************************************/
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	uint8_t iprx;
	uint8_t iprx_section;
	uint8_t shift_amount;

	iprx = IRQNumber / 4U;

	iprx_section = IRQNumber % 4U;

	shift_amount = (8U * iprx_section) + (8U - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASE_ADDR + iprx) &= ~(0xFFU << shift_amount);
	*(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}


/*
 ******************************************************************************************
 *
 * 							EVENT IRQ HANDLER
 *
 ******************************************************************************************
 */

/***********************************************************************************
 * @fn			- I2C_EV_IRQHandling
 *
 * @brief		- Single handler for the I2Cx_EV interrupt line, covering both
 *              master and slave event sequences (EV5-EV8 in the reference
 *              manual's event numbering). Each block below independently
 *              polls one SR1 flag; several can legitimately be true and
 *              handled in the same call.
 *
 * @note		- temp1/temp2 are read once up front as the ITEVTEN/ITBUFEN
 *              *enable* bits (from CR2), not status flags — every block below
 *              gates on temp1 (and temp2 where it's a buffer event) so a flag
 *              is only acted on if its interrupt was actually armed.
 ***********************************************************************************/
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	uint32_t temp1;
	uint32_t temp2;
	uint32_t temp3;

	// ITEVTEN
	temp1 = pI2CHandle->pI2Cx->CR2 & (1U << I2C_CR2_ITEVTEN);
	// ITBUFEN
	temp2 = pI2CHandle->pI2Cx->CR2 & (1U << I2C_CR2_ITBUFEN);
	// EV5 : SB — START condition has gone out on the bus. The next thing the
	// peripheral needs is the slave address, direction depends on whether
	// this is a master-transmit or master-receive sequence.
	temp3 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_SB);

	if (temp1 && temp3)
	{
		if (pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}
		else if (pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}
	}

	// EV6 : ADDR — address phase completed (master: address acked by slave;
	// slave: our own address matched). Clearing it here is what lets SCL
	// resume toggling, since ADDR being set stretches the clock.
	temp3 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_ADDR);

	if (temp1 && temp3)
	{
		I2C_ClearADDRFlag(pI2CHandle);
	}

	// EV8_2 : BTF (Byte Transfer Finished) alongside TXE — both set means the
	// shift register AND the data register are empty, i.e. the last byte has
	// actually gone out on the wire, not just been queued. This is the
	// correct point to generate STOP for a master-transmit; doing it on TXE
	// alone would risk cutting the final byte short.
	temp3 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_BTF);

	if (temp1 && temp3)
	{
		if (pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			if (pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_TXE))
			{
				if (pI2CHandle->TxLen == 0U)
				{
					//1. generate the STOP condition
					if (pI2CHandle->Sr == I2C_DISABLE_SR)
					{
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
					}
					//2. reset all the member elements of the handle structure.
					I2C_CloseSendData(pI2CHandle);
					//3. notify the application about transmission complete
					I2C_ApplicationEventCallback(pI2CHandle,I2C_EVENT_TX_CMPLT);
				}
			}
		}
	}

	//EV4 : STOPF — set in slave mode when a master ends the transfer with
	// STOP. Per the reference manual's clear sequence this needs a read of
	// SR1 (already done above via the flag checks) followed by any write to
	// CR1; writing PE=1 (its current value) is the conventional no-op write
	// used purely to satisfy that sequence.
	temp3 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_STOPF);

	if (temp1 && temp3)
	{
		// Clear STOPF
		pI2CHandle->pI2Cx->CR1 |= (1U << I2C_CR1_PE);

		I2C_ApplicationEventCallback(pI2CHandle, I2C_EVENT_STOP);
	}

	// EV8 : TXE — data register is empty and ready for the next byte.
	// Only meaningful with ITBUFEN (temp2) enabled, since TXE is a buffer
	// event. Master vs slave is disambiguated via SR2.MSL; in slave mode
	// TRA=1 tells us the master is reading from us (data requested), so we
	// hand it up to the application instead of pulling from a TX buffer we
	// don't own.
	temp3 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_TXE);

	if (temp1 && temp2 && temp3)
	{
		if (pI2CHandle->pI2Cx->SR2 & (1U << I2C_SR2_MSL))
		{
			if (pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
			{
				I2C_MasterHandleTXEInterrupt(pI2CHandle);
			}
		}
		else
		{
			if (pI2CHandle->pI2Cx->SR2 & (1U << I2C_SR2_TRA))
			{
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EVENT_DATA_REQ);
			}
		}
	}


	// EV7 : RXNE — data register has a received byte waiting. Same
	// master/slave split as EV8; in slave mode !TRA means we're the one
	// being written to, so a byte has arrived for the application to collect.
	temp3 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_RXNE);

	if (temp1 && temp2 && temp3)
	{
		if (pI2CHandle->pI2Cx->SR2 & (1U << I2C_SR2_MSL))
		{
			if (pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
			{
				I2C_MasterHandleRXNEInterrupt(pI2CHandle);
			}
		}
		else
		{
			if (!(pI2CHandle->pI2Cx->SR2 & (1U << I2C_SR2_TRA)))
			{
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EVENT_DATA_RCV);
			}
		}
	}
}


/*
 ******************************************************************************************
 *
 * 							ERROR IRQ HANDLER
 *
 ******************************************************************************************
 */


/*********************************************************************************
 * @fn			- I2C_ER_IRQHandling
 *
 * @brief		- Handler for the I2Cx_ER interrupt line. Unlike the SR1
 *              status flags in I2C_EV_IRQHandling (which mostly self-clear on
 *              a read-SR1-then-do-X sequence), these five error flags are
 *              cleared by writing 0 to them directly — that's the
 *              `&= ~(1 << ...)` on SR1 in each block below.
 *
 * @note		- All five gate on the single ITERREN enable bit (temp2);
 *              there's no per-error enable on the F411, so an application
 *              that wants error interrupts at all gets all five.
 *********************************************************************************/
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	uint32_t temp1;
	uint32_t temp2;

	temp2 = pI2CHandle->pI2Cx->CR2 & (1U << I2C_CR2_ITERREN);

	// Bus Error (BERR) — a START or STOP was detected at a point in the
	// frame where it's not a valid protocol position (e.g. mid-byte).
	temp1 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_BERR);

	if (temp1 && temp2)
	{
		pI2CHandle->pI2Cx->SR1 &= ~(1U << I2C_SR1_BERR);

		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_BERR);
	}


	// Arbitration Lost (ARLO) — only relevant on a multi-master bus; this
	// device lost control of SDA to another master mid-transfer and has
	// automatically reverted to slave mode.
	temp1 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_ARLO);

	if (temp1 && temp2)
	{
		pI2CHandle->pI2Cx->SR1 &= ~(1U << I2C_SR1_ARLO);

		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_ARLO);
	}


	// Acknowledge Failure (AF) — the addressed slave (or, in slave mode, the
	// master) didn't ACK a byte we sent. Most commonly: wrong slave address,
	// slave not present/powered, or slave NACKing a write it rejected.
	temp1 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_AF);

	if (temp1 && temp2)
	{
		pI2CHandle->pI2Cx->SR1 &= ~(1U << I2C_SR1_AF);

		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_AF);
	}

	// Overrun / Underrun (OVR) — in slave receive, a new byte arrived in DR
	// before the previous one was read out (overrun); in slave transmit, SCL
	// low-stretching couldn't hold and DR wasn't refilled in time (underrun).
	temp1 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_OVR);

	if (temp1 && temp2)
	{
		pI2CHandle->pI2Cx->SR1 &= ~(1U << I2C_SR1_OVR);

		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_OVR);
	}

	// Timeout — SMBus-style Tlow error: SCL was held low longer than the
	// protocol allows (typically points to a stuck/clock-stretching-forever
	// slave, or a bus lockup).
	temp1 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_TIMEOUT);

	if (temp1 && temp2)
	{
		pI2CHandle->pI2Cx->SR1 &= ~(1U << I2C_SR1_TIMEOUT);

		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_TIMEOUT);
	}
}


/*
 ******************************************************************************************
 *
 * 							APPLICATION CALLBACK
 *
 ******************************************************************************************
 */


/*********************************************************************************
 * @fn			- I2C_ApplicationEventCallback
 *
 * @brief		- Weak callback function for I2C events/errors.
 *
 *********************************************************************************/
__weak void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv)
{
	(void)pI2CHandle;
	(void)AppEv;
}
