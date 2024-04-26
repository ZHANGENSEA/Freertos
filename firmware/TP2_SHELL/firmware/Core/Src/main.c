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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "shell.h"
#include <stdlib.h>
#include "stm32f7xx_hal_tim.h"

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

TaskHandle_t handle_TaskShell;
TaskHandle_t handle_Task_led;
TaskHandle_t handle_Task_bidon;
TaskHandle_t handle_vATaskFunction;

static int leddelay = 1;

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

	return ch;
}

//TD 4.2.6
int fonction(int argc, char ** argv)
{
	printf("argc = %d\r\n", argc);

	for (int itr = 0; itr < argc; itr++)
	{
		printf("argv[%d] = %s\r\n", itr, argv[itr]);
	}

	//	printf("Je suis une fonction bidon\r\n");

	return 0;
}

//TP 3.1.5
void Task_bidon( void *pvParameters)
{
	//int tab[1000];
	for(;;) {
		//for (int i=0;i<10000;i++){
		//	tab[i] = i;
		//	printf("tab[%d]\r\n",tab[i]);
	}
	//}
}


//TD 4.2.7
int addition(int argc, char ** argv)
{

	if (argc != 3)
	{
		//Plante proporement si le (ni plus, ni moins)
		//printf("Error, expected 2 args\r\n");
		Error_Handler();
		return -1;
	}
	else
	{
		int a, b;
		a = atoi(argv[1]); //atoi = ASCII to Interger
		b = atoi(argv[2]); //atoi = ASCII to Interger

		int c = a + b;

		printf ("%d + %d = %d\r\n", a, b, c);
	}
	//récupérer deux paramètre (ni plus, ni moins)
	//convertir les deux paramètres en nombres entiers
	//les additiionner et afficher le résultat


}

//TP 3.4

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if (htim->Instance == TIM2) {
		HAL_IncTick();
		FreeRTOSRunTimeTicks++;
	}

}

void statistique (int argc, char ** argv)
{
	static char buf[256];
	vTaskList(buf);
	printf ("%s \r\n",buf);
}

uint8_t ADXL345_ReadRegister(uint8_t regAddress)
{

	uint8_t receivedVal = 0;

	uint8_t address = regAddress | 0x80; // Le bit MSB doit être mis à 1 pour indiquer une lecture.


	// Sélectionner l'ADXL345 en passant le pin NSS à bas.
	HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_RESET);


	// Envoyer l'adresse du registre à lire.
	HAL_SPI_Transmit(&hspi2, &address, 1, HAL_MAX_DELAY);

	// Lire la valeur du registre.
	HAL_SPI_Receive(&hspi2, &receivedVal, 1, HAL_MAX_DELAY);

	// Désélectionner l'ADXL345 en passant le pin NSS à haut.
	HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET);

	return receivedVal;
}

uint8_t ADXL345_WriteRegister(uint8_t regAddress, uint8_t value)
{
	uint8_t data[2];
	data[0] = regAddress;
	data[1] = value;

	// Sélectionner l'ADXL345 en passant le pin NSS à bas.
	HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_RESET);

	// Envoyer l'adresse du registre à lire.
	HAL_SPI_Transmit(&hspi2, &data[0], 2, HAL_MAX_DELAY);

	// LEnvoyer la valuer du registre à lire.
	HAL_SPI_Receive(&hspi2, &data[1], 2, HAL_MAX_DELAY);

	// Désélectionner l'ADXL345 en passant le pin NSS à haut.
	HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET);

}

void test (int argc, char ** argv)
{

	uint8_t add[2] = {0x80, 0};
	uint8_t data[2] = {0, 0};

	HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi2, add, data, 2, 100);
	HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET);


	printf("DEVID(0x00): 0x%2x\r\n", data[1]); // Afficher la valeur lue, devrait être 0xE5 pour ADXL345

}

void lire4(int argc, char ** argv)
{
	ADXL345_WriteRegister(0x31, 0x01);
	ADXL345_WriteRegister(0x2d, 0x00);
	ADXL345_WriteRegister(0x2d, 0x08);

	uint8_t data[6]; // Tableau pour stocker les données des axes x, y, z (2 octets par axe)
	uint8_t regAddress = 0x32; // Adresse du premier registre de données de l'axe X
	regAddress |= 0x80; // Le bit de lecture doit être mis à 1

	uint16_t Sumx[4];
	uint16_t Sumy[4];
	uint16_t Sumz[4];

	for(int i = 0; i < 4; i++){

		HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi2, &regAddress, 1, HAL_MAX_DELAY);
		HAL_SPI_Receive(&hspi2, data, 6, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET);

		uint16_t x = (int16_t)(data[1] << 8) | data[0];
		uint16_t y = (int16_t)(data[3] << 8) | data[2];
		uint16_t z = (int16_t)(data[5] << 8) | data[4];

		printf("X: %d, Y: %d, Z: %d\r\n", x, y, z);

		Sumx[i] = x;
		Sumy[i] = y;
		Sumz[i] = z;
	}

	uint16_t Moyx = (Sumx[0]+Sumx[1]+Sumx[2]+Sumx[3])/4;
	uint16_t Moyy = (Sumy[0]+Sumy[1]+Sumy[2]+Sumy[3])/4;
	uint16_t Moyz = (Sumz[0]+Sumz[1]+Sumz[2]+Sumz[3])/4;

	printf("La moyenne de x : %d\r\n",Moyx);
	printf("La moyenne de y : %d\r\n",Moyy);
	printf("La moyenne de z : %d\r\n",Moyz);

}



//TP 2.3
void Task_led ( void *pvParameters)
{

	for(;;){
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		vTaskDelay(leddelay);
	}
}

int led(int argc, char ** argv)
{
	if (argc != 2)
	{
		//Plante proporement si le (ni plus, ni moins)
		//printf("Error, expected 1 args\r\n");
		Error_Handler();
		return -1;
	}
	else
	{

		leddelay = atoi(argv[1]);

	}
	return 0;

}

//TP 2.4

int spam(int argc, char ** argv)
{
	if (argc != 2)
	{
		//Plante proporement si le (ni plus, ni moins)
		//printf("Error, expected 1 args\r\n");
		Error_Handler();
		return -1;
	}
	else
	{
		for (;;)
		{
			printf("%s\r\n", argv[1]);
		}
	}
	return 0;

}


void TaskShell(void *p)
{
	for(;;)
	{
		shell_init();
		shell_add('f', fonction, "Une fonction inutile");
		shell_add('a', addition, "Addition");
		shell_add('l', led, "Clignoter le LED");
		shell_add('s', spam, "Afficher le message");
		shell_add('i', statistique, "Afficher les statistiques");
		shell_add('t', test, "Tester l'ADXL345");
		shell_add('l',lire4, "Lire 4 valeurs consecutives dans l'ADXL345");
		shell_run();
	}
}




//TP 3.2.3

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	//Affichez le nom de la tâche qui a causé le débordement de la pile.
	printf("Stack overflow in task: %s\n", pcTaskName);

	//Empêchez que des interruptions ne viennent perturber le rapport d'erreur.
	portDISABLE_INTERRUPTS();

	//Bouclez indéfiniment pour que le comportement soit plus facile à détecter avec un débogueur.
	for( ;; );
}

//TP 3.2.4

void vATaskFunction(void *pvParameters)
{

	for(;;)
	{
		printf("Une tache est ici\r\n");
	}

}

//TP 3.3.6

void configureTimerForRunTimeStats(void){
	HAL_TIM_Base_Start(&htim2);
}

unsigned long getRunTimeCounterValue(void)
{
	// Récupérer la valeur courante du timer
	return (unsigned long)__HAL_TIM_GET_COUNTER(&htim2);
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
	MX_TIM2_Init();
	MX_SPI2_Init();
	/* USER CODE BEGIN 2 */

	/*	BaseType_t ret;
	for (int z = 0; z < 200; z++) {
	ret = xTaskCreate(vATaskFunction, "ATask", 256, NULL, 6, &handle_vATaskFunction);
	if (ret == pdPASS){
		printf("la tache %d se cree\r\n",z);
	}
	else
	{
		Error_Handler();
	}
	}*/

	xTaskCreate(TaskShell, "TaskShell", 256, NULL, 5, &handle_TaskShell);
	//	xTaskCreate(Task_led, "Task_led", 256, NULL, 4, &handle_Task_led);

	//	for (int z = 0; z < 200; z++) {
	//		ret = xTaskCreate(Task_bidon, "Task_bidon", 256, NULL, 9, &handle_Task_bidon);
	//		if (ret == pdPASS){
	//			printf("la tache %d se cree\r\n", z);
	//		}
	//		else
	//		{
	//			printf("Error, la tache n'est pas cree\r\n");
	//			Error_Handler();
	//		}
	//	}

	vTaskStartScheduler();

	// Comme pour le scheduler, il ne se passe plus rien après cette ligne
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
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 432;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Activate the Over-Drive mode
	 */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
	{
		Error_Handler();
	}
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
	printf("Il y a une erreur.\r\n");
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
