#ifndef COMMS_H
#define COMMS_H

#ifdef __cplusplus
extern "C" {
#endif

void comms_init(void);

void comms_send_sensor_data(int32_t wheel_speed);

void comms_send_voltage_data(int32_t millivolts, uint8_t voltage_spike);

#ifdef __cplusplus
}
#endif

#endif /* COMMS_H */
