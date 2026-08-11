/*
 * stm32f411xx_rcc_driver.c
 *
 *  Created on: Aug 10, 2026
 *  Author: Abhishek_Bharadwaj
 */


#include "stm32f411xx_rcc_driver.h"

/* AHB Prescaler values corresponding to HPRE[3:0] */
static const uint16_t AHB_PreScaler[8] =
{
    2,
    4,
    8,
    16,
    64,
    128,
    256,
    512
};

/* APB Prescaler values corresponding to PPRE[2:0] */
static const uint8_t APB_PreScaler[4] =
{
    2,
    4,
    8,
    16
};


/***********************************************************************
 * @fn             RCC_GetPCLK1Value
 *
 * @brief          Returns the APB1 peripheral clock frequency.
 *
 * @param[in]      None
 *
 * @return         APB1 peripheral clock frequency in Hz
 *
 * @Note           PCLK1 = SYSCLK / AHB_Prescaler / APB1_Prescaler
 *
 ***********************************************************************/
uint32_t RCC_GetPCLK1Value(void)
{
    uint32_t SystemClk;
    uint32_t pclk1;

    uint32_t ahbp;
    uint32_t apb1p;

    uint8_t clksrc;
    uint8_t temp;


    /* Get system clock source */
    clksrc = (uint8_t)((RCC->CFGR >> 2) & 0x03);


    /*
     * Determine system clock frequency
     *
     * 00 -> HSI
     * 01 -> HSE
     * 10 -> PLL
     * (11 -> Not applicable)
     */
    if(clksrc == 0)
    {
        SystemClk = HSI_VALUE;
    }
    else if(clksrc == 1)
    {
        SystemClk = HSE_VALUE;
    }
    else if(clksrc == 2)
    {
        SystemClk = RCC_GetPLLOutputClock();
    }
    else
    {
        SystemClk = HSI_VALUE;
    }


    /* Get AHB prescaler */
    temp = (uint8_t)((RCC->CFGR >> 4) & 0x0F);

    if(temp < 8)
    {
        ahbp = 1;
    }
    else
    {
        ahbp = AHB_PreScaler[temp - 8];
    }


    /* Get APB1 prescaler */
    temp = (uint8_t)((RCC->CFGR >> 10) & 0x07);

    if(temp < 4)
    {
        apb1p = 1;
    }
    else
    {
        apb1p = APB_PreScaler[temp - 4];
    }


    /* Calculate PCLK1 */
    pclk1 = (SystemClk / ahbp) / apb1p;


    return pclk1;
}


/***********************************************************************
 * @fn             RCC_GetPCLK2Value
 *
 * @brief          Returns the APB2 peripheral clock frequency.
 *
 * @param[in]      None
 *
 * @return         APB2 peripheral clock frequency in Hz
 *
 * @Note           PCLK2 = SYSCLK / AHB_Prescaler / APB2_Prescaler
 *
 ***********************************************************************/
uint32_t RCC_GetPCLK2Value(void)
{
    uint32_t SystemClk;
    uint32_t pclk2;

    uint32_t ahbp;
    uint32_t apb2p;

    uint8_t clksrc;
    uint8_t temp;


    /* Get system clock source */
    clksrc = (uint8_t)((RCC->CFGR >> 2) & 0x03);


    /*
     * Determine system clock frequency
     *
     * 00 -> HSI
     * 01 -> HSE
     * 10 -> PLL
     */
    if(clksrc == 0)
    {
        SystemClk = HSI_VALUE;
    }
    else if(clksrc == 1)
    {
        SystemClk = HSE_VALUE;
    }
    else if(clksrc == 2)
    {
        SystemClk = RCC_GetPLLOutputClock();
    }
    else
    {
        SystemClk = HSI_VALUE;
    }


    /* Get AHB prescaler */
    temp = (uint8_t)((RCC->CFGR >> 4) & 0x0F);

    if(temp < 8)
    {
        ahbp = 1;
    }
    else
    {
        ahbp = AHB_PreScaler[temp - 8];
    }


    /* Get APB2 prescaler */
    temp = (uint8_t)((RCC->CFGR >> 13) & 0x07);

    if(temp < 4)
    {
        apb2p = 1;
    }
    else
    {
        apb2p = APB_PreScaler[temp - 4];
    }


    /* Calculate PCLK2 */
    pclk2 = (SystemClk / ahbp) / apb2p;


    return pclk2;
}


/***********************************************************************
 * @fn             RCC_GetPLLOutputClock
 *
 * @brief          Returns the PLL output clock frequency.
 *
 * @param[in]      None
 *
 * @return         PLL output clock frequency in Hz
 *
 * @Note
 *
 *          PLLCLK = (PLL Input Clock / PLLM) * PLLN / PLLP
 *
 *          PLLSRC:
 *          0 -> HSI
 *          1 -> HSE
 *
 *          PLLP encoding:
 *          00 -> 2
 *          01 -> 4
 *          10 -> 6
 *          11 -> 8
 *
 ***********************************************************************/
uint32_t RCC_GetPLLOutputClock(void)
{
    uint32_t pll_input_clk;
    uint32_t vco_output_clk;
    uint32_t pll_output_clk;

    uint32_t pllm;
    uint32_t plln;
    uint32_t pllp;


    /* Determine PLL input clock source */

    if(RCC->PLLCFGR & (1U << 22))
    {
        /* PLL source = HSE */
        pll_input_clk = HSE_VALUE;
    }
    else
    {
        /* PLL source = HSI */
        pll_input_clk = HSI_VALUE;
    }


    /* Get PLLM value */

    pllm = RCC->PLLCFGR & 0x3F;


    /* Get PLLN value */

    plln = (RCC->PLLCFGR >> 6) & 0x1FF;


    /*
     * Get PLLP value
     *
     * PLLP bits [17:16]
     */
    pllp = (RCC->PLLCFGR >> 16) & 0x03;


    /*
     * Decode PLLP
     *
     * 00 -> 2
     * 01 -> 4
     * 10 -> 6
     * 11 -> 8
     */
    if(pllp == 0)
    {
        pllp = 2;
    }
    else if(pllp == 1)
    {
        pllp = 4;
    }
    else if(pllp == 2)
    {
        pllp = 6;
    }
    else
    {
        pllp = 8;
    }


    /*
     * Calculate VCO output frequency
     *
     * VCO = PLL input / PLLM * PLLN
     */
    vco_output_clk = (pll_input_clk / pllm) * plln;


    /*
     * Calculate PLL output frequency
     *
     * PLLCLK = VCO / PLLP
     */
    pll_output_clk = vco_output_clk / pllp;


    return pll_output_clk;
}
