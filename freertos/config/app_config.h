#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define configHOST_NAME "sensor-firmware"
#define configDEVICE_NICK_NAME configHOST_NAME

#define SENSOR_NAME "Sensor"
#define SENSOR_PRIO (tskIDLE_PRIORITY + 3)
#define SENSOR_STACK_SIZE configMINIMAL_STACK_SIZE

#define COMMS_NAME "Comms"
#define COMMS_PRIO (tskIDLE_PRIORITY + 2)
#define COMMS_STACK_SIZE configMINIMAL_STACK_SIZE

#ifdef __cplusplus
}
#endif

#endif /* APP_CONFIG_H */
