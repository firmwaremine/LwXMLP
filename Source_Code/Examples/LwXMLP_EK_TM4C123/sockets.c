/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== sockets.c ========
 */

#include <stdbool.h>
#include <string.h>

#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/WiFi.h>

/* SimpleLink Wi-Fi Host Driver Header files */
#include <osi.h>
#include <simplelink.h>
#include "VERBOSE.h"
/* Example/Board Header file */
#include "Board.h"

#if defined(MSP430WARE) || defined(MSP432WARE)
#define SPI_BIT_RATE    2000000
#elif defined(CC32XXWARE)
#define SPI_BIT_RATE    20000000
#else
#define SPI_BIT_RATE    14000000
#endif

bool deviceConnected = false;
bool ipAcquired = false;
bool smartConfigFlag = false;

/*
 *  ======== SimpleLinkGeneralEventHandler ========
 *  SimpleLink Host Driver callback for general device errors & events.
 */
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    System_printf("General event occurred, Event ID: %x", pDevEvent->Event);
}

/*
 *  ======== SimpleLinkHttpServerCallback ========
 *  SimpleLink Host Driver callback for HTTP server events.
 */
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
    SlHttpServerResponse_t *pHttpResponse)
{
    switch (pHttpEvent->Event) {
        case SL_NETAPP_HTTPGETTOKENVALUE_EVENT:
        case SL_NETAPP_HTTPPOSTTOKENVALUE_EVENT:
        default:
            break;
    }
}

/*
 *  ======== SimpleLinkNetAppEventHandler ========
 *  SimpleLink Host Driver callback for asynchronous IP address events.
 */
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pArgs)
{
    switch (pArgs->Event) {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
            ipAcquired = true;
            break;

        default:
            break;
    }
}

/*
 *  ======== SimpleLinkSockEventHandler ========
 *  SimpleLink Host Driver callback for socket event indication.
 */
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    switch(pSock->Event) {
        case SL_SOCKET_TX_FAILED_EVENT:
        default:
            break;
    }
}

/*
 *  ======== SimpleLinkWlanEventHandler ========
 *  SimpleLink Host Driver callback for handling WLAN connection or
 *  disconnection events.
 */
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pArgs)
{
    switch (pArgs->Event) {
        case SL_WLAN_CONNECT_EVENT:
            deviceConnected = true;
            break;

        case SL_WLAN_DISCONNECT_EVENT:
            deviceConnected = false;
            break;

        default:
            break;
    }
}

/*
 *  ======== setStationMode ========
 *  Sets the SimpleLink Wi-F in station mode and enables DHCP client
 */
void setStationMode(void) {
    int           mode;
    int           response;
    unsigned char param;

    mode = sl_Start(0, 0, 0);
    if (mode < 0) {
        System_abort("Could not initialize SimpleLink Wi-Fi");
    }

    /* Change network processor to station mode */
    if (mode != ROLE_STA) {
        sl_WlanSetMode(ROLE_STA);

        /* Restart network processor */
        sl_Stop(BIOS_WAIT_FOREVER);
        mode = sl_Start(0, 0, 0);
        if (mode < 0) {
            System_abort("Failed to set SimpleLink Wi-Fi to Station mode");
        }
    }

    sl_WlanDisconnect();

    /* Set auto connect policy */
    response = sl_WlanPolicySet(SL_POLICY_CONNECTION,
            SL_CONNECTION_POLICY(1, 0, 0, 0, 0), NULL, 0);
    if (response < 0) {
        System_abort("Failed to set connection policy to auto");
    }

    /* Enable DHCP client */
    param = 1;
    response = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, 1, 1, &param);
    if(response < 0) {
        System_abort("Could not enable DHCP client");
    }

    sl_Stop(BIOS_WAIT_FOREVER);

    /* Set connection variables to initial values */
    deviceConnected = false;
    ipAcquired = false;
}

/*
 *  ======== socketsShutDown ========
 *  Generic routine, defined to close down the WiFi in this case.
 */
void socketsShutDown(void *handle)
{
    sl_Stop(BIOS_WAIT_FOREVER);

    WiFi_close((WiFi_Handle)handle);
}

/*
 *  ======== socketsStartUp ========
 *  Generic routine, in this case defined to open the WiFi and await a
 *  connection, using Smart Config if the appropriate button is pressed.
 */
void *socketsStartUp(void)
{
    WiFi_Params        wifiParams;
    WiFi_Handle        handle;

    SlNetCfgIpV4Args_t ipV4;
    uint8_t            len = sizeof(ipV4);
    uint8_t            dhcpIsOn;
    int                result;

    /*
     * Board_LED1 is used as a connection indicator.  It will blink until a
     * connection is establish with the AP.
     */
    GPIO_write(Board_LED1, Board_LED_OFF);

    /* Open WiFi driver */
    WiFi_Params_init(&wifiParams);
    wifiParams.bitRate = SPI_BIT_RATE;
    handle = WiFi_open(Board_WIFI, Board_WIFI_SPI, NULL, &wifiParams);
    if (handle == NULL) {
        VERBOSE_printf("WiFi driver failed to open.\n\r");
    }

    /* Set the CC3X00 into station mode for this example */
    setStationMode();

    /* Host driver starts the network processor */
    result = sl_Start(NULL, NULL, NULL);
    if (result < 0) {
        VERBOSE_printf("Could not intialize SimpleLink Wi-Fi\n\r");
    }

    /*
     * Wait for SimpleLink to connect to an AP. If connecting to the AP for
     * the first time, press Board_BUTTON0 to start SmartConfig.
     */
    while ((deviceConnected != true) || (ipAcquired != true)) {
        if(smartConfigFlag) {
            /*
             *  Starts the SmartConfig process which allows the user to tell
             *  the CC3X00 which AP to connect to.
             *  Downloads available here: http://www.ti.com/tool/wifistarter
             */
            sl_WlanSmartConfigStart(NULL, SMART_CONFIG_CIPHER_NONE, NULL, NULL,
                    NULL, NULL, NULL, NULL);

            smartConfigFlag = false;
        }
        Task_sleep(50);
    }

    GPIO_write(Board_LED1, Board_LED_ON);

    /* Print IP address */
    sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO, &dhcpIsOn, &len,
            (unsigned char *)&ipV4);
    VERBOSE_printf(
            "CC3X00 has connected to an AP and acquired an IP address.\n\r");
    VERBOSE_printf("IP Address: %d.", SL_IPV4_BYTE(ipV4.ipV4, 3));
    VERBOSE_printf("%d.", SL_IPV4_BYTE(ipV4.ipV4, 2));
    VERBOSE_printf("%d.", SL_IPV4_BYTE(ipV4.ipV4, 1));
    VERBOSE_printf("%d\n\r", SL_IPV4_BYTE(ipV4.ipV4, 0));
    System_printf(
            "CC3X00 has connected to an AP and acquired an IP address.\n\r");
    System_printf("IP Address: %d.", SL_IPV4_BYTE(ipV4.ipV4, 3));
    System_printf("%d.", SL_IPV4_BYTE(ipV4.ipV4, 2));
    System_printf("%d.", SL_IPV4_BYTE(ipV4.ipV4, 1));
    System_printf("%d\n\r", SL_IPV4_BYTE(ipV4.ipV4, 0));

    System_flush();

    /* pass back the handle to our WiFi device */
    return ((void *)handle);
}
