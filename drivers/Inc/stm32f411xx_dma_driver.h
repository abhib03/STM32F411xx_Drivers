/*
 * stm32f411xx_dma_driver.h
 *
 *  Created on: 08-Sept-2026
 *      Author: ASUS
 */

#ifndef INC_STM32F411XX_DMA_DRIVER_H_
#define INC_STM32F411XX_DMA_DRIVER_H_

#include "stm32f411xx.h"
#include <stdint.h>

typedef struct
{
    uint32_t DMA_Channel;
    uint32_t DMA_Stream;
    uint32_t DMA_PeripheralAddress;
    uint32_t DMA_Memory0Address;
    uint32_t DMA_Memory1Address;
    uint32_t DMA_NumberOfData;

    uint32_t DMA_Direction;
    uint32_t DMA_PeripheralIncrement;
    uint32_t DMA_MemoryIncrement;
    uint32_t DMA_PeripheralDataSize;
    uint32_t DMA_MemoryDataSize;
    uint32_t DMA_Mode;
    uint32_t DMA_Priority;
    uint32_t DMA_FIFOMode;
    uint32_t DMA_FIFOThreshold;
    uint32_t DMA_MemoryBurst;
    uint32_t DMA_PeripheralBurst;

} DMA_Config_t;

#endif /* INC_STM32F411XX_DMA_DRIVER_H_ */
