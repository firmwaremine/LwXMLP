/*
***********************************************************************************************************************
*                                               Firmware Mine
*
*                                     (c) Copyright 2013-2018, Firmware Mine.
*
* All rights reserved. This program is open source software: you can redistribute it and/or modify it under the terms
* of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the terms of Firmware Mine commercial licenses,
* which expressly supersede the GNU General Public License and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with this program. If not,
* see "www.gnu.org/licenses/".
*
* Contact information:
* firmwaremine.com
* mailto:support@firmwaremine.com
***********************************************************************************************************************/
/**
 * @file STD_TYPE.h
 * @brief This file contains the redefinition for the standard types
 * @author Mohammed Fawzy
 * @version 1.0.0
 **********************************************************************************************************************/
#ifndef  STD_TYPE_H
#define  STD_TYPE_H



/*
********************************************************************************************************************
*                                                     INCLUDES
********************************************************************************************************************
*/


/*
********************************************************************************************************************
************************************************************************************************************************
*                                              # G L O B A L  M A C R O S
************************************************************************************************************************
********************************************************************************************************************
*/

/*
********************************************************************************************************************
************************************************************************************************************************
*                                           G L O B A L  D A T A   T Y P E S
************************************************************************************************************************
********************************************************************************************************************
*/
typedef unsigned char        INT8U;                    /*!< Unsigned  8 bit quantity                           */
typedef          char        INT8S;                    /*!< Signed    8 bit quantity                           */
typedef unsigned short       INT16U;                   /*!< Unsigned 16 bit quantity                           */
typedef signed   short       INT16S;                   /*!< Signed   16 bit quantity                           */
typedef unsigned int         INT32U;                   /*!< Unsigned 32 bit quantity                           */
typedef signed   int         INT32S;                   /*!< Signed   32 bit quantity                           */
typedef unsigned long long   INT64U;                   /*!< Unsigned 64 bit quantity                           */
typedef signed   long long   INT64S;                   /*!< Signed   64 bit quantity                           */
typedef float                FP32;                     /*!< Single precision floating point                    */
typedef double               FP64;                     /*!< Double precision floating point                    */
typedef unsigned char        BOOLEAN;                  /*!< Unsigned  1 bit quantity                           */

typedef INT8U StdReturnType;                           /*!< Standard return which may be E_OK or E_NOT_OK      */
/*! \struct STD_strVersionInfoType
    \brief it the structure which capsulate the firmware version information
*/
typedef struct
{
    INT8U  u8MajorVersion;                             /*!< The firmware module Major version number             */
    INT8U  u8MinorVersion;                             /*!< The firmware module Minor version number             */
    INT8U  u8CustomVersion;                            /*!< The firmware module Custom version number            */
} STD_strVersionInfoType;
/*! \enum STD_enumStateType
    \brief define the state of the firmware module
*/
typedef enum
{
    STD_csUnInitialized,                              /*!< The firmware module is not intialized                */
    STD_csInitialized                                 /*!< The firmware module is intialized                    */
}STD_enumStateType;



/*
********************************************************************************************************************
************************************************************************************************************************
*                                           G L O B A L   V A R I A B L E S
************************************************************************************************************************
********************************************************************************************************************
*/



/*
********************************************************************************************************************
************************************************************************************************************************
*                                                   E X T E R N A L S
************************************************************************************************************************
********************************************************************************************************************
*/


/*
********************************************************************************************************************
************************************************************************************************************************
*                                    G L O B A L  F U N C T I O N   P R O T O T Y P E S
************************************************************************************************************************
********************************************************************************************************************
*/


/*
********************************************************************************************************************
************************************************************************************************************************
*                                                   # D E F I N E S
************************************************************************************************************************
********************************************************************************************************************
*/
#define STD_HIGH                        0x01U           /*!< Standard High which shall be given to indicate the physical
                                                             high state of the signal*/
#define STD_LOW                         0x00U           /*!< Standard low which shall be given to indicate the physical
                                                             low state of the signal*/
#define STD_ON                          0x01U           /*!< Standard ON state of an object  */
#define STD_OFF                         0x00U           /*!< Standard OFF state of an object  */
#define STD_TRUE                        ((BOOLEAN)0x01) /*!< Standard logic TRUE  */
#define STD_FALSE                       ((BOOLEAN)0x00) /*!< Standard logic FALSE  */
#define STD_NULL                        ((void*)0)      /*!< Standard NULL pointer  */
/*
********************************************************************************************************************
* DEFINE VERBOSE LEVEL IN THE MODULE
********************************************************************************************************************

*/

/*
 * Standard return type
 */
#define E_OK                           ((StdReturnType)0x00)/*!< Standard return which indicate the propore execuation
                                                                 for the function*/
#define E_NOT_OK                       ((StdReturnType)0x01)/*!< Standard return which indicate the inpropore execuation
                                                                 for the function*/





/*
********************************************************************************************************************
*                                                     MODULE END
********************************************************************************************************************
*/

#endif /* end #ifndef  STD_TYPE_H */



