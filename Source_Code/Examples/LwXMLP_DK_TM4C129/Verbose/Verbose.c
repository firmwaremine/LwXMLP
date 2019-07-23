/*
***********************************************************************************************************************
*                                               ATI Systems, Inc.
*
*                                     (c) Copyright 2013, ATI Systems, Inc.
*
* All rights reserved. Company Name's source code is an unpublished work and the include of a copyright notice does not
* imply otherwise. This source code contains confidential, trade secret material of ATI Systems, Inc. Any attempt or
* participation in deciphering, decoding, reverse engineering or in any way altering the source code is strictly
* prohibited, unless the prior written consent of Company Name is obtained.
*
* Filename         : VERBOSE.c
* Version          : 0.0.0
* Created          : 2014/03/24
* File Description : this is the core module of VERBOSE that will hold all the basic functionalities of the module
* Modifications    :
* Author: Mohamed Fawzy     Date:2014/03/24                  Rev:0                  Task ID:
* - Created File structure templates according to the General Requirement of Embedded Software Modules document
*   (ESWM SRS).
* Author:                   Date:                            Rev:                   Task ID:
* - list the modifications here
* - list the modifications here
* - list the modifications here
***********************************************************************************************************************
*
***********************************************************************************************************************
*/




/*
********************************************************************************************************************
************************************************************************************************************************
*                                                   # I N C L U D E S
************************************************************************************************************************
********************************************************************************************************************
*/
/*
 ==================================================================================================================
 *                                                  Environment Include
 ==================================================================================================================
 */
#include <xdc/runtime/Types.h>
#include <stdint.h>
#include <stdbool.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>

#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_gpio.h>

#include <driverlib/flash.h>
#include <driverlib/gpio.h>
#include <driverlib/i2c.h>
#include <driverlib/pin_map.h>
#include <driverlib/pwm.h>
#include <driverlib/ssi.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <driverlib/udma.h>

/*
 ==================================================================================================================
 *                                                  Application Include
 ==================================================================================================================
 */

#include "VERBOSE.h"

/*
********************************************************************************************************************
*                                                     INCLUDES
*******************************************************************************************************************/



/*
********************************************************************************************************************
************************************************************************************************************************
*                                                   # D E F I N E S
************************************************************************************************************************
********************************************************************************************************************
*/

/*
********************************************************************************************************************
************************************************************************************************************************
*                                                # L O C A L  M A C R O S
************************************************************************************************************************
********************************************************************************************************************
*/

/*
********************************************************************************************************************
************************************************************************************************************************
*                                             L O C A L  D A T A   T Y P E S
************************************************************************************************************************
********************************************************************************************************************
*/

/*
********************************************************************************************************************
************************************************************************************************************************
*                                              L O C A L   V A R I A B L E S
************************************************************************************************************************
********************************************************************************************************************
*/



/*
********************************************************************************************************************
************************************************************************************************************************
*                                                L O C A L   T A B L E S
************************************************************************************************************************
********************************************************************************************************************
*/



/*
********************************************************************************************************************
************************************************************************************************************************
*                                    L O C A L  F U N C T I O N   P R O T O T Y P E S
************************************************************************************************************************
********************************************************************************************************************
*/
static const char * const g_pcHex = "0123456789abcdef";
static INT32S VERBOSE_s32Write(const char *pcBuf, uint32_t ui32Len);
static void VERBOSE_vInnerprintf1(const char *pcString, va_list vaArgP);
static INT32U u32UARTBaseAddress = UART0_BASE;
/*
********************************************************************************************************************
************************************************************************************************************************
*                                              G L O B A L   V A R I A B L E S
************************************************************************************************************************
********************************************************************************************************************
*/
BOOLEAN VERBOSE_bSTarted = STD_FALSE;
/*
********************************************************************************************************************
************************************************************************************************************************
*                                             G L O B A L   F U N C T I O N S
************************************************************************************************************************
********************************************************************************************************************
*/
void VERBOSE_vInit()
{
    Types_FreqHz  strFrequencyHz;
     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
     if(u32UARTBaseAddress == UART0_BASE)
     {
         //
         // Check to make sure the UART peripheral is present.
         //
         if(!SysCtlPeripheralPresent(SYSCTL_PERIPH_UART0))
         {
             return;
         }

             //
         // Enable the UART peripheral for use.
         //
         SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
         GPIOPinConfigure(GPIO_PA0_U0RX);
         GPIOPinConfigure(GPIO_PA1_U0TX);
         GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
         /* get current processor frquency */
         BIOS_getCpuFreq(&strFrequencyHz);

         //
         // Configure the UART for 115200, n, 8, 1
         //
         UARTConfigSetExpClk(u32UARTBaseAddress, strFrequencyHz.lo, 115200,
                                 (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
                                  UART_CONFIG_WLEN_8));



         //
         // Enable the UART operation.
         //
         UARTEnable(u32UARTBaseAddress);
     }
     else
     {
         SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
         //
         // Check to make sure the UART peripheral is present.
         //
         if(!SysCtlPeripheralPresent(SYSCTL_PERIPH_UART3))
         {
             return;
         }

             //
         // Enable the UART peripheral for use.
         //
         SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);

         GPIOPinConfigure(GPIO_PJ1_U3TX);              /* Enable pin PJ1 for UART3 U3TX, TX of auxaliary RS232 */
         GPIOPinTypeUART(GPIO_PORTJ_BASE, GPIO_PIN_1);
         GPIOPinConfigure(GPIO_PJ4_U3RTS);             /* Enable pin PJ4 for UART3 U3RTS, RTS of auxaliary RS232 */
         GPIOPinTypeUART(GPIO_PORTJ_BASE, GPIO_PIN_4);
         GPIOPinConfigure(GPIO_PJ5_U3CTS);             /* Enable pin PJ5 for UART3 U3CTS, CTS of auxaliary RS232 */
         GPIOPinTypeUART(GPIO_PORTJ_BASE, GPIO_PIN_5);
         GPIOPinConfigure(GPIO_PJ0_U3RX);              /* Enable pin PJ0 for UART3 U3RX, RX of auxaliary RS232 */
         GPIOPinTypeUART(GPIO_PORTJ_BASE, GPIO_PIN_0);

         //
         // Configure the UART for 115200, n, 8, 1
         //
         /* get current processor frquency */
         BIOS_getCpuFreq(&strFrequencyHz);

         //
         // Configure the UART for 115200, n, 8, 1
         //
         UARTConfigSetExpClk(u32UARTBaseAddress, strFrequencyHz.lo, 115200,
                                 (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
                                  UART_CONFIG_WLEN_8));

         //
         // Enable the UART operation.
         //
         UARTEnable(u32UARTBaseAddress);
     }

    VERBOSE_bSTarted = STD_TRUE;
}
const INT8S s8StartUpString[] = "\n\r================================= Unit is starting=================================\n\r";
void
VERBOSE_printf(const char *pcString, ...)
{
    static BOOLEAN bFirstTimeRun = STD_TRUE;
    va_list vaArgP;

    if(bFirstTimeRun == STD_TRUE)
    {
        bFirstTimeRun = STD_FALSE;
        VERBOSE_s32Write(s8StartUpString, sizeof(s8StartUpString));
    }
    //
    // Start the varargs processing.
    //
    va_start(vaArgP, pcString);

    VERBOSE_vInnerprintf1(pcString, vaArgP);

    //
    // We're finished with the varargs now.
    //
    va_end(vaArgP);
}
static void VERBOSE_vInnerprintf1(const char *pcString, va_list vaArgP)
{
    uint32_t ui32Idx, ui32Value, ui32Pos, ui32Count, ui32Base, ui32Neg;
    char *pcStr, pcBuf[16], cFill;

    //
    // Check the arguments.
    //

 //   VERBOSE_s32Write("\n", 1);
    //
    // Loop while there are more characters in the string.
    //
    while(*pcString)
    {
        //
        // Find the first non-% character, or the end of the string.
        //
        for(ui32Idx = 0;
            (pcString[ui32Idx] != '%') && (pcString[ui32Idx] != '\0');
            ui32Idx++)
        {
        }

        //
        // Write this portion of the string.
        //
        VERBOSE_s32Write(pcString, ui32Idx);

        //
        // Skip the portion of the string that was written.
        //
        pcString += ui32Idx;

        //
        // See if the next character is a %.
        //
        if(*pcString == '%')
        {
            //
            // Skip the %.
            //
            pcString++;

            //
            // Set the digit count to zero, and the fill character to space
            // (in other words, to the defaults).
            //
            ui32Count = 0;
            cFill = ' ';

            //
            // It may be necessary to get back here to process more characters.
            // Goto's aren't pretty, but effective.  I feel extremely dirty for
            // using not one but two of the beasts.
            //
again:

            //
            // Determine how to handle the next character.
            //
            switch(*pcString++)
            {
                //
                // Handle the digit characters.
                //
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                {
                    //
                    // If this is a zero, and it is the first digit, then the
                    // fill character is a zero instead of a space.
                    //
                    if((pcString[-1] == '0') && (ui32Count == 0))
                    {
                        cFill = '0';
                    }

                    //
                    // Update the digit count.
                    //
                    ui32Count *= 10;
                    ui32Count += pcString[-1] - '0';

                    //
                    // Get the next character.
                    //
                    goto again;
                }

                //
                // Handle the %c command.
                //
                case 'c':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ui32Value = va_arg(vaArgP, uint32_t);

                    //
                    // Print out the character.
                    //
                    VERBOSE_s32Write((char *)&ui32Value, 1);

                    //
                    // This command has been handled.
                    //
                    break;
                }

                //
                // Handle the %d and %i commands.
                //
                case 'd':
                case 'i':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ui32Value = va_arg(vaArgP, uint32_t);

                    //
                    // Reset the buffer position.
                    //
                    ui32Pos = 0;

                    //
                    // If the value is negative, make it positive and indicate
                    // that a minus sign is needed.
                    //
                    if((int32_t)ui32Value < 0)
                    {
                        //
                        // Make the value positive.
                        //
                        ui32Value = -(int32_t)ui32Value;

                        //
                        // Indicate that the value is negative.
                        //
                        ui32Neg = 1;
                    }
                    else
                    {
                        //
                        // Indicate that the value is positive so that a minus
                        // sign isn't inserted.
                        //
                        ui32Neg = 0;
                    }

                    //
                    // Set the base to 10.
                    //
                    ui32Base = 10;

                    //
                    // Convert the value to ASCII.
                    //
                    goto convert;
                }

                //
                // Handle the %s command.
                //
                case 's':
                {
                    //
                    // Get the string pointer from the varargs.
                    //
                    pcStr = va_arg(vaArgP, char *);

                    //
                    // Determine the length of the string.
                    //
                    for(ui32Idx = 0; pcStr[ui32Idx] != '\0'; ui32Idx++)
                    {
                    }

                    //
                    // Write the string.
                    //
                    VERBOSE_s32Write(pcStr, ui32Idx);

                    //
                    // Write any required padding spaces
                    //
                    if(ui32Count > ui32Idx)
                    {
                        ui32Count -= ui32Idx;
                        while(ui32Count--)
                        {
                            VERBOSE_s32Write(" ", 1);
                        }
                    }

                    //
                    // This command has been handled.
                    //
                    break;
                }

                //
                // Handle the %u command.
                //
                case 'u':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ui32Value = va_arg(vaArgP, uint32_t);

                    //
                    // Reset the buffer position.
                    //
                    ui32Pos = 0;

                    //
                    // Set the base to 10.
                    //
                    ui32Base = 10;

                    //
                    // Indicate that the value is positive so that a minus sign
                    // isn't inserted.
                    //
                    ui32Neg = 0;

                    //
                    // Convert the value to ASCII.
                    //
                    goto convert;
                }

                //
                // Handle the %x and %X commands.  Note that they are treated
                // identically; in other words, %X will use lower case letters
                // for a-f instead of the upper case letters it should use.  We
                // also alias %p to %x.
                //
                case 'x':
                case 'X':
                case 'p':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ui32Value = va_arg(vaArgP, uint32_t);

                    //
                    // Reset the buffer position.
                    //
                    ui32Pos = 0;

                    //
                    // Set the base to 16.
                    //
                    ui32Base = 16;

                    //
                    // Indicate that the value is positive so that a minus sign
                    // isn't inserted.
                    //
                    ui32Neg = 0;

                    //
                    // Determine the number of digits in the string version of
                    // the value.
                    //
convert:
                    for(ui32Idx = 1;
                        (((ui32Idx * ui32Base) <= ui32Value) &&
                         (((ui32Idx * ui32Base) / ui32Base) == ui32Idx));
                        ui32Idx *= ui32Base, ui32Count--)
                    {
                    }

                    //
                    // If the value is negative, reduce the count of padding
                    // characters needed.
                    //
                    if(ui32Neg)
                    {
                        ui32Count--;
                    }

                    //
                    // If the value is negative and the value is padded with
                    // zeros, then place the minus sign before the padding.
                    //
                    if(ui32Neg && (cFill == '0'))
                    {
                        //
                        // Place the minus sign in the output buffer.
                        //
                        pcBuf[ui32Pos++] = '-';

                        //
                        // The minus sign has been placed, so turn off the
                        // negative flag.
                        //
                        ui32Neg = 0;
                    }

                    //
                    // Provide additional padding at the beginning of the
                    // string conversion if needed.
                    //
                    if((ui32Count > 1) && (ui32Count < 16))
                    {
                        for(ui32Count--; ui32Count; ui32Count--)
                        {
                            pcBuf[ui32Pos++] = cFill;
                        }
                    }

                    //
                    // If the value is negative, then place the minus sign
                    // before the number.
                    //
                    if(ui32Neg)
                    {
                        //
                        // Place the minus sign in the output buffer.
                        //
                        pcBuf[ui32Pos++] = '-';
                    }

                    //
                    // Convert the value into a string.
                    //
                    for(; ui32Idx; ui32Idx /= ui32Base)
                    {
                        pcBuf[ui32Pos++] =
                            g_pcHex[(ui32Value / ui32Idx) % ui32Base];
                    }

                    //
                    // Write the string.
                    //
                    VERBOSE_s32Write(pcBuf, ui32Pos);

                    //
                    // This command has been handled.
                    //
                    break;
                }

                //
                // Handle the %% command.
                //
                case '%':
                {
                    //
                    // Simply write a single %.
                    //
                    VERBOSE_s32Write(pcString - 1, 1);

                    //
                    // This command has been handled.
                    //
                    break;
                }

                //
                // Handle all other commands.
                //
                default:
                {
                    //
                    // Indicate an error.
                    //
                    VERBOSE_s32Write("ERROR", 5);

                    //
                    // This command has been handled.
                    //
                    break;
                }
            }
        }
    }
}


static INT32S VERBOSE_s32Write(const char *pcBuf, uint32_t ui32Len)
{
    unsigned int uIdx;

    //
    // Check for valid UART base address, and valid arguments.
    //



    //
    // Send the characters
    //
    for(uIdx = 0; uIdx < ui32Len; uIdx++)
    {
#if 0
        //
        // If the character to the UART is \n, then add a \r before it so that
        // \n is translated to \n\r in the output.
        //
        if(pcBuf[uIdx] == '\n')
        {
            UARTCharPut(UART0_BASE, '\r');
        }
#endif
        //
        // Send the character to the UART output.
        //
        UARTCharPut(u32UARTBaseAddress, pcBuf[uIdx]);
    }

    //
    // Return the number of characters written.
    //
    return(uIdx);

}



/*
********************************************************************************************************************
************************************************************************************************************************
*                                               L O C A L   F U N C T I O N S
************************************************************************************************************************
********************************************************************************************************************
*/

/*
********************************************************************************************************************
*                                                      MODULE END
********************************************************************************************************************
*/




