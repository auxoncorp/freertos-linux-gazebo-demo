/* Wheel speed sensor peripheral driver task */

#include "FreeRTOS.h"
#include "task.h"

#include "app_config.h"
#include "logging.h"
#include "comms.h"
#include "wss.h"

#include "stm32f4xx_hal_conf.h"

#define WSS_POLL_PERIOD_MS pdMS_TO_TICKS(100)

#define WSS_ADDR (0x50070000U)

#define WSS_CTRL_INIT (1 << 0)
#define WSS_CTRL_USING_SIMULATOR (1 << 7)
#define WSS_STATUS_WHEEL_SPEED_VALID (1 << 0)

typedef struct
{
    volatile uint32_t CTRL;         /* 0x00 : Control register */
    volatile uint32_t STATUS;       /* 0x04 : Status register */
    volatile uint32_t WHEEL_SPEED;  /* 0x08 : Wheel speed register */
} __attribute__((packed)) WSS_s;

static void wss_task(void* params);

static WSS_s* WSS = (WSS_s*) WSS_ADDR;

void wss_init(void)
{
    INFO("Initializing wheel speed sensor");

    WSS->CTRL |= (WSS_CTRL_INIT | WSS_CTRL_USING_SIMULATOR);

    xTaskCreate(wss_task, SENSOR_NAME, SENSOR_STACK_SIZE, NULL, SENSOR_PRIO, NULL);
}

static void wss_task(void* params)
{
    int32_t wheel_speed;
    traceString ch;
    TickType_t next_wake;
    (void) params;

    ch = xTraceRegisterString("wheel_speed");
    next_wake = xTaskGetTickCount();
    while(1)
    {
        const BaseType_t was_delayed = xTaskDelayUntil(&next_wake, WSS_POLL_PERIOD_MS);
        if(was_delayed == pdFALSE)
        {
            WARN("WSS sensor task deadline missed");
        }

        if((WSS->STATUS & WSS_STATUS_WHEEL_SPEED_VALID) != 0)
        {
            wheel_speed = (int32_t) WSS->WHEEL_SPEED;
            vTracePrintF(ch, "%d", wheel_speed);
            comms_send_sensor_data(wheel_speed);
        }
    }
}
