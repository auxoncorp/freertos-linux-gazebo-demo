/* TODO make this a shared header, curently dup'd between the linux and freertos apps
 * until the docker setup is refactored
 */

#ifndef WIRE_PROTOCOL_H
#define WIRE_PROTOCOL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SENSOR_DATA_PORT (9889)

#define WIRE_MAGIC0 (0xAA)
#define WIRE_MAGIC1 (0xBB)

#define WIRE_TYPE_WHEEL_SPEED (0xF0)

#define WIRE_TID_LEN (32 + 4 + 1)

typedef struct
{
    uint8_t magic0; /* WIRE_MAGIC0 */
    uint8_t magic1; /* WIRE_MAGIC1 */
    uint8_t type;
    uint32_t seqnum;
    int32_t wheel_speed; /* 1/1000 radians/second */
    char tid[WIRE_TID_LEN];
} __attribute__((packed, aligned(1))) wire_msg_s;

#define WIRE_MSG_SIZE (sizeof(wire_msg_s))

#ifdef __cplusplus
}
#endif

#endif /* WIRE_PROTOCOL_H */
