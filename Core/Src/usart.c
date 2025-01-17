/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
#include "string.h"
#include "dac.h"

uint8_t g_rx_buffer[1];    /* HAL库使用的串口接收缓冲 */
USART_DATA g_usart1;

void _sys_exit(int x)
{
    x = x;
}

/* 标准库需要的支持类型 */
struct __FILE
{
    int handle;
};

FILE __stdout;

int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
    return ch;
}
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  HAL_UART_Receive_IT(&huart1, (uint8_t *)g_rx_buffer, 1);
  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
  {
    if(g_usart1.rxcount < 127) {
      g_usart1.rxbuf[g_usart1.rxcount++] = g_rx_buffer[0];
    }
    g_usart1.rxsta = 0;
    HAL_UART_Receive_IT(&huart1, (uint8_t *)g_rx_buffer, 1);
  }
}

void usart_recvwait(void)
{
  if(g_usart1.rxcount > 0)
    g_usart1.rxsta++;
  if(g_usart1.rxsta > 10) {
    g_usart1.rxlen = g_usart1.rxcount;
    g_usart1.rxbuf[g_usart1.rxcount] = '\0';
    g_usart1.rxcount = 0;
    g_usart1.rxsta = 0;
    g_usart1.rxend = 1;
  }
}

LengthType convert_length_to_type(const char* length) {
  if (strcmp(length, "355P") == 0) {
      return LENGTH_355P;
  } else if (strcmp(length, "355S") == 0) {
      return LENGTH_355S;
  } else if (strcmp(length, "386") == 0) {
      return LENGTH_386;
  } else if (strcmp(length, "407") == 0) {
      return LENGTH_407;
  } else if (strcmp(length, "532P") == 0) {
      return LENGTH_532P;
  } else if (strcmp(length, "532S") == 0) {
      return LENGTH_532S;
  } else if (strcmp(length, "607") == 0) {
      return LENGTH_607;
  } else if (strcmp(length, "1064") == 0) {
      return LENGTH_1064;
  } else {
      return LENGTH_UNKNOWN;  // 未知类型
  }
}

void usart1_process(void) {
  int wave_type = 0;
  char length_str[32] = {0};  // 增大缓冲区
  LengthType length_type;
  char *subString;
  if(g_usart1.rxend == 1) {
    if ((subString = strstr((const char *)g_usart1.rxbuf, "SET MODE")) != NULL) {
      // 使用更严格的格式匹配
      if(sscanf(subString, "SET MODE:%31[^,],%d\r\n", length_str, &wave_type) == 2) {
        // 去除可能的空格
        char *p = length_str;
        while(*p) {
          if(*p == ' ') {
            memmove(p, p+1, strlen(p));
          } else {
            p++;
          }
        }
        length_type = convert_length_to_type(length_str);
        if(length_type != LENGTH_UNKNOWN) {
          if(wave_type >= 1 && wave_type <= 4) {
            DAC_CreateWave(length_type, wave_type - 1);
            printf("OK\r\n");
          } else {
            printf("ERROR: Unknown mode\r\n");
          }
        } else {
          printf("ERROR: Unknown length: %s\r\n", length_str);
        }
      } else {
        printf("ERROR: Unknown command\r\n");
      }
    }
    else if (strcmp((const char *)g_usart1.rxbuf, "GET STATUS\r\n") == 0) {
      printf("WAVE:%d|%d|%d,TEMP:0.00\r\n", gWave.LengthType, gWave.Type, gWave.Freq);
    }
    else {
      printf("ERROR: Unknown command\r\n");
    }
    g_usart1.rxend = 0;
  }
}
/* USER CODE END 1 */
