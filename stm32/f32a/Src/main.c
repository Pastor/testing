#include "main.h"
#include <string.h>
#include <stdlib.h>
#include <stm32f1xx_hal_i2c.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>
#include <f32.h>

#define EEPROM_DEVICE_ADDRESS      0x50
#define EEPROM_DATA_START_ADDRESS  0x00

extern char *f18_instruction_name[];

I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart1;
struct Node g_node = {0};

void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void MX_I2C1_Init(void);

static void MX_SPI1_Init(void);

static void MX_USART1_UART_Init(void);

void HAL_I2C_Print_Error(char *mode, HAL_StatusTypeDef status, uint16_t reading_address);

enum State {
    READING_READY,
    READING_MEMORY,
    READING_COMPLETE
};

static char print_buffer[256];

void print(char *fmt, ...) {

    int ret;
    va_list args;

    va_start(args, fmt);
    ret = vsnprintf(print_buffer, sizeof(print_buffer), fmt, args);
    HAL_UART_Transmit(&huart1, (uint8_t *) print_buffer, ret, 10000);
    va_end(args);
}

void println(char *fmt, ...) {
    int ret;
    va_list args;

    va_start(args, fmt);
    ret = vsnprintf(print_buffer, sizeof(print_buffer), fmt, args);
    HAL_UART_Transmit(&huart1, (uint8_t *) print_buffer, ret, 10000);
    HAL_UART_Transmit(&huart1, (uint8_t *) "\r\n", 2, 10000);
    va_end(args);
}

static void write_io_register(struct Node *node, u18 io_register, u18 value) {
    switch (io_register) {
        case IOREG_USART: {
            print("%c", value);
            break;
        }
        default:
            print("[%08x] %c", io_register, value);
            break;
    }
}

static u18 read_io_register(struct Node *node, u18 io_register) {
    char buf[256];
    char *ptr;
    u18 instruction;
    u18 instruction_x;
    u18 dest;
    int i;
    size_t r;

    again:
    if ((node->flags & FLAG_TERMINATE) && io_register != IOREG_USART)
        return 0;


    // Interpreter mode channel
    i = 0;
    while (i < (sizeof(buf) - 1)) {
        HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, (uint8_t *)&buf[i], 1, 0x1000000);
        if (status != HAL_OK) {
            if (status == HAL_ERROR)  // it was stdin !
                node->flags |= FLAG_TERMINATE;  // lets terminate
            break;
        } else if (buf[i] == '\n')
            break;
        i++;
    }
    buf[i] = '\0';
    VERBOSE(node, "TXT[RD%04d]<--[%s]\n", io_register, buf);
    ptr = buf;
    i = parse_instruction(&ptr, &instruction_x, &dest);
    switch (i) {
        case TOKEN_EMPTY:
            goto again;
        case TOKEN_MNEMONIC1:
            instruction = (instruction_x << 13);
            break;
        case TOKEN_MNEMONIC2:
            // instruction part is encoded (^IMASK) but dest is not (why)
            instruction = (instruction_x << 13) ^ IMASK;                // encode instruction
            instruction = (instruction & ~MASK10) | (dest & MASK10);  // set address bits
            return instruction;
        case TOKEN_VALUE:
            return (instruction_x & MASK18);  // value not encoded
        default:
            goto error;
    }
    i = parse_instruction(&ptr, &instruction_x, &dest);
    switch (i) {
        case TOKEN_EMPTY: // assume rest of opcode are nops (warn?)
            instruction = (instruction | (INS_NOP << 8) | (INS_NOP << 3) | (INS_NOP >> 2)) ^ IMASK;
            return instruction;
        case TOKEN_MNEMONIC1:
            instruction |= (instruction_x << 8);
            break;
        case TOKEN_MNEMONIC2:
            instruction = (instruction | (instruction_x << 8)) ^ IMASK;      // encode instruction
            instruction = (instruction & ~MASK8) | (dest & MASK8);  // set address bits
            return instruction;
        default:
            goto error;
    }
    i = parse_instruction(&ptr, &instruction_x, &dest);
    switch (i) {
        case TOKEN_EMPTY:
            instruction = (instruction | (INS_NOP << 3) | (INS_NOP >> 2)) ^ IMASK;
            return instruction;
        case TOKEN_MNEMONIC1:
            instruction |= (instruction_x << 3);
            break;
        case TOKEN_MNEMONIC2:
            instruction = (instruction | (instruction_x << 3)) ^ IMASK;      // encode instruction
            instruction = (instruction & ~MASK3) | (dest & MASK3);  // set address bits
            return instruction;
        default:
            goto error;
    }
    i = parse_instruction(&ptr, &instruction_x, &dest);
    switch (i) {
        case TOKEN_EMPTY:
            instruction = (instruction | (INS_NOP >> 2)) ^ IMASK;
            return instruction;
        case TOKEN_MNEMONIC1:
            if ((instruction_x & 3) != 0)
                println("scan error: bad slot3 instruction used %s",
                        f18_instruction_name[instruction_x]);
            instruction = (instruction | (instruction_x >> 2)) ^ IMASK; // add op and encode
            return instruction;
        default:
            goto error;
    }

    error:
    println("io error when reading ioreg=%x, error=%s", io_register, strerror(errno));
    return 0;
}

static void Node_Init() {
    memset(&g_node, 0, sizeof(struct Node));
    g_node.read = read_io_register;
    g_node.write = write_io_register;
    g_node.b = IOREG_USART;
    g_node.p = IOREG_I2C1;
}

#define DEVICE_ADDRESS(da) ((da) << 1)

int main(void) {
    enum State state = READING_READY;
    HAL_StatusTypeDef status;
    uint16_t reading_address = 0;
    uint16_t device_address = EEPROM_DEVICE_ADDRESS;

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_SPI1_Init();
    MX_USART1_UART_Init();
    Node_Init();
#if 1
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
        println("[EEPROM] Device 0x%04X not detected", device_address);
        device_address = EEPROM_DEVICE_ADDRESS;
    }
#if 0
    else {
        status = HAL_I2C_Mem_Write(&hi2c1, DEVICE_ADDRESS(device_address), 0x001F, I2C_MEMADD_SIZE_16BIT,
                                   (uint8_t *) "DATA", 4, HAL_MAX_DELAY);
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
#endif
//    f18_emulate(&g_node);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {
        uint8_t buf[64];
        switch (state) {
            case READING_READY: {
                println("[EEPROM] Start reading");
                state = READING_MEMORY;
                break;
            }
            case READING_COMPLETE: {
                println("[EEPROM] Complete reading. Wait some times.");
                for (int i = 0; i < 0x1000000; i++);
                reading_address = EEPROM_DATA_START_ADDRESS;
                state = READING_READY;
                break;
            }
            case READING_MEMORY: {
                memset(buf, 0, sizeof(buf));
                status = HAL_I2C_Mem_Read(&hi2c1, DEVICE_ADDRESS(device_address), reading_address,
                                          I2C_MEMADD_SIZE_8BIT, (uint8_t *) buf, sizeof(buf), HAL_MAX_DELAY);
                if (status != HAL_OK) {
                    HAL_I2C_Print_Error("Reading", status, reading_address);
                    reading_address = EEPROM_DATA_START_ADDRESS;
                    state = READING_READY;
                    while (HAL_I2C_IsDeviceReady(&hi2c1, device_address, 3, 0x1000000) != HAL_OK);
                    for (int i = 0; i < 0x100000; i++);
                } else {
                    bool has_zero = false;
                    for (size_t i = 0; i < sizeof(buf); ++i) {
                        if (buf[i] == 0x00 || buf[i] == 0xFF) {
                            has_zero = true;
                            break;
                        }

#if 0
                        if ((reading_address + i) % 16 == 0) {
                            println("");
                            print("0x%04X| ", reading_address + i);
                        }
                        print("0x%02X(%c) ", buf[i], buf[i]);
#else
                        if (buf[i] == '\n') {
                            println("");
                        } else {
                            print("%c", buf[i]);
                        }
#endif
                    }
                    if (reading_address >= 0xFFFF || has_zero) {
                        state = READING_COMPLETE;
                        println("");
                    } else {
                        reading_address += sizeof(buf);
                    }
                }
                break;
            }
            default:
                state = READING_READY;
                break;
        }


        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        for (int i = 0; i < 0x10000; i++);
    }
#pragma clang diagnostic pop
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

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_I2C1_Init(void) {
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
}

static void MX_SPI1_Init(void) {
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
}

static void MX_USART1_UART_Init(void) {
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
}

static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void Error_Handler(void) {
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {

}
#endif
