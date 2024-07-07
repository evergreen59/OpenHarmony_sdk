/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (C) 2022, Xiaohua Semiconductor Co.
  * All rights reserved.
  *
  * This software component is licensed by XHSC under BSD 3-Clause license
  * (the "License"); You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                    opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "los_config.h"
#include "ev_hc32f4a0_lqfp176.h"

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    int ret;
    /* MCU Peripheral registers write unprotected */
    LL_PERIPH_WE(LL_PERIPH_EFM | LL_PERIPH_FCG | LL_PERIPH_GPIO | LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_SRAM);

    /* Initialize BSP system clock. */
    BSP_CLK_Init();

    DDL_PrintfInit(BSP_PRINTF_DEVICE, 115200, BSP_PRINTF_Preinit);
    printf("%s user main!\n", __TIME__);
    ret = LOS_KernelInit();
    if (ret != LOS_OK) {
        printf("Liteos kernel init failed! ERROR: 0x%x\n", ret);
    } else {
        DeviceManagerStart();
        OHOS_SystemInit();
        //TaskSample();
        LOS_Start();
    }

    /* Infinite loop */
    while (1)
    {
    }
}

/**
  * @}
  */

/**
  * @}
  */

