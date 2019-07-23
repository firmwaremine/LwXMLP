/*
 * Copyright (c) 2015, Texas Instruments Incorporated
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
 *    ======== tcpEchoHooks.c ========
 *    Contains non-BSD sockets code (NDK Network Open Hook)
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/inc/_stack.h>
#include <ti/ndk/inc/netmain.h>
#include <VERBOSE.h>
#include <STD_TYPE.h>
#define TCPPORT 1000

#define TCPHANDLERSTACK 1024

/* Prototypes */
Void tcpHandler(UArg arg0, UArg arg1);

/*
 *  ======== netOpenHook ========
 *  NDK network open hook used to initialize IPv6
 */
void netOpenHook()
{
    Task_Handle taskHandle;
    Task_Params taskParams;
    Error_Block eb;

    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    /*
     *  Create the Task that farms out incoming TCP connections.
     *  arg0 will be the port that this task listens to.
     */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TCPHANDLERSTACK;
    taskParams.priority = 1;
    taskParams.arg0 = TCPPORT;
    taskHandle = Task_create((Task_FuncPtr)tcpHandler, &taskParams, &eb);
    if (taskHandle == NULL) {
        System_printf("netOpenHook: Failed to create tcpHandler Task\n\r");
    }

    System_flush();
}
void NetworkStatusChangeHook(INT32U u32Item, INT32U u32Status, INT32U u32Report, HANDLE hHandle)
{
    INT16U      u16NumberOfInterfaces;
    INT32S      s32ReturnCode;
    INT32U      u23InstanceIndex, u32IPAddress;
    CI_ROUTE    objstrRouterConfiguration;
    static CI_IPNET    objstrIPNETConfiguration;
    NIMU_IF_REQ objstrInterfaceRequest;
    INT8U       u8InterfaceIndex;
    INT16U      u16MTU;
    static INT8S       s8DeviceName[MAX_INTERFACE_NAME_LEN];
    IPN         u32IPv4Address;

    if((u32Report & 0xFF) == 17)
    {
        /* Get a number of all devices present in the system.*/
        s32ReturnCode = NIMUIoctl(NIMU_GET_NUM_NIMU_OBJ, STD_NULL, &u16NumberOfInterfaces, sizeof(u16NumberOfInterfaces));
        if(s32ReturnCode >= 0) /* Got correct status */
        {
            VERBOSE_printf("Number of IP Interfaces %d\n\r", u16NumberOfInterfaces);
            /* Scan all IF's in the CFG for network information */
            for(u8InterfaceIndex = 1; u8InterfaceIndex <= u16NumberOfInterfaces ; u8InterfaceIndex++)
            {
               objstrInterfaceRequest.name[0] = 0;
               objstrInterfaceRequest.index   = u8InterfaceIndex;
               /* Get the device MTU */
               s32ReturnCode = NIMUIoctl(NIMU_GET_DEVICE_MTU, &objstrInterfaceRequest, &u16MTU, sizeof(u16MTU));
               if (s32ReturnCode >= 0)
               {
                   /* Get the device NAME. */
                   s32ReturnCode = NIMUIoctl(NIMU_GET_DEVICE_NAME, &objstrInterfaceRequest, &s8DeviceName[0], \
                                             sizeof(s8DeviceName));
                   if(s32ReturnCode >= 0)
                   {
                       VERBOSE_printf("IP device name is : %s\n\r", (INT32U)&s8DeviceName[0]);
                   }
                   else
                   {

                   }
               }
               if(s32ReturnCode >= 0)
               {
                   if (CfgGetImmediate(0, CFGTAG_IPNET, objstrInterfaceRequest.index, 1,\
                           sizeof(objstrIPNETConfiguration), (INT8U *)&objstrIPNETConfiguration) \
                           == sizeof(objstrIPNETConfiguration))
                   {
                   }
                   u32IPAddress = ntohl(objstrIPNETConfiguration.IPAddr);
                   VERBOSE_printf("IP Address: %d.%d.%d.%d\n\r",\
                                      (INT8U)(u32IPAddress >> 24) & 0xFF,\
                                      (INT8U)(u32IPAddress >> 16) & 0xFF,\
                                      (INT8U)(u32IPAddress >> 8) & 0xFF, \
                                      (INT8U)u32IPAddress & 0xFF);
                   u32IPAddress = ntohl(objstrIPNETConfiguration.IPMask);
                   VERBOSE_printf("Subnet Mask Address: %d.%d.%d.%d\n\r",\
                                      (INT8U)(u32IPAddress >> 24) & 0xFF,\
                                      (INT8U)(u32IPAddress >> 16) & 0xFF,\
                                      (INT8U)(u32IPAddress >> 8) & 0xFF, \
                                      (INT8U)u32IPAddress & 0xFF);

               }
            }
            /* Now scan all routes entered via the configuration */
            for(u23InstanceIndex = 1; ; u23InstanceIndex++)
            {
                if(CfgGetImmediate( 0,\
                                     CFGTAG_ROUTE, \
                                     0, \
                                     u23InstanceIndex, \
                                     sizeof(objstrRouterConfiguration), \
                                     (INT8U *)&objstrRouterConfiguration ) == sizeof(objstrRouterConfiguration))
                {
                    /* Look only for a default route */
                    if( objstrRouterConfiguration.IPDestAddr == 0 && objstrRouterConfiguration.IPDestMask == 0 )
                    {
                        u32IPAddress = ntohl(objstrRouterConfiguration.IPGateAddr);
                        VERBOSE_printf("Gateway Address: %d.%d.%d.%d\n\r",\
                                           (INT8U)(u32IPAddress >> 24) & 0xFF,\
                                           (INT8U)(u32IPAddress >> 16) & 0xFF,\
                                           (INT8U)(u32IPAddress >> 8) & 0xFF, \
                                           (INT8U)u32IPAddress & 0xFF);
                    }
                    else
                    {
                        u32IPAddress = ntohl(objstrRouterConfiguration.IPDestAddr);
                        VERBOSE_printf("Destination Network Address: %d.%d.%d.%d\n\r",\
                                           (INT8U)(u32IPAddress >> 24) & 0xFF,\
                                           (INT8U)(u32IPAddress >> 16) & 0xFF,\
                                           (INT8U)(u32IPAddress >> 8) & 0xFF, \
                                           (INT8U)u32IPAddress & 0xFF);
                        u32IPAddress = ntohl(objstrRouterConfiguration.IPDestMask);
                        VERBOSE_printf("Subnet Mask of Destination: %d.%d.%d.%d\n\r",\
                                           (INT8U)(u32IPAddress >> 24) & 0xFF,\
                                           (INT8U)(u32IPAddress >> 16) & 0xFF,\
                                           (INT8U)(u32IPAddress >> 8) & 0xFF, \
                                           (INT8U)u32IPAddress & 0xFF);
                        u32IPAddress = ntohl(objstrRouterConfiguration.IPGateAddr);
                        VERBOSE_printf("Gateway IP Address: %d.%d.%d.%d\n\r",\
                                           (INT8U)(u32IPAddress >> 24) & 0xFF,\
                                           (INT8U)(u32IPAddress >> 16) & 0xFF,\
                                           (INT8U)(u32IPAddress >> 8) & 0xFF, \
                                           (INT8U)u32IPAddress & 0xFF);
                    }
                }
                else /* No routing information is available */
                {
                    break;
                }
            }
            /* Now scan all DNS servers entered via the configuration */
            for(u23InstanceIndex = 1; ; u23InstanceIndex++)
            {
                if(CfgGetImmediate( 0,\
                                    CFGTAG_SYSINFO, \
                                    CFGITEM_DHCP_DOMAINNAMESERVER, \
                                    u23InstanceIndex, \
                                    sizeof(u32IPv4Address), \
                                   (INT8U *)&u32IPv4Address ) == sizeof(u32IPv4Address))
                {
                    u32IPAddress = ntohl(u32IPv4Address);
                    VERBOSE_printf("DNS Server Address: %d.%d.%d.%d\n\r",\
                                       (INT8U)(u32IPAddress >> 24) & 0xFF,\
                                       (INT8U)(u32IPAddress >> 16) & 0xFF,\
                                       (INT8U)(u32IPAddress >> 8) & 0xFF, \
                                       (INT8U)u32IPAddress & 0xFF);
                }
                else
                {
                    break;
                }
            }
            /* Now scan all NBNS servers entered via the configuration */
            for(u23InstanceIndex = 1; ; u23InstanceIndex++)
            {
                if(CfgGetImmediate( 0,\
                                    CFGTAG_SYSINFO, \
                                    CFGITEM_DHCP_NBNS, \
                                    u23InstanceIndex, \
                                    sizeof(u32IPv4Address), \
                                   (INT8U *)&u32IPv4Address ) == sizeof(u32IPv4Address))
                {
                    u32IPAddress = ntohl(u32IPv4Address);
                    VERBOSE_printf("NBNS Server Address: %d.%d.%d.%d\n\r",\
                                       (INT8U)(u32IPAddress >> 24) & 0xFF,\
                                       (INT8U)(u32IPAddress >> 16) & 0xFF,\
                                       (INT8U)(u32IPAddress >> 8) & 0xFF, \
                                       (INT8U)u32IPAddress & 0xFF);
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            VERBOSE_printf("NIMUIOCTL Failed with error code: %d\n\r", s32ReturnCode);
        }
    }
}
