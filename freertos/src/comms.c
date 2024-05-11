#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "app_config.h"
#include "logging.h"
#include "wire_protocol.h"
#include "comms.h"

#define MSG_TYPE_WHEEL_SPEED (1)
#define MSG_TYPE_VOLTAGE (2)

typedef struct
{
    uint8_t type;
    int32_t wheel_speed;
    int32_t millivolts;
    uint8_t voltage_spike;
} comms_msg_s;

static void comms_task(void* params);

#define COMMS_QUEUE_LENGTH (8)
static QueueHandle_t g_comms_queue = NULL;

void comms_init(void)
{
    g_comms_queue = xQueueCreate(COMMS_QUEUE_LENGTH, sizeof(comms_msg_s));
    configASSERT(g_comms_queue != NULL);

    xTaskCreate(comms_task, COMMS_NAME, COMMS_STACK_SIZE, NULL, COMMS_PRIO, NULL);
}

void comms_send_sensor_data(int32_t wheel_speed)
{
    comms_msg_s msg =
    {
        .type = MSG_TYPE_WHEEL_SPEED,
        .wheel_speed = wheel_speed,
    };

    const BaseType_t was_posted = xQueueSend(g_comms_queue, &msg, 0);
    if(was_posted != pdTRUE)
    {
        ERR("Failed to send sensor data");
    }
}

void comms_send_voltage_data(int32_t millivolts, uint8_t voltage_spike)
{
    comms_msg_s msg =
    {
        .type = MSG_TYPE_VOLTAGE,
        .millivolts = millivolts,
        .voltage_spike = voltage_spike,
    };

    const BaseType_t was_posted = xQueueSend(g_comms_queue, &msg, 0);
    if(was_posted != pdTRUE)
    {
        ERR("Failed to send voltage data");
    }
}

static comms_msg_s comms_recv(void)
{
    comms_msg_s msg;
    const BaseType_t was_recvd = xQueueReceive(g_comms_queue, &msg, portMAX_DELAY);
    configASSERT(was_recvd == pdTRUE);
    return msg;
}

static void comms_task(void* params)
{
    BaseType_t status;
    traceString ch;
    wire_msg_s wire_msg = {0};
    Socket_t socket = FREERTOS_INVALID_SOCKET;
    struct freertos_sockaddr addr = {0};
    uint8_t mutator_state = 0;
    uint8_t voltage_spiked = 0;
    (void) params;

    ch = xTraceRegisterString("comms_tx");

    while(FreeRTOS_IsNetworkUp() == pdFALSE)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    addr.sin_port = FreeRTOS_htons(SENSOR_DATA_PORT);
    addr.sin_addr = FreeRTOS_inet_addr_quick(
            configIP_ADDR0,
            configIP_ADDR1,
            configIP_ADDR2,
            81);

    socket = FreeRTOS_socket(
            FREERTOS_AF_INET,
            FREERTOS_SOCK_DGRAM,
            FREERTOS_IPPROTO_UDP);
    configASSERT(socket != FREERTOS_INVALID_SOCKET);

    INFO("Comms network ready");

    while(1)
    {
        const comms_msg_s comms_msg = comms_recv();

        if(comms_msg.type == MSG_TYPE_VOLTAGE)
        {
            if(comms_msg.voltage_spike != 0)
            {
                // Mutate the next comms message
                voltage_spiked = 1;
                mutator_state += 1;
                if(mutator_state == 3)
                {
                    mutator_state = 0;
                }
            }

            continue;
        }

        configASSERT(comms_msg.type == MSG_TYPE_WHEEL_SPEED);

        wire_msg.magic0 = WIRE_MAGIC0;
        wire_msg.magic1 = WIRE_MAGIC1;
        wire_msg.type = WIRE_TYPE_WHEEL_SPEED;
        wire_msg.seqnum += 1;
        wire_msg.wheel_speed = comms_msg.wheel_speed;

        if(voltage_spiked != 0)
        {
            if(mutator_state == 0)
            {
                wire_msg.magic0 = ~wire_msg.magic0;
            }
            else if(mutator_state == 1)
            {
                wire_msg.magic1 = ~wire_msg.magic1;
            }
            else if(mutator_state == 2)
            {
                wire_msg.type = WIRE_TYPE_WHEEL_SPEED + 1;
            }
            voltage_spiked = 0;
        }

        xTracePrintF(ch, "%u %u %d", wire_msg.type, wire_msg.seqnum, wire_msg.wheel_speed);

        status = FreeRTOS_sendto(socket, &wire_msg, sizeof(wire_msg), 0, &addr, sizeof(addr));
        if(status < 0)
        {
            ERR("Failed to send sensor data wire message");
        }
    }
}
