/* Voltage monitor task */

#include "FreeRTOS.h"
#include "task.h"

#include "app_config.h"
#include "logging.h"
#include "comms.h"
#include "voltage_monitor.h"

#include "stm32f4xx_hal_conf.h"

#define UPDATE_PERIOD_MS pdMS_TO_TICKS(100)

// 12000 is nominal
#define MV_MAX (13000)
#define MV_MIN (11000)
#define MV_SPIKE (13000)

static void voltage_monitor_task(void* params);
static uint8_t rand8(void);

/* NOTE: zero means not-set, the test framework must write a value here regardless of use */
static __attribute__((section(".noinit"))) volatile uint32_t g_do_voltage_spike;

void voltage_monitor_init(void)
{
    xTaskCreate(voltage_monitor_task, VOLTAGE_MONITOR_NAME, VOLTAGE_MONITOR_STACK_SIZE, NULL, VOLTAGE_MONITOR_PRIO, NULL);
}

static void voltage_monitor_task(void* params)
{
    int32_t mv_delta = rand8();
    int32_t millivolts = MV_MIN;
    traceString ch;
    traceString spike_ch;
    TickType_t next_wake;
    (void) params;

    ch = xTraceRegisterString("voltage");
    spike_ch = xTraceRegisterString("voltage_spike");
    vTaskDelay(UPDATE_PERIOD_MS * 2);
    next_wake = xTaskGetTickCount();
    while(1)
    {
        const BaseType_t was_delayed = xTaskDelayUntil(&next_wake, UPDATE_PERIOD_MS);
        if(was_delayed == pdFALSE)
        {
            WARN("Voltage monitor task deadline missed");
        }

        vTracePrintF(ch, "%d", millivolts);

        if(millivolts >= MV_SPIKE)
        {
            vTracePrintF(spike_ch, "%d", millivolts);
        }

        comms_send_voltage_data(millivolts, (uint8_t) (millivolts >= MV_SPIKE));

        millivolts += mv_delta;
        if(millivolts >= MV_MAX)
        {
            millivolts = MV_MAX;
            mv_delta = -rand8();
        }
        else if (millivolts <= MV_MIN)
        {
            millivolts = MV_MIN;
            mv_delta = rand8();
        }

        if(g_do_voltage_spike == 0)
        {
            if(millivolts >= MV_SPIKE)
            {
                millivolts = (MV_SPIKE - 1);
            }
        }
    }
}

// https://stackoverflow.com/a/16761955
#define STATE_BYTES 7
#define MULT 0x13B /* for STATE_BYTES==6 only */
#define MULT_LO (MULT & 255)
#define MULT_HI (MULT & 256)

static uint8_t rand8(void)
{
    static uint8_t state[STATE_BYTES] =
    { 0x87, 0xdd, 0xdc, 0x10, 0x35, 0xbc, 0x5c };
    static uint16_t c = 0x42;
    static int i = 0;
    uint16_t t;
    uint8_t x;

    x = state[i];
    t = (uint16_t)x * MULT_LO + c;
    c = t >> 8;
#if MULT_HI
    c += x;
#endif
    x = t & 255;
    state[i] = x;
    if (++i >= sizeof(state))
        i = 0;
    return x;
}
