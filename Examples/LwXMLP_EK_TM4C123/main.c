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
 *  ======== tcpEchoCC3X00.c ========
 */
#include <string.h>
#include <stdbool.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/SecondsClock.h>
/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include "driverlib/uart.h"
/* SimpleLink Wi-Fi Host Driver Header files */
#include <simplelink.h>

/* Example/Board Header file */
#include "Board.h"

/* Local Platform Specific Header file */
#include "sockets.h"

#include "LwXMLP_CORE.h"
/* Port number for listening for TCP packets */
#define TCPPORT         1000

#define TCPPACKETSIZE   256
#define TASKSTACKSIZE   2048
#define MAX_TCP_RECEIVE_BUFFER 500
#define TIME_OUT_ON_CLIENT            4/* time out in seconds */

extern bool smartConfigFlag;
Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];
INT32U u32counter;
typedef enum
{
    csParseInit,
    csParseReceiveData,
    csParsingCloseState,
    csXMLparsingClosedState
}enumParsingStatType;
#ifdef USE_COLOR
    #define RED_TEXT      "\033[1;31m"
    #define WHITE_TEXT    "\033[0m"
#else
    #define RED_TEXT
    #define WHITE_TEXT
#endif
#define TEST_TOOL_END_OF_LINE   "E\r\n"
#define REGULAR_END_OF_LINE     "\n\r"
#define END_OF_LINE        TEST_TOOL_END_OF_LINE

#if(ALLOCATION_TYPE == EXTERNAL_ALLOCATION)
    LwXMLP_strXMLParseInstanceType objParserExternalInstanceType;
#endif
static INT8U u8ReceiveBuffer[MAX_TCP_RECEIVE_BUFFER];
static StdReturnType APP_srParserEventHandler(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
static StdReturnType APP_srParserEventHandler(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    StdReturnType  srParserHandlerStatus = E_OK;
    INT8U          u8AttributesIndex;
    INT8U         const *pu8CurrentPath;

    pu8CurrentPath = LwXMLP_pu8GetCurrentPath((LwXMLP_strXMLParseInstanceType const *)pstrParserInstanceType);
    switch(LwXMLP_enumGetXMLEventType(pstrParserInstanceType))
    {
        case csCommentEvent: /* Comment Event is fired */
             /*
              * To get the use of the received comment use LwXMLP_pu8GetParsedComment(pstrParserInstanceType) which
              * will return a pointer to the parsed comment.
              * in case of a verification for the XML path is needed, use
              * LwXMLP_pu8GetCurrentPath(pstrParserInstanceType) which will return a pointer to the current XML path
              */

#if 1
            VERBOSE_printf(RED_TEXT "Comment: "WHITE_TEXT"%s", LwXMLP_pu8GetParsedComment(pstrParserInstanceType));
#endif
             break;
        case csProcessInstructionEvent: /* Process instruction Event is fired */
             /*
              * To get the use of the received PI target and data, use
              * LwXMLP_pu8GetParsedPITarget(pstrParserInstanceType) which will return a pointer to the parsed Process
              * Instruction Target and use LwXMLP_pu8GetParsedPIData(pstrParserInstanceType) which will return a pointer
              * to the parsed Process Instruction Data.
              * in case of a verification for the XML path is needed, use
              * LwXMLP_pu8GetCurrentPath(pstrParserInstanceType) which will return a pointer to the current XML path
              */
#if 1
             if(LwXMLP_pu8GetParsedPIData(pstrParserInstanceType) != STD_NULL)
             {
                 VERBOSE_printf(RED_TEXT "PI target: "\
                                WHITE_TEXT"%s" RED_TEXT " PI Data: " WHITE_TEXT "%s",\
                                LwXMLP_pu8GetParsedPITarget(pstrParserInstanceType), \
                               LwXMLP_pu8GetParsedPIData(pstrParserInstanceType));
             }
             else
             {
                 VERBOSE_printf(RED_TEXT "PI target: "\
                                WHITE_TEXT"%s" RED_TEXT,\
                                LwXMLP_pu8GetParsedPITarget(pstrParserInstanceType));
             }
#endif
             break;
        case csStartElementEvent:/* Start TAG Event is fired */
             /*
              * To get the name of the start TAG, use LwXMLP_pu8GetElementName(pstrParserInstanceType) whcih will
              * return a pointer to the Name string.
              * To get the XML path is needed, use LwXMLP_pu8GetCurrentPath(pstrParserInstanceType)
              * which will return a pointer to the current XML path
              * To check the available attribute counts, use LwXMLP_s16GetAttributeCount(pstrParserInstanceType) which
              * will return the attribute counts if there.
              * To get specific attribute name, use LwXMLP_pu8GetAttributeName(pstrParserInstanceType, u8AttributesIndex)
              * where the u8AttributesIndex is the zero index of the avaliable attributes.
              * To get specific attribute value, use LwXMLP_pu8GetAttributeValue(pstrParserInstanceType, u8AttributesIndex)
              * where the u8AttributesIndex is the zero index of the available attributes
              *
              */
#if 1
//             VERBOSE_printf(RED_TEXT"XML Element Start path: " WHITE_TEXT "%s\n", pu8CurrentPath);
//             VERBOSE_printf(END_OF_LINE); /* needed for test tool */
             VERBOSE_printf(RED_TEXT"Start: " WHITE_TEXT "%s",\
                            LwXMLP_pu8GetElementName(pstrParserInstanceType));
             if(LwXMLP_s16GetAttributeCount(pstrParserInstanceType) > 0)
             {
                 for(u8AttributesIndex = 0; \
                     u8AttributesIndex < LwXMLP_s16GetAttributeCount(pstrParserInstanceType) ; \
                     ++u8AttributesIndex)
                 {
                     VERBOSE_printf(RED_TEXT" AN: " WHITE_TEXT "%s" RED_TEXT " AV: " WHITE_TEXT "%s", \
                                   LwXMLP_pu8GetAttributeName(pstrParserInstanceType, u8AttributesIndex),\
                                    LwXMLP_pu8GetAttributeValue(pstrParserInstanceType, u8AttributesIndex));

                 }
             }
#endif
             break;
        case csEndElementEvent:/* End TAG Event is fired */
            /*
             * To get the name of the End TAG, use LwXMLP_pu8GetElementName(pstrParserInstanceType) which will
             * return a pointer to the Name string.
             * To get the XML path is needed, use LwXMLP_pu8GetCurrentPath(pstrParserInstanceType)
             * which will return a pointer to the current XML path
             */

#if 1
//            VERBOSE_printf(RED_TEXT"XML Element End path: " WHITE_TEXT "%s\n",pu8CurrentPath);
//            VERBOSE_printf(END_OF_LINE); /* needed for test tool */
            VERBOSE_printf(RED_TEXT"End: " WHITE_TEXT "%s",\
                          LwXMLP_pu8GetElementName(pstrParserInstanceType));

#endif

             break;
        case csNotationEvent:
#if 1
             VERBOSE_printf(RED_TEXT "Notation: " WHITE_TEXT "%s", \
                            LwXMLP_pu8GetParsedNotation(pstrParserInstanceType));
#endif
             break;
        case csCDataEvent: /* CDATA event is fired */
             /*
              * To get the received Text, use LwXMLP_pu8GetParsedText(pstrParserInstanceType) which will
              * return a pointer to the Name string.
              * To get the XML path is needed, use LwXMLP_pu8GetCurrentPath(pstrParserInstanceType)
              * which will return a pointer to the current XML path
              */
#if 1
              VERBOSE_printf(RED_TEXT "CDATA: " WHITE_TEXT "%s", \
                             LwXMLP_pu8GetParsedText(pstrParserInstanceType));
#endif

             break;
        case csTextNodeEvent:/* Text Event is fired */
            /*
             * To get the received Text, use LwXMLP_pu8GetParsedText(pstrParserInstanceType) which will
             * return a pointer to the Name string.
             * To get the XML path is needed, use LwXMLP_pu8GetCurrentPath(pstrParserInstanceType)
             * which will return a pointer to the current XML path
             */
#if 1
//             VERBOSE_printf(RED_TEXT"XML Element Text path: " WHITE_TEXT "%s\n", pu8CurrentPath);
//             VERBOSE_printf(END_OF_LINE); /* needed for test tool */
             VERBOSE_printf(RED_TEXT "Text: " WHITE_TEXT "%s", \
                           LwXMLP_pu8GetParsedText(pstrParserInstanceType));
#endif
             break;
        default: /* Unknown Event is fired */
             /*
              * The handler shall return error to the parser, to terminate the parsing process in this case, as if the
              * fired event isn't known for the application
              */
             srParserHandlerStatus = E_OK;
             break;
    }
    VERBOSE_printf(END_OF_LINE); /* needed for test tool */

    return srParserHandlerStatus;
}


/*
 *  ======== gpioButtonFxn ========
 *  Callback function for the GPIO interrupt on Board_BUTTON1.
 */
void gpioButtonFxn(unsigned int index)
{
    /* Begin smart config process */
    smartConfigFlag = true;
}
/*
 *  ======== tcpWorker ========
 *  Task to handle TCP connection. Can be multiple Tasks running
 *  this function.
 */
Void tcpWorker(UArg arg0, UArg arg1)
{
    INT32U u32FreeSpace;
    INT32S s32NumberOfReadyBytes = 0;
    INT32S s32SocketError;
    int  clientfd = (int)arg0;
    int                optval;
    int                optlen = sizeof(optval);

#if(ALLOCATION_TYPE == EXTERNAL_ALLOCATION)
    static LwXMLP_strXMLParseInstanceType *pstrParserInstanceType = &objParserExternalInstanceType;
#else
    static LwXMLP_strXMLParseInstanceType *pstrParserInstanceType;
#endif
    LwXMLP_enumParserStatusType objenumParserStatusType;
    StdReturnType srParserStatus = E_OK;
    enumParsingStatType objenumParsingStatType = csParseInit;
    INT32U u32LastSecondsTime, u32PasreProcessStartTime, u32ActualAddedLength;

    u32LastSecondsTime = SecondsClock_get(); /* startup delay */
    VERBOSE_printf("Init Parsing Procces");
    VERBOSE_printf(END_OF_LINE); /* needed for test tool */
    System_printf("\nInit Parsing Procces: %d", u32counter++);
    System_flush();
    if (setsockopt(clientfd, SOL_SOCKET, SO_NONBLOCKING, &optval, optlen) < 0)
    {
        VERBOSE_printf("Error: setsockopt failed\n");
    }

    while(objenumParsingStatType != csXMLparsingClosedState)
    {
        switch(objenumParsingStatType)
        {
            case csParseInit:
                 u32PasreProcessStartTime = SecondsClock_get();
#if(ALLOCATION_TYPE == EXTERNAL_ALLOCATION)
                 srParserStatus = LwXMLP_srInitParserExternal(pstrParserInstanceType, APP_srParserEventHandler);
#else
                 srParserStatus = LwXMLP_srInitParserInternal(&pstrParserInstanceType, APP_srParserEventHandler);
#endif
                 if(srParserStatus == E_OK)
                 {
                     objenumParsingStatType = csParseReceiveData;
                 }
                 else
                 {
                     objenumParsingStatType = csParsingCloseState;
                     VERBOSE_printf("Can't init the parser\n");
                     VERBOSE_printf(END_OF_LINE); /* needed for test tool */
                     System_printf("\nCan't init the parser\n");
                     System_flush();
                 }
                 break;
            case csParseReceiveData:
                 u32FreeSpace = LwXMLP_u32GetFreeSpace(pstrParserInstanceType);
                 if(u32FreeSpace > MAX_TCP_RECEIVE_BUFFER)
                 {
                     u32FreeSpace = MAX_TCP_RECEIVE_BUFFER;
                 }
                 s32NumberOfReadyBytes = recv(clientfd, \
                                              u8ReceiveBuffer, \
                                              u32FreeSpace, \
                                              0);
                 if(s32NumberOfReadyBytes > 0)
                 {
                     u32ActualAddedLength = LwXMLP_u32AddXMLPortion(pstrParserInstanceType, (void *)u8ReceiveBuffer, s32NumberOfReadyBytes);
                     if(u32ActualAddedLength > 0U)
                      {
                          u32LastSecondsTime = SecondsClock_get(); /* update the last receiption time */
                      }
                      else
                      {
                          /*
                           * Nothing to do
                           */
                      }
                   }
                 if((s32NumberOfReadyBytes < 0) && (s32NumberOfReadyBytes != SL_EAGAIN))
                 {
                     VERBOSE_printf("Connection Error: %d", s32SocketError);
                     VERBOSE_printf(END_OF_LINE); /* needed for test tool */
                     System_printf("\nConnection Error: %d", s32SocketError);
                     System_flush();
                     objenumParsingStatType = csParsingCloseState;
                 }
                 else
                 {
                     objenumParserStatusType = LwXMLP_enumXMLParserEngine(pstrParserInstanceType);
                     switch(objenumParserStatusType)
                     {
                          case csUnkownParserStatus:
                          case csParserError:
                               VERBOSE_printf("Parser Error: id = %d description = %s line number:%d coulmn:%d", \
                                              LwXMLP_enumGetError(pstrParserInstanceType),\
                                              LwXMLP_ps8GetErrorString(pstrParserInstanceType),\
                                              LwXMLP_u32GetLineOfError(pstrParserInstanceType),\
                                              LwXMLP_u32GetCoulmnOfError(pstrParserInstanceType));
                               VERBOSE_printf(END_OF_LINE); /* needed for test tool */
                               System_printf("\nParser Error: id = %d description = %s line number:%d coulmn:%d", \
                                              LwXMLP_enumGetError(pstrParserInstanceType),\
                                              LwXMLP_ps8GetErrorString(pstrParserInstanceType),\
                                              LwXMLP_u32GetLineOfError(pstrParserInstanceType),\
                                              LwXMLP_u32GetCoulmnOfError(pstrParserInstanceType));
                                System_flush();
                               objenumParsingStatType = csParsingCloseState;
                               break;
                          case csContinueAddingXMLData:
                               if((SecondsClock_get() - u32LastSecondsTime) > TIME_OUT_ON_CLIENT)
                               {
                                   VERBOSE_printf("Time out on the client");
                                   VERBOSE_printf(END_OF_LINE); /* needed for test tool */
                                   System_printf("\nTime out on the client");
                                   System_flush();
                                   objenumParsingStatType = csParsingCloseState;
                               }
                               break;
                          case csParsingFinished:
                               VERBOSE_printf("Reach end of XML Document");
                               VERBOSE_printf(END_OF_LINE); /* needed for test tool */
                               System_printf("\nReach end of XML Document");
                               System_flush();
                               objenumParsingStatType = csParsingCloseState;  /* will count on time out, to allow multiple successive XML document */
                               break;
                       }
                   }
                   break;
            case csParsingCloseState:
                 /* do any work before termination */
                 LwXMLP_srCloseParser(pstrParserInstanceType);
                 close(clientfd);
                 VERBOSE_printf("Parsing process Finished, taked %u seconds", SecondsClock_get() - u32PasreProcessStartTime);
                 VERBOSE_printf(END_OF_LINE); /* needed for test tool */
                 System_printf("\nParsing process Finished, taked %u seconds", SecondsClock_get() - u32PasreProcessStartTime);
                 System_flush();
                 objenumParsingStatType = csXMLparsingClosedState;
                 break;
        }
        Task_sleep(2);
    }
    Task_exit();
}

/*
 *  ======== echoFxn ========
 *  Echoes TCP messages.
 */
void echoFxn(int port)
{
    int         status;
    int         clientfd;
    int         server;
    sockaddr_in localAddr;
    sockaddr_in clientAddr;
    Task_Handle        taskHandle;
    Task_Params        taskParams;
    socklen_t   addrlen = sizeof(clientAddr);
    Error_Block        eb;

    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1)
    {
        VERBOSE_printf("Error: socket not created");
        VERBOSE_printf(END_OF_LINE); /* needed for test tool */
        goto shutdown;
    }

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(port);

    status = bind(server, (const sockaddr *)&localAddr, sizeof(localAddr));
    if (status == -1)
    {
        VERBOSE_printf("Error: bind failed");
        VERBOSE_printf(END_OF_LINE); /* needed for test tool */
        goto shutdown;
    }

    status = listen(server, 0);
    if (status == -1){
        VERBOSE_printf("Error: listen failed");
        VERBOSE_printf(END_OF_LINE); /* needed for test tool */
        goto shutdown;
    }
    struct SlTimeval_t timeVal;
    timeVal.tv_sec =  1;             // Seconds
    timeVal.tv_usec = 0;             // Microseconds. 10000 microseconds resolution
    sl_SetSockOpt(server,SL_SOL_SOCKET,SL_SO_RCVTIMEO, (_u8 *)&timeVal, sizeof(timeVal));    // Enable receive timeout


    while ((clientfd =
            accept(server, (sockaddr *)&clientAddr, &addrlen)) != -1)
    {

//        VERBOSE_printf("tcpHandler: Creating thread clientfd = %d", clientfd);
//        VERBOSE_printf(END_OF_LINE); /* needed for test tool */
        System_printf("\ntcpHandler: Creating thread clientfd = %d");
        System_flush();


        /* Init the Error_Block */
        Error_init(&eb);

        /* Initialize the defaults and set the parameters. */
        Task_Params_init(&taskParams);
        taskParams.arg0 = (UArg)clientfd;
        taskParams.stackSize = 2048;
        taskParams.priority = 5;
        taskHandle = Task_create((Task_FuncPtr)tcpWorker, &taskParams, &eb);
        if (taskHandle == NULL)
        {
            VERBOSE_printf("Error: Failed to create new Task");
            VERBOSE_printf(END_OF_LINE); /* needed for test tool */
            System_printf("Error: Failed to create new Task");
            System_flush();
            close(clientfd);
        }

        /* addrlen is a value-result param, must reset for next accept call */
        addrlen = sizeof(clientAddr);
    }

    VERBOSE_printf("Error: accept failed");
    VERBOSE_printf(END_OF_LINE); /* needed for test tool */

shutdown:
    if (server >= 0) {
        close(server);
    }
}

/*
 *  ======== tcpEchoTask ========
 */
Void tcpEchoTask(UArg arg0, UArg arg1)
{
    void *netIF;

    /* Open WiFi and await a connection */
    netIF = socketsStartUp();

    echoFxn(TCPPORT);

    /* Close the network - don't do this if other tasks are using it */
    socketsShutDown(netIF);
}

/*
 *  ======== main ========
 */
int main(void)
{
    /* Construct BIOS objects */
    Task_Params taskParams;

    /* Call board init functions. */
    Board_initGeneral();
    Board_initGPIO();
    Board_initWiFi();

    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    taskParams.priority = 1;
    Task_construct(&task0Struct, (Task_FuncPtr)tcpEchoTask, &taskParams, NULL);

    /* Install Button callback */
    GPIO_setCallback(Board_BUTTON1, gpioButtonFxn);

    /* Enable interrupts */
    GPIO_enableInt(Board_BUTTON1);

    /* Turn on user LED */
    GPIO_write(Board_LED0, Board_LED_ON);

    VERBOSE_vInit();


    VERBOSE_printf("Starting XML barser demo\n\r");

    /* Start BIOS */
    BIOS_start();

    return (0);
}
