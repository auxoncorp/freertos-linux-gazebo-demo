#include "FreeRTOS.h"
#include "task.h"

#include "FreeRTOS_IP.h"

//#define PRINT_STATS
#ifdef PRINT_STATS
#include <stdio.h>
#endif
#include <string.h>

#include "app_config.h"
#include "stats.h"

#define STATS_PERIOD_MS pdMS_TO_TICKS(1000)
#define MAX_TASKS (12)


void stats_init(void)
{
}

