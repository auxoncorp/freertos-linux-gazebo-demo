#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_HWTC_TYPE TRC_FREE_RUNNING_32BIT_INCR
#define TRC_HWTC_COUNT (timer_read())
#define TRC_HWTC_PERIOD 0
#define TRC_HWTC_DIVISOR 1
#define TRC_HWTC_FREQ_HZ (480000)
#define TRC_IRQ_PRIORITY_ORDER 0

#define TRC_PORT_SPECIFIC_INIT() timer_init()

void timer_init(void);

uint32_t timer_read(void);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_H */
