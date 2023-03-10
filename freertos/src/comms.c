#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "app_config.h"
#include "logging.h"
#include "wire_protocol.h"
#include "comms.h"

typedef struct
{
    int32_t wheel_speed;
} comms_msg_s;

static void comms_task(void* params);

#define COMMS_QUEUE_LENGTH (8)
static QueueHandle_t g_comms_queue = NULL;

#define UID_W0() (*((volatile uint32_t*) 0x1FFF7A10))
#define UID_W1() (*((volatile uint32_t*) 0x1FFF7A14))
#define UID_W2() (*((volatile uint32_t*) 0x1FFF7A18))
#define TID_NBYTES (16)
#define TID_NCHARS ((TID_NBYTES * 2) + 4 + 1) /* 4 '-' chars, plus NULL */
static char g_tid[TID_NCHARS] = {0};

static void generate_tid(void)
{
    uint32_t word;
    unsigned char raw_uuid[TID_NBYTES];
    char hex_str[(TID_NBYTES * 2) + 1];
    char* ptr;

    // Use the 96-bit random device ID (randomly generated in renode)
    word = UID_W0();
    (void) memcpy(&raw_uuid[0], &word, sizeof(word));
    (void) memcpy(&raw_uuid[12], &word, sizeof(word));
    word = UID_W1();
    (void) memcpy(&raw_uuid[4], &word, sizeof(word));
    word = UID_W2();
    (void) memcpy(&raw_uuid[8], &word, sizeof(word));

    // Make sure bits are inline with RFC 4122 4.4
    raw_uuid[6] = 0x40 | (raw_uuid[6] & 0xF0);
    raw_uuid[8] = 0x80 | (raw_uuid[8] & 0x3F);

    // Convert to hex string
    ptr = &hex_str[0];
    for(word = 0; word < TID_NBYTES; word += 1)
    {
        ptr += sprintf(ptr, "%02X", raw_uuid[word]);
    }

    // Add hyphens
    memset(g_tid, '\0', TID_NCHARS);
    strncpy(g_tid, &hex_str[0], 8);
    strcat(g_tid, "-");
    strncat(g_tid, &hex_str[8], 4);
    strcat(g_tid, "-");
    strncat(g_tid, &hex_str[12], 4);
    strcat(g_tid, "-");
    strncat(g_tid, &hex_str[16], 4);
    strcat(g_tid, "-");
    strncat(g_tid, &hex_str[20], 12);
}

void comms_init(void)
{
    generate_tid();

    g_comms_queue = xQueueCreate(COMMS_QUEUE_LENGTH, sizeof(comms_msg_s));
    configASSERT(g_comms_queue != NULL);

    xTaskCreate(comms_task, COMMS_NAME, COMMS_STACK_SIZE, NULL, COMMS_PRIO, NULL);

    traceString ch = xTraceRegisterString("modality_timeline_id");
    xTracePrintF(ch, "name=%s,id=%s", xTraceRegisterString(COMMS_NAME), xTraceRegisterString(g_tid));

    printf(COMMS_NAME " timeline-id: %s\n", g_tid);
}

void comms_send_sensor_data(int32_t wheel_speed)
{
    comms_msg_s msg =
    {
        .wheel_speed = wheel_speed,
    };

    const BaseType_t was_posted = xQueueSend(g_comms_queue, &msg, 0);
    if(was_posted != pdTRUE)
    {
        ERR("Failed to send sensor data");
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
    (void) params;

    (void) memcpy(&wire_msg.tid[0], &g_tid[0], WIRE_TID_LEN);

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
            255);

    socket = FreeRTOS_socket(
            FREERTOS_AF_INET,
            FREERTOS_SOCK_DGRAM,
            FREERTOS_IPPROTO_UDP);
    configASSERT(socket != FREERTOS_INVALID_SOCKET);

    INFO("Comms network ready");

    while(1)
    {
        const comms_msg_s comms_msg = comms_recv();

        wire_msg.magic0 = WIRE_MAGIC0;
        wire_msg.magic1 = WIRE_MAGIC1;
        wire_msg.type = WIRE_TYPE_WHEEL_SPEED;
        wire_msg.seqnum += 1;
        wire_msg.wheel_speed = comms_msg.wheel_speed;

        if((wire_msg.seqnum > 10) && ((wire_msg.seqnum) % 5 == 0))
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
                wire_msg.type += 1;
            }

            mutator_state += 1;
            if(mutator_state == 3)
            {
                mutator_state = 0;
            }
        }

        xTracePrintF(ch, "%u %u %d %u", wire_msg.type, wire_msg.seqnum, wire_msg.wheel_speed, wire_msg.seqnum);

        status = FreeRTOS_sendto(socket, &wire_msg, sizeof(wire_msg), 0, &addr, sizeof(addr));
        if(status < 0)
        {
            ERR("Failed to send sensor data wire message");
        }
    }
}
