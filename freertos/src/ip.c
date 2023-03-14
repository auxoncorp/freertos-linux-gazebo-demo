#include "FreeRTOS.h"
#include "task.h"

#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "stm32f4xx_hal_eth.h"

#include <stdio.h>
#include <time.h>

#include "stm32f4xx_hal_conf.h"
#include "app_config.h"
#include "ip.h"

const uint8_t MAC_ADDR[6] = {
    configMAC_ADDR0,
    configMAC_ADDR1,
    configMAC_ADDR2,
    configMAC_ADDR3,
    configMAC_ADDR4,
    configMAC_ADDR5
};

static const uint8_t IP_ADDR[4] = {
    configIP_ADDR0,
    configIP_ADDR1,
    configIP_ADDR2,
    configIP_ADDR3
};

static const uint8_t NET_MASK[4] = {
    configNET_MASK0,
    configNET_MASK1,
    configNET_MASK2,
    configNET_MASK3
};

static const uint8_t GW_ADDR[4] = {
    configGATEWAY_ADDR0,
    configGATEWAY_ADDR1,
    configGATEWAY_ADDR2,
    configGATEWAY_ADDR3
};

static const uint8_t DNS_ADDR[4] = {
    configDNS_SERVER_ADDR0,
    configDNS_SERVER_ADDR1,
    configDNS_SERVER_ADDR2,
    configDNS_SERVER_ADDR3
};

static UBaseType_t g_next_rand = 0;

void ip_init(void)
{
    time_t now;

    printf("hostname: %s\n", configHOST_NAME);

    /* Seed the random number generator. */
    time(&now);

    printf("Seed for randomiser: %ld\n", (unsigned long) now);
    g_next_rand = (UBaseType_t) now;
    printf("Random numbers: %08lX %08lX %08lX %08lX\n",
            ipconfigRAND32(),
            ipconfigRAND32(),
            ipconfigRAND32(),
            ipconfigRAND32());

    FreeRTOS_IPInit(
            IP_ADDR,
            NET_MASK,
            GW_ADDR,
            DNS_ADDR,
            MAC_ADDR);
}

UBaseType_t uxRand(void)
{
    const uint32_t mult = 0x015a4e35UL, inc = 1UL;

    /* Utility function to generate a pseudo random number. */
    g_next_rand = ( mult * g_next_rand ) + inc;
    return (int) (g_next_rand >> 16UL) & 0x7FFFUL;
}

uint32_t ulApplicationGetNextSequenceNumber(
        uint32_t src_addr,
        uint16_t src_port,
        uint32_t dst_addr,
        uint16_t dst_port)
{
    (void) src_addr;
    (void) src_port;
    (void) dst_addr;
    (void) dst_port;

    return uxRand();
}

BaseType_t xApplicationGetRandomNumber(uint32_t* num)
{
    *num = uxRand();
    return pdTRUE;
}

#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) || ( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
const char * pcApplicationHostnameHook(void)
{
    return configHOST_NAME;
}
#endif

#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 )
BaseType_t xApplicationDNSQueryHook(const char* name)
{
    BaseType_t xReturn;

    if(strcasecmp(name, pcApplicationHostnameHook()) == 0)
    {
        xReturn = pdPASS;
    }
    else if(strcasecmp(name, configDEVICE_NICK_NAME) == 0)
    {
        xReturn = pdPASS;
    }
    else
    {
        xReturn = pdFAIL;
    }

    return xReturn;
}
#endif

void vApplicationIPNetworkEventHook(eIPCallbackEvent_t network_event)
{
    uint32_t ip_addr;
    uint32_t net_mask;
    uint32_t gw_addr;
    uint32_t dns_addr;
    char cBuffer[16];

    if(network_event == eNetworkUp)
    {
        FreeRTOS_GetAddressConfiguration(&ip_addr, &net_mask, &gw_addr, &dns_addr);
        FreeRTOS_inet_ntoa(ip_addr, cBuffer);
        printf("IP Address: %s\n", cBuffer);
    }
    else
    {
        printf("Network down\n");
    }
}

void HAL_ETH_MspInit(ETH_HandleTypeDef * heth)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    gpio.Speed = GPIO_SPEED_HIGH;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Alternate = GPIO_AF11_ETH;
    gpio.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOB, &gpio);

    gpio.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
    HAL_GPIO_Init(GPIOC, &gpio);

    gpio.Pin =  GPIO_PIN_11 | GPIO_PIN_13;
    HAL_GPIO_Init(GPIOG, &gpio);

    HAL_NVIC_SetPriority(ETH_IRQn, ipconfigMAC_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(ETH_IRQn);

    __HAL_RCC_ETHMAC_CLK_ENABLE();
    __HAL_RCC_ETHMACTX_CLK_ENABLE();
    __HAL_RCC_ETHMACRX_CLK_ENABLE();
    __HAL_RCC_ETH_CLK_ENABLE();
}
