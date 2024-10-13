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

/* Includes ------------------------------------------------------------------*/
#include <unistd.h>
#include <errno.h>
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
#include "stm32f103xb.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "defines.h"
#include "lib_log.h"
#include "hard/gpio.h"
#include "lib_callbacks.h"
#include "debug.h"

#define LIFE_TEMPO 5000

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/*-------------------------------------------------*/
/* Variables mutex pour la gestion de la main_loop */
/*-------------------------------------------------*/
volatile bool life_flag = false;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Private user code ---------------------------------------------------------*/

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    GPIO_Init();
    USART_Init();
    MX_TIM2_Init();

    // Welcoming and application starting debug messages
    /*-------------------------------------------------------------------------------------------------*/
    LOG_INFO("**************************************************");
    LOG_INFO("                     Relay");
    LOG_INFO("**************************************************");
    LOG_INFO("Compiled: %s %s", __DATE__, __TIME__);
    LOG_INFO("Target: STM32F103");
    LOG_INFO("Version: %s", VERSION_NAME);
    LOG_INFO("**************************************************");
    /*-------------------------------------------------------------------------------------------------*/

    callback_init();
    debug_init();

    // Initialisation des flags
    callback_timer_register_flag(&life_flag, LIFE_TEMPO, true);

    /*-------------------------------------------------------------------------------------------------*/
    /* Timer2 initialisation                                                                           */
    /*-------------------------------------------------------------------------------------------------*/
    HAL_TIM_Base_Start_IT(&htim2);

    /* Infinite loop */
    while (1) {

        /*-------------------------------------------------------------------------------------------------*/
        /* Gestion trame de vie                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        if (life_flag) {
            life_flag = false;
            LOG_DEBUG("[RUN] Alive");

            GPIO_toggle(GPIO_RELAY_1);
            GPIO_toggle(GPIO_RELAY_2);
        }

        debug_run();
    }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    LOG_EMERG("Error_Handler");

    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {}
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
    /* User can add his own implementation to report the file name and line number */
    LOG_CRIT("Wrong parameters value: file %s on line %d", file, line);
}
#endif /* USE_FULL_ASSERT */