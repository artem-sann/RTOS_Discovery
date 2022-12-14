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
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
 I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

osThreadId Move_Servo_TaskHandle;
osThreadId Bluetooth_ReadHandle;
osThreadId USB_ReadHandle;
osThreadId USB_SendHandle;
osThreadId Bluetooth_SendHandle;
osThreadId LED_ControlHandle;
osThreadId Button_readHandle;
osMessageQId Servo_AngleHandle;
osMessageQId From_COM_PortHandle;
osMessageQId To_COM_PortHandle;
osMessageQId From_BluetoothHandle;
osMessageQId To_BluetoothHandle;
osSemaphoreId Red_LEDHandle;
osStaticSemaphoreDef_t Red_LEDControl;
osSemaphoreId Blue_LEDHandle;
osSemaphoreId Orange_LEDHandle;
osSemaphoreId Green_LEDHandle;
/* USER CODE BEGIN PV */
char str_rx[21];
char str_tx[21];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);
void Move_Servo(void const * argument);
void Bluetooth_Read_Task(void const * argument);
void USB_Read_Task(void const * argument);
void USB_Send_Task(void const * argument);
void Bluetooth_Send_Task(void const * argument);
void LED_Control_Task(void const * argument);
void Button_read_Task(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int Set_Servo_Angle(uint8_t Angle) // from 0 to 180 degrees
{
    uint16_t Pulse_length = 500;

    if (Angle > 0  &&  Angle <= 180) {
        Pulse_length += (2700-500)/180 * Angle;
    }

    TIM3->CCR1=Pulse_length;
    return 0;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    //str_rx[0] = '0';

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
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of Red_LED */
  osSemaphoreStaticDef(Red_LED, &Red_LEDControl);
  Red_LEDHandle = osSemaphoreCreate(osSemaphore(Red_LED), 15);

  /* definition and creation of Blue_LED */
  osSemaphoreDef(Blue_LED);
  Blue_LEDHandle = osSemaphoreCreate(osSemaphore(Blue_LED), 15);

  /* definition and creation of Orange_LED */
  osSemaphoreDef(Orange_LED);
  Orange_LEDHandle = osSemaphoreCreate(osSemaphore(Orange_LED), 15);

  /* definition and creation of Green_LED */
  osSemaphoreDef(Green_LED);
  Green_LEDHandle = osSemaphoreCreate(osSemaphore(Green_LED), 15);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of Servo_Angle */
  osMessageQDef(Servo_Angle, 50, uint8_t);
  Servo_AngleHandle = osMessageCreate(osMessageQ(Servo_Angle), NULL);

  /* definition and creation of From_COM_Port */
  osMessageQDef(From_COM_Port, 100, uint8_t);
  From_COM_PortHandle = osMessageCreate(osMessageQ(From_COM_Port), NULL);

  /* definition and creation of To_COM_Port */
  osMessageQDef(To_COM_Port, 100, uint8_t);
  To_COM_PortHandle = osMessageCreate(osMessageQ(To_COM_Port), NULL);

  /* definition and creation of From_Bluetooth */
  osMessageQDef(From_Bluetooth, 100, uint8_t);
  From_BluetoothHandle = osMessageCreate(osMessageQ(From_Bluetooth), NULL);

  /* definition and creation of To_Bluetooth */
  osMessageQDef(To_Bluetooth, 100, uint8_t);
  To_BluetoothHandle = osMessageCreate(osMessageQ(To_Bluetooth), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Move_Servo_Task */
  osThreadDef(Move_Servo_Task, Move_Servo, osPriorityLow, 0, 128);
  Move_Servo_TaskHandle = osThreadCreate(osThread(Move_Servo_Task), NULL);

  /* definition and creation of Bluetooth_Read */
  osThreadDef(Bluetooth_Read, Bluetooth_Read_Task, osPriorityLow, 0, 128);
  Bluetooth_ReadHandle = osThreadCreate(osThread(Bluetooth_Read), NULL);

  /* definition and creation of USB_Read */
  osThreadDef(USB_Read, USB_Read_Task, osPriorityNormal, 0, 128);
  USB_ReadHandle = osThreadCreate(osThread(USB_Read), NULL);

  /* definition and creation of USB_Send */
  osThreadDef(USB_Send, USB_Send_Task, osPriorityLow, 0, 128);
  USB_SendHandle = osThreadCreate(osThread(USB_Send), NULL);

  /* definition and creation of Bluetooth_Send */
  osThreadDef(Bluetooth_Send, Bluetooth_Send_Task, osPriorityLow, 0, 128);
  Bluetooth_SendHandle = osThreadCreate(osThread(Bluetooth_Send), NULL);

  /* definition and creation of LED_Control */
  osThreadDef(LED_Control, LED_Control_Task, osPriorityLow, 0, 128);
  LED_ControlHandle = osThreadCreate(osThread(LED_Control), NULL);

  /* definition and creation of Button_read */
  osThreadDef(Button_read, Button_read_Task, osPriorityLow, 0, 128);
  Button_readHandle = osThreadCreate(osThread(Button_read), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 95;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 20000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : OTG_FS_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OTG_FS_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PDM_OUT_Pin */
  GPIO_InitStruct.Pin = PDM_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(PDM_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : I2S3_WS_Pin */
  GPIO_InitStruct.Pin = I2S3_WS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(I2S3_WS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CLK_IN_Pin PB12 */
  GPIO_InitStruct.Pin = CLK_IN_Pin|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin
                           Audio_RST_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : I2S3_MCK_Pin I2S3_SCK_Pin I2S3_SD_Pin */
  GPIO_InitStruct.Pin = I2S3_MCK_Pin|I2S3_SCK_Pin|I2S3_SD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MEMS_INT2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_Move_Servo */
/**
  * @brief  Function implementing the Move_Servo_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Move_Servo */
void Move_Servo(void const * argument)
{
  /* init code for USB_DEVICE */

  /* USER CODE BEGIN 5 */


  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  /* Infinite loop */
  for(;;)
  {
      Set_Servo_Angle(90);
      osDelay(10);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Bluetooth_Read_Task */
/**
* @brief Function implementing the Bluetooth_Read thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Bluetooth_Read_Task */
void Bluetooth_Read_Task(void const * argument)
{
  /* USER CODE BEGIN Bluetooth_Read_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Bluetooth_Read_Task */
}

/* USER CODE BEGIN Header_USB_Read_Task */
/**
* @brief Function implementing the USB_Read thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_USB_Read_Task */

void USB_Read_Task(void const * argument)
{
  /* USER CODE BEGIN USB_Read_Task */
  MX_USB_DEVICE_Init();
  /* Infinite loop */
  for(;;)
  {

          for (int j = 0; j < 21; j++) {
              str_tx[j] = str_rx[j];
          }
          osDelay(1);


  }
  /* USER CODE END USB_Read_Task */
}

/* USER CODE BEGIN Header_USB_Send_Task */
/**
* @brief Function implementing the USB_Send thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_USB_Send_Task */
void USB_Send_Task(void const * argument)
{
  /* USER CODE BEGIN USB_Send_Task */
  sprintf(str_tx,"USB tttttttttttttt\r\n");
  /* Infinite loop */
  for(;;)
  {
      CDC_Transmit_FS((unsigned char*)str_tx, strlen(str_tx));
      osDelay(500);
  }
  /* USER CODE END USB_Send_Task */
}

/* USER CODE BEGIN Header_Bluetooth_Send_Task */
/**
* @brief Function implementing the Bluetooth_Send thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Bluetooth_Send_Task */
void Bluetooth_Send_Task(void const * argument)
{
  /* USER CODE BEGIN Bluetooth_Send_Task */
  /* Infinite loop */
  for(;;)
  {

    osDelay(10);
  }
  /* USER CODE END Bluetooth_Send_Task */
}

/* USER CODE BEGIN Header_LED_Control_Task */
/**
* @brief Function implementing the LED_Control thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LED_Control_Task */
void LED_Control_Task(void const * argument)
{
  /* USER CODE BEGIN LED_Control_Task */
  /* Infinite loop */
  for(;;)
  {
        /*
      if (xSemaphoreTake(Red_LEDHandle, portTICK_PERIOD_MS*5)) {
          HAL_GPIO_TogglePin(GPIOD, LD5_Pin); //Red LED
          osDelay(200);
      }
         */


      HAL_GPIO_WritePin(GPIOD, LD3_Pin, SET); //Orange LED
      HAL_GPIO_WritePin(GPIOD, LD4_Pin, SET); //Green LED
      //HAL_GPIO_WritePin(GPIOD, LD5_Pin, SET); //Red LED
      HAL_GPIO_WritePin(GPIOD, LD6_Pin, SET); //Blue LED
      osDelay(100);
  }
  /* USER CODE END LED_Control_Task */
}

/* USER CODE BEGIN Header_Button_read_Task */
/**
* @brief Function implementing the Button_read thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Button_read_Task */
void Button_read_Task(void const * argument)
{
  /* USER CODE BEGIN Button_read_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Button_read_Task */
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

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
