/* Modality utilities */

#include "FreeRTOS.h"

#include <stdio.h>

#include "app_config.h"
#include "logging.h"
#include "modality.h"

/* NOTE: zero means no-set, the test framework must write a value here regardless of use */
static __attribute__((section(".noinit"))) volatile uint32_t g_startup_nonce;

static TraceStringHandle_t g_startup_ch = 0;

void modality_trace_startup_nonce(void)
{
    traceResult tr;

    if(g_startup_nonce != 0)
    {
        tr = xTraceStringRegister("test_framework_nonce", &g_startup_ch);
        configASSERT(tr == TRC_SUCCESS);

        xTracePrintF(g_startup_ch, "%u", g_startup_nonce);

        g_startup_nonce = 0;
    }
}
