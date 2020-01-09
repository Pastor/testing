/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdlib.h>
#include <stm32f1xx_hal_i2c.h>
#include <stdio.h>
#include <stdarg.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define EEPROM_DEVICE_ADDRESS      0x36
#define EEPROM_DATA_START_ADDRESS  0x00
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void MX_I2C1_Init(void);

static void MX_SPI1_Init(void);

static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
void HAL_I2C_Print_Error(char *mode, HAL_StatusTypeDef status, uint16_t reading_address);

/* USER CODE BEGIN 0 */
enum State {
    READING_READY,
    READING_MEMORY,
    READING_COMPLETE
};

static char print_buffer[256];

static void print(char *fmt, ...) {

    int ret;
    va_list args;

    va_start(args, fmt);
    ret = vsnprintf(print_buffer, sizeof(print_buffer), fmt, args);
    HAL_UART_Transmit(&huart1, (uint8_t *) print_buffer, ret, 10000);
    va_end(args);
}

static void println(char *fmt, ...) {
    int ret;
    va_list args;

    va_start(args, fmt);
    ret = vsnprintf(print_buffer, sizeof(print_buffer), fmt, args);
    HAL_UART_Transmit(&huart1, (uint8_t *) print_buffer, ret, 10000);
    HAL_UART_Transmit(&huart1, (uint8_t *) "\r\n", 2, 10000);
    va_end(args);
}

HAL_StatusTypeDef Read_From_24LCxx(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                   uint16_t MemAddress, uint8_t *pData, uint16_t len) {
    HAL_StatusTypeDef returnValue;
    uint8_t addr[2];

    /* We compute the MSB and LSB parts of the memory address */
    addr[0] = (uint8_t) ((MemAddress & 0xFF00) >> 8);
    addr[1] = (uint8_t) (MemAddress & 0xFF);

    /* First we send the memory location address where start reading data */
    returnValue = HAL_I2C_Master_Transmit(hi2c, DevAddress, addr, 2, HAL_MAX_DELAY);
    if (returnValue != HAL_OK)
        return returnValue;

    /* Next we can retrieve the data from EEPROM */
    returnValue = HAL_I2C_Master_Receive(hi2c, DevAddress, pData, len, HAL_MAX_DELAY);

    return returnValue;
}

HAL_StatusTypeDef Write_To_24LCxx(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                  uint16_t MemAddress, uint8_t *pData, uint16_t len) {
    HAL_StatusTypeDef returnValue;
    uint8_t *data;

    data = (uint8_t *) malloc(sizeof(uint8_t) * (len + 2));

    /* We compute the MSB and LSB parts of the memory address */
    data[0] = (uint8_t) ((MemAddress & 0xFF00) >> 8);
    data[1] = (uint8_t) (MemAddress & 0xFF);


    memcpy(data + 2, pData, len);


    returnValue = HAL_I2C_Master_Transmit(hi2c, DevAddress, data, len + 2,
                                          HAL_MAX_DELAY);
    if (returnValue != HAL_OK)
        return returnValue;

    free(data);


    while (HAL_I2C_Master_Transmit(hi2c, DevAddress, 0, 0, HAL_MAX_DELAY) != HAL_OK);

    return HAL_OK;
}

#define DEVICE_ADDRESS(da) ((da) << 1)
//#define DEVICE_ADDRESS(da) ((da))
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */
    enum State state = READING_READY;
    HAL_StatusTypeDef status;
    uint16_t reading_address = 0;
    uint16_t device_address = EEPROM_DEVICE_ADDRESS;
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
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */
#if 1
    /* Search address*/
    for (int i = 8; i < 127; ++i) {
        if (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c1, i, 1, 0x10000)) {
            device_address = i;
            println("[EEPROM] Found 0x%X device", device_address);
            break;
        }
    }
#else
    device_address = EEPROM_DEVICE_ADDRESS;
#endif
    if (HAL_OK != HAL_I2C_IsDeviceReady(&hi2c1, device_address, 3, 0x10000)) {
        println("[EEPROM] Device 0x%04X not found", device_address);
        device_address = EEPROM_DEVICE_ADDRESS;
    } else {
        status = HAL_I2C_Mem_Write(&hi2c1, DEVICE_ADDRESS(device_address), 0x001F, I2C_MEMADD_SIZE_16BIT,
                                   (uint8_t *) "DATA", 4, HAL_MAX_DELAY);
//        status = Write_To_24LCxx(&hi2c1, DEVICE_ADDRESS(device_address), 0x001F, (uint8_t *) "DATA", 4);
        switch (status) {
            case HAL_OK: {
                while (HAL_I2C_IsDeviceReady(&hi2c1, device_address, 3, 0x1000000) != HAL_OK);
                println("[EEPROM] Write OK");
                break;
            }
            default:
                HAL_I2C_Print_Error("Writing", status, 0x001F);
                break;
        }
    }

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {
        uint8_t buf[1];
        /* USER CODE END WHILE */
        switch (state) {
            case READING_READY: {
                println("[EEPROM] Start reading");
                state = READING_MEMORY;
                break;
            }
            case READING_COMPLETE: {
                println("[EEPROM] Complete reading");
                reading_address = EEPROM_DATA_START_ADDRESS;
                state = READING_READY;
                break;
            }
            case READING_MEMORY: {
                memset(buf, 0, sizeof(buf));
                status = HAL_I2C_Mem_Read(&hi2c1, DEVICE_ADDRESS(device_address), reading_address,
                                          I2C_MEMADD_SIZE_16BIT, (uint8_t *) buf, sizeof(buf), HAL_MAX_DELAY);
//                status = Read_From_24LCxx(&hi2c1, DEVICE_ADDRESS(device_address), reading_address, (uint8_t *) buf, sizeof(buf));
                if (status != HAL_OK) {
                    HAL_I2C_Print_Error("Reading", status, reading_address);
                    reading_address = EEPROM_DATA_START_ADDRESS;
                    state = READING_READY;
                    while (HAL_I2C_IsDeviceReady(&hi2c1, device_address, 3, 0x1000000) != HAL_OK);
                    for (int i = 0; i < 0x100000; i++);
                } else {
                    println("[EEPROM]: ");
                    for (size_t i = 0; i < sizeof(buf); ++i) {
                        if (i % 8 == 0) {
                            println("");
                            print("0x%04X| ", reading_address + i);
                        }
                        print("0x%02X(%c) ", buf[i], buf[i]);
                    }
                    println("");
                    if (reading_address >= 0xFFFF) {
                        state = READING_COMPLETE;
                    } else {
                        reading_address += sizeof(buf);
                    }
                }
                break;
            }
            default:
                break;
        }


        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        for (int i = 0; i < 0x10000; i++);
        /* USER CODE BEGIN 3 */
    }
#pragma clang diagnostic pop
    /* USER CODE END 3 */
}

void HAL_I2C_Print_Error(char *mode, HAL_StatusTypeDef status, uint16_t reading_address) {
    uint32_t error = HAL_I2C_GetError(&hi2c1);
    switch (status) {
        case HAL_ERROR:
            println("[EEPROM] %s(0x%04X) ERROR. Code: 0x%08X", mode, reading_address, error);
            break;
        case HAL_BUSY:
            println("[EEPROM] %s(0x%04X) BUSY. Code: 0x%08X", mode, reading_address, error);
            break;
        case HAL_TIMEOUT:
            println("[EEPROM] %s(0x%04X) TIMEOUT. Code: 0x%08X", mode, reading_address, error);
            break;
        default:
            println("[EEPROM] %s(0x%04X) UNKNOWN. Code: 0x%08X", mode, reading_address, error);
            break;
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void) {

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
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
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
static void MX_SPI1_Init(void) {

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
    if (HAL_SPI_Init(&hspi1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI1_Init 2 */

    /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void) {

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
    if (HAL_UART_Init(&huart1) != HAL_OK) {
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
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

    /*Configure GPIO pin : PC13 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
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
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

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
