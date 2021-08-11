/*
 * @Author: yz.zhang[zhang.yz2@yuyue.com.cn]
 * @Date: 2021-07-22 18:26:38
 * @LastEditors: yz.zhang
 * @LastEditTime: 2021-08-02 18:53:55
 * @FilePath: \STM32F10x_FWLIB_Template\User\SystemInit.c.txt
 * @Description:
 */

#include "SystemInit.h"

__IO Word_Type System_Init_Flag;

static void System_Clock_Init(void)
{
  RCC_DeInit();
  RCC_HSEConfig(RCC_HSE_OFF);
  RCC_AdjustHSICalibrationValue(0xF0);
  RCC_HSICmd(ENABLE);
  RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16); // 8M / 2 * 16 = 64M
  RCC_PLLCmd(ENABLE);
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  while (RCC_GetSYSCLKSource() != 0x08)
  {
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  }

  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  RCC_PCLK2Config(RCC_HCLK_Div1);

  SystemCoreClockUpdate();
  RCC_ITConfig(RCC_IT_HSERDY | RCC_IT_HSIRDY | RCC_IT_LSERDY | RCC_IT_LSIRDY |
                   RCC_IT_PLLRDY,
               DISABLE);
  RCC_LSEConfig(RCC_LSE_OFF);
  RCC_LSICmd(DISABLE);
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);
  RCC_ClockSecuritySystemCmd(DISABLE);

  RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  System_Init_Flag.Bits.b0 = 1;
}

static void GPIO_Function_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1);
  System_Init_Flag.Bits.b1 = 1;
}

static void USART_Function_Init(void)
{
  USART_DeInit(USART1);

  USART_InitTypeDef USART_InitStruct;
  USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Tx;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  USART_Init(USART1, &USART_InitStruct);

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  USART_Cmd(USART1, ENABLE);
  System_Init_Flag.Bits.b2 = 1;
}

static void BasicTimer_Function_Init(void)
{
  TIM_DeInit(TIM6);

  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
  TIM_TimeBaseInitStruct.TIM_Prescaler = SystemCoreClock / 1000 - 1; // 1ms
  TIM_TimeBaseInitStruct.TIM_Period = 500 - 1;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);

  TIM_ClearFlag(TIM6, TIM_FLAG_Update);
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM6, DISABLE);
  System_Init_Flag.Bits.b3 = 1;
}

static void ADC_Function_Init(void)
{
  ADC_DeInit(ADC1);

  ADC_InitTypeDef ADC_InitStruct;
  ADC_StructInit(&ADC_InitStruct);
  ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;

  RCC_ADCCLKConfig(RCC_PCLK2_Div8);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  ADC_Init(ADC1, &ADC_InitStruct);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_13Cycles5);
  ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

  /************ ADC1 Calibration ******************/
  ADC_Cmd(ADC1, ENABLE);
  ADC_ResetCalibration(ADC1);
  while (ADC_GetResetCalibrationStatus(ADC1) == SET)
    ;
  ADC_StartCalibration(ADC1);
  while (ADC_GetCalibrationStatus(ADC1) == SET)
    ;
  /************ ADC1 Calibration End **************/
  ADC_ExternalTrigConvCmd(ADC1, DISABLE);
  ADC_TempSensorVrefintCmd(ENABLE);
  System_Init_Flag.Bits.b4 = 1;
}

static unsigned char NVIC_Function_Init(void)
{
  __IO Byte_Type NVIC_Init_Flag;
  NVIC_Init_Flag.Byte = 0x00;
  // Set interrupt priority group, two Preemption two Sub
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitTypeDef NVIC_InitStruct;

  /***************** TIM6 ******************/
  NVIC_InitStruct.NVIC_IRQChannel = TIM6_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
  NVIC_Init_Flag.Bits.b0 = 1;
  /***************** TIM6 ******************/

  /***************** ADC1_2 ******************/
  NVIC_InitStruct.NVIC_IRQChannel = ADC1_2_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
  NVIC_Init_Flag.Bits.b1 = 1;
  /***************** ADC1_2 ******************/
  return NVIC_Init_Flag.Byte;
}

uint16_t ALL_System_Driver_Init(void)
{
  System_Init_Flag.Word = 0x0000;
  System_Clock_Init();
  GPIO_Function_Init();
  USART_Function_Init();
  BasicTimer_Function_Init();
  ADC_Function_Init();

  System_Init_Flag.Byte[1] = NVIC_Function_Init();
  return System_Init_Flag.Word;
}
