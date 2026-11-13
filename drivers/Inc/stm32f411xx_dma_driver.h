/*
 * stm32f411xx_dma_driver.h
 * Created on: 08-Sept-2026
 * Author: Abhishek_Bharadwaj
 */

#ifndef INC_STM32F411XX_DMA_DRIVER_H_
#define INC_STM32F411XX_DMA_DRIVER_H_

#include "stm32f411xx.h"
#include <stdint.h>

/*
 * DMA Configuration Structure
 * Used to configure a DMA stream for data transfer.
 */
typedef struct
{
    uint32_t DMA_Channel;
    uint32_t DMA_Stream;

    uint32_t DMA_PeripheralAddress;
    uint32_t DMA_Memory0Address;
    uint32_t DMA_Memory1Address;

    uint16_t DMA_NumberOfData;

    uint32_t DMA_Direction;

    uint32_t DMA_PeripheralIncrement;
    uint32_t DMA_MemoryIncrement;

    uint32_t DMA_PeripheralDataSize;
    uint32_t DMA_MemoryDataSize;

    uint32_t DMA_Mode;
    uint32_t DMA_DoubleBufferMode;

    uint32_t DMA_Priority;

    uint32_t DMA_FIFOMode;
    uint32_t DMA_FIFOThreshold;

    uint32_t DMA_MemoryBurst;
    uint32_t DMA_PeripheralBurst;

} DMA_Config_t;


/*
 * DMA Handle Structure
 * Used to manage the DMA controller and stream.
 */
typedef struct
{
    DMA_RegDef_t        *pDMAx;
    DMA_Stream_RegDef_t *pDMA_Stream;

    DMA_Config_t DMA_Config;

    uint8_t DMA_State;

} DMA_Handle_t;


/*=========================================================
 * DMA Channel Selection Macros
 *=========================================================*/
#define DMA_CHANNEL_0       0U
#define DMA_CHANNEL_1       1U
#define DMA_CHANNEL_2       2U
#define DMA_CHANNEL_3       3U
#define DMA_CHANNEL_4       4U
#define DMA_CHANNEL_5       5U
#define DMA_CHANNEL_6       6U
#define DMA_CHANNEL_7       7U


/*=========================================================
 * DMA Stream Selection Macros
 *=========================================================*/
#define DMA_STREAM_0        0U
#define DMA_STREAM_1        1U
#define DMA_STREAM_2        2U
#define DMA_STREAM_3        3U
#define DMA_STREAM_4        4U
#define DMA_STREAM_5        5U
#define DMA_STREAM_6        6U
#define DMA_STREAM_7        7U


/*=========================================================
 * DMA Direction Selection Macros
 * SxCR DIR[1:0]
 *=========================================================*/
#define DMA_DIR_PERIPHERAL_TO_MEMORY    0U
#define DMA_DIR_MEMORY_TO_PERIPHERAL    1U
#define DMA_DIR_MEMORY_TO_MEMORY        2U


/*=========================================================
 * DMA Data Size Selection Macros
 * SxCR PSIZE[1:0] and MSIZE[1:0]
 *=========================================================*/
#define DMA_SIZE_8BIT       0U
#define DMA_SIZE_16BIT      1U
#define DMA_SIZE_32BIT      2U


/*=========================================================
 * DMA Mode Selection Macros
 * SxCR CIRC bit
 *=========================================================*/
#define DMA_MODE_NORMAL     0U
#define DMA_MODE_CIRCULAR   1U


/*=========================================================
 * DMA Double Buffer Mode
 * SxCR DBM bit
 *=========================================================*/
#define DMA_DOUBLE_BUFFER_DISABLE    0U
#define DMA_DOUBLE_BUFFER_ENABLE     1U


/*=========================================================
 * DMA Priority Selection Macros
 * SxCR PL[1:0]
 *=========================================================*/
#define DMA_PRIORITY_LOW       0U
#define DMA_PRIORITY_MEDIUM    1U
#define DMA_PRIORITY_HIGH      2U
#define DMA_PRIORITY_VERY_HIGH 3U


/*=========================================================
 * DMA Increment Selection Macros
 * SxCR PINC and MINC bits
 *=========================================================*/
#define DMA_PINC_DISABLE    0U
#define DMA_PINC_ENABLE     1U

#define DMA_MINC_DISABLE    0U
#define DMA_MINC_ENABLE     1U


/*=========================================================
 * DMA FIFO Mode Selection Macros
 * SxFCR DMDIS bit
 *=========================================================*/
#define DMA_FIFO_MODE_DISABLE   0U
#define DMA_FIFO_MODE_ENABLE    1U


/*=========================================================
 * DMA FIFO Threshold Selection Macros
 * SxFCR FTH[1:0]
 *=========================================================*/
#define DMA_FIFO_THRESHOLD_1QUARTER    0U
#define DMA_FIFO_THRESHOLD_1HALF       1U
#define DMA_FIFO_THRESHOLD_3QUARTER    2U
#define DMA_FIFO_THRESHOLD_FULL        3U


/*=========================================================
 * DMA Burst Selection Macros
 * SxCR MBURST and PBURST
 *=========================================================*/
#define DMA_BURST_SINGLE     0U
#define DMA_BURST_INCR4      1U
#define DMA_BURST_INCR8      2U
#define DMA_BURST_INCR16     3U


/*=========================================================
 * DMA Interrupt Selection Macros
 * Software-defined interrupt mask
 *=========================================================*/
#define DMA_IT_TC       (1U << 0)
#define DMA_IT_HT       (1U << 1)
#define DMA_IT_TE       (1U << 2)
#define DMA_IT_DME      (1U << 3)
#define DMA_IT_FE       (1U << 4)


/*=========================================================
 * DMA Status Flag Selection Macros
 * Software-defined flag mask
 *=========================================================*/
#define DMA_FLAG_TC     (1U << 0)
#define DMA_FLAG_HT     (1U << 1)
#define DMA_FLAG_TE     (1U << 2)
#define DMA_FLAG_DME    (1U << 3)
#define DMA_FLAG_FE     (1U << 4)


/*=========================================================
 * DMA State Macros
 *=========================================================*/
#define DMA_READY       0U
#define DMA_BUSY        1U


/*=========================================================
 * DMA Application Event Macros
 *=========================================================*/
#define DMA_EVENT_TRANSFER_COMPLETE     0U
#define DMA_EVENT_HALF_TRANSFER         1U
#define DMA_EVENT_TRANSFER_ERROR        2U
#define DMA_EVENT_DIRECT_MODE_ERROR     3U
#define DMA_EVENT_FIFO_ERROR            4U


/*=========================================================
 * DMA Return Status Macros
 *=========================================================*/
#define DMA_STATUS_OK           0U
#define DMA_STATUS_ERROR        1U
#define DMA_STATUS_BUSY         2U
#define DMA_STATUS_INVALID      3U


/*=========================================================
 * Peripheral Clock Control
 *=========================================================*/
void DMA_PeriClkControl(DMA_RegDef_t *pDMAx, uint8_t EnorDi);


/*=========================================================
 * Initialization and Deinitialization
 *=========================================================*/
uint8_t DMA_Init(DMA_Handle_t *pDMAHandle);
void DMA_DeInit(DMA_RegDef_t *pDMAx);


/*=========================================================
 * Stream Control APIs
 *=========================================================*/
void DMA_StreamEnable(DMA_Handle_t *pDMAHandle, uint8_t EnorDi);


/*=========================================================
 * Address Configuration APIs
 *=========================================================*/
void DMA_SetPeripheralAddress(DMA_Handle_t *pDMAHandle, uint32_t PeriphAddress);
void DMA_SetMemory0Address(DMA_Handle_t *pDMAHandle, uint32_t MemAddress);
void DMA_SetMemory1Address(DMA_Handle_t *pDMAHandle, uint32_t MemAddress);


/*=========================================================
 * Transfer Configuration APIs
 *=========================================================*/
void DMA_SetNumberOfData(DMA_Handle_t *pDMAHandle, uint16_t NumData);
uint16_t DMA_GetRemainingData(DMA_Handle_t *pDMAHandle);


/*=========================================================
 * Transfer Start APIs
 *=========================================================*/
void DMA_Start(DMA_Handle_t *pDMAHandle);
void DMA_Start_IT(DMA_Handle_t *pDMAHandle);


/*=========================================================
 * DMA Flag APIs
 *=========================================================*/
uint8_t DMA_GetFlagStatus(DMA_Handle_t *pDMAHandle, uint32_t Flag);
void DMA_ClearFlag(DMA_Handle_t *pDMAHandle, uint32_t Flag);


/*=========================================================
 * DMA Interrupt APIs
 *=========================================================*/
void DMA_ITConfig(DMA_Handle_t *pDMAHandle, uint32_t ITMask, uint8_t EnorDi);
void DMA_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void DMA_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void DMA_IRQHandling(DMA_Handle_t *pDMAHandle);


/*=========================================================
 * Application Callback
 * Implemented as weak function in .c file.
 *=========================================================*/
void DMA_ApplicationEventCallback(DMA_Handle_t *pDMAHandle, uint8_t AppEv);


#endif /* INC_STM32F411XX_DMA_DRIVER_H_ */
