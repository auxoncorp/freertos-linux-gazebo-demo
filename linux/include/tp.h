#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER actuator

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./tp.h"

#if !defined(TP_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define TP_H

#include <lttng/tracepoint.h>
#include <stdint.h>
#include "app_types.h"

TRACEPOINT_EVENT(
    actuator,
    startup,
    TP_ARGS(),
    TP_FIELDS()
)

TRACEPOINT_EVENT(
    actuator,
    init,
    TP_ARGS(),
    TP_FIELDS()
)

TRACEPOINT_EVENT(
    actuator,
    deinit,
    TP_ARGS(),
    TP_FIELDS()
)

TRACEPOINT_EVENT(
    actuator,
    received_msg,
    TP_ARGS(
        uint8_t, magic0,
        uint8_t, magic1,
        uint8_t, type,
        uint32_t, seqnum,
        int32_t, wheel_speed,
        uint32_t, remote_nonce,
        const char *, remote_timeline_id
    ),
    TP_FIELDS(
        ctf_integer_hex(int, magic0, magic0)
        ctf_integer_hex(uint8_t, magic1, magic1)
        ctf_integer_hex(uint8_t, type, type)
        ctf_integer(uint32_t, seqnum, seqnum)
        ctf_integer(int32_t, wheel_speed, wheel_speed)
        ctf_integer(uint32_t, remote_nonce, remote_nonce)
        ctf_string(remote_timeline_id, remote_timeline_id)
    )
)

TRACEPOINT_ENUM(
    actuator,
    invalid_msg_reason,
    TP_ENUM_VALUES(
        ctf_enum_value("BAD_MAGIC0", 0)
        ctf_enum_value("BAD_MAGIC1", 1)
        ctf_enum_value("BAD_TYPE", 2)
    )
)

TRACEPOINT_EVENT(
    actuator,
    invalid_msg,
    TP_ARGS(uint8_t, reason),
    TP_FIELDS(ctf_enum(actuator, invalid_msg_reason, uint8_t, reason, reason))
)

TRACEPOINT_EVENT(
    actuator,
    set_pwm,
    TP_ARGS(
        uint16_t, duty
    ),
    TP_FIELDS(
        ctf_integer(uint16_t, duty, duty)
    )
)

#endif /* TP_H */

#include <lttng/tracepoint-event.h>
