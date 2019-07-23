/*
***********************************************************************************************************************
*                                               Embedded Mine
*
*                                     (c) Copyright 2017, Embedded Mine
*
* All rights reserved. Embedded Mine's source code is an unpublished work and the include of a copyright notice does not
* imply otherwise. This source code contains confidential, trade secret material of Embedded Mine Any attempt or
* participation in deciphering, decoding, reverse engineering or in any way altering the source code is strictly
* prohibited, unless the prior written consent of Embedded Mine is obtained.
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



