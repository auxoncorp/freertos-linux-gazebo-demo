#include "FreeRTOS.h"

#include "stm32f4xx_hal_conf.h"

static USART_HandleTypeDef g_usart_handle = {0};

uint32_t HAL_GetTick(void)
{
    return 0;
}

void init_console_uart(void)
{
    HAL_StatusTypeDef ret;
    GPIO_InitTypeDef gpio;

    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpio.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &gpio);

    g_usart_handle.Instance = USART3;
    g_usart_handle.Init.BaudRate = 115200;
    g_usart_handle.Init.WordLength = USART_WORDLENGTH_8B;
    g_usart_handle.Init.StopBits = USART_STOPBITS_1;
    g_usart_handle.Init.Parity = USART_PARITY_NONE;
    g_usart_handle.Init.Mode = USART_MODE_TX_RX;
    ret = HAL_USART_Init(&g_usart_handle);
    configASSERT(ret == HAL_OK);
}

int _write(int file, char *buf, int len)
{
    if((buf == NULL) || (len <= 0))
    {
        len = 0;
    }
    else
    {
        HAL_USART_Transmit(&g_usart_handle, (const uint8_t*) buf, len, HAL_MAX_DELAY);
    }

    return len;
}
