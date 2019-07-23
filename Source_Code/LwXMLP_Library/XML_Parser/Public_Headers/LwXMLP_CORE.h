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
 * @file LwXMLP_CORE.h
 * @brief This header file contain all the global definitions for the LwXMLP_CORE
 * @author Mohammed Fawzy
 * @version 1.0.0
 **********************************************************************************************************************/


#ifndef  LwXMLP_CORE_H
#define  LwXMLP_CORE_H


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
#include "STD_TYPE.h"
#include "VERBOSE.h"
/*
 ==================================================================================================================
 *                                                  Application Include
 ==================================================================================================================
 */
#include "LwXMLP_FCFG.h"
#include "LwXMLP_TYPES.h"
/* the privacy of the LwXMLP instance will be broken, if the insatance is allocated externally */
#if(ALLOCATION_TYPE == EXTERNAL_ALLOCATION)
    #include "LwXMLP_PRIVATE_TYPES.h"
#endif

/*
********************************************************************************************************************
************************************************************************************************************************
*                                                   # D E F I N E S
************************************************************************************************************************
********************************************************************************************************************
*/
/*
 ==================================================================================================================
 *                                                  VERSION NUMBER
 ==================================================================================================================
 */
#define LwXMLP_MODULE_MAJOR                0x01U            /*!< The LwXMLP module Major version number*/
#define LwXMLP_MODULE_MINOR                0x00U            /*!< The LwXMLP module Minor version number*/
#define LwXMLP_MODULE_CUSTOM               0x00U            /*!< The LwXMLP module Custom version number*/

/*
********************************************************************************************************************
************************************************************************************************************************
*                                              # G L O B A L  M A C R O S
************************************************************************************************************************
********************************************************************************************************************
*/
/*!\fn         LwXMLP_mVerbose(REQUIRED_LEVEL,FORMAT, ...)

@brief         This Macro is used to send the log event to the available standard output

@param[in]     REQUIRED_LEVEL    : the level of importance of the log event

@param[in]     FORMAT            : the string that describe the event

@return        The free amount in the circular buffer

@note
*/
#if(LwXMLP_VERBOSE_ENABLE == STD_ON)
/*lint -estring(9026,LwXMLP_mVerbose)*/
    #define LwXMLP_mVerbose(REQUIRED_LEVEL,FORMAT, ...)  \
            mVerbose(LwXMLP_VERBOSE_MIN_LEVEL, REQUIRED_LEVEL, FORMAT, __VA_ARGS__)
#else
/*lint -estring(9026,LwXMLP_mVerbose)*/
    #define LwXMLP_mVerbose(REQUIRED_LEVEL,FORMAT, ...)
#endif
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
#ifdef __cplusplus
extern "C" {
#endif

/* Put the functions here */
#if((ALLOCATION_TYPE == INTERNAL_STATIC_ALLOCATION) || (ALLOCATION_TYPE == INTERNAL_DYNAMIC_ALLOCATION))
    StdReturnType            LwXMLP_srInitParserInternal(LwXMLP_strXMLParseInstanceType  **pstrParserInstanceType,\
                                                         LwXLMP_psrFuncCallBackType        objpsrFuncCallBackType);
#elif(ALLOCATION_TYPE == EXTERNAL_ALLOCATION)
    StdReturnType  LwXMLP_srInitParserExternal(LwXMLP_strXMLParseInstanceType  *pstrParserInstanceType,\
                                               LwXLMP_psrFuncCallBackType       objpsrFuncCallBackType);
#endif
INT8U                       *LwXMLP_pu8GetParsedComment(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
INT8U                       *LwXMLP_pu8GetParsedPIData(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
INT8U                       *LwXMLP_pu8GetParsedPITarget(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
INT8U                       *LwXMLP_pu8GetParsedText(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
INT8U                       *LwXMLP_pu8GetParsedNotation(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
INT8U                       *LwXMLP_pu8GetElementName(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
INT8U             const     *LwXMLP_pu8GetCurrentPath(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
INT8U                       *LwXMLP_pu8GetAttributeValue(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType, \
                                                         INT16S                                s16AttributeIndex);
INT8U                       *LwXMLP_pu8GetAttributeName(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType, \
                                                        INT16S                                s16AttributeIndex);
INT8S              const    *LwXMLP_ps8GetErrorString(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
StdReturnType                LwXMLP_srCloseParser(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
INT16S                       LwXMLP_s16GetAttributeCount(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
INT32U                       LwXMLP_u32GetLineOfError(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
INT32U                       LwXMLP_u32GetCoulmnOfError(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
INT32U                       LwXMLP_u32GetFreeSpace(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
INT32U                       LwXMLP_u32AddXMLPortion(      LwXMLP_strXMLParseInstanceType *pstrParserInstanceType,\
                                                     const INT8U                          *pu8XMLPortionBuffer, \
                                                           INT32U                          u32RequiredAddPortionSize);
LwXMLP_enumXMLEventTypes     LwXMLP_enumGetXMLEventType(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
LwXMLP_enumParserStatusType  LwXMLP_enumXMLParserEngine(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
LwXMLP_enumErrorType         LwXMLP_enumGetError(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
#ifdef __cplusplus
}
#endif


/*
********************************************************************************************************************
*                                                     MODULE END
********************************************************************************************************************
*/
#endif /* end #ifndef  TEST_OS_H */



