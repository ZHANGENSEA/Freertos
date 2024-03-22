/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "task.h"
#include "semphr.h"
#include "stm32F7xx.h"
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

/* USER CODE BEGIN PV */
#define MS_T0_TICKS(ms) ((TickType_t)(ms * configTICK_RATE_HZ / 1000))
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

TaskHandle_t handle_blink_led;
TaskHandle_t handle_uart;
TaskHandle_t pdTASK_GIVE_HANDLE = NULL;
TaskHandle_t pdTASK_TAKE_HANDLE = NULL;


int __io_putchar(int chr)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&chr, 1, HAL_MAX_DELAY);
	return chr;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) // On s'assure les interruptions
	{
		BaseType_t xHigherPriorityTaskWoken; //Pour l'instant on admet
		// Sans poser de questions SVP

		//uart_rx_flag = 1;
		vTaskNotifyGiveFromISR(handle_uart, &xHigherPriorityTaskWoken);

		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

void task_blink_led(void * unused)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		printf ("\r\n LED \r\n");
		vTaskDelay(100/portTICK_PERIOD_MS);
	}
}

void task_uart(void * unused)
{
	for(;;)
	{
		char chr;
		HAL_UART_Receive_IT(&huart1, (uint8_t *)&chr, 1); //IT pour d'accord avec interruptions
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		HAL_UART_Transmit(&huart1, (uint8_t *)&chr, 1, HAL_MAX_DELAY);
	}
}

//Sémaphores de taskGive et taskTake
SemaphoreHandle_t xSemaphore = NULL;

void taskGive(void *pvParameters){
	TickType_t xDelay = 100/ portTICK_PERIOD_MS;

	for(;;){
//		printf("\r\n taskGive : Prêt à donner le sémaphore \r\n");
//		xSemaphoreGive(xSemaphore);

		printf("\r\n taskGive : Prêt à enovyer la notification \r\n");
		xTaskNotifyGive(pdTASK_TAKE_HANDLE);//Envoie une notification à taskTake

		printf("\r\n taskGive : Notification envoyée \r\n");
//		printf("\r\n taskGive : Sémaphore deonné \r\n");

		vTaskDelay(xDelay);

		xDelay += 100/portTICK_PERIOD_MS; //Augmente le delai de 100ms
	}
}

void taskTake(void *pvParameters){
	for(;;){
//		printf("\r\n taskTake : Prêt à prendre le sémaphore \r\n");
		printf("\r\n taskTake : Prêt à recevoir la notification \r\n");

		//Attend la notification
		uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, MS_T0_TICKS(1000));

		if (ulNotificationValue >0){
			printf("\r\n taskTake: Notification reçue \r\n");
		}
//		if (xSemaphoreTake(xSemaphore, MS_T0_TICKS(1000)) == pdTRUE){
//			printf("\r\n taskTake: Sémaphore pris \r\n");
//		}
		else{
			printf("\r\n taskTake: Timeout de la notification, déclenchement d'un reset\r\n");
//			printf("\r\n taskTake: échec de la prise du sémaphore, déclenchement d'un reset \r\n");
			NVIC_SystemReset();//Reset logiciel du microcontrôleur
		}
	}
}

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
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */

/*	xSemaphore = xSemaphoreCreateBinary();

	if (xSemaphore != NULL){
		xTaskCreate(taskGive, "TaskGive", configMINIMAL_STACK_SIZE, NULL,4,NULL);
		xTaskCreate(taskTake, "TaskTake", configMINIMAL_STACK_SIZE, NULL,3,NULL);
		vTaskStartScheduler();
	}*/

	xTaskCreate(taskGive, "TaskGive", configMINIMAL_STACK_SIZE, NULL,4, &pdTASK_GIVE_HANDLE);
	xTaskCreate(taskTake, "TaskTake", configMINIMAL_STACK_SIZE, NULL,3, &pdTASK_TAKE_HANDLE);




/*	BaseType_t ret;

	ret = xTaskCreate(
			task_blink_led, //premiere interruption dans le flash
			"Blink LED", //un nom de la fonction
			256,//taille de la pile (en mot de 32 bits). si on met 1000000, il y a un "Error creating task blink led"
			NULL,//Paramètre  (pas utilisé ici)
			1,//(priorité)
			&handle_blink_led
	);

	if (ret != pdPASS)
	{
		printf("Error creating task blink led\r\n");
		Error_Handler();

	}

	ret = xTaskCreate (
			task_uart,
			"UART",
			256,
			NULL,
			2,
			&handle_uart
	);

	if (ret != pdPASS)
	{
		printf("Error creating task uart\r\n");
		Error_Handler();

	}

	printf("Task creation successful\r\n");

	vTaskStartScheduler();*/// Boucle infinie, on exécute rien après !!!

	/* USER CODE END 2 */

	/* Call init function for freertos objects (in freertos.c) */
	MX_FREERTOS_Init();

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/*Faire clignoter la LED*/

		//		HAL_GPIO_TogglePin(GPIOI, LED_Pin, GPIO_PIN_SET);

		//		HAL_Delay(100);

		//		HAL_GPIO_WritePin(GPIOI, LED_Pin, GPIO_PIN_RESET);

		//		HAL_Delay(100);


		char chr;
		HAL_UART_Receive(&huart1, (uint8_t *)&chr, 1, HAL_MAX_DELAY);

		//		HAL_GPIO_TogglePin(GPIOI, LED_Pin);//quand il y a un chr, le led s'allume

		HAL_UART_Transmit(&huart1, (uint8_t *)&chr, 1, HAL_MAX_DELAY);

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
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 10;
	RCC_OscInitStruct.PLL.PLLN = 210;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
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
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM6) {
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
