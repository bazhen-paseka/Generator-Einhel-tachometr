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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include "tm1637_sm.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

	#define 	MY_DEBUG
	#define 	UART_DEBUG 			&huart1
	#define 	SOFT_VERSION 		801

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

	char 		DataChar[0xFF]		= { 0 } ;
	uint8_t 	display_update_tim	= 0 ;
	uint8_t 	display_update_ext	= 0 ;
	uint32_t	tacho_value_u32		= 0 ;
	uint32_t	counter				= 0 ;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void UartDebug(char* _text);
void PrintSoftVersion(uint32_t _soft_version_u32);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  sprintf(DataChar,"\r\n\r\n\tGenerator Einhel 800/1 Tachometr"); UartDebug(DataChar) ;
  PrintSoftVersion(SOFT_VERSION);

  tm1637_struct htm1637;
  htm1637.clk_port = TM1637_CLK_GPIO_Port;
  htm1637.clk_pin  = TM1637_CLK_Pin;
  htm1637.dio_port = TM1637_DIO_GPIO_Port;
  htm1637.dio_pin  = TM1637_DIO_Pin;
  tm1637_Init( &htm1637 );
  tm1637_Set_Brightness( &htm1637, bright_full);
  tm1637_Display_Decimal( &htm1637, 1234, no_double_dot);
  HAL_Delay(1000);

  TIM3->CNT = 0 ;
  HAL_TIM_Base_Start_IT( &htim3) ;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (display_update_ext == 1) {
		  uint32_t tacho_res_u32 = (60*1000*1000)/(2*tacho_value_u32) ;
		  sprintf(DataChar," value = %lu\r\n", tacho_res_u32); UartDebug(DataChar) ;
		  tm1637_Display_Decimal( &htm1637, tacho_res_u32, no_double_dot);
		  HAL_Delay(500);
		  display_update_ext = 0;
	  }
	  if (display_update_tim == 1) {
		  tm1637_Display_Decimal( &htm1637, counter++, no_double_dot);
		  sprintf(DataChar,"%lu ", counter); UartDebug(DataChar) ;
		  HAL_Delay(200);
		  display_update_tim = 0;
	  }


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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void UartDebug(char* _text) {
#ifdef MY_DEBUG
	HAL_UART_Transmit(UART_DEBUG, (uint8_t*)_text, strlen(_text), 100);
#endif
} //**************************************************************************

void PrintSoftVersion(uint32_t _soft_version_u32) {
	int soft_version_arr_int[3];
	soft_version_arr_int[0] = (_soft_version_u32 / 100)     ;
	soft_version_arr_int[1] = (_soft_version_u32 /  10) %10 ;
	soft_version_arr_int[2] = (_soft_version_u32      ) %10 ;

	sprintf(DataChar," v%d.%d.%d \r\n",
				soft_version_arr_int[0] ,
				soft_version_arr_int[1] ,
				soft_version_arr_int[2] ) ;
	UartDebug(DataChar) ;

	#define 	DATE_as_int_str 	(__DATE__)
	#define 	TIME_as_int_str 	(__TIME__)
	sprintf(DataChar,"\tBuild: %s. Time: %s.\r\n" ,
			DATE_as_int_str ,
			TIME_as_int_str ) ;
	UartDebug(DataChar) ;
}//**************************************************************************

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if ( GPIO_Pin == BUTTON_Pin ) {
		if ( HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) == GPIO_PIN_RESET) {
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
			tacho_value_u32 = TIM3->CNT;
			TIM3->CNT = 0 ;
			display_update_ext = 1 ;
		}
	}

} //**************************************************************************

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if ( htim == &htim3 ) {
		tacho_value_u32 = 65535;
		display_update_tim = 1 ;
	}
} //**************************************************************************

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
