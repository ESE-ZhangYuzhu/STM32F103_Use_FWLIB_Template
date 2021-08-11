/*
 * @Author: yz.zhang[zhang.yz2@yuyue.com.cn]
 * @Date: 2021-07-23 10:26:26
 * @LastEditors: yz.zhang
 * @LastEditTime: 2021-08-02 19:01:54
 * @FilePath: \STM32F10x_FWLIB_Template\App\SysTick.h.txt
 * @Description:
 */
#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

void System_TickTimer_Init(uint32_t Time_base);
void SysTick_Delay_us(__IO uint32_t us);
void SysTick_Delay_ms(__IO uint32_t ms);

#endif  // !__SYSTICK_H
