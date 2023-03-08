#include "FreeRTOS.h"

#include "timer.h"

#include "stm32f4xx_hal_conf.h"

static TIM_HandleTypeDef g_timer;

void timer_init(void)
{
    HAL_StatusTypeDef ret;

    g_timer.Instance = TIM2;
    g_timer.Init.Prescaler = 20;
    g_timer.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_timer.Init.Period = 0xFFFFFFFF;
    g_timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g_timer.Init.RepetitionCounter = 0;
    g_timer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    ret = HAL_TIM_Base_Init(&g_timer);
    configASSERT(ret == HAL_OK);

    ret = HAL_TIM_Base_Start(&g_timer);
    configASSERT(ret == HAL_OK);
}

uint32_t timer_read(void)
{
    return g_timer.Instance->CNT;
}
