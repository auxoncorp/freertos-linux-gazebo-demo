#include "FreeRTOS.h"
#include "task.h"

#include <stdarg.h>
#include <stdio.h>

void vLoggingPrintf(const char* fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    vprintf(fmt, arg);
    va_end(arg);
}

void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    printf("vApplicationMallocFailedHook\n");
    while(1)
    {
        portNOP();
    }
}

void vApplicationStackOverflowHook(TaskHandle_t task, char* name)
{
    (void) task;
    (void) name;

    taskDISABLE_INTERRUPTS();
    printf("vApplicationStackOverflowHook task='%s'\n", name);
    while(1)
    {
        portNOP();
    }
}

void vAssertCalled(const char* file, int line)
{
    const char* err = NULL;

    taskDISABLE_INTERRUPTS();
    printf("vAssertCalled\n%s:%d\n", file, line);

    (void) xTraceErrorGetLast(&err);
    if(err != NULL)
    {
        printf("xTraceErrorGetLast: %s\n", err);
    }

    while(1)
    {
        portNOP();
    }
}
