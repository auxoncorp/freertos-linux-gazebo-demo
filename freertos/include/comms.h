#ifndef COMMS_H
#define COMMS_H

#ifdef __cplusplus
extern "C" {
#endif

void comms_init(void);

void comms_send_sensor_data(int32_t wheel_speed);

#ifdef __cplusplus
}
#endif

#endif /* COMMS_H */
