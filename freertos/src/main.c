#include "FreeRTOS.h"
#include "task.h"

#include "app_config.h"
#include "logging.h"
#include "ip.h"
#include "comms.h"
#include "wss.h"
#include "stats.h"

#include "stm32f4xx_hal_conf.h"

void init_console_uart(void);

int main(void)
{
    traceResult tr;

    /* NOTE: startup_stm32f407xx.s calls SystemInit() before main */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    init_console_uart();

    tr = xTraceEnable(TRC_START);
    configASSERT(tr == TRC_SUCCESS);

    logging_init();
    ip_init();
    comms_init();
    wss_init();
    stats_init();

    configASSERT(xTraceDiagnosticsCheckStatus() == TRC_SUCCESS);
    const char* err = NULL;
    (void) xTraceErrorGetLast(&err);
    if(err != NULL)
    {
        ERR("Trace error: %s", err);
    }
    configASSERT(err == NULL);

    INFO("System initialized");
    vTaskStartScheduler();

    while(1)
    {
        portNOP();
    }

    return 0;
}
