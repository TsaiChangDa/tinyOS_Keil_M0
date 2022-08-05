/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "os.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HardFault_Handler_c(uint32_t* hardfault_args)
{  //(void)hardfault_args;
  unsigned int stacked_r0;
  unsigned int stacked_r1;
  unsigned int stacked_r2;
  unsigned int stacked_r3;
  unsigned int stacked_r12;
  unsigned int stacked_lr;
  unsigned int stacked_pc;
  unsigned int stacked_psr;
  char pc_tmp[20];
  stacked_r0 = ((unsigned long) hardfault_args[0]);
  stacked_r1 = ((unsigned long) hardfault_args[1]);
  stacked_r2 = ((unsigned long) hardfault_args[2]);
  stacked_r3 = ((unsigned long) hardfault_args[3]);
  stacked_r12 = ((unsigned long) hardfault_args[4]);
  stacked_lr = ((unsigned long) hardfault_args[5]);
  stacked_pc = ((unsigned long) hardfault_args[6]);
  stacked_psr = ((unsigned long) hardfault_args[7]);
  sprintf(pc_tmp,"LR=%08x\n",stacked_lr);
  HAL_UART_Transmit(&huart1, pc_tmp, strlen(pc_tmp), 10000);
  sprintf(pc_tmp,"PC=%08x\n",stacked_pc);
  HAL_UART_Transmit(&huart1, pc_tmp, strlen(pc_tmp), 10000);
  sprintf(pc_tmp,"R0=%08x\n",stacked_r0);
  HAL_UART_Transmit(&huart1, pc_tmp, strlen(pc_tmp), 10000);
  sprintf(pc_tmp,"R1=%08x\n",stacked_r1);
  HAL_UART_Transmit(&huart1, pc_tmp, strlen(pc_tmp), 10000);
  sprintf(pc_tmp,"R2=%08x\n",stacked_r2);
  HAL_UART_Transmit(&huart1, pc_tmp, strlen(pc_tmp), 10000);
  sprintf(pc_tmp,"R3=%08x\n",stacked_r3);
  HAL_UART_Transmit(&huart1, pc_tmp, strlen(pc_tmp), 10000);
  sprintf(pc_tmp,"R12=%08x\n",stacked_r12);
  HAL_UART_Transmit(&huart1, pc_tmp, strlen(pc_tmp), 10000);
  sprintf(pc_tmp,"PSR=%08x\n",stacked_psr);
  HAL_UART_Transmit(&huart1, pc_tmp, strlen(pc_tmp), 10000);
  sprintf(pc_tmp,"BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38))));
  HAL_UART_Transmit(&huart1, pc_tmp, strlen(pc_tmp), 10000);
  sprintf(pc_tmp,"CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28))));
  HAL_UART_Transmit(&huart1, pc_tmp, strlen(pc_tmp), 10000);

  sprintf("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C))));
  HAL_UART_Transmit(&huart1, pc_tmp, strlen(pc_tmp), 10000);

  sprintf(pc_tmp,"DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30))));
  HAL_UART_Transmit(&huart1, pc_tmp, strlen(pc_tmp), 10000);

  sprintf(pc_tmp,"AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C))));
  HAL_UART_Transmit(&huart1, pc_tmp, strlen(pc_tmp), 10000);
//UART_PRINTF(("[Hard fault handler]\n"));
//UART_PRINTF( ("R0 = %x\n", stacked_r0));
//UART_PRINTF( ("R1 = %x\n", stacked_r1));
//UART_PRINTF( ("R2 = %x\n", stacked_r2));
//UART_PRINTF( ("R3 = %x\n", stacked_r3));
//UART_PRINTF( ("R12 = %x\n", stacked_r12));
//UART_PRINTF( ("LR = %x\n", stacked_lr));
//UART_PRINTF( ("PC = %x\n", stacked_pc));
//UART_PRINTF( ("PSR = %x\n", stacked_psr));
//UART_PRINTF( ("BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38)))));
//UART_PRINTF( ("CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28)))));
//UART_PRINTF( ("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C)))));
//UART_PRINTF( ("DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30)))));
//UART_PRINTF( ("AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C)))));


 /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 __attribute__ ((naked, optimize("-fno-stack-protector")))
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
//__asm volatile (
 //"   TST LR, #4 \n"
 // "   ITE EQ    \n"
 // " MRSEQ R0, MSP \n"
 // " MRSNE R0, PSP  \n"
 //  " B HardFault_Handler_c \n"
    __asm volatile("movs R0, #4");
    __asm volatile("mov R1, LR");
    __asm volatile("tst R0, R1"); // Check EXC_RETURN in Link register bit 2.
    __asm volatile("bne Uses_PSP");
    __asm volatile("mrs R0, MSP");// Stacking was using MSP.
    __asm volatile("b Pass_StackPtr");
    __asm volatile("Uses_PSP:");
    __asm volatile("mrs R0, PSP");  // Stacking was using PSP
    __asm volatile("Pass_StackPtr:");
    __asm volatile("ldr R2,=HardFault_Handler_c");
    __asm volatile("bx  R2");
    __asm volatile("");  
 // );
  /* USER CODE END HardFault_IRQn 0 */
  //while (1)
  //{
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  //}
}

void idleTaskOS(void)
{
     while(1)
		 {
		//   HAL_UART_Transmit(&huart1, (uint8_t *)"Idle\n", sizeof("Idle\n"), 10000);
        //   for (int i=0; i< 100000; i++)
        //   {

        //   }
		 }
}

void task0(void)
{
   while(1)
   {
	   HAL_UART_Transmit(&huart1, (uint8_t *)"Hello0\n", sizeof("Hello0\n"), 10000);
       HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
		  delayTickOS(20);
    }
}
void task1(void)
{
   while(1)
   {
	   HAL_UART_Transmit(&huart1, (uint8_t *)"Hello1\n", sizeof("Hello1\n"), 10000);
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		   delayTickOS(10);
    }
}



//void (*taskName[])(void)={task0,task1};
fn_ptr_t taskName[] ={task0,task1};

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	  char errorCode;
	  int startTaskIndex;
	  int arraySize;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
  HAL_UART_Transmit(&huart1, (uint8_t *)"Hello STM32Cube IDE\r\n", sizeof("Hello STM32Cube IDE\r\n"), 10000);
  /*
  while(1)
  {  HAL_UART_Transmit(&huart1, (uint8_t *)"Hello M\n", sizeof("Hello M\n"), 10000);
     HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
     for (int i=0; i< 1000000; i++)
     {

     }
  }
  */
  arraySize = sizeof(taskName) / sizeof(taskName[0]);
  startTaskIndex = 0;
   errorCode = startOS(taskName, arraySize, startTaskIndex, clockOS);  // create idleTaskOS()
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
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
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LD4_Pin|LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

