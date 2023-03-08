#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct actuator;
typedef struct actuator actuator_s;

actuator_s* actuator_new();

void actuator_free(actuator_s* act);

void actuator_set_vel(actuator_s* act, double vel);

#ifdef __cplusplus
}
#endif

#endif /* ACTUATOR_H */
