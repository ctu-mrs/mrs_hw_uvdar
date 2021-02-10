/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "uv_led_driver.h"
#include "stm32f0xx_hal_flash_ex.h"
#include "baca_protocol.h"
// Include string.h for memset
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FW_VERSION_H	1
#define FW_VERSION_L	0

#define BUFFER_SIZE 255
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
// extern variables from uv_led_driver.h
extern uint32_t uv_led_counter[8];
extern uint32_t uv_led_periods_ms[8];
extern uint8_t enable_global;
// USART1 Buffer
uint8_t uart_rx_buff[BUFFER_SIZE];
uint8_t uart_rx_buff_size = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void process_uart_message(void);
void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
void USAR_UART_IDLECallback(UART_HandleTypeDef *huart);
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
  MX_DMA_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  // Initialize UV LEDs - sets pins and loads values from FLASH
  uv_led_init();

  // Start UART RX using DMA
  HAL_UART_Receive_DMA(&huart1, (uint8_t*)uart_rx_buff, BUFFER_SIZE);

  // Start TIM3
  HAL_TIM_Base_Start_IT(&htim3);

  // Turn on the LED to signal the initialization is done
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
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
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 48-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 100-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

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

  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);  // Enable serial port idle interrupt

  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, UV_LED_0_Pin|UV_LED_1_Pin|UV_LED_2_Pin|UV_LED_3_Pin
                          |UV_LED_4_Pin|UV_LED_5_Pin|UV_LED_6_Pin|UV_LED_7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : UV_LED_0_Pin UV_LED_1_Pin UV_LED_2_Pin UV_LED_3_Pin
                           UV_LED_4_Pin UV_LED_5_Pin UV_LED_6_Pin UV_LED_7_Pin */
  GPIO_InitStruct.Pin = UV_LED_0_Pin|UV_LED_1_Pin|UV_LED_2_Pin|UV_LED_3_Pin
                          |UV_LED_4_Pin|UV_LED_5_Pin|UV_LED_6_Pin|UV_LED_7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SWITCH_Pin */
  GPIO_InitStruct.Pin = SWITCH_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SWITCH_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

}

/* USER CODE BEGIN 4 */
/* UART1 Interrupt Service Routine */
void USART1_IRQHandler(void)
{
    /* USER CODE BEGIN USART1_IRQn 0 */

    HAL_UART_IRQHandler(&huart1);

    // Function to handle the IDLE INTERRUPT
    USER_UART_IRQHandler(&huart1);
    /* USER CODE END USART1_IRQn 1 */
}

void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
	// Check if the interrupt came from USART1
    if(USART1 == huart1.Instance)
    {
    	// Check if the interrupt is IDLE INTERRUPT
        if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)
        {
        	// Clear the IDLE INTERRUPT flag
            __HAL_UART_CLEAR_IDLEFLAG(&huart1);
            // Call the interrupt Handler
            USAR_UART_IDLECallback(huart);
        }
    }
}

void USAR_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	//Stop UART DMA
    HAL_UART_DMAStop(&huart1);
	//Calculate the length of the received data
	uart_rx_buff_size  = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	// Process the message
	process_uart_message();
	// Clear Receiving Buffer
	memset(uart_rx_buff, 0, uart_rx_buff_size);
    //Restart to start DMA USART RX
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)uart_rx_buff, BUFFER_SIZE);
}

void HAL_DMA1_Channel2_3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	// If the Interrupt came from SWITCH Pin
	if (GPIO_Pin == SWITCH_Pin)
	{
		// Toggle the UVLEDs global enable flag
		enable_global = !enable_global;
	}
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 /* USER CODE BEGIN Callback 0 */
	// Check if the callback came from TIM3
	if (htim->Instance == TIM3)
	{
		// Check UVLED toggle counters
		for (int i = 0; i < 8; i++)
		{
			// If toggle counter reached the max value toggle the UVLED and reset counter
			if (uv_led_counter[i] >= uv_led_periods_ms[i])
			{
				uv_led_toggle(i);
				uv_led_counter[i] = 0;
			// increment counter
			} else{
				uv_led_counter[i]++;
			}
		}
	}
 /* USER CODE END Callback 0 */
 if (htim->Instance == TIM3) {
   HAL_IncTick();
 }
 /* USER CODE BEGIN Callback 1 */

 /* USER CODE END Callback 1 */
}

void process_uart_message(void)
{
	// Decode the received message
	struct baca_payload received_message = baca_protocol_decode(uart_rx_buff, uart_rx_buff_size);
	// if the received message is correct (is a baca protocol message)
	if (received_message.data_correct == 1)
	{
		// Get the message ID
		uint8_t baca_id = received_message.payload[0];
		switch (baca_id)
		{
			// Message to toggle the global enable flag
			case 0x90:
				enable_global = !enable_global;
				break;

			// Message to enable UVLEDs
			case 0x91:
				// Check if the payload has corect size (message id + 8 data bytes)
				if (received_message.payload_size == 9)
				{
					for (uint8_t i = 0; i < 8; i++)
					{
						uv_led_enable(i, (uint8_t) received_message.payload[i+1]);
					}
				}
				break;
			// Message to set frequencies of UVLEDs
			case 0x92:
				// Check if the payload has corect size (message id + 8 data bytes)
				if (received_message.payload_size == 9)
				{
					for (uint8_t i = 0; i < 8; i++)
					{
						if (received_message.payload[i] != 0)
						{
							uv_led_set_frequency(i, (uint32_t) received_message.payload[i+1]);
						}
					}
				}
				break;
			// Message to enable an individual UVLED
			case 0x93:
				// Check if the payload has corect size (message id + 2 data bytes)
				if (received_message.payload_size == 3)
				{
					uv_led_enable((uint8_t)received_message.payload[1],(uint8_t) received_message.payload[2]);
				}
				break;
			// Message to set frequency of an individual UVLED
			case 0x94:
				// Check if the payload has corect size (message id + 2 data bytes)
				if (received_message.payload_size == 3)
				{
					uv_led_set_frequency((uint8_t)received_message.payload[1],(uint8_t) received_message.payload[2]);
				}
				break;
			// Message to return the enable flags of UVLEDs
			case 0x95:
				{
					uint8_t baca_buffer[12];
					uint8_t baca_buffer_length = 12;
					// Encode the data using baca protocol
					baca_protocol_encode(baca_buffer, uv_led_enabled, 8, 0x95);
					// Transmit the data
					HAL_UART_Transmit(&huart1, baca_buffer, baca_buffer_length, 100);
				}
				break;
			// Message to return frequencies of UVLEDs
			case 0x96:
				{
					uint8_t baca_buffer[12];
					uint8_t baca_buffer_length = 12;
					uint8_t frequencies_to_send[8];
					// Calculate the frequency from set ticks
					for (uint8_t j = 0; j < 8; j++)
					{
						frequencies_to_send[j] = (uint8_t)(TIMER_FREQ/(2*uv_led_periods_ms[j]));
					}
					// Encode the data using baca protocol
					baca_protocol_encode(baca_buffer, frequencies_to_send , 8, 0x96);
					// Transmit the data
					HAL_UART_Transmit(&huart1, baca_buffer, baca_buffer_length, 100);
				}
				break;
			// Something else: Do nothing
			default:
				break;
		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	// Just blink the LED
	while (1)
	{
		  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		  HAL_Delay(250);
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
