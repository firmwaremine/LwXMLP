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
 * @file LwXMLP_CORE.C
 * @brief This is a source code file for the LwXMLP module which contain all the core API.
 * @author Mohammed Fawzy
 * @version 1.0.0
 **********************************************************************************************************************/
/*! \mainpage Light Weight XML Parser (LwXMLP) Library
 *
 * \section intro_sec Introduction
 *
 *  - This is the help for the LwXMLP library which describe all the interfaces to the library.
 *
 * \section feature_sec Features
 *
 *  - It is a non-validating XML processor.
 *  - It is C language based library.
 *  - The library is made for the embedded systems which has a limited resource in the code/data area.
 *  - It follows all the requirements provided by the W3C (check the LwXMLP test tool and W3C compliance matrix).
 *  - It targets the embedded system area with many limitations on the code, data and processing speed.
 *  - It is online streaming processor (incremental processor) such that it can generate correct partial output from
 *    partial input and this gives a degree of freedom on the XML document size, i.e. there is no restriction on the
 *    XML document size.
 *  - It is SAX or event-based processor. Such that it reads an XML document from beginning to end, and each time it
 *    encounters a syntax construction, it notifies the application that is running it, and the application must
 *    implement the appropriate methods to handle the callbacks and get the functionality needed.
 *  - It can receive the XML document from different stream type like file stream and IP stream (TCP/UDP),
 *    serial communication, so on.
 *  - The code/data size can be customize to enable/disable some of the feature on the expense of the code/data size
 *    using some macro definitions.
 *  - Thread-safe (more precisely: reentrant).
 *  - Support two models of memory allocation, static memory allocation and dynamic memory allocation.
 *  - The LWXML processor shall be able to detect the xml fetal errors which will cause the reporting for the error
 *    type and the XML line where the error is occurred then it will cause the termination for reading process.
 *  - Supports only for encoding types UTF-8, UTF-16BE and UTF-16LE.
 *  - It complies with MISRA-2012 (Check Compliance matrix).
 * \section limitation_sec limitation
 *  - No support for XML namespaces.
 *  - The interface of the parser does not conform to DOM specification.
 *  - Can not decide if the document as a whole is valid or well-formed but it is validating in a portion bases.
 *  - Can not generate an error for empty (no root) XML document due to the LwXMLP mechanism which parse the document partially not as a whole.
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
 *                                                  Application Include
 ==================================================================================================================
 */
#include "LwXMLP_CORE.h"
#ifndef  LwXMLP_ENCODING_H
    #include "LwXMLP_ENCODING.h"
#endif
#include <string.h>
#include <ctype.h>
#if((ALLOCATION_TYPE == INTERNAL_STATIC_ALLOCATION) || (ALLOCATION_TYPE == INTERNAL_DYNAMIC_ALLOCATION))
    #include "LwXMLP_PRIVATE_TYPES.h"
#endif
#if(ALLOCATION_TYPE == INTERNAL_DYNAMIC_ALLOCATION)
    #include <stdlib.h>
#endif

/*
********************************************************************************************************************
************************************************************************************************************************
*                                                   # D E F I N E S
************************************************************************************************************************
********************************************************************************************************************
*/
/*
 * the HEX value for some of the ASCII character
 */
#define NULL_ASCII_CHARACTER                 0U            /*!< The code of the ASCII character NULL */
#define HTAB_ASCII_CHARACTER                 9U            /*!< The code of the ASCII character Horizontal Tab */
#define LINE_FEED_ASCII_CHARACTER            10U           /*!< The code of the ASCII character LINE FEED */
#define CARRIAGE_RETURN_ASCII_CHARACTER      13U           /*!< The code of the ASCII character Carriage return */
#define SPACE_ASCII_CHARACTER                32U           /*!< The code of the ASCII character SPACE */
#define EXCLAM_MARK_ASCII_CHARACTER          33U           /*!< The code of the ASCII character ! */
#define DOUBLE_QUOT_ASCII_CHARACTER          34U           /*!< The code of the ASCII character " */
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    #define HASH_ASCII_CHARACTER                 35U           /*!< The code of the ASCII character # */
    #define DOLLAR_SIGN_ASCII_CHARACTER          36U           /*!< The code of the ASCII character $ */
    #define PERCENT_ASCII_CHARACTER              37U           /*!< The code of the ASCII character % */
#endif
#define AMPERSAND_ASCII_CHARACTER            38U           /*!< The code of the ASCII character & */
#define SINGLE_QUOT_ASCII_CHARACTER          39U           /*!< The code of the ASCII character ' */
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    #define OPEN_ROUND_BRACKET_ASCII_CHARACTER   40U           /*!< The code of the ASCII character ( */
    #define CLOSE_ROUND_BRACKET_ASCII_CHARACTER  41U           /*!< The code of the ASCII character ) */
    #define ASTERISK_ASCII_CHARACTER             42U           /*!< The code of the ASCII character * */
    #define PLUS_ASCII_CHARACTER                 43U           /*!< The code of the ASCII character + */
    #define COMMA_ASCII_CHARACTER                44U           /*!< The code of the ASCII character , */
#endif
#define HYPHEN_ASCII_CHARACTER               45U           /*!< The code of the ASCII character - */
#define DOT_ASCII_CHARACTER                  46U           /*!< The code of the ASCII character . */
#define SLASH_ASCII_CHARACTER                47U           /*!< The code of the ASCII character / */
#define ZERO_ASCII_CHARACTER                 48U           /*!< The code of the ASCII character 0 */
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    #define NINE_ASCII_CHARACTER                 57U           /*!< The code of the ASCII character 9 */
#endif
#define COLON_ASCII_CHARACTER                58U           /*!< The code of the ASCII character : */
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    #define SEMICOLON_ASCII_CHARACTER            59U           /*!< The code of the ASCII character ; */
#endif
#define LESS_THAN_ASCII_CHARACTER            60U           /*!< The code of the ASCII character < */
#define EQUAL_ASCII_CHARACTER                61U           /*!< The code of the ASCII character = */
#define GREATER_THAN_ASCII_CHARACTER         62U           /*!< The code of the ASCII character > */
#define QUESTION_MARK_ASCII_CHARACTER        63U           /*!< The code of the ASCII character ? */
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    #define AT_SYMBOLE_ASCII_CHARACTER           64U           /*!< The code of the ASCII character @ */
    #define A_CAPITAL_ASCII_CHARACTER            65U           /*!< The code of the ASCII character A */
    #define F_CAPITAL_ASCII_CHARACTER            70U           /*!< The code of the ASCII character F */
#endif
#if(ENABLE_DTD_RECEIPTION == STD_ON)
    #define OPENING_BRACKET_ASCII_CHARACTER      91U           /*!< The code of the ASCII character [ */
#endif
#define BACK_SLASH_ASCII_CHARACTER           92U           /*!< The code of the ASCII character \ */
#define CLOSING_BRACKET_CHARACTER            93U           /*!< The code of the ASCII character ] */
#define UNDERSCORE_ASCII_CHARACTER           95U           /*!< The code of the ASCII character _ */
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    #define A_SMALL_ASCII_CHARACTER              97U           /*!< The code of the ASCII character a */
    #define F_SMALL_ASCII_CHARACTER              102U          /*!< The code of the ASCII character f */
    #define X_SMALL_ASCII_CHARACTER              120U          /*!< The code of the ASCII character x */
    #define OR_ASCII_CHARACTER                   124U          /*!< The code of the ASCII character | */
#endif
/*
 * update entity option
 */
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    #define UPDATE_ENTITY_REFERENCE              0x0001U  /*!< Enable the update for entity references during resolving */
    #define UPDATE_CHARACTER_REFERENCE           0x0002U  /*!< Enable the update for character references during resolving */
    #define UPDATE_PARAMETER_REFERENCE           0x0004U  /*!< Enable the update for parameter references during resolving */
    #define UPDATE_DECIMAL_CHAR_REFERENCE        0x0008U  /*!< Enable the update for decimal character references during resolving */
    #define UPDATE_HEX_CHARACTER_REFERENCE       0x0010U  /*!< Enable the update for hex character references during resolving */
    #define REFERENCE_VALIDATEION_ONLY           0x0020U  /*!< Validate the entity only without resolving*/
    #define DONT_DEREF_LT_GENERAL_ENTITY         0x0200U  /*!< Don't resolve less than general entity */
    #define DONT_DEREF_AS_GENERAL_ENTITY         0x0400U  /*!< Don't resolve ampersand general entity */
#endif
/*
 * DTD portion mask enable feature
 */
#if(ENABLE_DTD_WELL_FORM_CHECK == STD_ON)
    #define NO_ROW_DEFINED                  0x0000U        /*!< No parsing options is enabled inside the DTD portion */
    #define PI_ROW                          0x0001U        /*!< Option to enable parsing for PI inside the DTD portion */
    #define COMMENT_ROW                     0x0002U        /*!< Option to enable parsing for comment inside the DTD portion*/
    #define PE_REFERENCE_ROW                0x0004U        /*!< Option to enable parsing for PE reference inside the DTD portion*/
    #define PUBLIC_EXTERNAL_ID_ROW          0x0008U        /*!< Option to enable parsing for PUBLIC external ID inside the DTD portion*/
    #define SYSTEM_EXTERNAL_ROW             0x0010U        /*!< Option to enable parsing for SYSTEM external ID inside the DTD portion*/
    #define OPENING_BRACKET_PORTION         0x0020U        /*!< Option to enable parsing for Opening bracket content inside the DTD portion*/
#endif
/*
 * Miscellaneous
 */
#define MAXIMUM_DIRECTIVE_LENTGH           11U                    /*!< The Maximum length of the Directive including the
                                                                       white space, so the XML shouldn't contain a
                                                                       directive with name larger than this*/
#if(ENABLE_ENCODING == STD_ON)
    #define MAX_ENCODING_LENGTH                4U                     /*!< Max number of bytes per characters per any
                                                                            supported encoding type */
#endif
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

#if(ALLOCATION_TYPE == INTERNAL_STATIC_ALLOCATION)
/*! \struct strStaticInstancesType
    \brief it hold the parser instance that may be allocated by the parser if it was enabled
    @var strStaticInstancesType::objstrXMLParseInstanceType
    Member 'objstrXMLParseInstanceType' the allocated pasrer instance
    @var strStaticInstancesType::bReserevedInstance
    Member 'bReserevedInstance' boolean indication if the instance is in use or not
*/
    typedef struct
    {
        LwXMLP_strXMLParseInstanceType objstrXMLParseInstanceType;
        BOOLEAN                        bReserevedInstance;
    }strStaticInstancesType;
#endif


/*
********************************************************************************************************************
************************************************************************************************************************
*                                              L O C A L   V A R I A B L E S
************************************************************************************************************************
********************************************************************************************************************
*/
#if(ALLOCATION_TYPE == INTERNAL_STATIC_ALLOCATION)
    static strStaticInstancesType objstrStaticInstancesArray[TOTAL_STATIC_XML_INSTANCES];
#endif

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
static BOOLEAN       LwXMLP_bValidateData(INT8U const *pu8DataString, INT32U u32dataLength);
static BOOLEAN       LwXMLP_bXMLPathIsEmpty(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
static BOOLEAN       LwXMLP_bIsWhiteSpace(INT8U const *pu8Character);
static BOOLEAN       LwXMLP_bIsLessThanCharacter(INT8U const *pu8Character);
static BOOLEAN       LwXMLP_bMoreCharacterAtEndOfXML(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
static BOOLEAN       LwXMLP_bNewAttribute(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType, \
                                                 INT8U                          const *pu8NewAttributeString);
static INT8U         LwXMLP_u8ValidCharacter(INT8U const *pu8Character);
static INT8U         LwXMLP_u8ValidNameStartCharacter(INT8U const *pu8Character);
static INT8U         LwXMLP_u8ValidNameCharacter(INT8U const *pu8Character);
static INT8U        *LwXMLP_pu8GetCurrentCharacter(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);
static INT8U         LwXMLP_u8CheckBOM(      LwXMLP_strXMLParseInstanceType *pstrParserInstanceType,\
                                              const INT8U                          *pu8XMLPortion, \
                                                    INT32U                          u32XMLPortionSize);

static INT32U        LwXMLP_u32GetReadyData(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType);

#if(ENABLE_ATTRIBUTE_Normalization == STD_ON)
    static void LwXMLP_vMoveStringBlockLeft(INT8U *pu8StringBlockAddress, INT32U u32MoveLength);
    static void          LwXMLP_vNormalizeAttributes(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
#endif
#if((ENABLE_ENTITY_SUPPORT == STD_ON) && (ENABLE_ATTRIBUTE_Normalization == STD_ON))
    static LwXMLP_enumErrorType LwXMLP_enumValidateAttributeValue(INT8U const *pu8AttributeValue);
#endif
static void          LwXMLP_vConsumeCharacter(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
static void          LwXMLP_vConsumeCharacterBulk(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType, \
                                                         INT32U                        u32AdvancedLength);
static void LwXMLP_vCheckDeclerationEncoding(LwXMLP_strXMLParseInstanceType       *pstrParserInstanceType, \
                                             INT8U                          const *pu8MLPortionBuffer,\
                                             INT32U                                 u32PortionLengthLength);
static void          LwXMLP_vCopy(INT8U *pu8Destination, INT8U const *pu8Source, INT32U u32Length);
static void          LwXMLP_vCheckavailableData(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
static void          LwXMLP_vCheckTagForDirective(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
static void          LwXMLP_vUpdatePathWithStartTag(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
static void          LwXMLP_vRemoveTagfromPath(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
static void          LwXMLP_vLoadDirectiveToBuffer(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    static LwXMLP_enumErrorType LwXMLP_enumUpdateEntityReference(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType, \
                                                                        INT8U                          *pu8BufferString,\
                                                                        INT32U                          u32MaxBlockLength,\
                                                                        INT32U                          u32UpdateOptionMask);
    static BOOLEAN LwXMLP_bValideEntity(INT8U* pu8Entity);
    static INT8U         LwXMLP_u8ConvertCodePointToUTF8(INT32U u32CodePoint, INT8U *pu8UTF8Code);
    static void          LwXMLP_vMoveBlockRight(INT8U  *pu8StringBlockAddress, \
                                                       INT32U  u32MoveLength, \
                                                       INT32U  u32BlockLength);
    static void          LwXMLP_vMoveBlockLeft(INT8U* pu8StringBlockAddress, \
                                                      INT32U u32MoveLength, \
                                                      INT32U u32BlockLength);
    static void LwXMLP_vSwitchToSecondLevelBuffer(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType, INT8U const *pu8PortionBuffer, INT32U u32PortionLength);
#endif
static void LwXMLP_vLoadTagToBuffer(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
static void LwXMLP_vProcesssTag(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
static void LwXMLP_vCheckTagCloserAndType(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
static StdReturnType LwXMLP_srInitParserObject(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType,\
                                                      LwXLMP_psrFuncCallBackType      objpsrFuncCallBackType);
static void LwXMLP_vCheckWellFormedDecl(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
#if(ENABLE_PI_WELL_FORM_CHECK == STD_ON)
    static void LwXMLP_vParsePI(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
#endif
#if(ENABLE_COMMENT_WELL_FORM_CHECK == STD_ON)
    static void LwXMLP_vCheckWellFormedComment(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
#endif
#if(ENABLE_CDATA_WELL_FORM_CHECK == STD_ON)
    static void LwXMLP_vCheckWellFormedCDATA(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
#endif
static BOOLEAN LwXMLP_bReachEndOfDirective(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
#if(ENABLE_EOL_NORMALIZATION == STD_ON)
    static void LwXMLP_vNormalizeEndOfLine(INT8U *pu8StringBuffer);
#endif

#if(ENABLE_DTD_RECEIPTION == STD_ON)
    static void LwXMLP_vCheckDTDPortion(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
#endif
#if(ALLOCATION_TYPE == INTERNAL_STATIC_ALLOCATION)
    static LwXMLP_strXMLParseInstanceType *LwXMLP_pstrGetStaticInstance(void);
    static StdReturnType                   LwXMLP_srReleaseStaticInstance(LwXMLP_strXMLParseInstanceType  const *pstrParserInstanceType);
#endif
#if(ENABLE_DTD_WELL_FORM_CHECK == STD_ON)
     static void                 LwXMLP_vValidateDTD(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
            static LwXMLP_enumErrorType LwXMLP_enumValidateName(INT8U *pu8Name, INT8U **pu8StartOfName, INT32U *pu32NameLength);
            static LwXMLP_enumErrorType LwXMLP_enumValidateExternalID(INT8U   *pu8Name,
                                                                     INT32U  *pu32PublicIDLength,
                                                                     BOOLEAN  bPublicLiteralDiffrentLength);
           static LwXMLP_enumErrorType LwXMLP_enumValidateEnumeration(INT8U *pu8Enumeration, INT32U *pu32EnumerationLength);
           static void LwXMLP_vValidateNotation(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
    static void LwXMLP_vValidateAttributeList(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
           static void LwXMLP_vValidateElement(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
           static void LwXMLP_vCheckWellFormedEntity(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);
           static LwXMLP_enumErrorType LwXMLP_enumValidatePEReference(INT8U  *pu8PEReference, \
                                                                      INT32U *pu32PEReferenceLength);
    static INT8U                LwXMLP_u8ValidPublicIDCharacter(INT8U const *pu8Character);
    static void                 LwXMLP_vRemoveWSInsideElement(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType);

#endif

/*
********************************************************************************************************************
************************************************************************************************************************
*                                              G L O B A L   V A R I A B L E S
************************************************************************************************************************
********************************************************************************************************************
*/

/*
********************************************************************************************************************
************************************************************************************************************************
*                                             G L O B A L   F U N C T I O N S
************************************************************************************************************************
********************************************************************************************************************
*/
/**********************************************************************************************************************/
/*!\fn         INT32U LwXMLP_u32GetFreeSpace(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         It is responsible to return the available free circular buffer in the pstrParserInstanceType
               instance

@param[in]     pstrParserInstanceType    : pointer to the XML instance

@return        The free amount in the circular buffer or
@return        0: if pstrParserInstanceType is passed as STD_NULL incorrectly.

@note

***********************************************************************************************************************/
INT32U LwXMLP_u32GetFreeSpace(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    INT32U u32FreeSpace;

    if(pstrParserInstanceType != STD_NULL)
    {
#if(ENABLE_ENCODING == STD_ON)
        u32FreeSpace = pstrParserInstanceType->u32FreeLocations/MAX_ENCODING_LENGTH;
#else
        u32FreeSpace = pstrParserInstanceType->u32FreeLocations;
#endif
    }
    else
    {
        u32FreeSpace = 0U;
    }

    return u32FreeSpace;
}
/**********************************************************************************************************************/
/*!\fn         INT32U LwXMLP_u32AddXMLPortion(      LwXMLP_strXMLParseInstanceType *pstrParserInstanceType,\
                                              const INT8U                          *pu8XMLPortionBuffer, \
                                                    INT32U                          u32RequiredAddPortionSize)


@brief         It is responsible to add XML portion pointed by pu8XMLPortionBuffer with length
               u32RequiredAddPortionSize to the circular buffer of the pstrParserInstanceType instance

@param[in,out]     pstrParserInstanceType    : pointer to the XML instance to get the current info and to be updated
                                               with the new info like the new free location and the circular buffer
                                               pointers

@param[in]     pu8XMLPortionBuffer       : pointer to XML portion that need to be added to the parser

@param[in]     u32RequiredAddPortionSize : The required XML portion length to that need to be added to the parser

@return        The actual added amount to the circular buffer if portion is added successfully, otherwise it return -1


@note          The required length to be added may be less than the actual added to the circular buffer

***********************************************************************************************************************/
INT32U LwXMLP_u32AddXMLPortion(      LwXMLP_strXMLParseInstanceType *pstrParserInstanceType,\
                               const INT8U                          *pu8XMLPortionBuffer, \
                                     INT32U                          u32RequiredAddPortionSize)
{
          INT8U         u8BOMLength = 0;
    const INT8U        *pu8NewXMLPortionBuffer = pu8XMLPortionBuffer;
          INT32U        u32ActualAddedLength;
          INT32U        u32ByteIndex;
          StdReturnType srStatus = E_OK;
    pstrParserInstanceType->u32NewLength = u32RequiredAddPortionSize;
    /* no addition shall be made if the second level parsing is being used */
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    if(pstrParserInstanceType->bSecondLevelBufferEnabled == STD_FALSE)
    {
#endif
        if(pstrParserInstanceType->bBOMIsChecked == STD_FALSE)
        {
            pstrParserInstanceType->bBOMIsChecked = STD_TRUE;
            u8BOMLength                           = LwXMLP_u8CheckBOM(pstrParserInstanceType, \
                                                                      pu8XMLPortionBuffer, \
                                                                      u32RequiredAddPortionSize);
            pu8NewXMLPortionBuffer                = &pu8XMLPortionBuffer[u8BOMLength];
            pstrParserInstanceType->u32NewLength  = u32RequiredAddPortionSize - u8BOMLength;
            /*
             * If no BOM is available, we will allow adding to XML deceleration
             * only to be parsed to detect the encoding scheme
             */
            if((u8BOMLength == 0U) && (pu8XMLPortionBuffer[1] == QUESTION_MARK_ASCII_CHARACTER))
            {
            /*
             * Look for end of deceleration inside the buffer
             */
               for(u32ByteIndex = 0U; u32ByteIndex < u32RequiredAddPortionSize; ++u32ByteIndex)
               {
                   if(pu8XMLPortionBuffer[u32ByteIndex] == GREATER_THAN_ASCII_CHARACTER)
                   {
                       LwXMLP_vCheckDeclerationEncoding(pstrParserInstanceType, pu8XMLPortionBuffer, u32ByteIndex + 1U);
                       if(pstrParserInstanceType->objenumErrorType == csNoError)
                       {
                           /*
                            * Nothing to do
                            */
                       }
                       else
                       {
                           srStatus = E_NOT_OK;
                       }
                       break;
                   }
                   else
                   {
                       /*
                        * Nothing to do
                        */
                   }
               }
               if(pstrParserInstanceType->objenumEncodingTypeType == csEncoding_NONE)
               {
                   /*
                    * Wait till receive complete deceleration
                    */
                   pstrParserInstanceType->u32NewLength = 0U;
               }
               else
               {
                   /*
                    * Nothing to do
                    */
               }
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
        }
        else
        {
            pu8NewXMLPortionBuffer = pu8XMLPortionBuffer;
        }
    #if(ENABLE_ENCODING == STD_ON)
        if(srStatus == E_OK)
        {
            /* it isn't allowed to have encoded length larger than the free amount of circular buffer */
            pstrParserInstanceType->u32Length = pstrParserInstanceType->u32FreeLocations;
            srStatus = LwXMLP_srEncodeXMLportion(pstrParserInstanceType->objenumEncodingTypeType,\
                                                 pu8NewXMLPortionBuffer,\
                                                 &pstrParserInstanceType->u32NewLength,\
                                                 pstrParserInstanceType->u8XMLEncodingBuffer,\
                                                 &pstrParserInstanceType->u32Length);
            if(srStatus == E_NOT_OK)
            {
                pstrParserInstanceType->objenumErrorType = csEncodingError;
                u32ActualAddedLength                     = 0xFFFFFFFFU;
            }
            else
            {
                pu8NewXMLPortionBuffer = pstrParserInstanceType->u8XMLEncodingBuffer;
                u32ActualAddedLength   = u8BOMLength + pstrParserInstanceType->u32NewLength;
                pstrParserInstanceType->u32NewLength   = pstrParserInstanceType->u32Length;
            }
        }
        else
        {
            /*
             * Nothing to do
             */
        }
    #else
        if(pstrParserInstanceType->u32FreeLocations < pstrParserInstanceType->u32NewLength)
        {
            pstrParserInstanceType->u32NewLength = pstrParserInstanceType->u32FreeLocations;
        }
        u32ActualAddedLength = u8BOMLength + pstrParserInstanceType->u32NewLength;
    #endif
        if(srStatus == E_OK)
        {
            if(pstrParserInstanceType->u32NewLength > 0U)
            {
                for(u32ByteIndex = 0; u32ByteIndex < pstrParserInstanceType->u32NewLength; u32ByteIndex++)
                {
                    if(pstrParserInstanceType->pu8CircularBufferHead == pstrParserInstanceType->pu8CircularBufferEnd)
                    {
                        pstrParserInstanceType->pu8CircularBufferHead = pstrParserInstanceType->u8XMLCircularBuffer;
                    }
                    else
                    {
                        /* nothing to do here */
                    }
                    *pstrParserInstanceType->pu8CircularBufferHead = pu8NewXMLPortionBuffer[u32ByteIndex];
                    pstrParserInstanceType->pu8CircularBufferHead++;
                }
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
            pstrParserInstanceType->u32FreeLocations -= pstrParserInstanceType->u32NewLength;
        }
        else
        {
            u32ActualAddedLength = 0;
        }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    }
    else
    {
        u32ActualAddedLength = 0;
    }
#endif

    return u32ActualAddedLength;
}
#if((ALLOCATION_TYPE == INTERNAL_STATIC_ALLOCATION) || (ALLOCATION_TYPE == INTERNAL_DYNAMIC_ALLOCATION))
/**********************************************************************************************************************/
/*!\fn         StdReturnType LwXMLP_srInitParserInternal(LwXMLP_strXMLParseInstanceType  **pstrParserInstanceType,\
                                                         LwXLMP_psrFuncCallBackType        objpsrFuncCallBackType)

@brief         It is responsible to allocate parser instance either statically or dynamically depending on ALLOCATION_TYPE
               macro definition and if the allocation is succeeded the instant’s  members will be initialized with
               the needed initial value beside the user callback function in objpsrFuncCallBackType

@param[out]    pstrParserInstanceType    : address of the pointer to the XML instance that will be created

@param[in]     objpsrFuncCallBackType    : pointer to the application call back which should be fired up on
                                           encountering a XML syntax construction like start TAG, end TAG, Data, so on.

@return        E_OK    : if the allocation/initialization process done successfully
@return        E_NOT_OK: if the allocation/initialization process failed or the one of the passing parameters is STD_NULL

@note          the number of internal instances are configured by the user, it the upper layer wants to run multiple
               parsers instances at the same time, and the maximum number of the multiple instances shall be configured
               using the macro TOTAL_STATIC_XML_INSTANCES
***********************************************************************************************************************/
StdReturnType LwXMLP_srInitParserInternal(LwXMLP_strXMLParseInstanceType  **pstrParserInstanceType,\
                                          LwXLMP_psrFuncCallBackType        objpsrFuncCallBackType)
{
    StdReturnType srInitParserStatus = E_OK;

    if((pstrParserInstanceType != STD_NULL) && (objpsrFuncCallBackType != STD_NULL))
    {
    #if(ALLOCATION_TYPE == INTERNAL_STATIC_ALLOCATION)
        *pstrParserInstanceType = LwXMLP_pstrGetStaticInstance();
    #else
       *pstrParserInstanceType = (LwXMLP_strXMLParseInstanceType *)calloc(1, sizeof(LwXMLP_strXMLParseInstanceType));
    #endif
        if(*pstrParserInstanceType != STD_NULL)
        {
            srInitParserStatus = LwXMLP_srInitParserObject(*pstrParserInstanceType, objpsrFuncCallBackType);
        }
        else
        {
            srInitParserStatus = E_NOT_OK;
            LwXMLP_mVerbose(VERBOSE_LEVEL_SERIOUS|VERBOSE_MESSAGE_ON, "Can't allocate the parser instance");
        }
    }
    else
    {
        srInitParserStatus = E_NOT_OK;
    }

    return srInitParserStatus;
}
#elif(ALLOCATION_TYPE == EXTERNAL_ALLOCATION)
StdReturnType  LwXMLP_srInitParserExternal(LwXMLP_strXMLParseInstanceType  *pstrParserInstanceType,\
                                           LwXLMP_psrFuncCallBackType       objpsrFuncCallBackType)
{
    StdReturnType srInitParserStatus = E_OK;

    if((pstrParserInstanceType != STD_NULL) && (objpsrFuncCallBackType != STD_NULL))
    {
        srInitParserStatus = LwXMLP_srInitParserObject(pstrParserInstanceType, objpsrFuncCallBackType);
    }
    else
    {
        srInitParserStatus = E_NOT_OK;
    }

    return srInitParserStatus;
}
#endif
/**********************************************************************************************************************/
/*!\fn         StdReturnType  LwXMLP_srInitParserExternal(LwXMLP_strXMLParseInstanceType  *pstrParserInstanceType,\
                                                          LwXLMP_psrFuncCallBackType       objpsrFuncCallBackType)


@brief         It is responsible to initialize the parser instances members. it should receive the new instance
               that had been allocated by the user externally

@param[out]    pstrParserInstanceType    : address of the pointer to the XML instance to be init

@param[in]     objpsrFuncCallBackType    : pointer to the application call back which should be fired up on
                                           encountering a XML syntax construction like start TAG, end TAG, Data, so on.

@return        E_OK    : if the initialization process done successfully
@return        E_NOT_OK: if the initialization process failed or the one of the passing parameters is STD_NULL

@note          The instance of pstrParserInstanceType shall be allocated by the upper layer which invoke
               LwXMLP_srInitParserExternal
***********************************************************************************************************************/
/**********************************************************************************************************************/
/*!\fn         StdReturnType LwXMLP_srCloseParser(LwXMLP_strXMLParseInstanceType  *pstrParserInstanceType)

@brief         It is responsible to release/deallocate the parser instance, such that the behavior of the
               API depends on the parser instance allocation type as the following:
               Internal static allocation : mark the internal static buffer as a free instance
               Internal dynamic allocation: Invoke free
               External allocation        : Nothing

@param[in]     pstrParserInstanceType    : the pointer to the XML instance that will be released/deallocated

@return         E_OK    : if the parser instance release/deallocate done successfully
@return         E_NOT_OK: if the parser instance release/deallocate failed

@note
***********************************************************************************************************************/
StdReturnType LwXMLP_srCloseParser(LwXMLP_strXMLParseInstanceType  *pstrParserInstanceType)
{
    StdReturnType srCloseStatus;

    if(pstrParserInstanceType != STD_NULL)
    {
#if(ALLOCATION_TYPE == INTERNAL_STATIC_ALLOCATION)
        srCloseStatus = LwXMLP_srReleaseStaticInstance(pstrParserInstanceType);
#elif (ALLOCATION_TYPE == INTERNAL_DYNAMIC_ALLOCATION)
        free((void*)pstrParserInstanceType);
        srCloseStatus = E_OK;
#else
        pstrParserInstanceType->bReachXMLEnd = STD_TRUE;
        srCloseStatus = E_OK;
#endif
    }
    else
    {
        srCloseStatus = E_NOT_OK;
    }

    return srCloseStatus;
}
/**********************************************************************************************************************/
/*!\fn         LwXMLP_enumXMLEventTypes LwXMLP_enumGetXMLEventType(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         It is responsible to return the fired event type for the given parser instance pstrParserInstanceType

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        the event types which may be one of the following:
               - csUnkownEvent, Event isn't known
               - csStartElementEvent, Event that should be fired up on detecting start element
               - csEndElementEvent, Event that should be fired up on detecting End element
               - csProcessInstructionEvent, Event that should be fired up on detecting Processing Instruction
               - csTextNodeEvent, Event that should be fired up on detecting text node
               - csCommentEvent, Event that should be fired up on detecting comment
               - csCDataEvent,  Event that should be fired up on detecting CData
               - csNotationEvent, Event that should be fired up on detection Notation

@note
***********************************************************************************************************************/
LwXMLP_enumXMLEventTypes LwXMLP_enumGetXMLEventType(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    LwXMLP_enumXMLEventTypes objenumXMLEventTypes;

    if(pstrParserInstanceType != STD_NULL)
    {
        objenumXMLEventTypes = pstrParserInstanceType->objenumXMLEventTypes;
    }
    else
    {
        objenumXMLEventTypes = csUnkownEvent;
    }

    return objenumXMLEventTypes;

}
/**********************************************************************************************************************/
/*!\fn         INT8U *LwXMLP_pu8GetParsedText(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         It is responsible to return the current parsed text string

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        pointer to parsed text node string
@return        STD_NULL: if pstrParserInstanceType is passed as STD_NULL incorrectly.

@note          this API shall be invoked from the upper layer when a csCDataEvent is fired, if it invoked in any other
               time, the returned value should be meaningless
***********************************************************************************************************************/
INT8U *LwXMLP_pu8GetParsedText(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    INT8U *pu8ParsedDataBuffer;

    if(pstrParserInstanceType != STD_NULL)
    {
        pu8ParsedDataBuffer = pstrParserInstanceType->pu8ParsedDataBuffer;
    }
    else
    {
        pu8ParsedDataBuffer = STD_NULL;
    }

    return pu8ParsedDataBuffer;
}
/**********************************************************************************************************************/
/*!\fn         INT8U *LwXMLP_pu8GetParsedNotation(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         This API is responsible to return the current parsed notation string

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        pointer to parsed text node string
@return        STD_NULL: if pstrParserInstanceType is passed as STD_NULL incorrectly.

@note          this API shall be invoked from the upper layer when a csNotationEvent is fired, if it invoked in any
               other time, the returned value should be meaningless
***********************************************************************************************************************/
INT8U *LwXMLP_pu8GetParsedNotation(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    INT8U *pu8ParsedNotaion;

    if(pstrParserInstanceType != STD_NULL)
    {
        pu8ParsedNotaion = pstrParserInstanceType->pu8ParsedNotaion;
    }
    else
    {
        pu8ParsedNotaion = STD_NULL;
    }

    return pu8ParsedNotaion;
}
/**********************************************************************************************************************/
/*!\fn         INT8U *LwXMLP_pu8GetElementName(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         This API is responsible to return the current Element name string

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        Pointer to the current Element name string
@return        STD_NULL: if pstrParserInstanceType is passed as STD_NULL incorrectly.

@note          this API shall be invoked from the upper layer when a csStartElementEvent or csEndElementEvent is fired,
               if it invoked in any other time, the returned value should be meaningless
***********************************************************************************************************************/
INT8U *LwXMLP_pu8GetElementName(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    INT8U *pu8ElementName;

    if(pstrParserInstanceType != STD_NULL)
    {
        pu8ElementName = pstrParserInstanceType->pu8ElementName;
    }
    else
    {
        pu8ElementName = STD_NULL;
    }

    return pu8ElementName;

}
/**********************************************************************************************************************/
/*!\fn         INT8U const *LwXMLP_pu8GetCurrentPath(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         It is responsible to return the current path of the XML element under processing. Like node1/node2/node3
               such that node1 is the root node and node2 is child node to the root and nood3 is a child node for
               the node1

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        Pointer to the current path of the element under processing
@return        STD_NULL: if pstrParserInstanceType is passed as STD_NULL incorrectly.

@note
***********************************************************************************************************************/
INT8U const *LwXMLP_pu8GetCurrentPath(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    INT8U const *pu8XMLPath;

    if(pstrParserInstanceType != STD_NULL)
    {
        pu8XMLPath = ((INT8U const *)&pstrParserInstanceType->u8XMLPathBuffer[0]);
    }
    else
    {
        pu8XMLPath = STD_NULL;
    }

    return pu8XMLPath;
}
/**********************************************************************************************************************/
/*!\fn         INT8U *LwXMLP_pu8GetParsedComment(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         It is responsible to return the current parsed comment string

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        Pointer to the parsed comment string
@return        STD_NULL, if pstrParserInstanceType is passed as STD_NULL incorrectly

@note          this API shall be invoked from the upper layer when a csCommentEvent event is fired, if it invoked in
               any other time, the returned value should be meaningless
***********************************************************************************************************************/
INT8U *LwXMLP_pu8GetParsedComment(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    INT8U *pu8ParsedComments;

    if(pstrParserInstanceType != STD_NULL)
    {
        pu8ParsedComments = pstrParserInstanceType->pu8ParsedComments;
    }
    else
    {
        pu8ParsedComments = STD_NULL;
    }

    return pu8ParsedComments;
}
/**********************************************************************************************************************/
/*!\fn         INT8U *LwXMLP_pu8GetParsedPITarget(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         It is responsible to return the current parsed processing instruction target string

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        Pointer to the parsed Processing Instruction target string
@return        STD_NULL: if pstrParserInstanceType is passed as STD_NULL incorrectly.

@note          this API shall be invoked from the upper layer when a csProcessInstructionEventevent is fired, if it
               invoked in any other time, the returned value should be meaningless
***********************************************************************************************************************/
INT8U *LwXMLP_pu8GetParsedPITarget(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    INT8U *pu8ParsedPITarget;

    if(pstrParserInstanceType != STD_NULL)
    {
        pu8ParsedPITarget = pstrParserInstanceType->pu8ParsedPITarget;
    }
    else
    {
        pu8ParsedPITarget = STD_NULL;
    }

    return pu8ParsedPITarget;
}
/**********************************************************************************************************************/
/*!\fn         INT8U *LwXMLP_pu8GetParsedPIData(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         It is responsible to return the current parsed processing instruction data string

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        Pointer to the parsed Processing Instruction data string
@return        STD_NULL: if pstrParserInstanceType is passed as STD_NULL incorrectly.

@note
***********************************************************************************************************************/
INT8U *LwXMLP_pu8GetParsedPIData(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    INT8U *pu8ParsedPIContent;

    if(pstrParserInstanceType != STD_NULL)
    {
        pu8ParsedPIContent = pstrParserInstanceType->pu8ParsedPIContent;
    }
    else
    {
        pu8ParsedPIContent = STD_NULL;
    }

    return pu8ParsedPIContent;
}
/**********************************************************************************************************************/
/*!\fn         INT16S LwXMLP_s16GetAttributeCount(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         It is responsible to return the number of the current parsed attributes inside the element if any

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        The number of the current parsed attributes which may be 0
@return        STD_NULL, if pstrParserInstanceType is passed as STD_NULL incorrectly

@note
***********************************************************************************************************************/
INT16S LwXMLP_s16GetAttributeCount(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    INT16S s16AttributeCounts;

    if(pstrParserInstanceType != STD_NULL)
    {
        s16AttributeCounts = pstrParserInstanceType->s16AttributeCounts;
    }
    else
    {
        s16AttributeCounts = 0;
    }

    return s16AttributeCounts;
}
/**********************************************************************************************************************/
/*!\fn         LwXMLP_enumErrorType LwXMLP_enumGetError(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         It is responsible to return the current parsing error

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        The current error number if any
@return        csInvalidPassingParameter: if pstrParserInstanceType is passed as STD_NULL incorrectly

@note
***********************************************************************************************************************/
LwXMLP_enumErrorType LwXMLP_enumGetError(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    LwXMLP_enumErrorType objenumErrorType;

    if(pstrParserInstanceType != STD_NULL)
    {
        objenumErrorType = pstrParserInstanceType->objenumErrorType;
    }
    else
    {
        objenumErrorType = csInvalidPassingParameter;
    }

    return objenumErrorType;

}
/**********************************************************************************************************************/
/*!\fn         INT8U * LwXMLP_pu8GetAttributeName(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType, \
                                                  INT16S                              s16AttributeIndex)

@brief         This API is responsible to return pointer to the attribute name string which its index is defined by
                u8AttributeIndex

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@param[in]     s16AttributeIndex          : Index of the parsed attribute

@return        pointer to the attribute name which its index is defined by u8AttributeIndex
@return        STD_NULL: if pstrParserInstanceType is passed as STD_NULL incorrectly or the u8AttributeIndex isn't
               correct.

@note
***********************************************************************************************************************/
INT8U * LwXMLP_pu8GetAttributeName(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType, \
                                   INT16S                                s16AttributeIndex)
{
    INT8U *pu8AttributeName;

    if(pstrParserInstanceType != STD_NULL)
    {
        if(s16AttributeIndex < pstrParserInstanceType->s16AttributeCounts)
        {
            pu8AttributeName = pstrParserInstanceType->pu8AttributeNameArray[s16AttributeIndex];
        }
        else
        {
            pu8AttributeName = STD_NULL;
        }
    }
    else
    {
        pu8AttributeName = STD_NULL;
    }

    return pu8AttributeName;
}
/**********************************************************************************************************************/
/*!\fn         INT8U * LwXMLP_pu8GetAttributeValue(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType, \
                                                   INT16S                              s16AttributeIndex)

@brief         It is responsible to return pointer to the attribute value string which its index is defined by
               u8AttributeIndex

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@param[in]     s16AttributeIndex          : Index of the parsed attribute

@return        pointer to the attribute value which its index is defined by u8AttributeIndex
@return        STD_NULL: if pstrParserInstanceType is passed as STD_NULL incorrectly or the u8AttributeIndex isn't
               correct.

@note
***********************************************************************************************************************/
INT8U * LwXMLP_pu8GetAttributeValue(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType, \
                                    INT16S                                s16AttributeIndex)
{
    INT8U *pu8AttributeValue;

    if(pstrParserInstanceType != STD_NULL)
    {
        if(s16AttributeIndex < pstrParserInstanceType->s16AttributeCounts)
        {
            pu8AttributeValue = pstrParserInstanceType->pu8AttributeValueArray[s16AttributeIndex];
        }
        else
        {
            pu8AttributeValue = STD_NULL;
        }
    }
    else
    {
        pu8AttributeValue = STD_NULL;
    }

    return pu8AttributeValue;
}
/**********************************************************************************************************************/
/*!\fn         LwXMLP_enumParserStatusType LwXMLP_enumXMLParserEngine(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)

@brief         it is the core engine of the parser which parse the available portion of the XML document according
               to the following properties and limitations:
               - It is online streaming processor (incremental processor) such that it can generate correct partial
                 output from partial input and this gives a degree of freedom on the XML document size, i.e. there
                 is no restriction on the XML document size.
               - It is SAX or event-based processor. Such that it reads an XML document from beginning to end, and each
                 time it encounters a syntax construction, it notifies the application that is running it, and the
                 application must implement the appropriate methods to handle the callbacks and get the functionality
                 needed.
               - It can receive the XML document from different stream type like file stream and IP stream (TCP/UDP),
                 serial communication, so on.
               - The code/data size can be customize to enable/disable some of the feature on the expense of the
                 code/data size using some macro definitions.
               - Thread-safe (more precisely: reentrant).
               - Support two models of memory allocation, static memory allocation and dynamic memory allocation.
               - It shall be able to detect the xml fetal errors which will cause the reporting for the error type and
                 the XML line where the error is occurred then it will cause the termination for reading process.
               - Support only for encoding types UTF-8, UTF-16BE and UTF-16LE.

@param[in,out]     pstrParserInstanceType    : pointer to the XML instance to get the current info and to be loaded
                                               with the new value

@return        the status of the parser engine which may be one of the following:
               - csUnkownEvent, Event isn't known
               - csStartElementEvent, Event that should be fired up on detecting start element
               - csEndElementEvent, Event that should be fired up on detecting End element
               - csProcessInstructionEvent, Event that should be fired up on detecting Processing Instruction
               - csTextNodeEvent, Event that should be fired up on detecting text node
               - csCommentEvent, Event that should be fired up on detecting comment
               - csCDataEvent,  Event that should be fired up on detecting CData
               - csNotationEvent, Event that should be fired up on detection Notation

@note          The XML parser engine shall be continuously invoked till parsing done or the parsing encountered an error
***********************************************************************************************************************/
LwXMLP_enumParserStatusType LwXMLP_enumXMLParserEngine(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
          BOOLEAN                      bStopLookingForData  = STD_FALSE;
    const INT8U                       *pu8CurrentCharacter;
          StdReturnType                srStatus;
          LwXMLP_enumParserStatusType  objenumParserStatusType = csUnkownParserStatus;

    LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Start parsing round");
    /* looking for the data of the XML node*/
    while(bStopLookingForData == STD_FALSE)
    {
        pu8CurrentCharacter = LwXMLP_pu8GetCurrentCharacter(pstrParserInstanceType);
        if(LwXMLP_u32GetReadyData(pstrParserInstanceType) == 0U)
        {
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "There is no data for parsing");
            objenumParserStatusType = csContinueAddingXMLData;
            bStopLookingForData = STD_TRUE;
        }
        else if(LwXMLP_bIsLessThanCharacter(pu8CurrentCharacter) == STD_TRUE)
        {
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found <");
            bStopLookingForData = STD_TRUE;
        }
#if(ENABLE_DTD_RECEIPTION == STD_ON)
        else if(pstrParserInstanceType->bParsingDTDInProgress == STD_TRUE)
        {
            LwXMLP_vCheckDTDPortion(pstrParserInstanceType);
            if(pstrParserInstanceType->objenumErrorType == csNoError)
            {
                objenumParserStatusType = csContinueParsingXMLData;
            }
            else
            {
                objenumParserStatusType = csParserError;
            }
            bStopLookingForData = STD_TRUE;
        }
#endif
        else
        {
            LwXMLP_vCheckavailableData(pstrParserInstanceType);
            if(pstrParserInstanceType->objenumErrorType == csNoError)
            {
                if(pstrParserInstanceType->bEndOfDataIsFound == STD_TRUE)
                {   /* data shall be available inside TAG*/
                    if(pstrParserInstanceType->u32PathLength != 0U)
                    {
#if(ENABLE_EOL_NORMALIZATION == STD_ON)
                        LwXMLP_vNormalizeEndOfLine(pstrParserInstanceType->u8XMLworkingBuffer);
#endif
                        pstrParserInstanceType->objenumXMLEventTypes = csTextNodeEvent;
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                        if(pstrParserInstanceType->bParserSilentModeIsEnabled == STD_FALSE)
                        {
#endif
                            srStatus = pstrParserInstanceType->objpsrFuncCallBackType(pstrParserInstanceType);
                            if(srStatus == E_OK)
                            {
                                objenumParserStatusType = csContinueParsingXMLData;
                            }
                            else
                            {
                                /*Terminate XML reception due to error at the user side*/
                                objenumParserStatusType                  = csParserError;
                                pstrParserInstanceType->objenumErrorType = csCallBackErrorOnData;
                            }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                        }
                        else
                        {
                            objenumParserStatusType = csContinueParsingXMLData;
                        }
#endif
                    }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                    else if(pstrParserInstanceType->bSecondLevelBufferEnabled == STD_TRUE)
                    {
                        objenumParserStatusType = csContinueParsingXMLData;
                    }
#endif
                    else
                    {
                        objenumParserStatusType                  = csParserError;
                        pstrParserInstanceType->objenumErrorType = csDataWithoutStartTag;
                    }
                    bStopLookingForData = STD_TRUE;
                }
                else
                {/* No Data or markup is found */
                    /*
                     * check if we reach the end of the XML and there is no more markup for parsing
                     */
                    if(pstrParserInstanceType->bReachXMLEnd == STD_TRUE)
                    {
                        pu8CurrentCharacter = LwXMLP_pu8GetCurrentCharacter(pstrParserInstanceType);
                        if(LwXMLP_bIsLessThanCharacter(pu8CurrentCharacter) == STD_FALSE)
                        {
                            objenumParserStatusType = csParsingFinished;
                        }
                        else
                        {
                            objenumParserStatusType = csContinueAddingXMLData;
                        }
                    }
                    else
                    {
                        objenumParserStatusType = csContinueAddingXMLData;
                    }
                    bStopLookingForData     = STD_TRUE;
                }
            }
            else
            {
                /*Terminate XML reception due to error */
                objenumParserStatusType = csParserError;
                bStopLookingForData     = STD_TRUE;
            }
        }
    }
    if(objenumParserStatusType == csUnkownParserStatus)
    {
        /*we need to check <> */
        /*we need to check < */
        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Parsing the content of <>");
        LwXMLP_vCheckTagForDirective(pstrParserInstanceType);
        if(pstrParserInstanceType->objenumErrorType == csNoError)
        {
            if(pstrParserInstanceType->bTagContainDirective == STD_TRUE)
            {
                /* grab what ever is in-between '<' and '>' and put it in s8XMLBuffer*/
                LwXMLP_vLoadDirectiveToBuffer(pstrParserInstanceType);
                if(pstrParserInstanceType->objenumErrorType == csNoError)
                {
                    if(pstrParserInstanceType->bEndOfDirectiveFound == STD_TRUE)
                    {
                        /* the TAG start or end are currently loaded to the working buffer */
                        switch(pstrParserInstanceType->objenumDirectiveTypeType)
                        {
                            case csProcessingInstructionMarkup:
                                 LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32DirectiveLength); /* consume Directive */
#if(ENABLE_PI_WELL_FORM_CHECK == STD_ON)
                                 LwXMLP_vParsePI(pstrParserInstanceType);
                                 if(pstrParserInstanceType->objenumErrorType == csNoError)
                                 {
                                     pstrParserInstanceType->objenumXMLEventTypes = csProcessInstructionEvent;
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                                     if(pstrParserInstanceType->bParserSilentModeIsEnabled == STD_FALSE)
                                     {
#endif
                                         srStatus = pstrParserInstanceType->objpsrFuncCallBackType(pstrParserInstanceType);
                                         if(srStatus == E_OK)
                                         {
                                             if((LwXMLP_bMoreCharacterAtEndOfXML(pstrParserInstanceType) == STD_FALSE) &&
                                                (pstrParserInstanceType->bReachXMLEnd == STD_TRUE))
                                             {
                                                 objenumParserStatusType = csParsingFinished;
                                             }
                                             else
                                             {
                                                 objenumParserStatusType = csContinueParsingXMLData;
                                             }
                                         }
                                         else
                                         {
                                             /*Terminate XML reception due to error at the user side*/
                                             objenumParserStatusType                  = csParserError;
                                             pstrParserInstanceType->objenumErrorType = csCallBackErrorOnPIDirective;
                                         }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                                     }
                                     else
                                     {
                                         objenumParserStatusType = csContinueParsingXMLData;
                                     }
#endif
                                 }
                                 else
                                 {
                                     objenumParserStatusType = csParserError;
                                 }
#else
                                 objenumParserStatusType = csContinueParsingXMLData;
#endif
                                 break;
                            case csCommentMarkup:
                                 LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32DirectiveLength); /* consume Directive */
#if(ENABLE_COMMENT_WELL_FORM_CHECK == STD_ON)
                                 LwXMLP_vCheckWellFormedComment(pstrParserInstanceType);
#endif
                                 if(pstrParserInstanceType->objenumErrorType == csNoError)
                                 {
                                      pstrParserInstanceType->objenumXMLEventTypes = csCommentEvent;
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                                      if(pstrParserInstanceType->bParserSilentModeIsEnabled == STD_FALSE)
                                      {
#endif
                                          srStatus = pstrParserInstanceType->objpsrFuncCallBackType(pstrParserInstanceType);
                                          if(srStatus == E_OK)
                                          {
                                              if(pstrParserInstanceType->bReachXMLEnd == STD_TRUE)
                                              {
                                                  if(LwXMLP_bMoreCharacterAtEndOfXML(pstrParserInstanceType) == STD_FALSE)
                                                  {
                                                      objenumParserStatusType = csParsingFinished;
                                                  }
                                                  else
                                                  {
                                                      objenumParserStatusType = csContinueParsingXMLData;
                                                  }
                                              }
                                              else
                                              {
                                                  objenumParserStatusType = csContinueParsingXMLData;
                                              }
                                          }
                                          else
                                          {
                                              /*Terminate XML reception due to error at the user side*/
                                              objenumParserStatusType                  = csParserError;
                                              pstrParserInstanceType->objenumErrorType = csCallBackErrorOnComment;
                                          }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                                      }
                                      else
                                      {
                                          objenumParserStatusType = csContinueParsingXMLData;
                                      }
#endif
                                 }
                                 else
                                 {
                                      objenumParserStatusType = csParserError;
                                 }
                                 break;
                            case csDeclarationMarkup:
                                 LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32DirectiveLength); /* consume Directive */
#if(ENABLE_DECL_WELL_FORM_CHECK == STD_ON)
                                 LwXMLP_vCheckWellFormedDecl(pstrParserInstanceType);
                                 if(pstrParserInstanceType->objenumErrorType == csNoError)
                                 {
                                     objenumParserStatusType = csContinueParsingXMLData;
                                 }
                                 else
                                 {
                                     objenumParserStatusType = csParserError;
                                 }
#else
                                 objenumParserStatusType = csContinueParsingXMLData;
#endif
                                 break;
#if(ENABLE_DTD_RECEIPTION == STD_ON)
                            case csDTDElementMarkup:
#if(ENABLE_DECL_WELL_FORM_CHECK == STD_ON)
                                 LwXMLP_vValidateElement(pstrParserInstanceType);
                                 if(pstrParserInstanceType->objenumErrorType == csNoError)
                                 {
                                     objenumParserStatusType = csContinueParsingXMLData;
                                 }
                                 else
                                 {
                                     objenumParserStatusType = csParserError;
                                 }
#else
                                 objenumParserStatusType = csContinueParsingXMLData;
#endif
                                 LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32DirectiveLength); /* consume Directive */
                                 break;
                            case csDTDEntitytMarkup:
                                 LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32DirectiveLength); /* consume Directive */
#if(ENABLE_DTD_WELL_FORM_CHECK == STD_ON)
                                 if(pstrParserInstanceType->bFoundExternalEntity == STD_FALSE)
                                 {
                                     LwXMLP_vCheckWellFormedEntity(pstrParserInstanceType);
                                 }
                                 else
                                 {
                                     /*
                                      * They must not process entity declarations or attribute-list declarations
                                      * encountered after a reference to a parameter entity that is not read,
                                      */
                                 }
                                 if(pstrParserInstanceType->objenumErrorType == csNoError)
                                 {
                                     objenumParserStatusType = csContinueParsingXMLData;
                                 }
                                 else
                                 {
                                     objenumParserStatusType = csParserError;
                                 }
#else
                                 objenumParserStatusType = csContinueParsingXMLData;
#endif
                                 break;
                            case csDTDNotationtMarkup:
#if(ENABLE_DTD_WELL_FORM_CHECK == STD_ON)
                                 LwXMLP_vValidateNotation(pstrParserInstanceType);
                                 if(pstrParserInstanceType->objenumErrorType == csNoError)
                                 {
#if 0
                                     pstrParserInstanceType->objenumXMLEventTypes = csNotationEvent;
                                     srStatus = pstrParserInstanceType->objpsrFuncCallBackType(pstrParserInstanceType);
                                     if(srStatus == E_OK)
                                     {
                                         objenumParserStatusType = csContinueParsingXMLData;
                                     }
                                     else
                                     {
                                         /*Terminate XML reception due to error at the user side*/
                                         objenumParserStatusType                  = csParserError;
                                         pstrParserInstanceType->objenumErrorType = csCallBackErrorOnData;
                                     }
#endif
                                     objenumParserStatusType = csContinueParsingXMLData;
                                 }
                                 else
                                 {
                                     objenumParserStatusType = csParserError;
                                 }
#else
                                 objenumParserStatusType = csContinueParsingXMLData;
#endif
                                 LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32DirectiveLength); /* consume Directive */
                                 break;
                            case csDTDAttlistMarkup:
#if(ENABLE_DTD_WELL_FORM_CHECK == STD_ON)
                                 if(pstrParserInstanceType->bFoundExternalEntity == STD_FALSE)
                                 {
                                     LwXMLP_vValidateAttributeList(pstrParserInstanceType);
                                 }
                                 else
                                 {
                                     /*
                                      * They must not process entity declarations or attribute-list declarations
                                      * encountered after a reference to a parameter entity that is not read,
                                      */
                                 }
                                 if(pstrParserInstanceType->objenumErrorType == csNoError)
                                 {
                                     objenumParserStatusType = csContinueParsingXMLData;
                                 }
                                 else
                                 {
                                     objenumParserStatusType = csParserError;
                                 }
#else
                                 objenumParserStatusType = csContinueParsingXMLData;
#endif
                                 LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32DirectiveLength); /* consume Directive */
                                 break;
                            case csInternalDTDMarkup:
                            case csExternalDTDMarkup:
                            case csDTDMarkup:
                                 LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32DirectiveLength); /* consume Directive */
#if(ENABLE_DTD_WELL_FORM_CHECK == STD_ON)
                                 LwXMLP_vValidateDTD(pstrParserInstanceType);
                                 if(pstrParserInstanceType->objenumErrorType == csNoError)
                                 {
                                     objenumParserStatusType = csContinueParsingXMLData;
                                 }
                                 else
                                 {
                                     objenumParserStatusType = csParserError;
                                 }
#else
                                 objenumParserStatusType = csContinueParsingXMLData;
#endif
                                 break;
#else
                            case csDTDMarkup:
                                 /*
                                  * Nothing to do
                                  */
                                 break;
#endif
                            case csCDATAMarkup:
#if(ENABLE_CDATA_WELL_FORM_CHECK == STD_ON)
                                 LwXMLP_vCheckWellFormedCDATA(pstrParserInstanceType);
                                 if(pstrParserInstanceType->objenumErrorType == csNoError)
                                 {
                                     pstrParserInstanceType->objenumXMLEventTypes = csCDataEvent;
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                                     if(pstrParserInstanceType->bParserSilentModeIsEnabled == STD_FALSE)
                                     {
#endif
                                         if(pstrParserInstanceType->u32PathLength != 0U)
                                         {
#if(ENABLE_EOL_NORMALIZATION == STD_ON)
                                             LwXMLP_vNormalizeEndOfLine(pstrParserInstanceType->pu8ParsedDataBuffer);
#endif
                                             srStatus = pstrParserInstanceType->objpsrFuncCallBackType(pstrParserInstanceType);
                                             if(srStatus == E_OK)
                                             {
                                                 objenumParserStatusType = csContinueParsingXMLData;
                                             }
                                             else
                                             {
                                                 /*Terminate XML reception due to error at the user side*/
                                                 objenumParserStatusType                  = csParserError;
                                                 pstrParserInstanceType->objenumErrorType = csCallBackErrorOnData;
                                             }
                                         }
                                         else
                                         {
                                             objenumParserStatusType                  = csParserError;
                                             pstrParserInstanceType->objenumErrorType = csInvalidRootToken;
                                         }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                                     }
                                     else
                                     {
                                         objenumParserStatusType = csContinueParsingXMLData;
                                     }
#endif
                                 }
                                 else
                                 {
                                     objenumParserStatusType = csParserError;
                                 }
#else
                                 objenumParserStatusType = csContinueParsingXMLData;
#endif
                                 LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32DirectiveLength); /* consume Directive */
                                 break;
                            default:
                                 objenumParserStatusType = csContinueAddingXMLData;
                                 break;
                        }
                    }
                    else /* we couldn't find the end of the directive*/
                    {
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                        /* at this point if we were parsing mixed content and we didn't find a markup, we shall consume what we have */
                        if(pstrParserInstanceType->bSecondLevelBufferEnabled == STD_TRUE)
                        {
                            LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32SecondLevelReadyAmount);
                            objenumParserStatusType                  = csParserError;
                            pstrParserInstanceType->objenumErrorType = csMissingEndOfEntity;
                        }
                        else
                        {
                            objenumParserStatusType = csContinueAddingXMLData;
                            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "There is no data for parsing");
                        }
#else
                        objenumParserStatusType = csContinueAddingXMLData;
                        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "There is no data for parsing");
#endif
                    }
                }
                else
                {
                    objenumParserStatusType = csParserError;
                }
            }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
            else if((pstrParserInstanceType->bParsingDTDInProgress == STD_TRUE) && \
                    (pstrParserInstanceType->bSecondLevelBufferEnabled == STD_FALSE))
            {
                pstrParserInstanceType->objenumErrorType = csNoError;
                pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
            }
#endif
            else
            {
                /* grab what ever is in-between '<' and '>' and put it in s8XMLBuffer*/
                LwXMLP_vLoadTagToBuffer(pstrParserInstanceType);
                if(pstrParserInstanceType->objenumErrorType == csNoError)
                {
                    if(pstrParserInstanceType->bTagCloserIsFound == STD_TRUE)
                    {
                        /* the TAG start or end are currently loaded to the working buffer */
                        switch(pstrParserInstanceType->objenumTagTypeType)
                        {
                            case csStartTagType:
                                 if(pstrParserInstanceType->bReachXMLEnd == STD_FALSE)
                                 {
                                     LwXMLP_vUpdatePathWithStartTag(pstrParserInstanceType);
                                     /* consume the read TAG */
                                     LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32CurrentTagLength);
                                     if(pstrParserInstanceType->objenumErrorType == csNoError)
                                     {
#if(ENABLE_EOL_NORMALIZATION == STD_ON)
                                         LwXMLP_vNormalizeEndOfLine(pstrParserInstanceType->u8XMLworkingBuffer);
#endif
                                         pstrParserInstanceType->objenumXMLEventTypes = csStartElementEvent;
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                                         if(pstrParserInstanceType->bParserSilentModeIsEnabled == STD_FALSE)
                                         {
#endif
                                             srStatus = pstrParserInstanceType->objpsrFuncCallBackType(pstrParserInstanceType);
                                             if(srStatus == E_OK)
                                             {
                                                 objenumParserStatusType = csContinueParsingXMLData;
                                             }
                                             else
                                             {
                                                 /*Terminate XML reception due to error at the user side*/
                                                 objenumParserStatusType                  = csParserError;
                                                 pstrParserInstanceType->objenumErrorType = csCallBackErrorOnStartElement;
                                             }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                                         }
                                         else
                                         {
                                             objenumParserStatusType = csContinueParsingXMLData;
                                         }
#endif
                                     }
                                     else
                                     {
                                         /*Terminate XML reception due to error at the user side*/
                                         objenumParserStatusType = csParserError;
                                     }
                                 }
                                 else
                                 {
                                     pstrParserInstanceType->objenumErrorType = csExtraContentAtTheEnd;
                                     objenumParserStatusType                  = csParserError;
                                 }
                                 break;
                            case csEndTagType:
                                 pstrParserInstanceType->objenumXMLEventTypes = csEndElementEvent;
#if(ENABLE_EOL_NORMALIZATION == STD_ON)
                                 LwXMLP_vNormalizeEndOfLine(pstrParserInstanceType->u8XMLworkingBuffer);
#endif
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                                 if(pstrParserInstanceType->bParserSilentModeIsEnabled == STD_FALSE)
                                 {
#endif
                                     srStatus = pstrParserInstanceType->objpsrFuncCallBackType(pstrParserInstanceType);
                                     if(srStatus == E_OK)
                                     {
                                         LwXMLP_vRemoveTagfromPath(pstrParserInstanceType);
                                         /* consume the read TAG */
                                         LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32CurrentTagLength);
                                         if(pstrParserInstanceType->objenumErrorType == csNoError)
                                         {
                                             if(LwXMLP_bXMLPathIsEmpty(pstrParserInstanceType) == STD_TRUE)
                                             {
                                                 pstrParserInstanceType->bReachXMLEnd = STD_TRUE;
                                                 /* check for extra content at the end of the document */
                                                 if(LwXMLP_bMoreCharacterAtEndOfXML(pstrParserInstanceType) == STD_TRUE)
                                                 {/* The only allowed content at the end of the XML document is PI and comment */
                                                     LwXMLP_vCheckTagForDirective(pstrParserInstanceType);
                                                     if((pstrParserInstanceType->objenumErrorType == csNoError)&&
                                                       (pstrParserInstanceType->bTagContainDirective == STD_TRUE)&&
                                                       ((pstrParserInstanceType->objenumDirectiveTypeType == csProcessingInstructionMarkup) ||\
                                                        (pstrParserInstanceType->objenumDirectiveTypeType == csCommentMarkup)))
                                                     {
                                                         objenumParserStatusType = csContinueParsingXMLData;
                                                     }
                                                     else
                                                     {
                                                         objenumParserStatusType                  = csParserError;
                                                         pstrParserInstanceType->objenumErrorType = csExtraContentAtTheEnd;
                                                     }
                                                 }
                                                 else
                                                 {
                                                     objenumParserStatusType = csParsingFinished;
                                                 }
                                             }
                                             else
                                             {
                                                 objenumParserStatusType = csContinueParsingXMLData;
                                             }
                                         }
                                         else
                                         {
                                             objenumParserStatusType = csParserError;
                                         }
                                     }
                                     else
                                     {
                                         /*Terminate XML reception due to error at the user side*/
                                         objenumParserStatusType = csParserError;
                                         pstrParserInstanceType->objenumErrorType = csCallBackErrorOnStartElement;
                                     }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                                 }
                                 else
                                 {
                                     LwXMLP_vRemoveTagfromPath(pstrParserInstanceType);
                                     /* consume the read TAG */
                                     LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32CurrentTagLength);
                                     objenumParserStatusType = csContinueParsingXMLData;
                                 }
#endif
                                 break;
                            case csEmptyTagType:
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                                 if(pstrParserInstanceType->bParserSilentModeIsEnabled == STD_FALSE)
                                 {
#endif
                                     LwXMLP_vUpdatePathWithStartTag(pstrParserInstanceType);
                                     pstrParserInstanceType->objenumXMLEventTypes = csStartElementEvent;
                                     srStatus = pstrParserInstanceType->objpsrFuncCallBackType(pstrParserInstanceType);
                                     if(srStatus == E_OK)
                                     {
                                         pstrParserInstanceType->objenumXMLEventTypes = csEndElementEvent;
                                         srStatus = pstrParserInstanceType->objpsrFuncCallBackType(pstrParserInstanceType);
                                         if(srStatus == E_OK)
                                         {
                                             LwXMLP_vRemoveTagfromPath(pstrParserInstanceType);
                                             if(pstrParserInstanceType->objenumErrorType == csNoError)
                                             {
                                                 LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32CurrentTagLength);
                                                 if(LwXMLP_bXMLPathIsEmpty(pstrParserInstanceType) == STD_TRUE)
                                                 {
                                                     pstrParserInstanceType->bReachXMLEnd = STD_TRUE;
                                                     objenumParserStatusType              = csParsingFinished;
                                                     /* check for extra content at the end of the document */
                                                     if(LwXMLP_bMoreCharacterAtEndOfXML(pstrParserInstanceType) == STD_TRUE)
                                                     {/* The only allowed content at the end of the XML document is PI and comment */
                                                         LwXMLP_vCheckTagForDirective(pstrParserInstanceType);
                                                         if((pstrParserInstanceType->objenumErrorType == csNoError)&&
                                                           (pstrParserInstanceType->bTagContainDirective == STD_TRUE)&&
                                                           ((pstrParserInstanceType->objenumDirectiveTypeType == csProcessingInstructionMarkup) ||\
                                                            (pstrParserInstanceType->objenumDirectiveTypeType == csCommentMarkup)))
                                                         {
                                                             objenumParserStatusType = csContinueParsingXMLData;
                                                         }
                                                         else
                                                         {
                                                             objenumParserStatusType                  = csParserError;
                                                             pstrParserInstanceType->objenumErrorType = csExtraContentAtTheEnd;
                                                         }
                                                     }
                                                     else
                                                     {
                                                         objenumParserStatusType = csParsingFinished;
                                                     }
                                                 }
                                                 else
                                                 {
                                                     objenumParserStatusType = csContinueParsingXMLData;
                                                 }
                                             }
                                             else
                                             {
                                                 objenumParserStatusType = csParserError;
                                             }
                                         }
                                         else
                                         {
                                             /*Terminate XML reception due to error at the user side*/
                                             objenumParserStatusType = csParserError;
                                             pstrParserInstanceType->objenumErrorType = csCallBackErrorOnStartElement;
                                         }
                                     }
                                     else
                                     {
                                         /*Terminate XML reception due to error at the user side*/
                                         objenumParserStatusType = csParserError;
                                         pstrParserInstanceType->objenumErrorType = csCallBackErrorOnStartElement;
                                     }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                                 }
                                 else
                                 {
                                     objenumParserStatusType = csContinueParsingXMLData;
                                     LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32CurrentTagLength);
                                 }
#endif
                                 break;
                            case csUnkownTagType:
                            default:
                                /*Terminate XML reception due to error */
                                objenumParserStatusType = csParserError;
                                pstrParserInstanceType->objenumErrorType = csUnexpectedElementType;
                                break;
                        }
                    }
                    else
                    {
                        /* at this point if we were parsing mixed content and we didn't find a markup, we shall consume what we have */
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
                        if(pstrParserInstanceType->bSecondLevelBufferEnabled == STD_TRUE)
                        {
                            LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, pstrParserInstanceType->u32SecondLevelReadyAmount);
                            objenumParserStatusType = csContinueParsingXMLData;
                        }
                        else
#endif
                        if(pstrParserInstanceType->bReachXMLEnd == STD_TRUE)
                        {
                            objenumParserStatusType = csParsingFinished;
                        }
                        else
                        {
                            objenumParserStatusType = csContinueAddingXMLData;
                        }
                        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "There is no data for parsing");
                    }
                }
                else
                {
                    /*Terminate XML reception due to error, The error type will be set by LwXMLP_srLoadTagToBuffer */
                    objenumParserStatusType = csParserError;
                }
            }
        }
        else
        {
            /*Terminate XML reception due to error, the error type will be set by LwXMLP_srCheckTagForDirective */
            objenumParserStatusType = csParserError;
        }
    }

    return objenumParserStatusType;
}
/**********************************************************************************************************************/
/*!\fn         INT8S const *LwXMLP_ps8GetErrorString(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         It is responsible to return the error string description of the received parser instance

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        Pointer to constant of the error string

@note
***********************************************************************************************************************/
INT8S const *LwXMLP_ps8GetErrorString(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    const INT8S *ps8XMLErrorString;

#if(SUPPORT_ERROR_STRING == STD_ON)
    if(pstrParserInstanceType != STD_NULL)
    {
        switch(pstrParserInstanceType->objenumErrorType)
        {
            case csInvalidPassingParameter:
                 ps8XMLErrorString = "the parser received invalid passing parameter";
                 break;
            case csInternalError:
                 ps8XMLErrorString = "Error detected in the internal buffer during parsing";
                 break;
            case csFeatureNotSupported:
                 ps8XMLErrorString = "Feature isn't supported by the parser, it my be disabled by the user";
                 break;
            case csAttributeInEndTag:
                 ps8XMLErrorString = "Attribute in End TAG isn't allowed";
                 break;
            case csMissingSeparator:
                 ps8XMLErrorString = "missing separator in choice";
                 break;
            case csInvalidDirectReference:
                 ps8XMLErrorString = "Direct reference to an entity with &lt;";
                 break;
            case csGeneralEntityInDTD:
                 ps8XMLErrorString = "Found General entity in DTD";
                 break;
            case csPartialCharacterReference:
                 ps8XMLErrorString = "Found Partial Character Reference";
                 break;
            case csIncompleteEntityContent:
                 ps8XMLErrorString = "Incomplete entity content";
                 break;
            case csInvalidOperator:
                 ps8XMLErrorString = "Invalid Operator";
                 break;
            case csMissingPITarget:
                 ps8XMLErrorString = "Missing PI Target ";
                 break;
            case csEntityRefereToItself:
                 ps8XMLErrorString = "It isn't allowed to let the entity to refer to itself";
                 break;
            case csMissingEndOfEntity:
                 ps8XMLErrorString = "Missing End of Entity";
                 break;
            case csDTDInvalidDefaultDeclaration:
                 ps8XMLErrorString = "DTD Invalid Attribute Default Declaration";
                 break;
            case csDTDInvalidAttributeType:
                 ps8XMLErrorString = "DTD Invalid Attribute Type";
                 break;
            case csDTDMissingSystemIDError:
                 ps8XMLErrorString = "DTD is missing System ID";
                 break;
            case csInvalidParameterEntity:
                 ps8XMLErrorString = "DTD Invalid parameter entity";
                 break;
            case csDTDMissingPortionError:
                 ps8XMLErrorString = "DTD is missing portion";
                 break;
            case csInvalidAttributeValue:
                 ps8XMLErrorString = "Invalid Attribute Value";
                 break;
            case csInvalidRootToken:
                 ps8XMLErrorString = "Invalid Token at the root document";
                 break;
            case csIllegalWhiteSpace:
                 ps8XMLErrorString = "Illegal WhiteSpace";
                 break;
            case csMissingWhiteSpaceCharacter:
                 ps8XMLErrorString = "Missing WhiteSpace";
                 break;
            case csDTDCapitalPortionError:
                 ps8XMLErrorString = "DTD small letter portion is used";
                 break;
            case csMissingAttributeValue:
                 ps8XMLErrorString = "Attribute value is missing";
                 break;
            case csWrongDirectiveEnd:
                 ps8XMLErrorString = "Can't find the end of the directive";
                 break;
            case csDoubleHyphenInComment:
                 ps8XMLErrorString = "Double Hyphen inside the comment";
                 break;
            case csMissingEqual:
                 ps8XMLErrorString = "The = is missing between the attribute name and the attribute value";
                 break;
            case csMissingQuote:
                 ps8XMLErrorString = "missing quote";
                 break;
            case csQuotesMissmatch:
                 ps8XMLErrorString = "Quotes miss match";
                 break;
            case csInvalidStartNameCharacter:
                 ps8XMLErrorString = "Invalid Start Name Character";
                 break;
            case csInvalidNameCharacter:
                 ps8XMLErrorString = "Invalid Name Character";
                 break;
            case csInvalidPIName:
                ps8XMLErrorString = "Invalid PI Name";
                 break;
            case csInvalidXMLCharacter:
                 ps8XMLErrorString = "Invalid XML Character";
                 break;
            case csCallBackErrorOnPIDirective:
                 ps8XMLErrorString = "User Callback Error on the Process Instruction";
                 break;
            case csCallBackErrorOnStartElement:
                 ps8XMLErrorString = "User Callback Error on the start TAG";
                 break;
            case csCallBackErrorOnData:
                 ps8XMLErrorString = "User Callback Error on the text node";
                 break;
            case csCallBackErrorOnEndElement:
                ps8XMLErrorString = "User Callback Error on the End element";
                 break;
            case csCallBackErrorOnComment:
                 ps8XMLErrorString = "User Callback Error on the comment";
                 break;
            case csUnexpectedElementType:
                 ps8XMLErrorString = "Unexpected element Type";
                 break;
            case csFailedToGetElementInfo:
                 ps8XMLErrorString = "Failed to Extract element info";
                 break;
            case csFailedToAddElementToPath:
                 ps8XMLErrorString = "Failed to add new element to the Path buffer";
                 break;
            case csFailedToExtractAttributes:
                 ps8XMLErrorString = "Failed to Extract the attributes";
                 break;
            case csWrongNesting:
                ps8XMLErrorString = "Wrong nesting";
                 break;
            case csLargeElementLength:
                 ps8XMLErrorString = "Long element length";
                 break;
            case csWrongAttributeFormat:
                 ps8XMLErrorString = "Missing attribute value";
                 break;
            case csWrongDeclarationLocation:
                 ps8XMLErrorString = "Wrong Declaration Location";
                 break;
            case csUnsupportedEncodingScheme:
                ps8XMLErrorString = "unsupported encoding scheme";
                 break;
            case csUnExpectedDirectiveType:
                 ps8XMLErrorString = "Received unexpected directive type";
                 break;
            case csNoError:
                 ps8XMLErrorString = "No Error";
                 break;
            case csEncodingError:
                 ps8XMLErrorString = "problem occurred during encoding XML portion";
                 break;
            case csMissingSemicolon:
                 ps8XMLErrorString = "missing semi-colon from entity reference";
                 break;
            case csExtraContentAtTheEnd:
                 ps8XMLErrorString = "Extra content at the end of the document";
                 break;
            case csRepeatedAttributeName:
                 ps8XMLErrorString = "Repeated attribute Name";
                 break;
            case csInvalidSDeclOrder:
                 ps8XMLErrorString = "Invalid SDecl Order";
                 break;
            case csLargeResolvingTrialsProperty:
                 ps8XMLErrorString = "More entities need to be resolved, increase MAX_ENTITY_RESOLVING_TRIALS";
                 break;
            case csLargeChildrenOperatorsProperty:
                 ps8XMLErrorString = "Can't process all received attributes, increase MAX_OPERATORS_COUNT";
                 break;
            case csLargeNumberOfAttributes:
                 ps8XMLErrorString = "Can't process all received attributes, increase MAX_NUMBER_OF_ATTRIBUTES";
                 break;
            case csLargeElementProperty:
                 ps8XMLErrorString = "Can't load the element into working buffer, increase MAX_CIRCULAR_BUFFER_SIZE & MAX_WORKING_BUFFER_SIZE";
                 break;
            case csLargeDatalength:
                 ps8XMLErrorString = "Can't load the data into working buffer, increase MAX_CIRCULAR_BUFFER_SIZE & MAX_WORKING_BUFFER_SIZE";
                 break;
            case csLargeDTDPortionlength:
                 ps8XMLErrorString = "Can't load the DTD portion into working buffer, increase MAX_CIRCULAR_BUFFER_SIZE & MAX_WORKING_BUFFER_SIZE";
                 break;
            case csLargeDirectiveProperty:
                 ps8XMLErrorString = "Can't load the Directive into working buffer, increase MAX_CIRCULAR_BUFFER_SIZE & MAX_WORKING_BUFFER_SIZE";
                 break;
            case csLargeAttributeNameProperty:
                 ps8XMLErrorString = "Can't load attribute name, increase MAX_ATTRIBUTES_NAME_LENGTH";
                 break;
            case csLargeElementNameProperty:
                 ps8XMLErrorString = "Can't load element name, increase MAX_ELEMENT_NAME_LENGTH";
                 break;
            case csLargeEntityCountProperty:
                 ps8XMLErrorString = "Can't load entity, increase MAX_ENTITIES";
                 break;
            case csLargeEntityNameProperty:
                 ps8XMLErrorString = "Can't load entity name, increase MAX_ENTITY_NAME_LENGTH";
                 break;
            case csLargeEntityValueProperty:
                 ps8XMLErrorString = "Can't load entity name, increase MAX_ENTITY_VALUE_LENGTH";
                 break;
            case csLargeNumberOfAttributeList:
                 ps8XMLErrorString = "Can't load Attribute list declarations, increase MAX_ATTRIBUTES_LIST";
                 break;
            case csDataWithoutStartTag:
                 ps8XMLErrorString = "Found data without being included in a start TAG";
                 break;
            case csInvalidReference:
                 ps8XMLErrorString = "Invalid Reference";
                 break;
            case csInvalidVersionOrder:
                 ps8XMLErrorString = "Invalid Version Attribute Order";
                 break;
            case csParsedEntityError:
                 ps8XMLErrorString = "Parsed entity isn't well-formed";
                 break;
            case csPEReferencesinInInternalSubset:
                 ps8XMLErrorString = "PEReferences forbidden in internal subset";
                 break;
            case csEncodingDiscrepancy:
                 ps8XMLErrorString = "The parser detected Discrepancy between BOM and encoding type in deceleration";
                 break;
            case csMaximumXMLError:
            default:
                 ps8XMLErrorString = "Unknown Error";
                 break;
        }
    }
    else
    {
        ps8XMLErrorString = "The parser received invalid passing parameter";
    }
#else
    if(pstrParserInstanceType != STD_NULL)
    {
        ps8XMLErrorString = "Error String isn't supported";
    }
    else
    {
        ps8XMLErrorString = "The parser received invalid passing parameter";
    }
#endif
    return (ps8XMLErrorString);
}
/**********************************************************************************************************************/
/*!\fn         INT32U LwXMLP_u32GetLineOfError(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         It is responsible to return the line number where the error is occurred for the received parser
               instance

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        The line number where the error is occurred or,
@return        0: if the pstrParserInstanceType is received incorrectly STD_NULL

@note
***********************************************************************************************************************/
INT32U LwXMLP_u32GetLineOfError(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    INT32U u32LineNumber;

    if(pstrParserInstanceType != STD_NULL)
    {
        u32LineNumber = pstrParserInstanceType->u32LineNumber;
    }
    else
    {
        u32LineNumber = 0U;
    }

    return (u32LineNumber);
}
/**********************************************************************************************************************/
/*!\fn         INT32U LwXMLP_u32GetCoulmnOfError(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         This API is responsible to return the column number where the error is occurred for the received parser
               instance

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        The line number where the error is occurred or,
@return        0: if the pstrParserInstanceType is received incorrectly STD_NULL

@note
***********************************************************************************************************************/
INT32U LwXMLP_u32GetCoulmnOfError(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    INT32U u32CoulmnNumber;

    if(pstrParserInstanceType != STD_NULL)
    {
        u32CoulmnNumber = pstrParserInstanceType->u32CoulmnNumber;
    }
    else
    {
        u32CoulmnNumber = 0U;
    }

    return (u32CoulmnNumber);
}

/*
********************************************************************************************************************
************************************************************************************************************************
*                                               L O C A L   F U N C T I O N S
************************************************************************************************************************
********************************************************************************************************************
*/
/**********************************************************************************************************************/
/*!\fn         StdReturnType LwXMLP_srReleaseStaticInstance(LwXMLP_strXMLParseInstanceType  const *pstrParserInstanceType)

@brief         This API is responsible to release the statically allocated instance

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        E_NOT_OK: if the release process is failed
               E_OK    : if the release process is succeeded

@note
***********************************************************************************************************************/
/**********************************************************************************************************************/
/*!\fn         LwXMLP_strXMLParseInstanceType  *LwXMLP_pstrGetStaticInstance(void)

@brief         This API is responsible to get a free instance and if it is found, it will be marked as reversed not
               to be used by other caller and it will be failed if there is no available static instances

@return        STD_NULL:                       if there is no free instances
               LwXMLP_strXMLParseInstanceType: pointer to the allocated instance if there is a free instance

@note          the upper layer caller shall fail and not to continue the parsing process if this API returned STD_NULL
***********************************************************************************************************************/

#if(ALLOCATION_TYPE == INTERNAL_STATIC_ALLOCATION)
static StdReturnType LwXMLP_srReleaseStaticInstance(LwXMLP_strXMLParseInstanceType  const *pstrParserInstanceType)
{
    INT8U         u8FreeInstanceIndex;
    StdReturnType srReleaseStatus = E_NOT_OK; /* Assume that the release process is failed */


    for(u8FreeInstanceIndex = 0; u8FreeInstanceIndex < TOTAL_STATIC_XML_INSTANCES; u8FreeInstanceIndex++)
    {
        if(&objstrStaticInstancesArray[u8FreeInstanceIndex].objstrXMLParseInstanceType == pstrParserInstanceType)
        {
            objstrStaticInstancesArray[u8FreeInstanceIndex].bReserevedInstance = STD_FALSE;
            srReleaseStatus                                                    = E_OK;
            pstrParserInstanceType                                             = STD_NULL;
            break;
        }
    }

    return srReleaseStatus;
}
static LwXMLP_strXMLParseInstanceType  *LwXMLP_pstrGetStaticInstance(void)
{
    INT8U                           u8FreeInstanceIndex;
    LwXMLP_strXMLParseInstanceType *pstrXMLParseInstanceType = STD_NULL;

    for(u8FreeInstanceIndex = 0; u8FreeInstanceIndex < TOTAL_STATIC_XML_INSTANCES; u8FreeInstanceIndex++)
    {
        if(objstrStaticInstancesArray[u8FreeInstanceIndex].bReserevedInstance == STD_FALSE)
        {
            pstrXMLParseInstanceType = &objstrStaticInstancesArray[u8FreeInstanceIndex].objstrXMLParseInstanceType;
            objstrStaticInstancesArray[u8FreeInstanceIndex].bReserevedInstance = STD_TRUE;
            break;
        }
    }

    return pstrXMLParseInstanceType;
}
#endif
/**********************************************************************************************************************/
/*!\fn         StdReturnType LwXMLP_srInitParserObject(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType,\
                                                       LwXLMP_psrFuncCallBackType      objpsrFuncCallBackType)

@brief         This API is responsible to initialize the parser instance with the proper initial value for its members

@param[in,out]     pstrParserInstanceType    : pointer to the XML instance to get the current info and to be loaded
                                               with the initial value

@param[in]         objpsrFuncCallBackType    : pointer to the call back function assigned by upper layer


@return        E_NOT_OK: if the initialization process is failed, and it will fail if the upper layer passed STD_NULL
                         pointer in the parser instance
               E_OK    : if the initialization process is succeeded

@note          the upper layer caller shall fail and not to continue the parsing process if this API returned STD_NULL
***********************************************************************************************************************/
static StdReturnType LwXMLP_srInitParserObject(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType,\
                                                      LwXLMP_psrFuncCallBackType      objpsrFuncCallBackType)
{
    StdReturnType srInitParserStatusObject;

    if(pstrParserInstanceType != STD_NULL)
    {
        srInitParserStatusObject                         = E_OK;
        pstrParserInstanceType->u32FreeLocations         = MAX_CIRCULAR_BUFFER_SIZE;
        pstrParserInstanceType->pu8LastParserLocation    = pstrParserInstanceType->u8XMLCircularBuffer;
        pstrParserInstanceType->pu8CircularBufferHead    = pstrParserInstanceType->u8XMLCircularBuffer;
        pstrParserInstanceType->pu8CircularBufferEnd     = &pstrParserInstanceType->u8XMLCircularBuffer[MAX_CIRCULAR_BUFFER_SIZE];
        pstrParserInstanceType->objpsrFuncCallBackType   = objpsrFuncCallBackType;
        pstrParserInstanceType->u32CoulmnNumber          = 1U;
        pstrParserInstanceType->u32LineNumber            = 1U;
        pstrParserInstanceType->objenumXMLCheckStateType = csBOMChecking;
        pstrParserInstanceType->objenumEncodingTypeType  = csEncoding_NONE;
        pstrParserInstanceType->u8XMLPathBuffer[0]       = 0U;
        pstrParserInstanceType->u32PathLength            = 0U;
        pstrParserInstanceType->bEndOfDataIsFound        = STD_FALSE;
        pstrParserInstanceType->bBOMIsChecked            = STD_FALSE;
        pstrParserInstanceType->bEndOfDirectiveFound     = STD_FALSE;
        pstrParserInstanceType->bTagContainDirective     = STD_FALSE;
        pstrParserInstanceType->objenumErrorType         = csNoError;
        pstrParserInstanceType->s16AttributeCounts       = 0;
        pstrParserInstanceType->u32CurrentTagLength      = 0U;
        pstrParserInstanceType->u32DirectiveLength       = 0U;
        pstrParserInstanceType->u32ParsedLength          = 0U;
        pstrParserInstanceType->bReachXMLEnd             = STD_FALSE;
        pstrParserInstanceType->u8AvaliableEntities      = 0U;
#if(ENABLE_DTD_RECEIPTION == STD_ON)
        pstrParserInstanceType->bParsingDTDInProgress      = STD_FALSE;
#endif
        pstrParserInstanceType->u8AvaliableAttibuteListCount = 0U;
        pstrParserInstanceType->bStandAloneDocument        = STD_FALSE;
        pstrParserInstanceType->bFoundExternalEntity       = STD_FALSE;
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
        pstrParserInstanceType->bParserSilentModeIsEnabled = STD_FALSE;
        pstrParserInstanceType->bSecondLevelBufferEnabled  = STD_FALSE;
        pstrParserInstanceType->u8SecondLevelPathBuffer[0] = 0U;
        pstrParserInstanceType->u32SecondLevelPathLength   = 0U;
        pstrParserInstanceType->u32LastReferenceLine       = 0U;
#endif
    }
    else
    {
        srInitParserStatusObject = E_NOT_OK;
    }

    return srInitParserStatusObject;

}
/**********************************************************************************************************************/
/*!\fn         void LwXMLP_vCopy(INT8U *pu8Destination, INT8U const *pu8Source, INT32U u32Length)

@brief         This API is helper function to do copy from source to destination with specific length

@param[in]     pu8Destination            : the pointer to destination location

@param[in]     pu8Source                 : the pointer to source location

@param[in]     u32Length                 : the length of the required data to be copied in bytes

@return

@note
***********************************************************************************************************************/
static void LwXMLP_vCopy(INT8U *pu8Destination, INT8U const *pu8Source, INT32U u32Length)
{
    memcpy(pu8Destination, pu8Source, u32Length);
}
/**********************************************************************************************************************/
/*!\fn         INT32U LwXMLP_u32GetReadyData(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)

@brief         This API return the available length of XML document for parsing

@param[in]     pstrParserInstanceType    : the pointer to the XML instance

@return        the number of bytes of the available length of XML document for parsing

@note
***********************************************************************************************************************/
static INT32U LwXMLP_u32GetReadyData(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    INT32U u32ReadyAmount;
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    if(pstrParserInstanceType->bSecondLevelBufferEnabled == STD_TRUE)
    {
        u32ReadyAmount = pstrParserInstanceType->u32SecondLevelReadyAmount;
    }
    else
    {
        u32ReadyAmount = MAX_CIRCULAR_BUFFER_SIZE - pstrParserInstanceType->u32FreeLocations;
    }
#else
    u32ReadyAmount = MAX_CIRCULAR_BUFFER_SIZE - pstrParserInstanceType->u32FreeLocations;
#endif
    return u32ReadyAmount;
}
/**********************************************************************************************************************/
/*!\fn         INT8U LwXMLP_u8CheckBOM(      LwXMLP_strXMLParseInstanceType *pstrParserInstanceType,\
                                      const INT8U                          *pu8XMLPortion, \
                                            INT32U                          u32XMLPortionSize)

@brief         This API check the available BOM in the XML document

@param[in,out]     pstrParserInstanceType    : pointer to the XML instance to get the current info and to be loaded
                                               with the found BOM
@param[in]         pu8XMLPortion             : pointer to the XML portion to be parsed to find the BOM
@param[in]         u32XMLPortionSize         : The size of the XML portion

@return           The found BOM length

@note
***********************************************************************************************************************/
static INT8U LwXMLP_u8CheckBOM(      LwXMLP_strXMLParseInstanceType *pstrParserInstanceType,\
                                      const INT8U                          *pu8XMLPortion, \
                                            INT32U                          u32XMLPortionSize)
{
                 INT8U  u8BOMIndex, u8BOMByteIndex;
    static const INT8U  cu8UCS4_BEBOM[]             = {0x00U, 0x00U, 0x00U, 0x3CU};
    static const INT8U  cu8UCS4_2143BOM[]           = {0x00U, 0x00U, 0x3CU, 0x00U};
    static const INT8U  cu8UCS4_3412BOM[]           = {0x00U, 0x3CU, 0x00U, 0x00};
    static const INT8U  cu8EBCDICBOM[]              = {0x4CU, 0x6FU, 0xA7U, 0x94};
    static const INT8U  cu8UCS4_LEBOM[]             = {0x3CU, 0x00U, 0x00U, 0x00};
    static const INT8U  cu8UTF16BOMLittleEndian_1[] = {0x3CU, 0x00U, 0x3FU, 0x00U};
    static const INT8U  cu8UTF16BOMBigEndian_1[]    = {0x00U, 0x3CU, 0x00U, 0x3FU};
    static const INT8U  cu8UTF8BOM_2[]              = {0xEFU, 0xBBU, 0xBFU};
    static const INT8U  cu8UTF16BOMLittleEndian_2[] = {0xFFU, 0xFEU};
    static const INT8U  cu8UTF16BOMBigEndian_2[]    = {0xFEU, 0xFFU};
                 INT8U  u8availableBOMLength = 0;
    static const strBOMDescriptorType objstrBOMDescriptorType[] =
    {
        {.pu8EncodingBOM = cu8UCS4_BEBOM,\
         .u8EncodingBOMLength = 4U,\
         .objenumEncodingTypeType = csEncoding_UCS4LE},
        {.pu8EncodingBOM = cu8UCS4_LEBOM,\
         .u8EncodingBOMLength = 4U,\
         .objenumEncodingTypeType = csEncoding_UCS4BE},
        {.pu8EncodingBOM = cu8UCS4_2143BOM,\
         .u8EncodingBOMLength = 4U,\
         .objenumEncodingTypeType = csEncoding_UCS4_2143},
        {.pu8EncodingBOM = cu8UCS4_3412BOM,\
         .u8EncodingBOMLength = 4U,\
         .objenumEncodingTypeType = csEncoding_UCS4_3412},
        {.pu8EncodingBOM = cu8EBCDICBOM,\
         .u8EncodingBOMLength = 4U,\
         .objenumEncodingTypeType = csEncoding_EBCDIC},
        {.pu8EncodingBOM = cu8UTF16BOMLittleEndian_1,\
         .u8EncodingBOMLength = 4U,\
         .objenumEncodingTypeType =csEncoding_UTF16LE},
         {.pu8EncodingBOM = cu8UTF16BOMBigEndian_1,\
          .u8EncodingBOMLength = 4U,\
          .objenumEncodingTypeType = csEncoding_UTF16BE},
        {.pu8EncodingBOM = cu8UTF8BOM_2,\
         .u8EncodingBOMLength = 3U,\
         .objenumEncodingTypeType = csEncoding_UTF8},
        {.pu8EncodingBOM = cu8UTF16BOMLittleEndian_2,\
         .u8EncodingBOMLength = 2U,\
         .objenumEncodingTypeType =csEncoding_UTF16LE},
        {.pu8EncodingBOM = cu8UTF16BOMBigEndian_2,\
         .u8EncodingBOMLength = 2U,\
         .objenumEncodingTypeType = csEncoding_UTF16BE},
    };
    for(u8BOMIndex = 0U; u8BOMIndex < (sizeof(objstrBOMDescriptorType) / sizeof(strBOMDescriptorType)); ++u8BOMIndex )
    {
        for(u8BOMByteIndex = 0U; u8BOMByteIndex < objstrBOMDescriptorType[u8BOMIndex].u8EncodingBOMLength; u8BOMByteIndex++)
        {
            if((INT8U)objstrBOMDescriptorType[u8BOMIndex].pu8EncodingBOM[u8BOMByteIndex] != \
               (INT8U)pu8XMLPortion[u8BOMByteIndex])
            {
                break;
            }
        }
        if((u8BOMByteIndex == objstrBOMDescriptorType[u8BOMIndex].u8EncodingBOMLength) &&
           ((objstrBOMDescriptorType[u8BOMIndex].u8EncodingBOMLength <= u32XMLPortionSize)))
        {
            pstrParserInstanceType->objenumEncodingTypeType  = \
                    objstrBOMDescriptorType[u8BOMIndex].objenumEncodingTypeType;
            /* will not consume the length of the BOM to be parsed by the parser */
            if(pu8XMLPortion[0] == 0x3CU)
            {
                u8availableBOMLength = 0;
            }
            else
            {
                u8availableBOMLength = objstrBOMDescriptorType[u8BOMIndex].u8EncodingBOMLength;
            }
            switch(objstrBOMDescriptorType[u8BOMIndex].objenumEncodingTypeType)
            {
                case csEncoding_EBCDIC:
                     /* EBCDIC */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: EBCDIC");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_UCS4_2143:
                     /* UCS4_2143 */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: UCS4_2143");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_UCS4_3412:
                     /* UCS4_3412 */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: UCS4_3412");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_UTF8:
                     /* UTF-8 */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: UTF8");
                     break;
                case csEncoding_UTF16LE:
                     /* UTF-16LE */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: UTF16LE");
                     break;
                case csEncoding_UTF16BE:
                     /* UTF-16BE */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: UTF16BE");
                     break;
                case csEncoding_UCS4LE:
                     /* UCS4LE */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: UCS4LE");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_UCS4BE:
                     /* UCS4BE */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: UCS4BE");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_UCS2:
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: UCS2");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_8859_1:      /*!< ISO-8859-1 ISO Latin 1 */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: 8859_1");
                     break;
                case csEncoding_8859_2:      /*!< ISO-8859-2 ISO Latin 2 */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: 8859_2");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_8859_3:      /*!< ISO-8859-3 */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: 8859_3");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_8859_4:      /*!< ISO-8859-4 */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: 8859_4");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_8859_5:      /*!< ISO-8859-5 */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: 8859_5");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_8859_6:      /*!< ISO-8859-6 */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: 8859_6");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_8859_7:      /*!< ISO-8859-7 */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: 8859_7");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_8859_8:      /*!< ISO-8859-8 */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: 8859_8");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_8859_9:      /*!< ISO-8859-9 */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: 8859_9");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_2022_JP:     /*!< ISO-2022-JP */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: 2022_JP");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_SHIFT_JIS:   /*!< Shift_JIS */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: SHIFT_JIS");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_EUC_JP:      /*!< EUC-JP */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: EUC_JP");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_ASCII:       /*!< pure ASCII */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: ASCII");
                     break;
                case csEncoding_ERROR:
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found BOM Type: ERROR");
                     pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                     break;
                case csEncoding_NONE:
                     /* Nothing */
                default:
                     /* NO BOM, will use the UTF-8 s a default encoding scheme */
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "No BOM is Found use: UTF8");
                     break;
            }
           break;
        }
        else
        {
            /*
             * Nothing to do
             */
        }
    }

#if(SUPPORT_ASCII_ONLY == STD_ON)
    if((pstrParserInstanceType->objenumEncodingTypeType != csEncoding_ASCII) &&\
       (pstrParserInstanceType->objenumEncodingTypeType != csEncoding_NONE))
    {
        pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
    }
    else
    {
        /*
         * Nothing to do
         */
    }
#endif

    return u8availableBOMLength;
}
/**********************************************************************************************************************/
/*!\fn         INT8U LwXMLP_u8ValidNameCharacter(INT8U const *pu8Character)

@brief         This API is responsible to Validate the character if it is NameChar according to W3C definition which
               should be in range  NameStartChar | "-" | "." | [0-9] | #xB7 | [#x0300-#x036F] | [#x203F-#x2040]
               The LwXMLP assume that all the parsed string are encoded in UTF-8, so this API will get the code point
               from the UTF-8 sequence to be compared with the W3C range.

@param[in]     pu8Character : Pointer to the location of the character

@return        Length of the valid UTF-8 sequence


@note
***********************************************************************************************************************/
static INT8U LwXMLP_u8ValidNameCharacter(INT8U const *pu8Character)
{
#if(SUPPORT_ASCII_ONLY == STD_OFF)
    INT32U  u32CodePoint = 0U;
#endif
    INT8U   u8CharacterLength;

    if((isalpha((INT8U)(*pu8Character)) != 0) || \
       ((INT8U)(*pu8Character) == COLON_ASCII_CHARACTER) || \
       ((INT8U)(*pu8Character) == UNDERSCORE_ASCII_CHARACTER)||\
       ((((INT8U)(*pu8Character)) - ZERO_ASCII_CHARACTER) < 10U) ||\
       ((INT8U)(*pu8Character) == DOT_ASCII_CHARACTER) ||\
       ((INT8U)(*pu8Character) == HYPHEN_ASCII_CHARACTER))
    {
        u8CharacterLength = 1U;
    }
#if(SUPPORT_ASCII_ONLY == STD_OFF)
    else if((*pu8Character & 0xE0U) == 0xC0U) /* 110X XXXX  two *pu8Characterlength */
    {
        u8CharacterLength = 2U;
        u32CodePoint = (((INT32U)pu8Character[0] - 192U) * 64U) + \
                        ((INT32U)pu8Character[1U] - 128U);
    }
    else if((*pu8Character & 0xF0U) == 0xE0U) /* 1110 XXXX  three *pu8Characterlength */
    {
        u8CharacterLength = 3U;
        u32CodePoint = (((INT32U)pu8Character[0] - 224U) * 4096U) + \
                       (((INT32U)pu8Character[1U] - 128U) * 64U) + \
                       ((INT32U)pu8Character[2] - 128U);
    }
    else if((*pu8Character & 0xF8U) == 0xF0U) /* 1111 0XXX  four *pu8Characterlength */
    {
        u8CharacterLength = 4U;
        u32CodePoint  = (((INT32U)pu8Character[0] - 240U) * 262144U) + \
                        (((INT32U)pu8Character[1U] - 128U) * 4096U) + \
                        (((INT32U)pu8Character[2] - 128U) * 64U) + \
                        ((INT32U)pu8Character[3] - 128U);
    }
#endif
    else
    {
        u8CharacterLength = 0U;
    }
#if(SUPPORT_ASCII_ONLY == STD_OFF)
    if(u8CharacterLength > 1U)
    {
        if((u32CodePoint == 0x00B7U)||\
           ((u32CodePoint >= 0x00C0U) && (u32CodePoint <= 0x00D6U))||\
           ((u32CodePoint >= 0x00D8U) && (u32CodePoint <= 0x00F6U))||\
           ((u32CodePoint >= 0x00F8U) && (u32CodePoint <= 0x37DU))||\
           ((u32CodePoint >= 0x037FU) && (u32CodePoint <= 0x1FFFU))||\
           ((u32CodePoint >= 0x200CU) && (u32CodePoint <= 0x200DU))||\
           ((u32CodePoint >= 0x203FU) && (u32CodePoint <= 0x2040U))||\
           ((u32CodePoint >= 0x2070U) && (u32CodePoint <= 0x218FU))||\
           ((u32CodePoint >= 0x2C00U) && (u32CodePoint <= 0x2FEFU))||\
           ((u32CodePoint >= 0x3001U) && (u32CodePoint <= 0xD7FFU))||\
           ((u32CodePoint >= 0xF900U) && (u32CodePoint <= 0xFDCFU))||\
           ((u32CodePoint >= 0xFDF0U) && (u32CodePoint <= 0xFFFDU))||\
           ((u32CodePoint >= 0x10000U) && (u32CodePoint <= 0xEFFFFU)))
        {
            /*
             * Nothing to do
             */
        }
        else
        {
            u8CharacterLength = 0U;
        }
    }
#endif

    return u8CharacterLength;
}
/**********************************************************************************************************************/
/*!\fn         INT8U LwXMLP_u8ValidNameStartCharacter(INT8U const *pu8Character)

@brief         Validate the character if it is NameStartChar according to W3C definition which should be in range
                ":" | [A-Z] | "_" | [a-z] | [#xC0-#xD6] | [#xD8-#xF6] | [#xF8-#x2FF] | [#x370-#x37D] |
                [#x37F-#x1FFF] | [#x200C-#x200D] | [#x2070-#x218F] | [#x2C00-#x2FEF] | [#x3001-#xD7FF] |
                [#xF900-#xFDCF] | [#xFDF0-#xFFFD] | [#x10000-#xEFFFF]
               the LwXMLP assume that all the parsed string are encoded in UTF-8, so this API will get the code point
               from the UTF-8 sequence to be compared with the W3C range.

@param[in]       pu8Character : Pointer to the location of the character

@return         Length of the valid UTF-8 sequence

@note
***********************************************************************************************************************/
static INT8U LwXMLP_u8ValidNameStartCharacter(INT8U const *pu8Character)
{
#if(SUPPORT_ASCII_ONLY == STD_OFF)
    INT32U  u32CodePoint = 0U;
#endif
    INT8U   u8CharacterLength;

    if((isalpha((INT8U)(*pu8Character)) != 0) || \
       ((INT8U)(*pu8Character) == COLON_ASCII_CHARACTER) || \
       ((INT8U)(*pu8Character) == UNDERSCORE_ASCII_CHARACTER))
    {
        u8CharacterLength = 1U;
    }
#if(SUPPORT_ASCII_ONLY == STD_OFF)
    else if((*pu8Character & 0xE0U) == 0xC0U) /* 110X XXXX  two *pu8Characterlength */
    {
        u8CharacterLength = 2U;
        u32CodePoint = (((INT32U)pu8Character[0] - 192U) * 64U) + \
                       ((INT32U)pu8Character[1U] - 128U);
    }
    else if((*pu8Character & 0xF0U) == 0xE0U) /* 1110 XXXX  three *pu8Characterlength */
    {
        u8CharacterLength = 3U;
        u32CodePoint = (((INT32U)pu8Character[0] - 224U) * 4096U) + \
                       (((INT32U)pu8Character[1U] - 128U) * 64U) + \
                       ((INT32U)pu8Character[2] - 128U);
    }
    else if((*pu8Character & 0xF8U) == 0xF0U) /* 1111 0XXX  four *pu8Characterlength */
    {
        u8CharacterLength = 4U;
        u32CodePoint  = (((INT32U)pu8Character[0] - 240U) * 262144U) + \
                        (((INT32U)pu8Character[1U] - 128U) * 4096U) + \
                        (((INT32U)pu8Character[2] - 128U) * 64U) + \
                        ((INT32U)pu8Character[3] - 128U);
    }
#endif
    else
    {
        u8CharacterLength = 0U;
    }
#if(SUPPORT_ASCII_ONLY == STD_OFF)
    if(u8CharacterLength > 1U)
    {
        if(((u32CodePoint >= 0x00C0U) && (u32CodePoint <= 0x00D6U))||\
           ((u32CodePoint >= 0x00D8U) && (u32CodePoint <= 0x00F6U))||\
           ((u32CodePoint >= 0x00F8U) && (u32CodePoint <= 0x2FFU))||\
           ((u32CodePoint >= 0x0370U) && (u32CodePoint <= 0x37DU))||\
           ((u32CodePoint >= 0x037FU) && (u32CodePoint <= 0x1FFFU))||\
           ((u32CodePoint >= 0x200CU) && (u32CodePoint <= 0x200DU))||\
           ((u32CodePoint >= 0x2070U) && (u32CodePoint <= 0x218FU))||\
           ((u32CodePoint >= 0x2C00U) && (u32CodePoint <= 0x2FEFU))||\
           ((u32CodePoint >= 0x3001U) && (u32CodePoint <= 0xD7FFU))||\
           ((u32CodePoint >= 0xF900U) && (u32CodePoint <= 0xFDCFU))||\
           ((u32CodePoint >= 0xFDF0U) && (u32CodePoint <= 0xFFFDU))||\
           ((u32CodePoint >= 0x10000U) && (u32CodePoint <= 0xEFFFFU)))
        {
            /*
             * Nothing to do
             */
        }
        else
        {
            u8CharacterLength = 0U;
        }
    }
#endif

    return u8CharacterLength;
}
/**********************************************************************************************************************/
/*!\fn         INT8U LwXMLP_u8ValidCharacter(INT8U const *pu8Character)

@brief         Validate the character if it is Char according to W3C definition which should be in range
               #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]
               the LwXMLP assume that all the parsed string are encoded in UTF-8, so this API will get the code point
               from the UTF-8 sequence to be compared with the W3C range.

@param[in]       pu8Character : Pointer to the location of the character

@return         Length of the valid UTF-8 sequence

@note
***********************************************************************************************************************/
static INT8U LwXMLP_u8ValidCharacter(INT8U const *pu8Character)
{
#if(SUPPORT_ASCII_ONLY == STD_OFF)
    INT32U  u32CodePoint = 0U;
#endif
    INT8U   u8CharacterLength;

    if(*pu8Character <= 0x7FU)
    {
        if((*pu8Character == 0x09U) ||
           (*pu8Character == 0x0AU) ||
           (*pu8Character == 0x0DU) ||
           (*pu8Character >= 0x20U))
        {
            u8CharacterLength = 1U;
        }
        else
        {
            u8CharacterLength = 0U;
        }
    }
#if(SUPPORT_ASCII_ONLY == STD_OFF)
    else if((*pu8Character & 0xE0U) == 0xC0U) /* 110X XXXX  two *pu8Characterlength */
    {
        u8CharacterLength = 2U;
        u32CodePoint = (((INT32U)pu8Character[0] - 192U) * 64U) + \
                        ((INT32U)pu8Character[1U] - 128U);
    }
    else if((*pu8Character & 0xF0U) == 0xE0U) /* 1110 XXXX  three *pu8Characterlength */
    {
        u8CharacterLength = 3U;
        u32CodePoint = (((INT32U)pu8Character[0] - 224U) * 4096U) + \
                       (((INT32U)pu8Character[1U] - 128U) * 64U) + \
                       ((INT32U)pu8Character[2] - 128U);
    }
    else if((*pu8Character & 0xF8U) == 0xF0U) /* 1111 0XXX  four *pu8Characterlength */
    {
        u8CharacterLength = 4U;
        u32CodePoint  = (((INT32U)pu8Character[0] - 240U) * 262144U) + \
                        (((INT32U)pu8Character[1U] - 128U) * 4096U) + \
                        (((INT32U)pu8Character[2] - 128U) * 64U) + \
                        ((INT32U)pu8Character[3] - 128U);
    }
#endif
    else
    {
        u8CharacterLength = 0U;
    }
#if(SUPPORT_ASCII_ONLY == STD_OFF)
    if(u8CharacterLength > 1U)
    {
        if(((u32CodePoint >= 0x0080U) && (u32CodePoint <= 0xD7FFU))||\
           ((u32CodePoint >= 0xE000U) && (u32CodePoint <= 0xFFFDU))||\
           ((u32CodePoint >= 0x10000U) && (u32CodePoint <= 0x10FFFFU)))
        {
            /*
             * Nothing to do
             */
        }
        else
        {
            u8CharacterLength = 0U;
        }
    }
#endif

    return u8CharacterLength;
}
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_bIsLessThanCharacter                                        <LwXMLP_CORE>

SYNTAX:         BOOLEAN LwXMLP_bIsLessThanCharacter(INT8S *pu8Character)

DESCRIPTION :   Validate the character if it is acceptable character to be inside an XML file or not

PARAMETER1  :   s8Character : The received character

RETURN VALUE:   STD_TRUE : if valid XML character is received
                STD_FALSE: if invalid XML character is received

Note
END DESCRIPTION *******************************************************************************************************/
static BOOLEAN LwXMLP_bIsLessThanCharacter(INT8U const *pu8Character)
{
    BOOLEAN IsLessThanCharacter = STD_FALSE;

    if((INT8U)(*pu8Character) == LESS_THAN_ASCII_CHARACTER)
    {
        IsLessThanCharacter = STD_TRUE;
    }

    return IsLessThanCharacter;
}
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_bIsWhiteSpace                                        <LwXMLP_CORE>

SYNTAX:         BOOLEAN LwXMLP_bIsWhiteSpace(INT8S *pu8Character)

DESCRIPTION :   check if the received character shall be ignored or not

PARAMETER1  :   s8Character : The received character

RETURN VALUE:   STD_TRUE : if the character shall be ignored
                STD_FALSE: if the character shall not be ignored

Note        :   XML considers four characters to be whitespace: the carriage return (\r or ch(13)), the linefeed
                (\n or ch(10U)), the tab(\t), and the spacebar (' '). In XML documents, there are two types of
                whitespace:
                - Significant whitespace is part of the document content and should be preserved.
                - Insignificant whitespace is used when editing XML documents for readability. These whitespaces are
                  typically not intended for inclusion in the delivery of the document.

END DESCRIPTION *******************************************************************************************************/
static BOOLEAN LwXMLP_bIsWhiteSpace(INT8U const *pu8Character)
{
    BOOLEAN bIsWhiteSpace = STD_FALSE;

    if(((INT8U)(*pu8Character) == SPACE_ASCII_CHARACTER) ||\
       ((INT8U)(*pu8Character) == CARRIAGE_RETURN_ASCII_CHARACTER) || \
       ((INT8U)(*pu8Character) == LINE_FEED_ASCII_CHARACTER) || \
       ((INT8U)(*pu8Character) == HTAB_ASCII_CHARACTER))
    {
        bIsWhiteSpace = STD_TRUE;
    }

    return bIsWhiteSpace;
}
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_pu8GetCurrentCharacter                                        <LwXMLP_CORE>

SYNTAX:         INT8S LwXMLP_pu8GetCurrentCharacter(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)

DESCRIPTION :   get current character pointed by the XML instance

PARAMETER1  :   pstrParserInstanceType : pointer to the XML instance

RETURN VALUE:   the current character


Note        :

END DESCRIPTION *******************************************************************************************************/
static INT8U * LwXMLP_pu8GetCurrentCharacter(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    return (pstrParserInstanceType->pu8LastParserLocation);
}
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_vPointToNextCharacter                                        <LwXMLP_CORE>

SYNTAX:         void LwXMLP_vPointToNextCharacter(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)

DESCRIPTION :   move to the next character pointed by the XML instance

PARAMETER1  :   pstrParserInstanceType : pointer to the XML instance

RETURN VALUE:   the current character


Note        :

END DESCRIPTION *******************************************************************************************************/
static void LwXMLP_vConsumeCharacter(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    if(pstrParserInstanceType->bSecondLevelBufferEnabled == STD_TRUE)
    {
        *pstrParserInstanceType->pu8LastParserLocation = NULL_ASCII_CHARACTER;
        pstrParserInstanceType->pu8LastParserLocation++;
        pstrParserInstanceType->u32SecondLevelReadyAmount--;
        if(pstrParserInstanceType->u32SecondLevelReadyAmount == 0U)
        {
            pstrParserInstanceType->bSecondLevelBufferEnabled = STD_FALSE;
            pstrParserInstanceType->bParserSilentModeIsEnabled = STD_FALSE;
            pstrParserInstanceType->pu8LastParserLocation     = pstrParserInstanceType->pu8SavedLastParserLocation;
            pstrParserInstanceType->pu8CircularBufferEnd      = pstrParserInstanceType->pu8SavedCircularBufferEnd;
            if(pstrParserInstanceType->u32SecondLevelPathLength != 0U)
            {
                pstrParserInstanceType->objenumErrorType = csParsedEntityError;
            }
        }
        else
        {
            /*
             * Nothing to do
             */
        }
    }
    else
    {
        if((INT8U)(*pstrParserInstanceType->pu8LastParserLocation) == LINE_FEED_ASCII_CHARACTER)
        {
            pstrParserInstanceType->u32LineNumber++;
            pstrParserInstanceType->u32CoulmnNumber = 0;
        }
        else
        {
            pstrParserInstanceType->u32CoulmnNumber++;
        }
        LwXMLP_mVerbose(VERBOSE_LEVEL_DEBUGGING, \
                        "Consuming character %c",
                        *pstrParserInstanceType->pu8LastParserLocation);
        *pstrParserInstanceType->pu8LastParserLocation = NULL_ASCII_CHARACTER;
        pstrParserInstanceType->pu8LastParserLocation++;
        pstrParserInstanceType->u32ParsedLength++;
        if(pstrParserInstanceType->pu8CircularBufferEnd == pstrParserInstanceType->pu8LastParserLocation)
        {
            pstrParserInstanceType->pu8LastParserLocation = pstrParserInstanceType->u8XMLCircularBuffer;
        }
        else
        {
            /* nothing to do here */
        }
        pstrParserInstanceType->u32FreeLocations++;
    }
#else
    if((INT8U)(*pstrParserInstanceType->pu8LastParserLocation) == LINE_FEED_ASCII_CHARACTER)
    {
        pstrParserInstanceType->u32LineNumber++;
        pstrParserInstanceType->u32CoulmnNumber = 0;
    }
    else
    {
        pstrParserInstanceType->u32CoulmnNumber++;
    }
    if(pstrParserInstanceType->u32LineNumber == 169)
    {
        pstrParserInstanceType->u32LineNumber = 169;
    }
    LwXMLP_mVerbose(VERBOSE_LEVEL_DEBUGGING, \
                    "Consuming character %c",
                    *pstrParserInstanceType->pu8LastParserLocation);
    *pstrParserInstanceType->pu8LastParserLocation = NULL_ASCII_CHARACTER;
    pstrParserInstanceType->pu8LastParserLocation++;
    pstrParserInstanceType->u32ParsedLength++;
    if(pstrParserInstanceType->pu8CircularBufferEnd == pstrParserInstanceType->pu8LastParserLocation)
    {
        pstrParserInstanceType->pu8LastParserLocation = pstrParserInstanceType->u8XMLCircularBuffer;
    }
    else
    {
        /* nothing to do here */
    }
    pstrParserInstanceType->u32FreeLocations++;
#endif

}
static void LwXMLP_vCheckDeclerationEncoding(LwXMLP_strXMLParseInstanceType       *pstrParserInstanceType, \
                                             INT8U                          const *pu8MLPortionBuffer,\
                                             INT32U                                 u32PortionLengthLength)
{
    if(strncmp((INT8S const *)pu8MLPortionBuffer, "<?xml", strlen("<?xml")) == 0)
    {
        memcpy(pstrParserInstanceType->u8XMLworkingBuffer, pu8MLPortionBuffer, u32PortionLengthLength);
        pstrParserInstanceType->u32DirectiveLength = u32PortionLengthLength;
        LwXMLP_vCheckWellFormedDecl(pstrParserInstanceType);
    }
    else
    {
        /*
         * Nothign to do
         */
    }
}
static void LwXMLP_vConsumeCharacterBulk(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType, INT32U u32AdvancedLength)
{
    INT32U u32HeaderIndex;

    for(u32HeaderIndex = 0U; u32HeaderIndex < u32AdvancedLength; u32HeaderIndex++)
    {
        LwXMLP_vConsumeCharacter(pstrParserInstanceType);
    }
}
#if(ENABLE_DTD_RECEIPTION == STD_ON)
static void LwXMLP_vCheckDTDPortion(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    INT8U         u8EntityIndex;
    INT8U        *pu8ParameterEntity;
    INT8U  const *pu8EntityValue = STD_NULL;
    INT32U        u32EntityValueLength = 0U;
    BOOLEAN       bParameterEntities = STD_FALSE;
    BOOLEAN       bFoundGeneralEntities = STD_FALSE;
#endif
    INT8U        *pu8WorkingBuffer;
    INT8U  const *pu8CurrentXMLCharacter;
    INT32U         u32BufferIndex, u32CheckingDataAmount, u32BracketIndex = 0U;
    INT32U         u32DataLength = 0U;
    BOOLEAN        bFoundNonWhiteSpaceCharacter = STD_FALSE;
    BOOLEAN        bDTDEndIsFound = STD_FALSE, bNewMarkupIsFound = STD_FALSE;

    pu8WorkingBuffer                          = pstrParserInstanceType->u8XMLworkingBuffer;
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    pu8ParameterEntity                        = pu8WorkingBuffer; /* Initail assumption */
#endif
    pu8CurrentXMLCharacter                    = pstrParserInstanceType->pu8LastParserLocation;
    u32CheckingDataAmount                     = LwXMLP_u32GetReadyData(pstrParserInstanceType);
    pstrParserInstanceType->bEndOfDataIsFound = STD_FALSE;
    for(u32BufferIndex = 0U; \
       (u32BufferIndex < u32CheckingDataAmount) && \
       (pstrParserInstanceType->objenumErrorType == csNoError)&&\
       (bNewMarkupIsFound == STD_FALSE)&&\
       (u32BufferIndex < MAX_WORKING_BUFFER_SIZE); \
       ++u32BufferIndex)
    {
        if(*pu8CurrentXMLCharacter == LESS_THAN_ASCII_CHARACTER)
        {
            bNewMarkupIsFound = STD_TRUE;
        }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
        else if(((INT8U)*pu8CurrentXMLCharacter) == PERCENT_ASCII_CHARACTER)
        {
            bParameterEntities           = STD_TRUE;
            bFoundNonWhiteSpaceCharacter = STD_TRUE;
            pu8ParameterEntity           = pu8WorkingBuffer;
        }
        else if(((INT8U)*pu8CurrentXMLCharacter) == AMPERSAND_ASCII_CHARACTER)
        {
            bFoundGeneralEntities = STD_TRUE;
        }
        else if((((INT8U)*pu8CurrentXMLCharacter) == SEMICOLON_ASCII_CHARACTER) && (bParameterEntities == STD_TRUE))
        {
            bNewMarkupIsFound = STD_TRUE;
            *pu8WorkingBuffer = *pu8CurrentXMLCharacter;
            pu8WorkingBuffer++;
            u32DataLength++;
        }
        else if((((INT8U)*pu8CurrentXMLCharacter) == SEMICOLON_ASCII_CHARACTER) && (bFoundGeneralEntities == STD_TRUE))
        {
            pstrParserInstanceType->objenumErrorType = csGeneralEntityInDTD;
        }
#endif
        else if(((INT8U)*pu8CurrentXMLCharacter) == CLOSING_BRACKET_CHARACTER)
        {
            if(bDTDEndIsFound == STD_FALSE)
            {
                bDTDEndIsFound  = STD_TRUE;
                u32BracketIndex = u32BufferIndex;
            }
            else
            {
                pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
            }
        }
        else if(*pu8CurrentXMLCharacter == GREATER_THAN_ASCII_CHARACTER)
        {
            if(((u32BracketIndex + 1U) == u32BufferIndex) &&  (bDTDEndIsFound == STD_TRUE))
            {
                u32DataLength++;
            }
            else
            {
                pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
            }
            break;
        }
        else if(LwXMLP_bIsWhiteSpace(pu8CurrentXMLCharacter) == STD_FALSE)
        {
            bFoundNonWhiteSpaceCharacter = STD_TRUE;
        }
        else
        {
            /*
             * Nothing to do
             */
        }
        if(bNewMarkupIsFound == STD_FALSE)
        {
            *pu8WorkingBuffer = *pu8CurrentXMLCharacter;
            pu8WorkingBuffer++;
            u32DataLength++;
            pu8CurrentXMLCharacter++;
            if(pstrParserInstanceType->pu8CircularBufferEnd == pu8CurrentXMLCharacter)
            {
                pu8CurrentXMLCharacter = pstrParserInstanceType->u8XMLCircularBuffer;
            }
            else
            {
                /* nothing to do here */
            }
        }
        else
        {
            /*
             * Nothing to do
             */
        }
    }
    if((bDTDEndIsFound == STD_TRUE) && (pstrParserInstanceType->objenumErrorType == csNoError))
    {
        pstrParserInstanceType->bParsingDTDInProgress = STD_FALSE;
        LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, u32DataLength);
    }
    else if((bNewMarkupIsFound == STD_TRUE) && (pstrParserInstanceType->objenumErrorType == csNoError))
    {
        if(bFoundNonWhiteSpaceCharacter == STD_TRUE)
        {
            *pu8WorkingBuffer = NULL_ASCII_CHARACTER;
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
            /* Now check the Parameter Entities if any */
            if(bParameterEntities == STD_TRUE)
            {
                LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, u32DataLength);
                pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidatePEReference(\
                                                pstrParserInstanceType->u8XMLworkingBuffer, \
                                                &pstrParserInstanceType->u32Length);
                if(pstrParserInstanceType->objenumErrorType == csNoError)
                {
                    for(u8EntityIndex = 0; u8EntityIndex < pstrParserInstanceType->u8AvaliableEntities; ++u8EntityIndex)
                    {
                        if(strncmp((INT8S const *)&pu8ParameterEntity[1U], \
                                  (INT8S const *)pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityName, \
                                  pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityNameLength) == 0)
                        {
                            u32EntityValueLength   = pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityValueLength;
                            pu8EntityValue         = pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityValue;
                            break;
                        }
                    }
                    if(pu8EntityValue == STD_NULL)
                    {
                        pstrParserInstanceType->objenumErrorType = csInvalidParameterEntity;
                    }
                    else if(pstrParserInstanceType->objstrEntityType[u8EntityIndex].objenumEntityStoreTypeType == csExternalStorage)
                    {
                        pstrParserInstanceType->bFoundExternalEntity = STD_TRUE;
                    }
                    else
                    {
                        LwXMLP_vSwitchToSecondLevelBuffer(pstrParserInstanceType,\
                                                          pu8EntityValue,\
                                                          u32EntityValueLength);
                    }
                }
                else
                {
                    /*
                     * Nothing to do
                     */
                }
            }
            else
            {
                /* Nothing to do here */
            }
#else
            LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, u32DataLength);
#endif
        }
        else /* it is insignificant white space */
        {
            LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, u32DataLength);
            pstrParserInstanceType->bEndOfDataIsFound = STD_FALSE;
        }
    }
    else if((u32BufferIndex == MAX_CIRCULAR_BUFFER_SIZE) || (u32BufferIndex == MAX_WORKING_BUFFER_SIZE)) /* if we looked for character and isn't found and reach the buffer end */
    {
        pstrParserInstanceType->objenumErrorType = csLargeDTDPortionlength;
        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Can't load the XML Tag into the buffer");
    }
    else
    {
        /* unexpected */
    }
}
#endif
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_srCheckavailableData                                        <LwXMLP_CORE>

SYNTAX:         StdReturnType LwXMLP_srCheckavailableData(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType, \
                                                        BOOLEAN                      *pbEndOfDataFound)

DESCRIPTION :   This API is doing the following:
                - Check if the end of the data is available
                - Copy the data to the working buffer
                - Check if the data contains a predefined entity to be replaced

PARAMETER1  :   pstrParserInstanceType : pointer to the XML instance, and once the end of the data found, the
                                         pstrParserInstanceType->bEndOfDataIsFound will be set to STD_TRUE

PARAMETER2  :   pbEndOfDataFound       : pointer to boolean which will be set by the API, if the TAG is complete

RETURN VALUE:   E_NOT_OK: if the loading for the TAG into the working buffer is failed
                E_OK    : if the loading for the TAG into the working buffer is succeeded

Note        :   the API will fail to load the buffer in the following cases:
                - the Data length the larger than working buffer (MAX_WORKING_BUFFER_SIZE), and the error
                  (csLargeXMLProperty) will be set.

Note        :

END DESCRIPTION *******************************************************************************************************/
static void LwXMLP_vCheckavailableData(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    INT32U         u32ResolvingLength;
#endif
    INT8U         *pu8WorkingBuffer;
    INT8U   const *pu8CurrentXMLCharacter;
    INT32U         u32BufferIndex, u32CheckingDataAmount, u32DataLength;

    BOOLEAN        bFoundNonWhiteSpaceCharacter = STD_FALSE;
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    BOOLEAN        bFoundReference = STD_FALSE;
    INT8U         *pu8EntityValue = STD_NULL;
    INT8U         u8EntityReferenceUpdateTrials, u8EntityIndex, u8ReferenceIndex, u8LastReferencesCountFound = 0U;
#endif

    pu8WorkingBuffer                          = pstrParserInstanceType->u8XMLworkingBuffer;
    pu8CurrentXMLCharacter                    = pstrParserInstanceType->pu8LastParserLocation;
    u32CheckingDataAmount                     = LwXMLP_u32GetReadyData(pstrParserInstanceType);
    pstrParserInstanceType->bEndOfDataIsFound = STD_FALSE;
    u32DataLength                             = 0U;
    memset(pstrParserInstanceType->u8XMLworkingBuffer, 0x00, MAX_WORKING_BUFFER_SIZE);
    for(u32BufferIndex = 0U; \
       (u32BufferIndex < u32CheckingDataAmount) && (u32BufferIndex < MAX_WORKING_BUFFER_SIZE); \
       ++u32BufferIndex)
    {
        if(*pu8CurrentXMLCharacter == LESS_THAN_ASCII_CHARACTER)
        {
            pstrParserInstanceType->bEndOfDataIsFound = STD_TRUE;
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "End of Data is found");
            break;
        }

        else if(((INT8U)*pu8CurrentXMLCharacter) == AMPERSAND_ASCII_CHARACTER)
        {
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
            bFoundReference              = STD_TRUE;
            bFoundNonWhiteSpaceCharacter = STD_TRUE;
#else
            pstrParserInstanceType->objenumErrorType  = csFeatureNotSupported;
#endif
        }
        else if(LwXMLP_bIsWhiteSpace(pu8CurrentXMLCharacter) == STD_FALSE)
        {
            bFoundNonWhiteSpaceCharacter = STD_TRUE;
        }
        else
        {
            /*
             * Nothing to do
             */
        }
        *pu8WorkingBuffer = *pu8CurrentXMLCharacter;
        pu8WorkingBuffer++;
        u32DataLength++;
        pu8CurrentXMLCharacter++;
        if(pstrParserInstanceType->pu8CircularBufferEnd == pu8CurrentXMLCharacter)
        {
            pu8CurrentXMLCharacter = pstrParserInstanceType->u8XMLCircularBuffer;
        }
        else
        {
            /* nothing to do here */
        }
    }
    /*
     * Validate found data
     */
    if((pstrParserInstanceType->objenumErrorType == csNoError) && \
       (pstrParserInstanceType->bEndOfDataIsFound == STD_TRUE)&&\
       (bFoundNonWhiteSpaceCharacter == STD_TRUE))
    {
        if(LwXMLP_bValidateData(pstrParserInstanceType->u8XMLworkingBuffer, u32DataLength) == STD_FALSE)
        {
            pstrParserInstanceType->objenumErrorType = csInvalidXMLCharacter;
        }
        else
        {
            /*
             * Nothing to do
             */
        }
    }
    else
    {
        /*
         * Nothing to do
         */
    }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    if((pstrParserInstanceType->bEndOfDataIsFound == STD_TRUE) && \
       (pstrParserInstanceType->objenumErrorType == csNoError) && \
       (pstrParserInstanceType->bSecondLevelBufferEnabled == STD_FALSE))
#else
    if((pstrParserInstanceType->bEndOfDataIsFound == STD_TRUE) && \
       (pstrParserInstanceType->objenumErrorType == csNoError))
#endif
    {
        if(bFoundNonWhiteSpaceCharacter == STD_TRUE)
        {
            *pu8WorkingBuffer = NULL_ASCII_CHARACTER;
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
            if(bFoundReference == STD_TRUE)
            {
                /*
                 * Step 1: convert all entity references and check if entity is referring to itself
                 *         and check if the entity start in one domain and end in other one
                 */
                for(u8EntityReferenceUpdateTrials = 0; \
                    (u8EntityReferenceUpdateTrials < MAX_ENTITY_RESOLVING_DEPTH) && \
                    (pstrParserInstanceType->objenumErrorType == csNoError); \
                    ++u8EntityReferenceUpdateTrials)
                {
                    u32ResolvingLength = strlen((INT8S const *)pstrParserInstanceType->u8XMLworkingBuffer);
                    /* TODO: remove 40 */
                    u32ResolvingLength += 120U;
                    if(u32ResolvingLength < MAX_WORKING_BUFFER_SIZE)
                    {
                        pstrParserInstanceType->objenumErrorType = LwXMLP_enumUpdateEntityReference(pstrParserInstanceType, \
                                                                                                    pstrParserInstanceType->u8XMLworkingBuffer,\
                                                                                                    u32ResolvingLength,\
                                                                                                    UPDATE_ENTITY_REFERENCE | \
                                                                                                    DONT_DEREF_LT_GENERAL_ENTITY |\
                                                                                                    DONT_DEREF_AS_GENERAL_ENTITY);
                        if((pstrParserInstanceType->objenumErrorType == csMissingSemicolon) && (u8EntityReferenceUpdateTrials > 0U))
                        { /* may be & appears after second round, so it shouldn't consider an error */
                            pstrParserInstanceType->objenumErrorType = csNoError;
                        }
                        else
                        {
                            /*
                             * Nothing to do
                             */
                        }
                        if((pstrParserInstanceType->objenumErrorType == csInvalidReference) && (u8EntityReferenceUpdateTrials > 0U))
                        {/* may be the entity shouldn't be resolved, so it will be left to the next round */
                            pstrParserInstanceType->objenumErrorType = csNoError;
                        }
                        else
                        {
                            /*
                             * Nothing to do
                             */
                        }
                    }
                    else
                    {
                        pstrParserInstanceType->objenumErrorType = csInternalError;
                    }
                    if((pstrParserInstanceType->u8FoundReference > 0U) &&\
                       (pstrParserInstanceType->objenumErrorType == csNoError))
                    {
                        u8LastReferencesCountFound = pstrParserInstanceType->u8FoundReference;
                        /*
                        * check for any partial entity
                        */
                        for(u8ReferenceIndex = 0; \
                            (u8ReferenceIndex < pstrParserInstanceType->u8FoundReference) && \
                            (pstrParserInstanceType->objenumErrorType == csNoError); \
                            ++u8ReferenceIndex)
                        {
                            if(pstrParserInstanceType->objstrFoundReferenceType[u8ReferenceIndex].objenumReferenceTypeType == csEntityReference)
                            {
                                u8EntityIndex  = pstrParserInstanceType->objstrFoundReferenceType[u8ReferenceIndex].u8EntityIndex;
                                pu8EntityValue = pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityValue;
                                u32ResolvingLength = strlen((INT8S const *)pu8EntityValue);
                                if(u32ResolvingLength < MAX_ENTITY_VALUE_LENGTH)
                                {
                                    pstrParserInstanceType->objenumErrorType = LwXMLP_enumUpdateEntityReference(pstrParserInstanceType, \
                                                                                                                pu8EntityValue,\
                                                                                                                u32ResolvingLength,\
                                                                                                                REFERENCE_VALIDATEION_ONLY|\
                                                                                                                UPDATE_CHARACTER_REFERENCE | \
                                                                                                                DONT_DEREF_LT_GENERAL_ENTITY |\
                                                                                                                DONT_DEREF_AS_GENERAL_ENTITY);
                                    if(pstrParserInstanceType->objenumErrorType == csMissingSemicolon)
                                    {
                                        /*
                                         * Chnage the error type
                                         */
                                        pstrParserInstanceType->objenumErrorType = csPartialCharacterReference;
                                    }
                                    else
                                    {
                                        /*
                                         * Nothing to do
                                         */
                                    }
                                }
                                else
                                {
                                    pstrParserInstanceType->objenumErrorType = csInternalError;
                                }
                            }
                            else
                            {
                                /*
                                 * Nothing to do
                                 */
                            }
                        }
                        pstrParserInstanceType->u8FoundReference = u8LastReferencesCountFound;
                    }
                    else
                    {
                        break;
                    }
                }
                if((u8LastReferencesCountFound > 0U) && (pstrParserInstanceType->objenumErrorType == csNoError))
                {
                    for(u8ReferenceIndex = 0U; \
                        (u8ReferenceIndex < u8LastReferencesCountFound) && \
                        (pstrParserInstanceType->objenumErrorType == csNoError); \
                        ++u8ReferenceIndex)
                    {
                        if(pstrParserInstanceType->objstrFoundReferenceType[u8ReferenceIndex].objenumReferenceTypeType == csEntityReference)
                        {   /* check that the found reference had been checked before if it is began in one entity and end in another */
                            /* Rule 4.3.2 */
                            u8EntityIndex  = pstrParserInstanceType->objstrFoundReferenceType[u8ReferenceIndex].u8EntityIndex;
                            pu8EntityValue = pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityValue;
                            if(pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityValueLength > 0U)
                            {
                                /* check if the still have unresolved entity, it may referee to itself */
                                if(pstrParserInstanceType->u8FoundReference > 0U)
                                {
                                    if(strstr((INT8S const *)pstrParserInstanceType->u8XMLworkingBuffer, (INT8S const *)pu8EntityValue) != STD_NULL)
                                    {
                                        pstrParserInstanceType->objenumErrorType = csEntityRefereToItself;
                                    }
                                    else
                                    {
                                        pstrParserInstanceType->objenumErrorType = csLargeResolvingTrialsProperty;
                                    }
                                }
                                else
                                {
                                    if(pstrParserInstanceType->u32LastReferenceLine != pstrParserInstanceType->u32LineNumber)
                                    {/* entity is not validated before */
                                        pstrParserInstanceType->u32LastReferenceLine       = pstrParserInstanceType->u32LineNumber;
                                        pstrParserInstanceType->bParserSilentModeIsEnabled = STD_TRUE;
                                        LwXMLP_vSwitchToSecondLevelBuffer(pstrParserInstanceType,\
                                                                          pu8EntityValue,\
                                                                          strlen((INT8S const *)pu8EntityValue));
                                    }
                                    else /* entity is validated before in the content and ready for Parsing */
                                    {
                                        pstrParserInstanceType->bParserSilentModeIsEnabled = STD_FALSE;
                                        LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, u32BufferIndex);
                                        LwXMLP_vSwitchToSecondLevelBuffer(pstrParserInstanceType,\
                                                                          pstrParserInstanceType->u8XMLworkingBuffer,\
                                                                          strlen((INT8S const *)pstrParserInstanceType->u8XMLworkingBuffer));
                                    }
                                    break;
                                }
                            }
                            else
                            {
                                /*
                                 * Nothing to do
                                 */
                            }
                        }
                        else
                        {
                            /*
                             * Nothing to do
                             */
                        }
                    }
                }
                else /* there is no more references */
                {
                    /*TODO: erro */
                }
                /*
                 * Step 2: update all character reference
                 */
                if((pstrParserInstanceType->objenumErrorType == csNoError) && (pstrParserInstanceType->bSecondLevelBufferEnabled == STD_FALSE))
                {
                    u32ResolvingLength = strlen((INT8S const *)pstrParserInstanceType->u8XMLworkingBuffer);
                    if(u32ResolvingLength < MAX_WORKING_BUFFER_SIZE)
                    {
                        pstrParserInstanceType->objenumErrorType = LwXMLP_enumUpdateEntityReference(pstrParserInstanceType, \
                                                                                                    pstrParserInstanceType->u8XMLworkingBuffer,\
                                                                                                    u32ResolvingLength,\
                                                                                                    UPDATE_CHARACTER_REFERENCE |\
                                                                                                    DONT_DEREF_LT_GENERAL_ENTITY |\
                                                                                                    DONT_DEREF_AS_GENERAL_ENTITY);
                    }
                    else
                    {
                        pstrParserInstanceType->objenumErrorType = csInternalError;
                    }
                }
                else
                {
                    /*
                     * Nothing to do
                     */
                }
                if(pstrParserInstanceType->bSecondLevelBufferEnabled == STD_TRUE)
                {
                    pstrParserInstanceType->bEndOfDataIsFound = STD_FALSE;
                }
                else
                {/* we prevented the dereference for the &lt during the validation, so it is time to be referenced before reporting */
                    if(pstrParserInstanceType->objenumErrorType == csNoError)
                    {
                        u32ResolvingLength = strlen((INT8S const *)pstrParserInstanceType->u8XMLworkingBuffer);
                        if(u32ResolvingLength < MAX_WORKING_BUFFER_SIZE)
                        {
                            pstrParserInstanceType->objenumErrorType = LwXMLP_enumUpdateEntityReference(pstrParserInstanceType, \
                                                                                                        pstrParserInstanceType->u8XMLworkingBuffer,\
                                                                                                        u32ResolvingLength,\
                                                                                                        UPDATE_ENTITY_REFERENCE);
                            if(pstrParserInstanceType->objenumErrorType == csMissingSemicolon)
                            { /* may be & appears due to character entity */
                                pstrParserInstanceType->objenumErrorType = csNoError;
                            }
                            else
                            {
                                /*
                                 * Nothing to do
                                 */
                            }
                        }
                        else
                        {
                            pstrParserInstanceType->objenumErrorType = csInternalError;
                        }
                    }
                    else
                    {
                        /*
                         * Nothing to do
                         */
                    }
                }
            }
            else /* no available references */
            {
                /*
                 * Nothing to do
                 */
            }
#endif
            if((pstrParserInstanceType->objenumErrorType == csNoError) && (pstrParserInstanceType->bEndOfDataIsFound == STD_TRUE))
            {
                LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found Data %s", pstrParserInstanceType->u8XMLworkingBuffer);
                pstrParserInstanceType->pu8ParsedDataBuffer = &pstrParserInstanceType->u8XMLworkingBuffer[0];
                LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, u32BufferIndex); /* consume the read data */
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }

        }
        else
        {
            if(pstrParserInstanceType->objenumXMLEventTypes == csCDataEvent) /* If we already reported with CDATA, so it is significant whitespace */
            {
                *pu8WorkingBuffer                           = NULL_ASCII_CHARACTER;
                pstrParserInstanceType->pu8ParsedDataBuffer = &pstrParserInstanceType->u8XMLworkingBuffer[0];
                LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, u32DataLength);
            }
            else/* it is insignificant white space, so it shouldn't reported as a data */
            {
                LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, u32DataLength);
                pstrParserInstanceType->bEndOfDataIsFound = STD_FALSE;
            }
        }
    }
    else if((u32BufferIndex == MAX_CIRCULAR_BUFFER_SIZE) || (u32BufferIndex == MAX_WORKING_BUFFER_SIZE)) /* if we looked for character and isn't found and reach the buffer end */
    {
        pstrParserInstanceType->objenumErrorType = csLargeDatalength;
        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Can't load the XML Tag into the buffer");
    }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    else if(pstrParserInstanceType->bSecondLevelBufferEnabled == STD_TRUE)/* the second level buffer contain data */
    {
        *pu8WorkingBuffer = NULL_ASCII_CHARACTER;
        u32ResolvingLength = strlen((INT8S const *)pstrParserInstanceType->u8XMLworkingBuffer);
        if(u32ResolvingLength < MAX_WORKING_BUFFER_SIZE)
        {
            pstrParserInstanceType->objenumErrorType = LwXMLP_enumUpdateEntityReference(pstrParserInstanceType, \
                                                                                        pstrParserInstanceType->u8XMLworkingBuffer,\
                                                                                        u32ResolvingLength,\
                                                                                        UPDATE_ENTITY_REFERENCE | UPDATE_CHARACTER_REFERENCE);
            if(pstrParserInstanceType->objenumErrorType == csNoError)
            {
                u32DataLength = strlen((INT8S const *)pstrParserInstanceType->u8XMLworkingBuffer);
                if(LwXMLP_bValidateData(pstrParserInstanceType->u8XMLworkingBuffer, u32DataLength) == STD_FALSE)
                {
                    pstrParserInstanceType->objenumErrorType = csInvalidXMLCharacter;
                }
                else
                {
                    LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found Data %s", pstrParserInstanceType->u8XMLworkingBuffer);
                    pstrParserInstanceType->pu8ParsedDataBuffer = &pstrParserInstanceType->u8XMLworkingBuffer[0];
                    /*
                     * if we have data in non silent mode, we have to report
                     */
                    if(pstrParserInstanceType->bParserSilentModeIsEnabled == STD_FALSE)
                    {
                        pstrParserInstanceType->bEndOfDataIsFound = STD_TRUE;
                    }
                    else
                    {
                        /*
                         * Nothing to do
                         */
                    }
                    LwXMLP_vConsumeCharacterBulk(pstrParserInstanceType, u32BufferIndex); /* consume the read data */
                }
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
        }
        else
        {
            pstrParserInstanceType->objenumErrorType = csInternalError;
        }
    }
#endif
    else
    {
        /* unexpected */
    }
}
/*
 * search for ]]>

 */
static BOOLEAN LwXMLP_bValidateData(INT8U const *pu8DataString, INT32U u32dataLength)
{
    BOOLEAN bValidData = STD_TRUE;
    INT32U  u32DataIndex, u32CharacterLength;

    for(u32DataIndex = 0U; \
        (u32DataIndex < u32dataLength) && (bValidData == STD_TRUE);\
        u32DataIndex += u32CharacterLength)
    {
        if(u32dataLength >= 3U)
        {
            if(pu8DataString[u32DataIndex] == CLOSING_BRACKET_CHARACTER)
            {
                if(pu8DataString[u32DataIndex + 1U] == CLOSING_BRACKET_CHARACTER)
                {
                    if(pu8DataString[u32DataIndex + 2U] == GREATER_THAN_ASCII_CHARACTER)
                    {
                        bValidData = STD_FALSE;
                    }
                    else
                    {
                        /*
                         * Nothing to do
                         */
                    }
                }
                else
                {
                    /*
                     * Nothing to do
                     */
                }
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
        }
        u32CharacterLength = LwXMLP_u8ValidCharacter(&pu8DataString[u32DataIndex]);
        if(u32CharacterLength == 0U)
        {
            bValidData = STD_FALSE;
        }
        else
        {
            /*
             * Nothing to do
             */
        }
    }

    return bValidData;
}
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_enumUpdateEntityReference                                        <LwXMLP_CORE>

SYNTAX:         LwXMLP_enumErrorType LwXMLP_enumUpdateEntityReference(NT8U *pu8BufferString)

DESCRIPTION :   this API responsible to search in the working buffer for the

PARAMETER1  :   pstrParserInstanceType : pointer to the buffer that contains the Entity Reference

RETURN VALUE:


Note        :   this API assume that the (& ;) are available if the & is only

END DESCRIPTION *******************************************************************************************************/
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
static LwXMLP_enumErrorType LwXMLP_enumUpdateEntityReference(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType, \
                                                                    INT8U                          *pu8BufferString,\
                                                                    INT32U                          u32MaxBlockLength,\
                                                                    INT32U                          u32UpdateOptionMask)
{
    typedef enum
    {
        csCheckSemicolonDelimiterState,
        csCheckAmpersandDelimiterState
    }enumEntityUpdateStateType;
                 BOOLEAN                   bStopChecking = STD_FALSE;
    static const INT8S                     cs8SmallerCharEntity[]          = "lt";
    static const INT8S                     cs8GreaterCharEntity[]          = "gt";
    static const INT8S                     cs8AmpersandCharEntity[]        = "amp";
    static const INT8S                     cs8SingleQuoteCharEntity[]      = "apos";
    static const INT8S                     cs8DoubleQuoteCharEntity[]      = "quot";
                 INT8U                    *pu8AmpersandDelimiter, *pu8TempString, u8UTF8CodeLength, u8EntityIndex;
                 INT8U   const            *pu8EntityValue;
                 INT32U                    u32CheckedLength = 0U, u32EntityValueLength = 0U, u32EntityNameLength = 0U;
                 INT32U                    u32TempUniCode = 0U, u32FinalUniCode = 0U, u32StringLength, u32MoveLength;
                 LwXMLP_enumErrorType      objenumErrorType = csNoError; /* Assume no error */
                 enumEntityUpdateStateType objenumEntityUpdateStateType = csCheckAmpersandDelimiterState;
                 LwXMLP_enumReferenceTypeType objenumReferenceTypeType = csUnkownEntity;

    pstrParserInstanceType->u8FoundReference = 0U;
    pu8AmpersandDelimiter                    = pu8BufferString; /*Initial value */
    u32StringLength                          = strlen((INT8S const *)pu8BufferString); /*Initial value */
    pu8TempString                            = pu8BufferString;
    do
    {
        switch(objenumEntityUpdateStateType)
        {
            case csCheckAmpersandDelimiterState:
                 if(*pu8TempString == AMPERSAND_ASCII_CHARACTER)
                 {
                     u32EntityNameLength   = 0U;/* Initialize to calculate the code length */
                     u32FinalUniCode       = 0U;
                     pu8AmpersandDelimiter = pu8TempString;
                     if(pu8AmpersandDelimiter[1U] == HASH_ASCII_CHARACTER)
                     {
                         if(pu8AmpersandDelimiter[2] == X_SMALL_ASCII_CHARACTER) /* Character reference in hexadecimal*/
                         {
                             objenumReferenceTypeType  = csHexCharcterReference;
                             pu8TempString             = &pu8TempString[3];
                             u32CheckedLength         += 3U;
                             u32EntityNameLength      += 4U; /* Add the &#x ; */
                         }
                         else
                         {
                             objenumReferenceTypeType  = csDecimalCharcterReference;
                             pu8TempString             = &pu8TempString[2];
                             u32CheckedLength         += 2U;
                             u32EntityNameLength      += 3U; /* Add the &# ; */
                         }
                     }
                     else
                     {
                         objenumReferenceTypeType = csEntityReference;
                         pu8TempString++;
                         u32CheckedLength++;
                     }
                     if(((u32UpdateOptionMask & REFERENCE_VALIDATEION_ONLY) != 0U)||
                        (((u32UpdateOptionMask & UPDATE_ENTITY_REFERENCE) != 0U) && \
                                (objenumReferenceTypeType == csEntityReference))||
                        (((u32UpdateOptionMask & UPDATE_PARAMETER_REFERENCE) != 0U) && \
                                (objenumReferenceTypeType == csEntityReference))||
                        (((u32UpdateOptionMask & UPDATE_DECIMAL_CHAR_REFERENCE) != 0U) && \
                                (objenumReferenceTypeType == csDecimalCharcterReference))||
                        (((u32UpdateOptionMask & UPDATE_HEX_CHARACTER_REFERENCE) != 0U) && \
                                (objenumReferenceTypeType == csHexCharcterReference))||
                        (((u32UpdateOptionMask & UPDATE_CHARACTER_REFERENCE) != 0U) && \
                                ((objenumReferenceTypeType == csDecimalCharcterReference) || \
                                        (objenumReferenceTypeType == csHexCharcterReference))))
                     {
                         objenumEntityUpdateStateType = csCheckSemicolonDelimiterState;
                     }
                     else
                     {
                         /*
                          * Nothing to do
                          */
                     }

                 }
                 else if(*pu8TempString == PERCENT_ASCII_CHARACTER)
                 {
                     objenumReferenceTypeType = csParamterEntity;
                     pu8TempString++;
                     u32CheckedLength++;
                 }
                 else
                 {
                     u32CheckedLength++;
                     pu8TempString++;
                 }
                 break;
            case csCheckSemicolonDelimiterState:
                 if(*pu8TempString == SEMICOLON_ASCII_CHARACTER)
                 { /* reach end of the reference and we are ready for substitution Original */
                     if(LwXMLP_bValideEntity(pu8AmpersandDelimiter) == STD_TRUE)
                     {
                         if(objenumReferenceTypeType == csEntityReference)
                         {
                             pu8EntityValue = STD_NULL;
                             if(strncmp((INT8S const *)&pu8AmpersandDelimiter[1U], cs8SmallerCharEntity, u32EntityNameLength) == 0)
                             {
                                 if(((u32UpdateOptionMask & DONT_DEREF_LT_GENERAL_ENTITY) != 0U) ||
                                    (((u32UpdateOptionMask & UPDATE_ENTITY_REFERENCE) != UPDATE_ENTITY_REFERENCE)&&
                                    ((u32UpdateOptionMask & REFERENCE_VALIDATEION_ONLY) != REFERENCE_VALIDATEION_ONLY)))
                                 {
                                     pu8TempString++; /* escape semicolomn */
                                 }
                                 else
                                 {
                                     pu8EntityValue = (INT8U const*)"<";
                                     u32EntityValueLength   = 1U;
                                     if(pstrParserInstanceType->u8FoundReference < MAX_FOUND_REFERENCE)
                                     {
                                         pstrParserInstanceType->objstrFoundReferenceType[pstrParserInstanceType->u8FoundReference].objenumReferenceTypeType = csCharacterEntityReference;
                                         pstrParserInstanceType->objstrFoundReferenceType[pstrParserInstanceType->u8FoundReference].u8EntityIndex = 0U;
                                         pstrParserInstanceType->u8FoundReference++;
                                     }
                                     else
                                     {
                                         /*
                                          * Nothing to do
                                          */
                                     }
                                 }
                             }
                             else if(strncmp((INT8S const *)&pu8AmpersandDelimiter[1U], cs8GreaterCharEntity, u32EntityNameLength) == 0)
                             {
                                 if(((u32UpdateOptionMask & UPDATE_ENTITY_REFERENCE) != 0U) || \
                                    ((u32UpdateOptionMask & REFERENCE_VALIDATEION_ONLY) != 0U))
                                 {
                                     pu8EntityValue = (INT8U const *)">";
                                     u32EntityValueLength   = 1U;
                                     if(pstrParserInstanceType->u8FoundReference < MAX_FOUND_REFERENCE)
                                     {
                                         pstrParserInstanceType->objstrFoundReferenceType[pstrParserInstanceType->u8FoundReference].objenumReferenceTypeType = csCharacterEntityReference;
                                         pstrParserInstanceType->objstrFoundReferenceType[pstrParserInstanceType->u8FoundReference].u8EntityIndex = 0U;
                                         pstrParserInstanceType->u8FoundReference++;
                                     }
                                     else
                                     {
                                         /*
                                          * Nothing to do
                                          */
                                     }
                                 }
                                 else
                                 {
                                     pu8TempString++; /* escape semicolomn */
                                 }
                             }
                             else if(strncmp((INT8S const *)&pu8AmpersandDelimiter[1U], cs8AmpersandCharEntity, u32EntityNameLength) == 0)
                             {
                                 if((((u32UpdateOptionMask & UPDATE_ENTITY_REFERENCE) != 0U) ||
                                    ((u32UpdateOptionMask & REFERENCE_VALIDATEION_ONLY) != 0U)) &&
                                    ((u32UpdateOptionMask & DONT_DEREF_AS_GENERAL_ENTITY) != DONT_DEREF_AS_GENERAL_ENTITY))
                                 {
                                     pu8EntityValue       = (INT8U const *)"&";
                                     u32EntityValueLength = 1U;
                                     if(pstrParserInstanceType->u8FoundReference < MAX_FOUND_REFERENCE)
                                     {
                                         pstrParserInstanceType->objstrFoundReferenceType[pstrParserInstanceType->u8FoundReference].objenumReferenceTypeType = csCharacterEntityReference;
                                         pstrParserInstanceType->objstrFoundReferenceType[pstrParserInstanceType->u8FoundReference].u8EntityIndex = 0U;
                                         pstrParserInstanceType->u8FoundReference++;
                                     }
                                     else
                                     {
                                         /*
                                          * Nothing to do
                                          */
                                     }
                                 }
                                 else
                                 {
                                     pu8TempString++; /* escape semicolomn */
                                 }
                             }
                             else if(strncmp((INT8S const *)&pu8AmpersandDelimiter[1U], cs8SingleQuoteCharEntity, u32EntityNameLength) == 0)
                             {
                                 if(((u32UpdateOptionMask & UPDATE_ENTITY_REFERENCE) != 0U) || \
                                    ((u32UpdateOptionMask & REFERENCE_VALIDATEION_ONLY) != 0U))
                                 {
                                     pu8EntityValue       = (INT8U const *)"'";
                                     u32EntityValueLength = 1U;
                                     if(pstrParserInstanceType->u8FoundReference < MAX_FOUND_REFERENCE)
                                     {
                                         pstrParserInstanceType->objstrFoundReferenceType[pstrParserInstanceType->u8FoundReference].objenumReferenceTypeType = csCharacterEntityReference;
                                         pstrParserInstanceType->objstrFoundReferenceType[pstrParserInstanceType->u8FoundReference].u8EntityIndex = 0U;
                                         pstrParserInstanceType->u8FoundReference++;
                                     }
                                     else
                                     {
                                         /*
                                          * Nothing to do
                                          */
                                     }
                                 }
                                 else
                                 {
                                     pu8TempString++; /* escape semicolomn */
                                 }
                             }
                             else if(strncmp((INT8S const *)&pu8AmpersandDelimiter[1U], cs8DoubleQuoteCharEntity, u32EntityNameLength) == 0)
                             {
                                 if((((u32UpdateOptionMask & UPDATE_ENTITY_REFERENCE) != 0U)||
                                    ((u32UpdateOptionMask & REFERENCE_VALIDATEION_ONLY) != 0U)))
                                 {
                                     pu8EntityValue       = (INT8U const *)"\"";
                                     u32EntityValueLength = 1U;
                                     if(pstrParserInstanceType->u8FoundReference < MAX_FOUND_REFERENCE)
                                     {
                                         pstrParserInstanceType->objstrFoundReferenceType[pstrParserInstanceType->u8FoundReference].objenumReferenceTypeType = csCharacterEntityReference;
                                         pstrParserInstanceType->objstrFoundReferenceType[pstrParserInstanceType->u8FoundReference].u8EntityIndex = 0U;
                                         pstrParserInstanceType->u8FoundReference++;
                                     }
                                     else
                                     {
                                         /*
                                          * Nothing to do
                                          */
                                     }
                                 }
                                 else
                                 {
                                     pu8TempString++; /* escape semicolomn */
                                 }
                             }
                             else if(((u32UpdateOptionMask & UPDATE_ENTITY_REFERENCE) != 0U) || \
                                     ((u32UpdateOptionMask & UPDATE_PARAMETER_REFERENCE) != 0U))
                             {
                                 for(u8EntityIndex = 0; u8EntityIndex < pstrParserInstanceType->u8AvaliableEntities; ++u8EntityIndex)
                                 {
                                     if(strncmp((INT8S const *)&pu8AmpersandDelimiter[1U], \
                                                (INT8S const *)pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityName, \
                                                 u32EntityNameLength) == 0)
                                     {
                                         if(u32EntityNameLength == pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityNameLength)
                                         {
                                             if(((pstrParserInstanceType->objstrEntityType[u8EntityIndex].objenumReferenceTypeType == csEntityReference) &&
                                                 ((u32UpdateOptionMask & UPDATE_ENTITY_REFERENCE) != 0U)) ||
                                                 ((pstrParserInstanceType->objstrEntityType[u8EntityIndex].objenumReferenceTypeType == csParamterEntity) &&
                                                 ((u32UpdateOptionMask & UPDATE_PARAMETER_REFERENCE) != 0U)))
                                             {
                                                 u32EntityValueLength   = pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityValueLength;
                                                 pu8EntityValue         = pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityValue;
                                                 objenumReferenceTypeType = csMixedContent;
                                                 break;
                                             }
                                             else
                                             {
                                                 /*
                                                  * Nothing to do
                                                  */
                                             }
                                         }
                                         else
                                         {
                                             /*
                                              * Nothing to do
                                              */
                                         }
                                     }
                                     else
                                     {
                                         /*
                                          * Nothing to do
                                          */
                                     }
                                 }
                                 /* if we reach the end without finding reference */
                                 if(u8EntityIndex == pstrParserInstanceType->u8AvaliableEntities)
                                 {
                                     if((u32UpdateOptionMask & UPDATE_PARAMETER_REFERENCE) != 0U)
                                     {

                                     }
                                     else
                                     {
                                         objenumErrorType = csInvalidReference;
                                     }
                                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Character Reference");
                                 }
                                 else
                                 {
                                     if(pstrParserInstanceType->u8FoundReference < MAX_FOUND_REFERENCE)
                                     {
                                         pstrParserInstanceType->objstrFoundReferenceType[pstrParserInstanceType->u8FoundReference].objenumReferenceTypeType = \
                                                 pstrParserInstanceType->objstrEntityType[u8EntityIndex].objenumReferenceTypeType;
                                         pstrParserInstanceType->objstrFoundReferenceType[pstrParserInstanceType->u8FoundReference].u8EntityIndex = \
                                                 u8EntityIndex;
                                         pstrParserInstanceType->u8FoundReference++;
                                     }
                                     else
                                     {
                                         /*
                                          * Nothing to do
                                          */
                                     }
                                 }
                             }
                             else
                             {
                                 pu8TempString++; /* escape semicolomn */
                             }
                             if((objenumErrorType == csNoError) && (pu8EntityValue != STD_NULL) && ((u32UpdateOptionMask & REFERENCE_VALIDATEION_ONLY) == 0U))
                             {
                                 if(objenumReferenceTypeType == csUnkownEntity)
                                 {
                                     objenumReferenceTypeType = csPredefinedCharacterEntity;
                                 }
                                 else
                                 {
                                     /*
                                      * Nothing to do
                                      */
                                 }
                                 u32EntityNameLength += 2U; /* Add the & ; */
                                 if(u32EntityNameLength >=  u32EntityValueLength)
                                 {
                                     LwXMLP_vMoveBlockLeft(&pu8AmpersandDelimiter[u32EntityValueLength], \
                                                             u32EntityNameLength - u32EntityValueLength,\
                                                             u32MaxBlockLength - u32CheckedLength);
                                 }
                                 else
                                 {
                                     if(((u32MaxBlockLength - u32CheckedLength)< u32MaxBlockLength) &&
                                         ((u32CheckedLength + (u32EntityValueLength - u32EntityNameLength)) < u32MaxBlockLength))
                                     {
                                         LwXMLP_vMoveBlockRight(&pu8AmpersandDelimiter[u32EntityNameLength],\
                                                                u32EntityValueLength - u32EntityNameLength,\
                                                                u32MaxBlockLength - u32CheckedLength);
                                     }
                                     else
                                     {
                                         objenumErrorType = csInternalError;
                                     }
                                 }
                                 pu8TempString = &pu8AmpersandDelimiter[u32EntityValueLength]; /*Adjust the pointer to the new string */
                                 memcpy(pu8AmpersandDelimiter, pu8EntityValue, u32EntityValueLength);
                             }
                             else
                             {
                                 /*
                                  * Nothing to do
                                  */
                             }
                         }
                         else
                         {
                             if((u32UpdateOptionMask & REFERENCE_VALIDATEION_ONLY) == 0U)
                             {
                                 u8UTF8CodeLength = LwXMLP_u8ConvertCodePointToUTF8(u32FinalUniCode, pu8AmpersandDelimiter);
                                 if(u8UTF8CodeLength > 0U)
                                 {
                                     if(LwXMLP_u8ValidCharacter(pu8AmpersandDelimiter) > 0U)
                                     {
                                         u32MoveLength = u32EntityNameLength - u8UTF8CodeLength;
                                         if(((u32MaxBlockLength - u32CheckedLength)< u32MaxBlockLength) &&
                                             (u32MoveLength < u32MaxBlockLength))
                                         {
                                             LwXMLP_vMoveBlockLeft(&pu8AmpersandDelimiter[u8UTF8CodeLength], \
                                                                    u32MoveLength,\
                                                                    u32MaxBlockLength - u32CheckedLength);
                                             pu8TempString = &pu8AmpersandDelimiter[u8UTF8CodeLength]; /*Adjust the pointer to the new string */
                                             if(pstrParserInstanceType->u8FoundReference < MAX_FOUND_REFERENCE)
                                             {
                                                 pstrParserInstanceType->objstrFoundReferenceType[pstrParserInstanceType->u8FoundReference].objenumReferenceTypeType = csCharacterReference;
                                                 pstrParserInstanceType->objstrFoundReferenceType[pstrParserInstanceType->u8FoundReference].u8EntityIndex            = 0U;
                                                 pstrParserInstanceType->u8FoundReference++;
                                             }
                                             else
                                             {
                                                 /*
                                                  * Nothing to do
                                                  */
                                             }
                                         }
                                         else
                                         {
                                             objenumErrorType = csInternalError;
                                         }
                                     }
                                     else
                                     {
                                         objenumErrorType = csInvalidXMLCharacter;
                                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid XML character");
                                     }
                                 }
                                 else
                                 {
                                     objenumErrorType = csInvalidReference;
                                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Character Reference");
                                 }
                             }
                             else
                             {
                                 /*
                                  * Nothing to do
                                  */
                             }
                         }
                         objenumEntityUpdateStateType = csCheckAmpersandDelimiterState;
                         u32CheckedLength++;
                     }
                     else
                     {
                         objenumErrorType = csInvalidReference;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Character Reference");
                     }
                 }
                 else
                 {/* update the code per character for the hex and decimal type */
                     if(objenumReferenceTypeType == csHexCharcterReference)
                     {
                         if((*pu8TempString >= ZERO_ASCII_CHARACTER) && (*pu8TempString <= NINE_ASCII_CHARACTER))
                         {
                             u32TempUniCode = (INT32U)(*pu8TempString) - ZERO_ASCII_CHARACTER;
                         }
                         else if((*pu8TempString >= A_SMALL_ASCII_CHARACTER) && (*pu8TempString <= F_SMALL_ASCII_CHARACTER))
                         {
                              u32TempUniCode = ((INT32U)(*pu8TempString) - A_SMALL_ASCII_CHARACTER) + 10U;
                         }
                         else if((*pu8TempString >= A_CAPITAL_ASCII_CHARACTER) && (*pu8TempString <= F_CAPITAL_ASCII_CHARACTER))
                         {
                             u32TempUniCode = ((INT32U)(*pu8TempString) - A_CAPITAL_ASCII_CHARACTER) + 10U;
                         }
                         else
                         {
                             if((INT8U)(*pu8TempString) == NULL_ASCII_CHARACTER) /* we reach the end without semicolon */
                             {
                                 objenumErrorType = csMissingSemicolon;
                             }
                             else
                             {
                                 objenumErrorType = csInvalidReference;
                                 LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Character Reference");
                             }
                         }
                         if(objenumErrorType == csNoError)
                         {/* check for 32bit integer overflow */
                             if(u32FinalUniCode <= (u32FinalUniCode * 16U))
                             {
                                 u32FinalUniCode *= 16U;
                                 u32FinalUniCode += u32TempUniCode;
                             }
                             else
                             {
                                 objenumErrorType = csInvalidReference;
                             }
                         }
                         else
                         {
                             /* nothing to do */
                         }
                     }
                     else if(objenumReferenceTypeType == csDecimalCharcterReference)
                     {
                         if(((INT8U)(*pu8TempString) >= ZERO_ASCII_CHARACTER) && \
                             ((INT8U)(*pu8TempString) <= NINE_ASCII_CHARACTER))
                         {
                             u32TempUniCode = (INT32U)(*pu8TempString) - ZERO_ASCII_CHARACTER;
                         }
                         else
                         {
                             if((INT8U)(*pu8TempString) == NULL_ASCII_CHARACTER) /* we reach the end without semicolon */
                             {
                                 objenumErrorType = csMissingSemicolon;
                             }
                             else
                             {
                                 objenumErrorType = csInvalidReference;
                                 LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Character Reference");
                             }
                         }
                         if(objenumErrorType == csNoError)
                         {/* check for 32bit integer overflow */
                             if(u32FinalUniCode <= (u32FinalUniCode * 10U))
                             {
                                 u32FinalUniCode *= 10U;
                                 u32FinalUniCode += u32TempUniCode;
                             }
                             else
                             {
                                 objenumErrorType = csInvalidReference;
                             }
                         }
                     }
                     else
                     {
                         /*
                          * Nothing to do
                          */
                     }
                     pu8TempString++;
                     u32CheckedLength++;
                     u32EntityNameLength++;
                 }
                 break;
            default:
                 /*
                  * Shouldn't be here
                  */
                 break;
        }
    }while((objenumErrorType == csNoError) && \
           (u32CheckedLength <= u32StringLength) &&\
           (bStopChecking == STD_FALSE));
    /* check the propose final state */
    if(objenumErrorType == csNoError)
    {
        switch(objenumEntityUpdateStateType)
        {
            case csCheckSemicolonDelimiterState:
                 objenumErrorType = csMissingSemicolon;
                 break;
            case csCheckAmpersandDelimiterState:
            default:
                 /*
                  * Nothing to do
                  */
                 break;
        }
    }

    return objenumErrorType;
}
static BOOLEAN LwXMLP_bValideEntity(INT8U* pu8Entity)
{
    typedef enum
    {
        csCheckAmperSand,
        csCheckStartName,
        csCheckName,
        csCheckHexCharEntity,
        csCheckCharacterEntityType,
        csCheckDecimalCharEntity,
        csCheckDone
    }enumValidEntityStateType;
    BOOLEAN  bValideEntity = STD_TRUE;
    INT8U   *pu8ValidatedEntity = pu8Entity;
    INT32U   u32CharacterLength = 0U, u32EntityLength = 0U;
    enumValidEntityStateType objenumValidEntityStateType = csCheckAmperSand;

    do
    {
        switch(objenumValidEntityStateType)
        {
            case csCheckAmperSand:
                 if(*pu8ValidatedEntity == AMPERSAND_ASCII_CHARACTER)
                 {
                     objenumValidEntityStateType = csCheckStartName;
                     u32CharacterLength          = 1U;
                 }
                 else
                 {
                     bValideEntity = STD_FALSE;
                 }
                 break;
            case csCheckStartName:
                 if(*pu8ValidatedEntity == SEMICOLON_ASCII_CHARACTER)
                 {
                     objenumValidEntityStateType = csCheckDone;
                 }
                 else if(*pu8ValidatedEntity == HASH_ASCII_CHARACTER)
                 {
                     objenumValidEntityStateType = csCheckCharacterEntityType;
                 }

                 else
                 {
                     u32CharacterLength = LwXMLP_u8ValidNameStartCharacter(pu8ValidatedEntity);
                     if(u32CharacterLength > 0U)
                     {
                         objenumValidEntityStateType = csCheckName;
                         u32EntityLength++;
                     }
                     else
                     {
                         bValideEntity = STD_FALSE;
                     }
                 }
                 break;
            case csCheckCharacterEntityType:
                 if(*pu8ValidatedEntity == SEMICOLON_ASCII_CHARACTER)
                 {
                     objenumValidEntityStateType = csCheckDone;
                 }
                 else if(*pu8ValidatedEntity == X_SMALL_ASCII_CHARACTER)
                 {
                     objenumValidEntityStateType = csCheckHexCharEntity;
                     u32CharacterLength          = 1U;
                 }
                 else
                 {
                     objenumValidEntityStateType = csCheckDecimalCharEntity;
                     u32CharacterLength          = 0U;
                 }
                 break;
            case csCheckDecimalCharEntity:
                 if(*pu8ValidatedEntity == SEMICOLON_ASCII_CHARACTER)
                 {
                     objenumValidEntityStateType = csCheckDone;
                 }
                 else
                 {
                     if(((INT8U)(*pu8ValidatedEntity) >= ZERO_ASCII_CHARACTER) && \
                       ((INT8U)(*pu8ValidatedEntity) <= NINE_ASCII_CHARACTER))
                     {
                         u32CharacterLength = 1U;
                         u32EntityLength++;
                     }
                     else
                     {
                         bValideEntity = STD_FALSE;
                     }
                 }
                 break;
            case csCheckHexCharEntity:
                 if(*pu8ValidatedEntity == SEMICOLON_ASCII_CHARACTER)
                 {
                     objenumValidEntityStateType = csCheckDone;
                 }
                 else
                 {
                     if(((*pu8ValidatedEntity >= ZERO_ASCII_CHARACTER) && (*pu8ValidatedEntity <= NINE_ASCII_CHARACTER))||
                       ((*pu8ValidatedEntity >= A_SMALL_ASCII_CHARACTER) && (*pu8ValidatedEntity <= F_SMALL_ASCII_CHARACTER))||
                       ((*pu8ValidatedEntity >= A_CAPITAL_ASCII_CHARACTER) && (*pu8ValidatedEntity <= F_CAPITAL_ASCII_CHARACTER)))
                     {
                         u32CharacterLength = 1U;
                         u32EntityLength++;
                     }
                     else
                     {
                         bValideEntity = STD_FALSE;
                     }
                 }
                 break;
            case csCheckName:
                 if(*pu8ValidatedEntity == SEMICOLON_ASCII_CHARACTER)
                 {
                     objenumValidEntityStateType = csCheckDone;
                 }
                 else
                 {
                     u32CharacterLength = LwXMLP_u8ValidNameCharacter(pu8ValidatedEntity);
                     if(u32CharacterLength  > 0U)
                     {
                         u32EntityLength++;
                     }
                     else
                     {
                         bValideEntity = STD_FALSE;
                     }
                 }
                 break;
            case csCheckDone:
                 break;
            default:
                 /*
                  * Nothing to do
                  */
                 break;
        }
        pu8ValidatedEntity = &pu8ValidatedEntity[u32CharacterLength];
    }
    while((objenumValidEntityStateType != csCheckDone) && (bValideEntity == STD_TRUE));
    if(u32EntityLength == 0U)
    {/* Invalid entity length */
        bValideEntity = STD_FALSE;
    }
    else
    {
        /*
         * Nothing to do
         */
    }

    return bValideEntity;
}
static void LwXMLP_vMoveBlockRight(INT8U* pu8StringBlockAddress, INT32U u32MoveLength, INT32U u32BlockLength)
{
    INT32U u32ByteIndex;

    for(u32ByteIndex = u32BlockLength; u32ByteIndex > 0U; --u32ByteIndex)
    {
        pu8StringBlockAddress[u32ByteIndex + u32MoveLength - 1U] = pu8StringBlockAddress[u32ByteIndex - 1U];
    }
    pu8StringBlockAddress[u32MoveLength + u32BlockLength] = NULL_ASCII_CHARACTER;
}
static void LwXMLP_vMoveBlockLeft(INT8U* pu8StringBlockAddress, INT32U u32MoveLength, INT32U u32BlockLength)
{
    INT32U u32ByteIndex;

    for(u32ByteIndex = 0U; u32ByteIndex < u32BlockLength; ++u32ByteIndex)
    {
        pu8StringBlockAddress[u32ByteIndex] = pu8StringBlockAddress[u32ByteIndex + u32MoveLength];
    }
}
/*
 * shouldn't larger than 4 byte sequence, such that the W3C dosn't allow for more than 4 byte sequence UTF-8
 */
static INT8U LwXMLP_u8ConvertCodePointToUTF8(INT32U u32CodePoint, INT8U *pu8UTF8Code)
{
    INT8U         u8CodeLength;

    if(u32CodePoint < 0x80U)    /* 1 byte sequence */
    {
        pu8UTF8Code[0U] = (INT8U) (u32CodePoint);
        u8CodeLength   = 1U;
        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                        "UTF-8 codes for 0x%x are =: 0x%x ", \
                        u32CodePoint, \
                        pu8UTF8Code[0U]);
    }
#if(SUPPORT_ASCII_ONLY == STD_OFF)
    else if(u32CodePoint < 0x800U)  /* 2 byte sequence */
    {
        pu8UTF8Code[1U] = (INT8U) ((u32CodePoint | 0x80U) & 0xBFU);
        u32CodePoint        >>= 6U;
        pu8UTF8Code[0U] = (INT8U) (u32CodePoint | 0xC0U);
        u8CodeLength   = 2U;
        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "UTF-8 codes for 0x%x are =: 0x%x 0x%x",\
                        u32CodePoint,\
                        pu8UTF8Code[0U],\
                        pu8UTF8Code[1U]);

    }
    else if(u32CodePoint < 0x10000U)    /* 3 byte sequence */
    {
        pu8UTF8Code[2U] = (INT8U) ((u32CodePoint | 0x80U) & 0xBFU);
        u32CodePoint        >>= 6U;
        pu8UTF8Code[1U] = (INT8U) ((u32CodePoint | 0x80U) & 0xBFU);
        u32CodePoint        >>= 6U;
        pu8UTF8Code[0U] = (INT8U) (u32CodePoint | 0xE0U);
        u8CodeLength   = 3;
        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "UTF-8 codes for 0x%x are =: 0x%x 0x%x 0x%x",\
                        u32CodePoint,\
                        pu8UTF8Code[0U],\
                        pu8UTF8Code[1U],\
                        pu8UTF8Code[2U]);
    }
    else if(u32CodePoint < 0x200000U)   /* 4 byte sequence */
    {
        pu8UTF8Code[3] = (INT8U) ((u32CodePoint | 0x80U) & 0xBFU);
        u32CodePoint        >>= 6U;
        pu8UTF8Code[2] = (INT8U) ((u32CodePoint | 0x80U) & 0xBFU);
        u32CodePoint        >>= 6U;
        pu8UTF8Code[1U] = (INT8U) ((u32CodePoint | 0x80U) & 0xBFU);
        u32CodePoint        >>= 6U;
        pu8UTF8Code[0] = (INT8U) (u32CodePoint | 0xF0U);
        u8CodeLength   = 4U;
        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "UTF-8 codes for 0x%x are =: 0x%x 0x%x 0x%x 0x%x",\
                        u32CodePoint,\
                        pu8UTF8Code[0U],\
                        pu8UTF8Code[1U],\
                        pu8UTF8Code[2U],\
                        pu8UTF8Code[3U]);
    }
#endif
    else
    {
        u8CodeLength = 0U;
    }

    return u8CodeLength;
}
static void LwXMLP_vSwitchToSecondLevelBuffer(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType, INT8U const *pu8PortionBuffer, INT32U u32PortionLength)
{
    pstrParserInstanceType->u32SecondLevelReadyAmount = u32PortionLength;
    memcpy(pstrParserInstanceType->u8SecondLevelBuffer, \
           pu8PortionBuffer, \
           u32PortionLength);
    pstrParserInstanceType->u8SecondLevelBuffer[pstrParserInstanceType->u32SecondLevelReadyAmount] = NULL_ASCII_CHARACTER;
    pstrParserInstanceType->pu8SavedLastParserLocation  = pstrParserInstanceType->pu8LastParserLocation;
    pstrParserInstanceType->pu8LastParserLocation       = &pstrParserInstanceType->u8SecondLevelBuffer[0];
    pstrParserInstanceType->pu8SavedCircularBufferEnd   = pstrParserInstanceType->pu8CircularBufferEnd;
    pstrParserInstanceType->pu8CircularBufferEnd        = &pstrParserInstanceType->u8SecondLevelBuffer[pstrParserInstanceType->u32SecondLevelReadyAmount];
    pstrParserInstanceType->u8SecondLevelPathBuffer[0]  = 0U;
    pstrParserInstanceType->u32SecondLevelPathLength    = 0U;
    pstrParserInstanceType->bSecondLevelBufferEnabled   = STD_TRUE;
}
#endif
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_srLoadTagToBuffer                                        <LwXMLP_CORE>

SYNTAX:         StdReturnType LwXMLP_srLoadTagToBuffer(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)

DESCRIPTION :   This API is doing the following by two steps:
                Step1 : load the TAG into the working buffer
                Step2 : If the TAG is complete, it will be parsed to get the Name, Attributes, so on

PARAMETER1  :   pstrParserInstanceType : pointer to the XML instance and if end of the TAG/TAG Closer is found the
                                         pstrParserInstanceType->bTagCloserIsFound will be set to STD_TRUE

RETURN VALUE:   E_NOT_OK: if the loading for the TAG into the working buffer is failed or parsing error is occured
                E_OK    : if the loading for the TAG into the working buffer is succeeded

Note
END DESCRIPTION *******************************************************************************************************/
static void LwXMLP_vLoadTagToBuffer(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    /* First round is to check the TAG closure and TAG type */
    LwXMLP_vCheckTagCloserAndType(pstrParserInstanceType);
    /* second round, if complete TAG is found, copy it to the working buffer */
    if((pstrParserInstanceType->bTagCloserIsFound == STD_TRUE) && (pstrParserInstanceType->objenumErrorType == csNoError))
    {
        LwXMLP_vProcesssTag(pstrParserInstanceType);
    }
}
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_srProcesssTag                                        <LwXMLP_CORE>

SYNTAX:         StdReturnType LwXMLP_srProcesssTag(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)

DESCRIPTION :   This API is doing the following:
                - Extract the TAG name and size.
                - Validate the TAG characters.
                - Extract the TAG attributes in case of the start and empty TAG

PARAMETER1  :   pstrParserInstanceType : pointer to the XML instance and if end of the TAG/TAG Closer is found the
                                         pstrParserInstanceType->bTagCloserIsFound will be set to STD_TRUE

RETURN VALUE:   E_NOT_OK: if the loading for the TAG into the working buffer is failed
                E_OK    : if the loading for the TAG into the working buffer is succeeded

Note        :   the API will fail to load the buffer in the following cases:
                - Detect invalid start Name character in the TAG, , and the error (csInvalidStartNameCharacter) will be set.
                - Detect invalid Name character in the TAG, , and the error (csInvalidNameCharacter) will be set.
                - The = is missing between the attribute name and the attribute value,
                  and the error (csMissingEqual) will be set.
                - an attribute value is missing quote" , and the error (csMissingQuote) will be set.
                - The quotes on both ends of an attribute value must match , and the error
                  (csQuotesMissmatch) will be set.
END DESCRIPTION *******************************************************************************************************/
static void LwXMLP_vProcesssTag(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    typedef enum
    {
        csCheckTAGNameStart,\
        csCheckTAGName,\
        csCheckAttributeNameStart, \
        csCheckAttributeName,\
        csCheckEquality,\
        csCheckAttributeValueStartQuot,\
        csCheckAttributeValueEndQuot
    }enumCheckTagStateType;
    /* bEnableWightSpaceChecking, needed to check the white space between attributes but not before the first one */
    BOOLEAN               bEnableWhiteSpaceChecking;
    INT8U                *pu8WorkingBuffer, u8AttributeEndCharacter = SPACE_ASCII_CHARACTER;
    INT32U                u32CheckingDataAmount, u32BufferIndex, u32CharacterLength = 0U;
    enumCheckTagStateType objenumCheckTagStateType;

    objenumCheckTagStateType  = csCheckTAGNameStart;
    bEnableWhiteSpaceChecking = STD_FALSE;
    u32CheckingDataAmount = strlen((INT8S const*)pstrParserInstanceType->u8XMLworkingBuffer);
    pu8WorkingBuffer      = pstrParserInstanceType->u8XMLworkingBuffer;
    pstrParserInstanceType->s16AttributeCounts = 0;
    /* escape <  */
    pu8WorkingBuffer++;
    u32CheckingDataAmount -= 2U;/* remove the < > length */
    if(pstrParserInstanceType->objenumTagTypeType == csEndTagType)
    {
        /* escape \  */
        pu8WorkingBuffer++;
        u32CheckingDataAmount--;/* remove the \ length */
    }
    else  if(pstrParserInstanceType->objenumTagTypeType == csEmptyTagType)
    {
        u32CheckingDataAmount--;/* remove the \ length */
    }
    else
    {
        /*
         * Nothing to do
         */
    }
    for(u32BufferIndex = 0U; \
       (u32BufferIndex < u32CheckingDataAmount) && (pstrParserInstanceType->objenumErrorType == csNoError);\
       u32BufferIndex += u32CharacterLength)
    {
        switch(objenumCheckTagStateType)
        {
            case csCheckTAGNameStart:
                 u32CharacterLength = LwXMLP_u8ValidNameStartCharacter(&pu8WorkingBuffer[u32BufferIndex]);
                 if(u32CharacterLength > 0U)
                 {
                     pstrParserInstanceType->pu8ElementName     = &pu8WorkingBuffer[u32BufferIndex];
                     pstrParserInstanceType->s16AttributeCounts = 0;
                     objenumCheckTagStateType                   = csCheckTAGName;
                 }
                 else
                 {
                      pstrParserInstanceType->objenumErrorType = csInvalidStartNameCharacter;
                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Start Name character");
                 }
                 break;
            case csCheckTAGName:
                 if(LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[u32BufferIndex]) == STD_TRUE)
                 {
                     /* reach end of the name */
                     pu8WorkingBuffer[u32BufferIndex] = NULL_ASCII_CHARACTER;
                     u32CharacterLength               = 1U; /* update character length */
                     objenumCheckTagStateType         = csCheckAttributeNameStart;
                 }
                 else
                 {
                     u32CharacterLength = LwXMLP_u8ValidNameCharacter(&pu8WorkingBuffer[u32BufferIndex]);
                     if(u32CharacterLength > 0U)
                     {
                         /*
                          * Nothing to do
                          */
                     }
                     else /* Invalid Name character */
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidNameCharacter;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Start Name character");
                     }

                 }
                 break;
            case csCheckAttributeNameStart:
                 if(bEnableWhiteSpaceChecking == STD_TRUE)
                 { /* Whitespace here is mandatory */
                     bEnableWhiteSpaceChecking = STD_FALSE;
                     if(LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[u32BufferIndex]) == STD_TRUE)
                     {
                          u32CharacterLength = 1U; /* Update character length */
                     }
                     else
                      {
                         pstrParserInstanceType->objenumErrorType = csMissingWhiteSpaceCharacter;
                          LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing white Space");
                     }
                 }/* Will ignore the white spaces between the element name and the attribute name */
                 else if(LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[u32BufferIndex]) == STD_TRUE)
                 {/* Option whitespace */
                     u32CharacterLength = 1U; /* update character length */
                 }
                 else
                 {
                     u32CharacterLength = LwXMLP_u8ValidNameStartCharacter(&pu8WorkingBuffer[u32BufferIndex]);
                     if(u32CharacterLength > 0U)
                     {
                         if(pstrParserInstanceType->objenumTagTypeType != csEndTagType)
                         {
                             pstrParserInstanceType->pu8AttributeNameArray[pstrParserInstanceType->s16AttributeCounts] = \
                                     &pu8WorkingBuffer[u32BufferIndex];
                             objenumCheckTagStateType  = csCheckAttributeName;
                         }
                         else
                         {
                             pstrParserInstanceType->objenumErrorType = csAttributeInEndTag;
                         }
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidStartNameCharacter;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Start Name character");
                     }
                 }
                 break;
            case csCheckAttributeName:
                 if(pu8WorkingBuffer[u32BufferIndex] == EQUAL_ASCII_CHARACTER)
                 {
                     /* reach end of the name */
                     pu8WorkingBuffer[u32BufferIndex] = NULL_ASCII_CHARACTER;
                     u32CharacterLength               = 1U; /* Update character length */
                     objenumCheckTagStateType         = csCheckAttributeValueStartQuot;
                 }
                 else if(LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[u32BufferIndex]) == STD_TRUE)
                 {
                     /* reach end of the name */
                     pu8WorkingBuffer[u32BufferIndex] = NULL_ASCII_CHARACTER;
                     u32CharacterLength               = 1U; /* Update character length */
                     objenumCheckTagStateType         = csCheckEquality;
                 }
                 else
                 {
                     u32CharacterLength = LwXMLP_u8ValidNameCharacter(&pu8WorkingBuffer[u32BufferIndex]);
                     if(u32CharacterLength > 0U)
                     {
                         /*
                          * Nothing to do
                          */
                     }
                     else/* Invalid Name character */
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidNameCharacter;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Start Name character");
                     }

                 }
                 break;
            case csCheckEquality:
                 if(pu8WorkingBuffer[u32BufferIndex] == EQUAL_ASCII_CHARACTER)
                 {
                     /* reach end of the name */
                     pu8WorkingBuffer[u32BufferIndex] = NULL_ASCII_CHARACTER;
                     u32CharacterLength               = 1U; /* Update character length */
                     objenumCheckTagStateType         = csCheckAttributeValueStartQuot;
                 }
                 else if(LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[u32BufferIndex]) == STD_TRUE)
                 {
                     u32CharacterLength = 1U; /* update character length */
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = csMissingEqual;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                                       "The = is missing between the attribute name and the attribute value");
                 }
                 break;
            case csCheckAttributeValueStartQuot:
                 if((pu8WorkingBuffer[u32BufferIndex] == DOUBLE_QUOT_ASCII_CHARACTER) || \
                    (pu8WorkingBuffer[u32BufferIndex] == SINGLE_QUOT_ASCII_CHARACTER))
                 {
                     pstrParserInstanceType->pu8AttributeValueArray[pstrParserInstanceType->s16AttributeCounts] = \
                     &pu8WorkingBuffer[u32BufferIndex + 1U] ; /* point to the attribute value in advance */
                     u8AttributeEndCharacter          = pu8WorkingBuffer[u32BufferIndex];
                     /* reach end of the name */
                     pu8WorkingBuffer[u32BufferIndex] = NULL_ASCII_CHARACTER;
                     u32CharacterLength               = 1U; /* update character length */
                     objenumCheckTagStateType         = csCheckAttributeValueEndQuot;
                 }
                 else if(LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[u32BufferIndex]) == STD_TRUE)
                 {
                     u32CharacterLength = 1U; /* update character length */
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = csMissingQuote;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing quote");
                 }
                 break;
            case csCheckAttributeValueEndQuot:
                 if(pu8WorkingBuffer[u32BufferIndex] == u8AttributeEndCharacter)
                 {
                     u32CharacterLength = 1U; /* update character length */
                     pu8WorkingBuffer[u32BufferIndex] = NULL_ASCII_CHARACTER;
                     if(LwXMLP_bNewAttribute(pstrParserInstanceType, \
                        pstrParserInstanceType->pu8AttributeNameArray[pstrParserInstanceType->s16AttributeCounts]) \
                             == STD_TRUE)
                     {
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                                         "Found attribute name: %s value: %s", \
                                         pstrParserInstanceType->pu8AttributeNameArray[pstrParserInstanceType->s16AttributeCounts],\
                                         pstrParserInstanceType->pu8AttributeValueArray[pstrParserInstanceType->s16AttributeCounts]);
                         if(pstrParserInstanceType->objenumErrorType == csNoError)
                         {
                             bEnableWhiteSpaceChecking = STD_TRUE;
                             pstrParserInstanceType->s16AttributeCounts++;
                             if(pstrParserInstanceType->s16AttributeCounts < MAX_NUMBER_OF_ATTRIBUTES)
                             {
                                 objenumCheckTagStateType = csCheckAttributeNameStart;
                             }
                             else
                             {
                                 pstrParserInstanceType->objenumErrorType = csLargeNumberOfAttributes;
                             }
                         }
                         else
                         {
                             /*
                              * Nothing to do
                              */
                         }
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csRepeatedAttributeName;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                                         "Repeated attribute Name");
                     }
                 }
                 else if(pu8WorkingBuffer[u32BufferIndex] == LESS_THAN_ASCII_CHARACTER)
                 {
                     pstrParserInstanceType->objenumErrorType = csInvalidAttributeValue;
                 }
                 else
                 {
                     u32CharacterLength = LwXMLP_u8ValidCharacter(&pu8WorkingBuffer[u32BufferIndex]);
                     if(u32CharacterLength == 0U)
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidXMLCharacter;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid XML character");
                     }
                     else/* valid Name character */
                     {
                         /*
                          * Nothing to do
                          */
                     }
                 }
                 break;
            default:
                /*
                 * Nothing to do
                 */
                 break;
        }
    }
    /* validation part, such that the state machine shall terminate on specific states, otherwise, it shall be error */
    if(pstrParserInstanceType->objenumErrorType == csNoError)
    {
        switch(objenumCheckTagStateType)
        {
            case csCheckAttributeName:
            case csCheckEquality:
                 pstrParserInstanceType->objenumErrorType = csMissingAttributeValue;
                 LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "an attribute value is missing");
                 break;
            case csCheckAttributeValueStartQuot:
            case csCheckAttributeValueEndQuot:
                 pstrParserInstanceType->objenumErrorType = csMissingQuote;
                 LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "an attribute value is missing its quote");
                 break;
            case csCheckTAGNameStart:
            case csCheckTAGName:
            case csCheckAttributeNameStart:
                 /* valid exit states */
            default:
                 /*
                  * Nothing to do
                  */
                 break;
        }
    }
    if(pstrParserInstanceType->objenumErrorType == csNoError)
    {
         if(pstrParserInstanceType->s16AttributeCounts > 0)
         {
#if(ENABLE_ATTRIBUTE_Normalization == STD_ON)
             LwXMLP_vNormalizeAttributes(pstrParserInstanceType);
#endif
         }
         else
         {
             /* String termination if there were not a white space at the end of the element */
             pu8WorkingBuffer[u32BufferIndex] = NULL_ASCII_CHARACTER;
         }
    }
    else
    {
        /*
         * Nothing to do
         */
    }

}
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_srCheckTagCloserAndType                                        <LwXMLP_CORE>

SYNTAX:         StdReturnType LwXMLP_srCheckTagCloserAndType(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)

DESCRIPTION :   This API is doing the following:
                - Check if the TAG Closer is available
                - Check the TAG type, Is it start, end or empty TAG
                - Copy TAG into the working buffer
                - Consume TAG

PARAMETER1  :   pstrParserInstanceType : pointer to the XML instance and if end of the TAG/TAG Closer is found the
                                         pstrParserInstanceType->bTagCloserIsFound will be set to STD_TRUE

RETURN VALUE:   E_NOT_OK: if the TAG can't fit into the working buffer
                E_OK    : if the TAG can fit into the working buffer

Note        :   the API will fail to load the buffer in the following cases:
                - the TAG length the larger than working buffer (MAX_WORKING_BUFFER_SIZE) or the TAG is larger
                  than the circular buffer, an the error (csLargeXMLProperty) will be set.
END DESCRIPTION *******************************************************************************************************/
static void LwXMLP_vCheckTagCloserAndType(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    BOOLEAN               bQuotIsFound      = STD_FALSE;
    INT8U                 u8LastCharacter   = SPACE_ASCII_CHARACTER; /* avoid to be being used without init */
    INT8U                 u8SecondCharacter = SPACE_ASCII_CHARACTER;/* avoid to be being used without init */
    INT8U                 u8QuotCharacter   = SPACE_ASCII_CHARACTER;/* avoid to be being used without init */
    INT8U                *pu8WorkingBuffer;
    INT8U          const *pu8CurrentXMLCharacter;
    INT32U                u32CheckingDataAmount, u32BufferIndex;

    pstrParserInstanceType->bTagCloserIsFound   = STD_FALSE;
    pu8CurrentXMLCharacter                       = pstrParserInstanceType->pu8LastParserLocation; /* we need to to escape < */
    u32CheckingDataAmount                        = LwXMLP_u32GetReadyData(pstrParserInstanceType);
    pu8WorkingBuffer                             = pstrParserInstanceType->u8XMLworkingBuffer;
    for(u32BufferIndex = 0U; \
        (u32BufferIndex < u32CheckingDataAmount)&& (u32BufferIndex < MAX_WORKING_BUFFER_SIZE); \
        ++u32BufferIndex)
    {
        *pu8WorkingBuffer = *pu8CurrentXMLCharacter;
        if(bQuotIsFound == STD_TRUE)
        {
            if(*pu8CurrentXMLCharacter == u8QuotCharacter)
            {
                bQuotIsFound = STD_FALSE;
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
        }
        else
        {
            if(*pu8CurrentXMLCharacter == GREATER_THAN_ASCII_CHARACTER)
            {
                pu8WorkingBuffer++;
                pstrParserInstanceType->bTagCloserIsFound  = STD_TRUE;
                LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "TAG Closer is found");
                break;
            }
            else if((*pu8CurrentXMLCharacter == DOUBLE_QUOT_ASCII_CHARACTER) || \
                    (*pu8CurrentXMLCharacter == SINGLE_QUOT_ASCII_CHARACTER))
            {
                bQuotIsFound    = STD_TRUE;
                u8QuotCharacter = *pu8CurrentXMLCharacter;
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
        }
        if(u32BufferIndex == 1U) /* capture / for the end TAG */
        {
            u8SecondCharacter = *pu8CurrentXMLCharacter;
        }
        else
        {
            /*
             * Nothing to do
             */
        }
        u8LastCharacter  = *pu8CurrentXMLCharacter; /* Save last character to check it against / for empty Tag */
        pu8WorkingBuffer++;
        pu8CurrentXMLCharacter++;
        if(pstrParserInstanceType->pu8CircularBufferEnd == pu8CurrentXMLCharacter)
        {
            pu8CurrentXMLCharacter = pstrParserInstanceType->u8XMLCircularBuffer;
        }
        else
        {
            /* nothing to do here */
        }
    }
    if(pstrParserInstanceType->bTagCloserIsFound == STD_TRUE)
    {
        *pu8WorkingBuffer = NULL_ASCII_CHARACTER; /* terminate teh TAG */
        if(u8SecondCharacter == SLASH_ASCII_CHARACTER)
        {
            pstrParserInstanceType->objenumTagTypeType  = csEndTagType;
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "End TAG is found");
        }
        else if(u8LastCharacter == SLASH_ASCII_CHARACTER)
        {
            pstrParserInstanceType->objenumTagTypeType  = csEmptyTagType;
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Empty TAG is found");
        }
        else
        {
            pstrParserInstanceType->objenumTagTypeType  = csStartTagType;
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Start TAG is found");
        }
        pstrParserInstanceType->u32CurrentTagLength = u32BufferIndex + 1U;
    }/* if we looked for character and isn't found and reach the buffer end */
    else if((u32BufferIndex == MAX_CIRCULAR_BUFFER_SIZE) || (u32BufferIndex == MAX_WORKING_BUFFER_SIZE))
    {
        pstrParserInstanceType->objenumErrorType = csLargeElementProperty;
        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Can't load the XML Tag into the buffer");
    }
    else
    {
        /* unexpected */
    }
}
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_bTagContainXMLDirective                                        <LwXMLP_CORE>

SYNTAX:         BOOLEAN LwXMLP_bTagContainXMLDirective(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)

DESCRIPTION :   check to see if this is an XML preprocessor directive

PARAMETER1  :   pstrParserInstanceType : pointer to the XML instance

RETURN VALUE:


Note        : This API must receive the pu8LastParserLocation point to '<' as a first character

END DESCRIPTION *******************************************************************************************************/
static void LwXMLP_vCheckTagForDirective(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    static const INT8S cs8DeclarationHeaderString[]           = "<?xml";
    static const INT8S cs8PIHeaderString[]                    = "<?";
    static const INT8S cs8CDATAHeaderString[]                 = "<![CDATA[";
    static const INT8S cs8DTDHeaderString[]                   = "<!DOCTYPE";
#if(ENABLE_DTD_RECEIPTION == STD_ON)
    static const INT8S cs8ElementHeaderString[]               = "<!ELEMENT";
    static const INT8S cs8EntityHeaderString[]                = "<!ENTITY";
    static const INT8S cs8NotationHeaderString[]              = "<!NOTATION";
    static const INT8S cs8AttlistHeaderString[]               = "<!ATTLIST";
#endif
    static const INT8S cs8CommentHeaderString[]               = "<!--";
    const INT8U  *pu8CurrentXMLCharacter;
          INT8U  *pu8WorkingBuffer;
          INT32U  u32byteIndex, u32DirectiveLength;

    pstrParserInstanceType->bTagContainDirective = STD_FALSE;
    pu8WorkingBuffer                             = pstrParserInstanceType->u8XMLworkingBuffer;
    pu8CurrentXMLCharacter                       = pstrParserInstanceType->pu8LastParserLocation;
    for(u32byteIndex = 0U; \
        (u32byteIndex < MAX_CIRCULAR_BUFFER_SIZE) && (u32byteIndex < MAXIMUM_DIRECTIVE_LENTGH); \
        ++u32byteIndex)
    {
        if(u32byteIndex == 1U)
        {
            if(((*pu8CurrentXMLCharacter) == EXCLAM_MARK_ASCII_CHARACTER) ||
               ((*pu8CurrentXMLCharacter) == QUESTION_MARK_ASCII_CHARACTER))
            {
                pu8WorkingBuffer[u32byteIndex]               = *pu8CurrentXMLCharacter;
                pstrParserInstanceType->bTagContainDirective = STD_TRUE;
            }
            else
            {/* No Directive is available */
                break;
            }
        }
        else
        {
            pu8WorkingBuffer[u32byteIndex] = *pu8CurrentXMLCharacter;
        }
        pu8CurrentXMLCharacter++;
        if(pstrParserInstanceType->pu8CircularBufferEnd == pu8CurrentXMLCharacter)
        {
            pu8CurrentXMLCharacter = pstrParserInstanceType->u8XMLCircularBuffer;
        }
        else
        {
            /* nothing to do here */
        }
    }
    if(pstrParserInstanceType->bTagContainDirective == STD_TRUE)
    {

        if(strncmp((INT8S const *)pu8WorkingBuffer, cs8DeclarationHeaderString, strlen(cs8DeclarationHeaderString)) == 0)
        {
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found Declaration header directive");
            u32DirectiveLength = strlen(cs8DeclarationHeaderString);
            if((pu8WorkingBuffer[u32DirectiveLength] == QUESTION_MARK_ASCII_CHARACTER) ||
               (LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[u32DirectiveLength]) == STD_TRUE))
            {
                if((strlen((INT8S const *)pstrParserInstanceType->u8XMLPathBuffer) == 0U) && (pstrParserInstanceType->u32ParsedLength == 0U))
                {
                    pstrParserInstanceType->objenumDirectiveTypeType  = csDeclarationMarkup;
                }
                else /* the declration MUST be first element in the document */
                {
                    pstrParserInstanceType->objenumErrorType = csWrongDeclarationLocation;
                }
            }
            else
            {
                pstrParserInstanceType->objenumDirectiveTypeType = csProcessingInstructionMarkup;
            }
        }
        else if(strncmp((INT8S const *)pu8WorkingBuffer, cs8PIHeaderString, strlen(cs8PIHeaderString)) == 0)
        {
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found processing Instruction header directive");
            pstrParserInstanceType->objenumDirectiveTypeType = csProcessingInstructionMarkup;
        }
        else if(strncmp((INT8S const *)pu8WorkingBuffer, cs8CDATAHeaderString, strlen(cs8CDATAHeaderString)) == 0)
        {
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found CDATA header directive");
            pstrParserInstanceType->objenumDirectiveTypeType = csCDATAMarkup;
        }
        else if(strncmp((INT8S const *)pu8WorkingBuffer, cs8CommentHeaderString, strlen(cs8CommentHeaderString)) == 0)
        {
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found Comment header directive");
            pstrParserInstanceType->objenumDirectiveTypeType = csCommentMarkup;
            pstrParserInstanceType->pu8ParsedComments = &pu8WorkingBuffer[4];
        }
        else if(strncmp((INT8S const *)pu8WorkingBuffer, cs8DTDHeaderString, strlen(cs8DTDHeaderString)) == 0)
        {
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found DTD header directive");
#if(ENABLE_DTD_RECEIPTION == STD_ON)
            /*
             * It will be coverted later to  csInternalDTDMarkup or csExternalDTDMarkup depending on the XML content
             */
            pstrParserInstanceType->objenumDirectiveTypeType = csDTDMarkup;
            pstrParserInstanceType->bParsingDTDInProgress    = STD_TRUE;
#else
            pstrParserInstanceType->objenumErrorType  = csFeatureNotSupported;
#endif
        }
#if(ENABLE_DTD_RECEIPTION == STD_ON)
        else if(strncmp((INT8S const *)pu8WorkingBuffer, cs8ElementHeaderString, strlen(cs8ElementHeaderString)) == 0)
        {
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found DTD Element directive");
            pstrParserInstanceType->objenumDirectiveTypeType = csDTDElementMarkup;
            if(LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[strlen(cs8ElementHeaderString)]) == STD_FALSE)
            {
                pstrParserInstanceType->objenumErrorType = csMissingWhiteSpaceCharacter;
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
        }
        else if(strncmp((INT8S const *)pu8WorkingBuffer, cs8EntityHeaderString, strlen(cs8EntityHeaderString)) == 0)
        {
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found DTD Entity directive");
            pstrParserInstanceType->objenumDirectiveTypeType = csDTDEntitytMarkup;
        }
        else if(strncmp((INT8S const *)pu8WorkingBuffer, cs8NotationHeaderString, strlen(cs8NotationHeaderString)) == 0)
        {
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found DTD Notation directive");
            pstrParserInstanceType->objenumDirectiveTypeType = csDTDNotationtMarkup;
            if(LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[strlen(cs8NotationHeaderString)]) == STD_FALSE)
            {
                pstrParserInstanceType->objenumErrorType = csMissingWhiteSpaceCharacter;
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
        }
        else if(strncmp((INT8S const *)pu8WorkingBuffer, cs8AttlistHeaderString, strlen(cs8AttlistHeaderString)) == 0)
        {
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found DTD Attlist directive");
            pstrParserInstanceType->objenumDirectiveTypeType = csDTDAttlistMarkup;
            if(LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[strlen(cs8AttlistHeaderString)]) == STD_FALSE)
            {
                pstrParserInstanceType->objenumErrorType = csMissingWhiteSpaceCharacter;
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
        }
#endif
        else
        {
            pstrParserInstanceType->objenumErrorType  = csUnExpectedDirectiveType;
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found unsupported directive");
        }
#if(ENABLE_DTD_RECEIPTION == STD_ON)
        if((pstrParserInstanceType->bParsingDTDInProgress == STD_FALSE) &&
           (pstrParserInstanceType->objenumErrorType == csNoError) &&
           ((pstrParserInstanceType->objenumDirectiveTypeType == csDTDElementMarkup)||\
            (pstrParserInstanceType->objenumDirectiveTypeType == csDTDAttlistMarkup)||\
            (pstrParserInstanceType->objenumDirectiveTypeType == csDTDEntitytMarkup)||\
            (pstrParserInstanceType->objenumDirectiveTypeType == csDTDNotationtMarkup)))
        {
            pstrParserInstanceType->objenumErrorType  = csUnExpectedDirectiveType;
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found unsupported directive");
        }
        else
        {
            /*
             * Nothing to do
             */
        }
#endif
    }
    else
    {
        /* nothing to do */
        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "No Directive is found");
    }
}
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_srLoadDirectiveToBuffer                                        <LwXMLP_CORE>

SYNTAX:         StdReturnType LwXMLP_srLoadDirectiveToBuffer(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)

DESCRIPTION :   This API is doing the following:
                - Check if the end of the data is available
                - Copy the data to the working buffer
                - Check if the data contains a predefined entity to be replaced

PARAMETER1  :   pstrParserInstanceType : pointer to the XML instance

PARAMETER2  :   pbEndOfDataFound       : pointer to boolean which will be set by the API, if the directive is complete

RETURN VALUE:   E_NOT_OK: if the loading for the directive into the working buffer is failed
                E_OK    : if the loading for the directive into the working buffer is succeeded

Note        :   The API will fail to load the buffer in the following cases:
                - The directive length the larger than working buffer (MAX_WORKING_BUFFER_SIZE), and the error
                  (csLargeXMLProperty) will be set.
                - The received is unexpected directive, and the error (csUnExpectedDirectiveType) will be set.

Note        :   This API assume that the directive is found and available in
                pstrParserInstanceType->objenumDirectiveTypeType

END DESCRIPTION *******************************************************************************************************/
static void LwXMLP_vLoadDirectiveToBuffer(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
   const INT8U                 *pu8CurrentXMLCharacter;
         INT8U                 *pu8WorkingBuffer;
         INT32U                 u32DirectiveLength = 0U;
         INT32U                 u32BufferIndex;
         INT32U                 u32CheckingDataAmount;

    pstrParserInstanceType->bEndOfDirectiveFound = STD_FALSE;
    pu8WorkingBuffer                             = pstrParserInstanceType->u8XMLworkingBuffer;
    pu8CurrentXMLCharacter                       = pstrParserInstanceType->pu8LastParserLocation;
    u32CheckingDataAmount                        = LwXMLP_u32GetReadyData(pstrParserInstanceType);
    for(u32BufferIndex = 0U;\
        (u32BufferIndex < u32CheckingDataAmount) && \
        (pstrParserInstanceType->bEndOfDirectiveFound == STD_FALSE) &&\
        (u32BufferIndex < MAX_WORKING_BUFFER_SIZE);\
        ++u32BufferIndex)
    {
        *pu8WorkingBuffer = *pu8CurrentXMLCharacter;
        u32DirectiveLength++;
#if(ENABLE_DTD_RECEIPTION == STD_ON)
        if(*pu8WorkingBuffer == OPENING_BRACKET_ASCII_CHARACTER)
        {
            if(pstrParserInstanceType->objenumDirectiveTypeType == csDTDMarkup)
            {
                pstrParserInstanceType->objenumDirectiveTypeType = csInternalDTDMarkup;
                pstrParserInstanceType->u32DirectiveLength       = u32DirectiveLength;
                pstrParserInstanceType->bEndOfDirectiveFound     = STD_TRUE;
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
        }
        else if(*pu8WorkingBuffer == GREATER_THAN_ASCII_CHARACTER)
        {
            if(pstrParserInstanceType->objenumDirectiveTypeType == csDTDMarkup)
            {
                pstrParserInstanceType->objenumDirectiveTypeType = csExternalDTDMarkup;
                pstrParserInstanceType->u32DirectiveLength       = u32DirectiveLength;
                pstrParserInstanceType->bEndOfDirectiveFound     = STD_TRUE;
            }
            else
            {
                pstrParserInstanceType->u32DirectiveLength = u32DirectiveLength; /* Guess length*/
                pstrParserInstanceType->bEndOfDirectiveFound = LwXMLP_bReachEndOfDirective(pstrParserInstanceType);
                if((pstrParserInstanceType->bEndOfDirectiveFound == STD_TRUE) ||
                   (pstrParserInstanceType->objenumErrorType != csNoError))
                {
                    break;
                }
                else
                {
                    /*
                     * Do nothing
                     */
                }
            }
        }
        else
        {
            /*
             * Nothing to do
             */
        }
#else
        if(*pu8WorkingBuffer == GREATER_THAN_ASCII_CHARACTER)
        {
            pstrParserInstanceType->u32DirectiveLength = u32DirectiveLength; /* Guess length*/
            pstrParserInstanceType->bEndOfDirectiveFound = LwXMLP_bReachEndOfDirective(pstrParserInstanceType);
            if((pstrParserInstanceType->bEndOfDirectiveFound == STD_TRUE) ||
               (pstrParserInstanceType->objenumErrorType != csNoError))
            {
                if(pstrParserInstanceType->objenumDirectiveTypeType == csCommentMarkup)
                { /* Remove --> */
                    pstrParserInstanceType->u8XMLworkingBuffer[pstrParserInstanceType->u32DirectiveLength - 3U] = \
                            NULL_ASCII_CHARACTER;
                }
                else
                {
                    /*
                     * Nothing to do
                     */
                }
                break;
            }
            else
            {
                /*
                 * Do nothing
                 */
            }
        }
#endif
        pu8WorkingBuffer++;
        pu8CurrentXMLCharacter++;
        if(pstrParserInstanceType->pu8CircularBufferEnd == pu8CurrentXMLCharacter)
        {
            pu8CurrentXMLCharacter = pstrParserInstanceType->u8XMLCircularBuffer;
        }
        else
        {
            /* nothing to do here */
        }
    }
    if(pstrParserInstanceType->bEndOfDirectiveFound == STD_TRUE)
    {
        pu8WorkingBuffer++;
        *pu8WorkingBuffer = NULL_ASCII_CHARACTER;
        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Found Directive %s", pstrParserInstanceType->u8XMLworkingBuffer);
    }
    else if((u32BufferIndex == MAX_CIRCULAR_BUFFER_SIZE) || (u32BufferIndex == MAX_WORKING_BUFFER_SIZE))
    {
        pstrParserInstanceType->objenumErrorType = csLargeDirectiveProperty;
        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Can't load the Directive into the buffer");
    }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    else if(pstrParserInstanceType->bSecondLevelBufferEnabled == STD_TRUE)
    {
        pstrParserInstanceType->objenumErrorType = csIncompleteEntityContent;
    }
#endif
    else
    {
        /*
         * may be the rest of directive will be received later
         */
    }
}
/*
 * assumption that the directive is in the working buffer totally or partially
 */
static BOOLEAN LwXMLP_bReachEndOfDirective(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
          BOOLEAN  bEndOfDirectiveFound = STD_FALSE, bFoundQuot = STD_FALSE, bCommentInprogress = STD_FALSE;
          INT32U   u32FooterLength, u32BufferIndex,u32ExpectCloser = 0;
          INT8U   *pu8WorkingBuffer;
          INT8U    u8LastQuotType = 0U;
    static const INT8S   *pcs8FooterString;
    static const INT8S cs8DeclarationFooterString[]           = "?>";
    static const INT8S cs8PIFooterString[]                    = "?>";
    static const INT8S cs8CDATAFooterString[]                 = "]]>";
    static const INT8S cs8RegularFooterString[]               = ">";
    static const INT8S cs8CommentFooterString[]               = "-->";
    static const INT8S cs8CommentHeaderString[]               = "<!--";

    switch(pstrParserInstanceType->objenumDirectiveTypeType)
    {
        case csCommentMarkup:
             u32FooterLength  = 3U;
             pcs8FooterString = cs8CommentFooterString;
             break;
        case csProcessingInstructionMarkup:
             u32FooterLength  = 2U;
             pcs8FooterString = cs8PIFooterString;
             break;
        case csDeclarationMarkup:
             u32FooterLength  = 2U;
             pcs8FooterString = cs8DeclarationFooterString;
             break;
        case csCDATAMarkup:
            u32FooterLength  = 3U;
            pcs8FooterString = cs8CDATAFooterString;
            break;
        case csDTDMarkup:
#if(ENABLE_DTD_RECEIPTION == STD_ON)
        case csDTDElementMarkup:
        case csDTDEntitytMarkup:
        case csDTDNotationtMarkup:
        case csDTDAttlistMarkup:
        case csInternalDTDMarkup:
        case csExternalDTDMarkup:
#endif
        default:
            u32FooterLength  = 1U;
            pcs8FooterString = cs8RegularFooterString;
            break;
    }
    pu8WorkingBuffer = pstrParserInstanceType->u8XMLworkingBuffer;
    u32BufferIndex = 0U;
    while(u32BufferIndex < pstrParserInstanceType->u32DirectiveLength)
    {
        if((pu8WorkingBuffer[0] ==(INT8U)pcs8FooterString[0]) && (bFoundQuot == STD_FALSE))
        {
            if((strncmp((INT8S const *)pu8WorkingBuffer, pcs8FooterString, u32FooterLength) == 0) && (u32ExpectCloser == 0U))
            {
                bEndOfDirectiveFound = STD_TRUE;
                break;
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
        }
        else
        {
            /*
             * Nothing to do
             */
        }
        if(*pu8WorkingBuffer == LESS_THAN_ASCII_CHARACTER)
        {
            if(strncmp((INT8S const *)pu8WorkingBuffer, cs8CommentHeaderString, strlen(cs8CommentHeaderString)) == 0)
            {
                bCommentInprogress = STD_TRUE;
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
        }
        else
        {
            /*
             * Nothing to do
             */
        }
        /* we should check the content of "" or '' */
        if(((*pu8WorkingBuffer == DOUBLE_QUOT_ASCII_CHARACTER) || (*pu8WorkingBuffer == SINGLE_QUOT_ASCII_CHARACTER)) &&
            ((bCommentInprogress == STD_FALSE) && (pstrParserInstanceType->objenumDirectiveTypeType != csProcessingInstructionMarkup)))
        {
            if (bFoundQuot == STD_TRUE)
            {
                if (u8LastQuotType == pu8WorkingBuffer[0])
                {
                    bFoundQuot = STD_FALSE;
                }
                else
                {
                    /*
                    * Nothing to do
                    */
                }
            }
            else
            {
                bFoundQuot = STD_TRUE;
                u8LastQuotType = pu8WorkingBuffer[0];
            }
        }
        else
        {
            /*
             * Nothing to do
             */
        }
        if((*pu8WorkingBuffer == HYPHEN_ASCII_CHARACTER) && (bCommentInprogress == STD_TRUE))
        {
            if(strncmp((INT8S const *)pu8WorkingBuffer, cs8CommentFooterString, strlen(cs8CommentFooterString)) == 0)
            {
                bCommentInprogress  = STD_FALSE;
                pu8WorkingBuffer    = &pu8WorkingBuffer[2]; /* escape the --> */
                u32BufferIndex     += 2U;
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
        }
        else
        {
            /*
             * Nothing to do
             */
        }
        pu8WorkingBuffer++;
        u32BufferIndex++;
    }

    return bEndOfDirectiveFound;
}
#if(ENABLE_PI_WELL_FORM_CHECK == STD_ON)
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_srParsePI                                        <LwXMLP_CORE>

SYNTAX:         StdReturnType LwXMLP_srParsePI(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)

DESCRIPTION :

PARAMETER1  :   pstrParserInstanceType : pointer to the XML instance


RETURN VALUE:   E_NOT_OK: if parsing the PI is failed
                E_OK    : if parsing the PI is succeeded

Note        :   The API will fail to load the buffer in the following cases:
                -

Note        :   This API assume that the directive is PI is exist in the working buffer <?  ?>

END DESCRIPTION *******************************************************************************************************/
static void LwXMLP_vParsePI(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    typedef enum
    {
        csCheckPINameStart,
        csCheckPIName,
        csCheckPIContent
    }enumParsePIState;
    INT8U         u8PITargetArray[3];
    INT32U        u32PICharacterIndex;
    enumParsePIState objenumParsePIState = csCheckPINameStart;
    INT8U  *pu8WorkingBuffer, u32CharacterLength;

    pu8WorkingBuffer = pstrParserInstanceType->u8XMLworkingBuffer;
    pstrParserInstanceType->pu8ParsedPITarget                         = STD_NULL;
    pstrParserInstanceType->pu8ParsedPIContent                        = STD_NULL;
    pu8WorkingBuffer[0U]                                               = NULL_ASCII_CHARACTER; /* Clear < */
    pu8WorkingBuffer[1U]                                               = NULL_ASCII_CHARACTER; /* Clear ? */
    pu8WorkingBuffer[pstrParserInstanceType->u32DirectiveLength - 2U] = NULL_ASCII_CHARACTER; /* Clear ? */
    pu8WorkingBuffer[pstrParserInstanceType->u32DirectiveLength - 1U] = NULL_ASCII_CHARACTER; /* Clear > */
    u8PITargetArray[0] = NULL_ASCII_CHARACTER;
    u8PITargetArray[1] = NULL_ASCII_CHARACTER;
    u8PITargetArray[2] = NULL_ASCII_CHARACTER;

    /* will escape <? ?>from checking in the for loop */
    for(u32PICharacterIndex = 2U; \
        (u32PICharacterIndex < (pstrParserInstanceType->u32DirectiveLength - 2U)) && \
        (pstrParserInstanceType->objenumErrorType == csNoError);\
        u32PICharacterIndex += u32CharacterLength)
    {
        u32CharacterLength = 1U; /* Assume that the character length is 1 */
        switch(objenumParsePIState)
        {
            case csCheckPINameStart:
                 u32CharacterLength = LwXMLP_u8ValidNameStartCharacter(&pu8WorkingBuffer[u32PICharacterIndex]);
                 if(u32CharacterLength > 0U)
                 {
                     pstrParserInstanceType->pu8ParsedPITarget  = &pu8WorkingBuffer[u32PICharacterIndex];
                     objenumParsePIState                        = csCheckPIName;
                 }
                 else
                 {
                      pstrParserInstanceType->objenumErrorType = csInvalidStartNameCharacter;
                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Start Name character");
                 }
                 break;
            case csCheckPIName:
                 if(LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[u32PICharacterIndex]) == STD_TRUE)
                 {
                      /* reach end of the name */
                     pu8WorkingBuffer[u32PICharacterIndex]      = NULL_ASCII_CHARACTER;
                     pstrParserInstanceType->pu8ParsedPIContent = &pu8WorkingBuffer[u32PICharacterIndex + 1U];
                     /* replace ? with NULL to terminate PI Content */
                     pu8WorkingBuffer[pstrParserInstanceType->u32DirectiveLength - 2U] = NULL_ASCII_CHARACTER;
                     u32CharacterLength                                                = 1U;
                     objenumParsePIState                                               = csCheckPIContent;
                 }
                 else
                 {
                     u32CharacterLength = LwXMLP_u8ValidNameCharacter(&pu8WorkingBuffer[u32PICharacterIndex]);
                     if(u32CharacterLength == 0U)
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidNameCharacter;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Start Name character");
                     }
                     else /* valid Name character */
                     {
                         /*
                          * Nothing to do
                          */
                     }

                 }
                 break;
            case csCheckPIContent:
                 u32CharacterLength = LwXMLP_u8ValidCharacter(&pu8WorkingBuffer[u32PICharacterIndex]);
                 if(u32CharacterLength == 0U)
                 {
                     pstrParserInstanceType->objenumErrorType = csInvalidXMLCharacter;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid XML character");
                 }
                 else
                 {
                     /*
                      * Nothing to do
                      */
                 }
                 break;
            default:
                /*
                 * Nothing to do
                 */
                 break;
        }
    }
    if(pstrParserInstanceType->pu8ParsedPITarget == STD_NULL)
    {
        pstrParserInstanceType->objenumErrorType = csMissingPITarget;
        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing PI Target");
    }
    else if(pstrParserInstanceType->pu8ParsedPIContent != STD_NULL)
    {
        LwXMLP_vNormalizeEndOfLine(pstrParserInstanceType->pu8ParsedPIContent);
    }
    else
    {
        /*
         * Nothing to do
         */
    }
    /*
     * Reload the original target
     */
    if(pstrParserInstanceType->pu8ParsedPITarget != STD_NULL)
    {
        if(strlen((INT8S const *)pstrParserInstanceType->pu8ParsedPITarget) == 3U)
        {
            /*
             * save and convert to lower case the first character
             */
            u8PITargetArray[0] = pstrParserInstanceType->pu8ParsedPITarget[0];
            u8PITargetArray[1] = pstrParserInstanceType->pu8ParsedPITarget[1];
            u8PITargetArray[2] = pstrParserInstanceType->pu8ParsedPITarget[2];
            pstrParserInstanceType->pu8ParsedPITarget[0] = (INT8U)tolower((INT32S)pstrParserInstanceType->pu8ParsedPITarget[0]);
            pstrParserInstanceType->pu8ParsedPITarget[1] = (INT8U)tolower((INT32S)pstrParserInstanceType->pu8ParsedPITarget[1]);
            pstrParserInstanceType->pu8ParsedPITarget[2] = (INT8U)tolower((INT32S)pstrParserInstanceType->pu8ParsedPITarget[2]);
            if(strncmp((INT8S const *)pstrParserInstanceType->pu8ParsedPITarget, "xml", 3U) == 0)
            {
                 pstrParserInstanceType->objenumErrorType = csInvalidPIName;
                 LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid PI Name");
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
            pstrParserInstanceType->pu8ParsedPITarget[0] = u8PITargetArray[0];
            pstrParserInstanceType->pu8ParsedPITarget[1] = u8PITargetArray[1];
            pstrParserInstanceType->pu8ParsedPITarget[2] = u8PITargetArray[2];
        }
        else
        {
            /*
             * Nothing to do
             */
        }
    }
    else
    {
        /*
         * Nothing to so
         */
    }
}
#endif
#if(ENABLE_CDATA_WELL_FORM_CHECK == STD_ON)
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_vCheckWellFormedCDATA                                        <LwXMLP_CORE>

SYNTAX:         StdReturnType LwXMLP_vCheckWellFormedCDATA(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)

DESCRIPTION :   this API will parse/validate the declaration

PARAMETER1  :   pstrParserInstanceType : pointer to the XML instance

RETURN VALUE:   E_NOT_OK: if parsing the Declaration is failed
                E_OK    : if parsing the comment is succeeded

Note        :   The API will fail to load the buffer in the following cases:
                - double -- are found inside the comment

Note        :   This API assume that the directive is exist in the working buffer <!--  -->

END DESCRIPTION *******************************************************************************************************/
static void LwXMLP_vCheckWellFormedCDATA(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    INT8U  *pu8WorkingBuffer;
    INT32U u32CheckingDataAmount, u32CheckedLength, u32CharacterLength;

    /* remove the <![CDATA[ ]]> length */
    pu8WorkingBuffer      = &pstrParserInstanceType->u8XMLworkingBuffer[9];
    pstrParserInstanceType->pu8ParsedDataBuffer = pu8WorkingBuffer;
    u32CheckingDataAmount = pstrParserInstanceType->u32DirectiveLength;
    u32CheckingDataAmount -= 12U;
    u32CharacterLength     = 0;
    u32CheckedLength       = 0;
    /* escape <![CDATA[   */
    while((u32CheckedLength < u32CheckingDataAmount) && (pstrParserInstanceType->objenumErrorType == csNoError))
    {
        u32CharacterLength = LwXMLP_u8ValidCharacter(pu8WorkingBuffer);
        if(u32CharacterLength == 0U)
        {
            pstrParserInstanceType->objenumErrorType = csInvalidXMLCharacter;
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid XML character");
        }
        else
        {
            u32CheckedLength   += u32CharacterLength; /* update character length */
            pu8WorkingBuffer   = &pu8WorkingBuffer[u32CharacterLength];
        }
    }
    if(pstrParserInstanceType->objenumErrorType == csNoError)
    {
        *pu8WorkingBuffer = NULL_ASCII_CHARACTER;
    }
}
#endif
#if(ENABLE_DTD_WELL_FORM_CHECK == STD_ON)
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_srParseDTD                                        <LwXMLP_CORE>

SYNTAX:         StdReturnType LwXMLP_srParseDTD(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)

DESCRIPTION :   this API will parse/validate the declaration

PARAMETER1  :   pstrParserInstanceType : pointer to the XML instance

RETURN VALUE:   E_NOT_OK: if parsing the Declaration is failed
                E_OK    : if parsing the comment is succeeded

Note        :   The API will fail to load the buffer in the following cases:
                - double -- are found inside the comment

Note        :   This API assume that the directive is exist in the working buffer <!--  -->

END DESCRIPTION *******************************************************************************************************/
static void LwXMLP_vValidateDTD(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    INT8U  *pu8WorkingBuffer;
    INT32U u32CheckingDataAmount, u32CheckedLength;
    INT32U u32ExpectedROWMask;

    /* remove the <!DOCTYPE > length */
    //pstrParserInstanceType->u8XMLworkingBuffer[9] = NULL_ASCII_CHARACTER;
    pu8WorkingBuffer       = &pstrParserInstanceType->u8XMLworkingBuffer[10];
    u32CheckingDataAmount  = pstrParserInstanceType->u32DirectiveLength;
    u32CheckingDataAmount -= 10U;
    u32ExpectedROWMask     = NO_ROW_DEFINED;
    pstrParserInstanceType->u32Length     = 0;
    u32CheckedLength       = 0;
    /* escape <!DOCTYPE   */
    do
    {
        pstrParserInstanceType->u32Length = 0;                   /* clear it for the next round */
        if(u32ExpectedROWMask == NO_ROW_DEFINED)
        {
            pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidateName(pu8WorkingBuffer, &pstrParserInstanceType->pu8StartOfName, &pstrParserInstanceType->u32Length);
            if(pstrParserInstanceType->objenumErrorType == csNoError)
            {
                if(pstrParserInstanceType->u32Length > 0U)
                {
                    u32ExpectedROWMask = OPENING_BRACKET_PORTION |
                                         PUBLIC_EXTERNAL_ID_ROW |
                                         SYSTEM_EXTERNAL_ROW |
                                         COMMENT_ROW;
                    u32CheckedLength  += pstrParserInstanceType->u32Length; /* update character length */
                    pu8WorkingBuffer  = &pu8WorkingBuffer[pstrParserInstanceType->u32Length];
                }
            }
        }
        if(((u32ExpectedROWMask & PUBLIC_EXTERNAL_ID_ROW) != 0U) || ((u32ExpectedROWMask & SYSTEM_EXTERNAL_ROW) != 0U))
        {
            pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidateExternalID(pu8WorkingBuffer, &pstrParserInstanceType->u32Length, STD_FALSE);
            if(pstrParserInstanceType->objenumErrorType == csNoError)
            {
                if(pstrParserInstanceType->u32Length > 0U)
                {
                    u32CheckedLength       += pstrParserInstanceType->u32Length; /* update character length */
                    pu8WorkingBuffer       = &pu8WorkingBuffer[pstrParserInstanceType->u32Length];
                    u32ExpectedROWMask     = OPENING_BRACKET_PORTION;
                }
            }
        }
        if((u32ExpectedROWMask & COMMENT_ROW) != 0U)
        {
            if(strncmp((INT8S const *)pu8WorkingBuffer, "<!--", strlen("<!--")) == 0)
            {
                /* will consume comment */
                pstrParserInstanceType->u32Length     = strlen("<!--");
                u32CheckedLength       += pstrParserInstanceType->u32Length; /* update character length */
                pu8WorkingBuffer       = &pu8WorkingBuffer[pstrParserInstanceType->u32Length];
                while(strncmp((INT8S const *)pu8WorkingBuffer, "-->", strlen("-->")) != 0)
                {
                    pu8WorkingBuffer++;
                    u32CheckedLength++;
                }
                pstrParserInstanceType->u32Length = strlen("-->");
                u32CheckedLength   += pstrParserInstanceType->u32Length; /* update character length */
                pu8WorkingBuffer   = &pu8WorkingBuffer[pstrParserInstanceType->u32Length];
            }
        }
        if((u32ExpectedROWMask & OPENING_BRACKET_PORTION) != 0U)
        {
            if(LwXMLP_bIsWhiteSpace(pu8WorkingBuffer) == STD_TRUE)
            {
                 pstrParserInstanceType->u32Length = 1U;
                 u32CheckedLength += pstrParserInstanceType->u32Length; /* update character length */
                 pu8WorkingBuffer = &pu8WorkingBuffer[pstrParserInstanceType->u32Length];
            }
            else if(*pu8WorkingBuffer == OPENING_BRACKET_ASCII_CHARACTER)
            {
                pstrParserInstanceType->u32Length = 1U;
                u32CheckedLength   += pstrParserInstanceType->u32Length; /* update character length */
                pu8WorkingBuffer   = &pu8WorkingBuffer[pstrParserInstanceType->u32Length];
                /* start looking for intSubset */
                u32ExpectedROWMask = PI_ROW |
                                     COMMENT_ROW |
                                     PE_REFERENCE_ROW;
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
        }
        if((u32ExpectedROWMask & PE_REFERENCE_ROW) != 0U)
        {

            if(*pu8WorkingBuffer == PERCENT_ASCII_CHARACTER)
            {
                pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidatePEReference(pu8WorkingBuffer, &pstrParserInstanceType->u32Length);
                if(pstrParserInstanceType->objenumErrorType == csNoError)
                {
                    if(pstrParserInstanceType->u32Length > 0U)
                    {
                        u32CheckedLength   += pstrParserInstanceType->u32Length; /* update character length */
                        pu8WorkingBuffer    = &pu8WorkingBuffer[pstrParserInstanceType->u32Length];
                    }
                    else
                    {
                        /*
                         * Nothing to do
                         */
                    }
                }
                else
                {
                    /*
                     * Nothing to do
                     */
                }
            }
        }
        if(*pu8WorkingBuffer == GREATER_THAN_ASCII_CHARACTER)
        {
            pstrParserInstanceType->u32Length = 1U;
            u32CheckedLength   += pstrParserInstanceType->u32Length; /* update character length */
            pu8WorkingBuffer   = &pu8WorkingBuffer[pstrParserInstanceType->u32Length];
        }
        else if(LwXMLP_bIsWhiteSpace(pu8WorkingBuffer) == STD_TRUE)
        {
            pstrParserInstanceType->u32Length = 1U;
            u32CheckedLength   += pstrParserInstanceType->u32Length; /* update character length */
            pu8WorkingBuffer   = &pu8WorkingBuffer[pstrParserInstanceType->u32Length];
        }
        else if(*pu8WorkingBuffer == CLOSING_BRACKET_CHARACTER)
        {
            pstrParserInstanceType->u32Length = 1U;
            u32CheckedLength   += pstrParserInstanceType->u32Length; /* update character length */
            pu8WorkingBuffer   = &pu8WorkingBuffer[pstrParserInstanceType->u32Length];
        }
        else
        {
            if(pstrParserInstanceType->u32Length == 0U) /* No portion is detected */
            {
                pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "unexpected");
            }
        }
    }
    while((u32CheckedLength < u32CheckingDataAmount) && (pstrParserInstanceType->objenumErrorType == csNoError));
    if(pstrParserInstanceType->objenumDirectiveTypeType == csExternalDTDMarkup)
    {
        pstrParserInstanceType->bParsingDTDInProgress = STD_FALSE;
    }
    else
    {
        /*
         * Nothing to do
         */
    }

}
static LwXMLP_enumErrorType LwXMLP_enumValidatePEReference(INT8U *pu8PEReference, INT32U *pu32PEReferenceLength)
{
    typedef enum
    {
        csCheckPercent,
        csCheckNameStartChar,
        csCheckNameChar,
        csCheckDone
    }enumCheckPEReferenceStateType;
    INT8U                    *pu8CurrentPEReference = pu8PEReference;
    INT32U                    u32CharacterLength;
    LwXMLP_enumErrorType      objenumErrorType = csNoError;
    enumCheckPEReferenceStateType objenumCheckPEReferenceStateType = csCheckPercent;

    *pu32PEReferenceLength = 0;
    u32CharacterLength  = 0;
    do
    {
        switch(objenumCheckPEReferenceStateType)
        {
            case csCheckPercent:
                 if(LwXMLP_bIsWhiteSpace(pu8CurrentPEReference) == STD_TRUE)
                 {
                     u32CharacterLength = 1U; /* update character length */
                 }
                 else if(*pu8CurrentPEReference == PERCENT_ASCII_CHARACTER)
                 {
                     u32CharacterLength = 1U; /* update character length */
                     objenumCheckPEReferenceStateType = csCheckNameStartChar;
                 }
                 else
                 {
                     objenumErrorType = csDTDMissingPortionError;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
                 }
                 break;
            case csCheckNameStartChar:
                 if(LwXMLP_bIsWhiteSpace(pu8CurrentPEReference) == STD_TRUE)
                 {
                     objenumErrorType = csIllegalWhiteSpace;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Illegal WhiteSpace");
                 }
                 else
                 {
                     u32CharacterLength = LwXMLP_u8ValidNameStartCharacter(pu8CurrentPEReference);
                     if(u32CharacterLength > 0U)
                     {
                         objenumCheckPEReferenceStateType = csCheckNameChar;
                     }
                     else
                     {
                          objenumErrorType = csInvalidStartNameCharacter;
                          LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Start Name character");
                     }
                 }
                 break;
            case csCheckNameChar:
                 if(*pu8CurrentPEReference == SEMICOLON_ASCII_CHARACTER)
                 {
                     objenumCheckPEReferenceStateType = csCheckDone;
                 }
                 else
                 {
                     u32CharacterLength = LwXMLP_u8ValidNameCharacter(pu8CurrentPEReference);
                     if(u32CharacterLength  == 0U)
                     {
                         objenumErrorType = csInvalidNameCharacter;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Name character");
                     }
                     else
                     {
                         /*
                          * Nothing to do
                          */
                     }
                 }
                 break;
            case csCheckDone:
            default:
                /*
                 * Nothing to do
                 */
                 break;
        }
        pu8CurrentPEReference          = &pu8CurrentPEReference[u32CharacterLength];
        *pu32PEReferenceLength += u32CharacterLength;
    }
    while((objenumCheckPEReferenceStateType != csCheckDone) && \
          (*pu8CurrentPEReference != GREATER_THAN_ASCII_CHARACTER) &&\
          (objenumErrorType == csNoError));

    if(objenumErrorType == csNoError)
    {
        switch(objenumCheckPEReferenceStateType)
        {
            case csCheckPercent:
                 break;
            case csCheckNameStartChar:
                 break;
            case csCheckNameChar:
                 break;
            case csCheckDone:
                 break;
            default:
                /*
                 * Nothing to do
                 */
                 break;
        }
    }

    return objenumErrorType;
}
static void LwXMLP_vCheckWellFormedEntity(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    typedef enum
    {
        csCheckEntityWhiteSpace,
        csCheckEntityType,
        csCheckAttributeName,
        csCheckEntityDefination,
        csCheckAttributeValueEndQuot,
        csCheckNDATA,
        csCheckNDATAName,
        csCheckDone
    }enumCheckElementStateType;
    typedef enum
    {
        csUnkownDeclaration,
        csGEDeclaration,
        csPEDeclaration
    }enumDeclarationTypeType;
    BOOLEAN                   bChecMandatoryWhiteSpace = STD_FALSE, bEntityAlreadydefined = STD_FALSE;
    INT8U                     *pu8Entity, u8AttributeEndCharacter = 0U, *pu8EntityValue, *pu8EntityName, u8EntityIndex;
    INT8U          const      *pu8EntityEnd;
    INT32U                     u32CurrentEntityLength = 0U;
    enumCheckElementStateType objenumCheckEntityStateType = csCheckEntityWhiteSpace;
    enumDeclarationTypeType   objenumDeclarationTypeType = csUnkownDeclaration;

    pu8Entity                                             = &pstrParserInstanceType->u8XMLworkingBuffer[8];
    pu8EntityEnd                                          = &pstrParserInstanceType->u8XMLworkingBuffer[pstrParserInstanceType->u32DirectiveLength - 1U];
    pu8Entity[pstrParserInstanceType->u32DirectiveLength] = NULL_ASCII_CHARACTER;
    pu8EntityValue                                        = pu8Entity; /* Initial value */
    pu8EntityName                                         = pu8Entity; /* Initial value */
    /* will escape <!ENTITY  > from checking in the for loop */
    pstrParserInstanceType->u32Length  = 0U;
    /* Assume parsed entity */
    pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].bParsedEntity = STD_TRUE;
    for(;((pu8Entity != pu8EntityEnd) && (pstrParserInstanceType->objenumErrorType == csNoError)); )
    {
        switch(objenumCheckEntityStateType)
        {
            case csCheckDone:
                 if(LwXMLP_bIsWhiteSpace(pu8Entity) == STD_FALSE) /* Check for unwanted characters*/
                 {
                     pstrParserInstanceType->objenumErrorType = csWrongDirectiveEnd;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Wrong directive end");
                 }
                 else
                 {
                     pstrParserInstanceType->u32Length = 1U; /* Update character length */
                 }
                 break;
            case csCheckEntityWhiteSpace:
                 if(LwXMLP_bIsWhiteSpace(pu8Entity) == STD_TRUE)
                 {
                     pstrParserInstanceType->u32Length          = 1U; /* update character length */
                     objenumCheckEntityStateType = csCheckEntityType;
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = csMissingWhiteSpaceCharacter;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing WhiteSpace");
                 }
                 break;
            case csCheckEntityType:
                 if(LwXMLP_bIsWhiteSpace(pu8Entity) == STD_TRUE)
                 {
                     pstrParserInstanceType->u32Length = 1U; /* update character length */
                 }
                 else if(*pu8Entity == PERCENT_ASCII_CHARACTER)
                 {
                     pstrParserInstanceType->u32Length          = 1U; /* update character length */
                     objenumCheckEntityStateType = csCheckAttributeName;
                     objenumDeclarationTypeType  = csPEDeclaration;
                     bChecMandatoryWhiteSpace    = STD_TRUE;
                     pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].objenumReferenceTypeType = csParamterEntity;
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidateName(pu8Entity, &pstrParserInstanceType->pu8StartOfName, &pstrParserInstanceType->u32Length);
                     if(pstrParserInstanceType->objenumErrorType == csNoError)
                     {
                         pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].objenumReferenceTypeType = csEntityReference;
                         if(strlen((INT8S const *)pstrParserInstanceType->pu8StartOfName) > MAX_ENTITY_NAME_LENGTH)/* include Ampersand and the semicolumn */
                         {
                             pstrParserInstanceType->objenumErrorType = csLargeEntityNameProperty;
                             LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Can't load the XML Tag into the buffer");
                         }
                         else if(pstrParserInstanceType->u8AvaliableEntities == MAX_ENTITIES)
                         {
                             pstrParserInstanceType->objenumErrorType = csLargeEntityCountProperty;
                             LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Can't load the XML Tag into the buffer");
                         }
                         else if(pstrParserInstanceType->u32Length > 0U)
                         {
                             objenumCheckEntityStateType = csCheckEntityDefination;
                             objenumDeclarationTypeType  = csGEDeclaration;
                             pu8EntityName                = pu8Entity;
                             u32CurrentEntityLength       = 0;
                         }
                         else
                         {
                             pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                             LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
                         }
                     }
                 }
                 break;
            case csCheckEntityDefination:
                 if((*pu8Entity == DOUBLE_QUOT_ASCII_CHARACTER) || \
                    (*pu8Entity == SINGLE_QUOT_ASCII_CHARACTER))
                 {
                     u8AttributeEndCharacter     = *pu8Entity;
                     *pu8Entity                  = NULL_ASCII_CHARACTER;
                     pu8EntityValue              = &pu8Entity[1U];
                     pstrParserInstanceType->u32Length          = 1U; /* update character length */
                     bEntityAlreadydefined       = STD_FALSE;
                     u32CurrentEntityLength      = 0U;
                     objenumCheckEntityStateType = csCheckAttributeValueEndQuot;
                     pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].objenumEntityStoreTypeType = csInternalStorage;
                 }
                 else if(LwXMLP_bIsWhiteSpace(pu8Entity) == STD_TRUE)
                 {
                     pstrParserInstanceType->u32Length = 1U; /* update character length */
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidateExternalID(pu8Entity, &pstrParserInstanceType->u32Length, STD_FALSE);
                     if(pstrParserInstanceType->objenumErrorType == csNoError)
                     {
                         pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].objenumEntityStoreTypeType = csExternalStorage;
                         if(pstrParserInstanceType->u32Length > 0U)
                         {
                             if(objenumDeclarationTypeType == csGEDeclaration)
                             {
                                 objenumCheckEntityStateType = csCheckNDATA;
                                 bChecMandatoryWhiteSpace    = STD_TRUE;
                             }
                             else /* csPEDeclaration */
                             {
                                 objenumCheckEntityStateType = csCheckDone;
                                 (void)strncpy((INT8S *)&pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityName[0],\
                                                                 (INT8S const *)pu8EntityName,\
                                                                 MAX_ENTITY_NAME_LENGTH);
                                 pu8EntityName = &pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityName[0];
                                 pu8EntityName[strlen((INT8S const *)pu8EntityName)] = NULL_ASCII_CHARACTER;
                                 pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityNameLength = \
                                         (INT8U)strlen((INT8S const *)pu8EntityName);
                                 pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityValueLength = 0U;
                                 pstrParserInstanceType->u8AvaliableEntities++;
                             }
                         }
                         else
                         {
                             pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                             LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
                         }
                     }
                     else
                     {
                         /*
                          * Nothing to do
                          */
                     }
                 }
                 break;
            case csCheckNDATA:
                 if((*pu8Entity == DOUBLE_QUOT_ASCII_CHARACTER) || \
                    (*pu8Entity == SINGLE_QUOT_ASCII_CHARACTER))
                 {
                     pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
                 }
                 else if(*pu8Entity == GREATER_THAN_ASCII_CHARACTER)
                 {
                     objenumCheckEntityStateType = csCheckDone;
                     pstrParserInstanceType->u32Length          = 0U; /* update character length */
                 }
                 else if((LwXMLP_bIsWhiteSpace(pu8Entity) == STD_FALSE) && (bChecMandatoryWhiteSpace == STD_TRUE))
                 {
                      pstrParserInstanceType->objenumErrorType = csMissingWhiteSpaceCharacter;
                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing WhiteSpace");
                 }
                 else if(LwXMLP_bIsWhiteSpace(pu8Entity) == STD_TRUE)
                 {
                      pstrParserInstanceType->u32Length         = 1U; /* update character length */
                      bChecMandatoryWhiteSpace = STD_FALSE;
                 }
                 else if(strncmp((INT8S const *)pu8Entity, "NDATA", strlen("NDATA")) == 0) /* check mixed content */
                 {
                     pstrParserInstanceType->u32Length          = strlen("NDATA");
                     objenumCheckEntityStateType = csCheckNDATAName;
                     bChecMandatoryWhiteSpace    = STD_TRUE;
                     pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].bParsedEntity = STD_FALSE;
                 }
                 else if(strncmp((INT8S const *)pu8Entity, "ndata", strlen("ndata")) == 0) /* check mixed content */
                 {
                     pstrParserInstanceType->objenumErrorType = csDTDCapitalPortionError;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD small letter portion is used");
                 }
                 else if(*pu8Entity == GREATER_THAN_ASCII_CHARACTER)
                 {
                     pstrParserInstanceType->u32Length          = 1U; /* update character length */
                     objenumCheckEntityStateType = csCheckDone; /* NDATA is optional */
                 }
                 else /* if we reach here without finding a portion, so it should be an error*/
                 {
                     pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                 }
                 break;
            case csCheckNDATAName:
                 if((LwXMLP_bIsWhiteSpace(pu8Entity) == STD_FALSE) && (bChecMandatoryWhiteSpace == STD_TRUE))
                 {
                      pstrParserInstanceType->objenumErrorType = csMissingWhiteSpaceCharacter;
                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing WhiteSpace");
                 }
                 else if(LwXMLP_bIsWhiteSpace(pu8Entity) == STD_TRUE)
                 {
                      pstrParserInstanceType->u32Length         = 1U; /* update character length */
                      bChecMandatoryWhiteSpace = STD_FALSE;
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidateName(pu8Entity, &pstrParserInstanceType->pu8StartOfName, &pstrParserInstanceType->u32Length);
                     if(pstrParserInstanceType->objenumErrorType == csNoError)
                     {
                         if(pstrParserInstanceType->u32Length > 0U)
                         {
                             objenumCheckEntityStateType = csCheckDone;
                         }
                         else
                         {
                             /*
                              * Nothing to do
                              */
                         }
                     }
                     else
                     {
                         /*
                          * Nothing to do
                          */
                     }
                 }
                 break;
            case csCheckAttributeName:
                 if((LwXMLP_bIsWhiteSpace(pu8Entity) == STD_FALSE) && (bChecMandatoryWhiteSpace == STD_TRUE))
                 {
                      pstrParserInstanceType->objenumErrorType = csMissingWhiteSpaceCharacter;
                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing WhiteSpace");
                 }
                 else if(LwXMLP_bIsWhiteSpace(pu8Entity) == STD_TRUE)
                 {
                      pstrParserInstanceType->u32Length         = 1U; /* update character length */
                      bChecMandatoryWhiteSpace = STD_FALSE;
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidateName(pu8Entity, &pstrParserInstanceType->pu8StartOfName, &pstrParserInstanceType->u32Length);
                     if(pstrParserInstanceType->objenumErrorType == csNoError)
                     {
                         objenumCheckEntityStateType = csCheckEntityDefination;
                         pu8EntityName               = pstrParserInstanceType->pu8StartOfName;
                     }
                     else
                     {
                         /*
                          * Nothing to do
                          */
                     }
                 }
                 break;
            case csCheckAttributeValueEndQuot:
                 if(*pu8Entity == u8AttributeEndCharacter)
                 {
                     pstrParserInstanceType->u32Length = 1U; /* update character length */
                     if(u32CurrentEntityLength < MAX_ENTITY_VALUE_LENGTH)
                     {
                         *pu8Entity         = NULL_ASCII_CHARACTER;
                         (void)strncpy((INT8S *)&pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityName[0],\
                                 (INT8S const *)pu8EntityName,\
                                 MAX_ENTITY_NAME_LENGTH);
                         pu8EntityName = &pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityName[0];
                         pu8EntityName[strlen((INT8S const *)pu8EntityName)] = NULL_ASCII_CHARACTER;
                         for(u8EntityIndex = 0; u8EntityIndex < pstrParserInstanceType->u8AvaliableEntities; ++u8EntityIndex)
                         {
                             if((strncmp((INT8S const *)pu8EntityName, \
                                (INT8S const *)pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityName, \
                                MAX_ENTITY_NAME_LENGTH) == 0) && \
                                (pstrParserInstanceType->objstrEntityType[u8EntityIndex].objenumReferenceTypeType == \
                              pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].objenumReferenceTypeType))
                             {
                                 bEntityAlreadydefined = STD_TRUE;
                                 break;
                             }
                         }
                         if(bEntityAlreadydefined == STD_FALSE)
                         {
                             pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityNameLength = (INT8U)strlen((INT8S const *)pu8EntityName);
                             (void)strncpy((INT8S *)&pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityValue[0],\
                                     (INT8S const *)pu8EntityValue,\
                                     MAX_ENTITY_VALUE_LENGTH);
                             pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityValueLength = \
                                     (INT8U)strlen((INT8S const *)pu8EntityValue);
                             if(pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityValueLength > 0U)
                             {
                                 LwXMLP_vNormalizeEndOfLine(pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityValue);
                                 pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityValueLength = \
                                         (INT8U)strlen((INT8S const *)pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityValue);
                                 pu8EntityValue = &pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityValue[0];
                                 /* The actual replacement text that is included as described above must contain
                                  * the replacement text of any parameter entities referred to, and must contain
                                  * the character referred to, in place of any character references in the literal
                                  * entity value; however, general-entity references must be left as-is,
                                  * unexpanded
                                  */
                                 pstrParserInstanceType->objenumErrorType = LwXMLP_enumUpdateEntityReference(\
                                                                            pstrParserInstanceType,\
                                                                            pu8EntityValue,\
                                                                            strlen((INT8S const *)pu8EntityValue),\
                                                                            UPDATE_CHARACTER_REFERENCE | UPDATE_PARAMETER_REFERENCE);
                                 pstrParserInstanceType->objstrEntityType[pstrParserInstanceType->u8AvaliableEntities].u8EntityValueLength = \
                                         (INT8U)strlen((INT8S const *)pu8EntityValue);

                             }
                             else
                             {
                                 /*
                                  * Nothing to do
                                  */
                             }
                             ++pstrParserInstanceType->u8AvaliableEntities;
                         }
                         else
                         {
                             /*
                              * Nothing to do
                              */
                         }
                         objenumCheckEntityStateType = csCheckDone;
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csLargeEntityValueProperty;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Can't load the XML Tag into the buffer");
                     }
                 }
                 else if(*pu8Entity == PERCENT_ASCII_CHARACTER)
                 {
                     pstrParserInstanceType->objenumErrorType = csPEReferencesinInInternalSubset;
                 }
                 else
                 {
                     pstrParserInstanceType->u32Length = LwXMLP_u8ValidCharacter(pu8Entity);
                     if(pstrParserInstanceType->u32Length == 0U)
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidXMLCharacter;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid XML character");
                     }
                     else/* valid Name character */
                     {
                         u32CurrentEntityLength += pstrParserInstanceType->u32Length; /* increase the length of the entity value */
                     }
                 }
                 break;
            default:
                /*
                 * Nothing to do
                 */
                 break;
        }
        pu8Entity = &pu8Entity[pstrParserInstanceType->u32Length];
    }
    if(pstrParserInstanceType->objenumErrorType == csNoError)
    {
        switch(objenumCheckEntityStateType)
        {
            case csCheckAttributeValueEndQuot:
            case csCheckEntityDefination:
            case csCheckNDATAName:
                 pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                 LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
                 break;
            case csCheckNDATA:
                break;
            case csCheckEntityType:
                 break;
            case csCheckAttributeName:
                 break;
            case csCheckDone:
            case csCheckEntityWhiteSpace:
            default:
                 /*
                  * Nothing to do
                  */
                 break;
        }
    }
}
static void LwXMLP_vValidateElement(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    typedef enum
    {
        csProcessOpenBracketState,
        csCheckName,
        csCheckContentSpec,
        csCheckMixedChildren,
        csCheckChildrenStartNameChar,
        csCheckChildrenEndName,
        csCheckNewName,
        csCkeckSeparator,
        csCheckAsterisk,
        csCheckPCData,
        csCheckDone
    }enumCheckElementStateType;
    BOOLEAN                    bCloseBracketFound = STD_FALSE, bGroupPCDATA = STD_FALSE, bSeparatorIsRequired = STD_FALSE;
    INT8U                      u8StackDepth = 0U;
    INT8U                     *pu8Element, u8NumberOfBrackets = 0U, u8CurrentOperator = NULL_ASCII_CHARACTER;
    INT8U            const    *pu8ElementEnd, *pu8LastCharacter;

    enumCheckElementStateType objenumCheckElementStateType = csCheckName;
    pu8Element                                             = &pstrParserInstanceType->u8XMLworkingBuffer[9];
    pu8Element[pstrParserInstanceType->u32DirectiveLength] = NULL_ASCII_CHARACTER;
    pu8LastCharacter                                       = pu8Element;
    LwXMLP_vRemoveWSInsideElement(pstrParserInstanceType);
    pu8ElementEnd                                          = &pu8Element[strlen((INT8S const *)pu8Element) - 1U];
    memset(pstrParserInstanceType->u8OperatorStack, 0x00, sizeof(pstrParserInstanceType->u8OperatorStack));
    /* will escape <!ELEMENT  > from checking in the for loop */
    pstrParserInstanceType->u32Length  = 0U;
    for(;((pu8Element != pu8ElementEnd) && (pstrParserInstanceType->objenumErrorType == csNoError)); )
    {
        switch(objenumCheckElementStateType)
        {
            case csCheckDone:
                 if(LwXMLP_bIsWhiteSpace(pu8Element) == STD_FALSE) /* check for unwanted characters*/
                 {
                     pstrParserInstanceType->objenumErrorType = csWrongDirectiveEnd;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Wrong directive end");
                 }
                 else
                 {
                     pstrParserInstanceType->u32Length = 1U; /* update character length */
                 }
                 break;
            case csCheckName:
                 pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidateName(pu8Element, &pstrParserInstanceType->pu8StartOfName, &pstrParserInstanceType->u32Length);
                 if(pstrParserInstanceType->objenumErrorType == csNoError)
                 {
                     objenumCheckElementStateType = csCheckContentSpec;
                 }
                 else
                 {
                     /*
                      * Nothing to do
                      */
                 }
                 break;
            case csCheckContentSpec:
                 if(LwXMLP_bIsWhiteSpace(pu8Element) == STD_TRUE)
                 {
                     pstrParserInstanceType->u32Length = 1U; /* update character length */
                 }
                 else if(strncmp((INT8S const *)pu8Element, "EMPTY", strlen("EMPTY")) == 0)
                 {
                     pstrParserInstanceType->u32Length             = strlen("EMPTY");
                     objenumCheckElementStateType = csCheckDone;
                 }
                 else if(strncmp((INT8S const *)pu8Element, "ANY", strlen("ANY")) == 0)
                 {
                     pstrParserInstanceType->u32Length             = strlen("ANY");
                     objenumCheckElementStateType = csCheckPCData;
                 }
                 else if(*pu8Element == OPEN_ROUND_BRACKET_ASCII_CHARACTER)
                 {
                     pstrParserInstanceType->u32Length            = 1U;
                     objenumCheckElementStateType  = csCheckMixedChildren;
                     u8NumberOfBrackets++;
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
                 }
                 break;
            case csCheckMixedChildren:
                 if(LwXMLP_bIsWhiteSpace(pu8Element) == STD_TRUE)
                 {
                      pstrParserInstanceType->u32Length = 1U; /* update character length */
                 }
                 else if(strncmp((INT8S const *)pu8Element, "#PCDATA", strlen("#PCDATA")) == 0) /* check mixed content */
                 {
                     pstrParserInstanceType->u32Length           = strlen("#PCDATA");
                     objenumCheckElementStateType = csCheckPCData;
                     bSeparatorIsRequired         = STD_TRUE;
                 }
                 else if(*pu8Element == OPEN_ROUND_BRACKET_ASCII_CHARACTER)
                 {
                     pstrParserInstanceType->u32Length           = 0U;
                     objenumCheckElementStateType = csProcessOpenBracketState;
                 }
                 else /* check children content */
                 {
                     objenumCheckElementStateType = csCheckChildrenStartNameChar;
                     pstrParserInstanceType->u32Length           = 0U;
                 }
                 break;
            case csProcessOpenBracketState:
                 if(*pu8Element == OPEN_ROUND_BRACKET_ASCII_CHARACTER)
                 {
                     pstrParserInstanceType->u32Length = 1U;
                     u8NumberOfBrackets++;
                     if(u8StackDepth < MAX_OPERATORS_COUNT)
                     {
                         pstrParserInstanceType->u8OperatorStack[u8StackDepth] = u8CurrentOperator;
                         ++u8StackDepth;
                         u8CurrentOperator             = NULL_ASCII_CHARACTER;
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csLargeChildrenOperatorsProperty;
                     }
                 }
                 else if(LwXMLP_bIsWhiteSpace(pu8Element) == STD_TRUE)
                 {
                     pstrParserInstanceType->u32Length = 1U; /* Update character length */
                 }
                 else
                 {
                     objenumCheckElementStateType = csCheckChildrenStartNameChar;
                     pstrParserInstanceType->u32Length           = 0U; /* Update character length */
                 }
                 break;
            case csCheckChildrenStartNameChar:
                 pstrParserInstanceType->u32Length = LwXMLP_u8ValidNameStartCharacter(pu8Element);
                 if(pstrParserInstanceType->u32Length > 0U)
                 {
                     objenumCheckElementStateType = csCheckChildrenEndName;
                 }
                 else
                 {
                      pstrParserInstanceType->objenumErrorType = csInvalidStartNameCharacter;
                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Start Name character");
                 }
                 break;
            case csCheckChildrenEndName:
                 if(*pu8Element == CLOSE_ROUND_BRACKET_ASCII_CHARACTER)
                 {
                     pstrParserInstanceType->u32Length = 1U; /* update character length */
                     bCloseBracketFound = STD_TRUE;
                     u8NumberOfBrackets--;
                     if(u8StackDepth > 0U)
                     {
                         --u8StackDepth;
                         u8CurrentOperator = pstrParserInstanceType->u8OperatorStack[u8StackDepth];
                     }
                     else
                     {
                         /*
                          * Nothing to do
                          */
                     }
                     /*
                      * Check what is next
                      */
                     if(LwXMLP_bIsWhiteSpace(&pu8Element[1]) == STD_TRUE)
                     {
                         objenumCheckElementStateType = csCkeckSeparator;
                     }
                     else if((pu8Element[1] == PLUS_ASCII_CHARACTER)||\
                             (pu8Element[1] == ASTERISK_ASCII_CHARACTER)||\
                             (pu8Element[1] == GREATER_THAN_ASCII_CHARACTER)||\
                             (pu8Element[1] == QUESTION_MARK_ASCII_CHARACTER)||\
                             (pu8Element[1] == COMMA_ASCII_CHARACTER)||\
                             (pu8Element[1] == OR_ASCII_CHARACTER)||\
                             (pu8Element[1] == CLOSE_ROUND_BRACKET_ASCII_CHARACTER))
                     {
                         /*
                          *
                          * Nothing to do
                          */
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidOperator;
                     }
                 }
                 else if(*pu8Element == OR_ASCII_CHARACTER)
                 {
                     if((u8CurrentOperator == NULL_ASCII_CHARACTER) || (u8CurrentOperator == OR_ASCII_CHARACTER))
                     {
                         u8CurrentOperator = OR_ASCII_CHARACTER;
                         pstrParserInstanceType->u32Length           = 1U; /* update character length */
                         if(pu8Element[1] == OPEN_ROUND_BRACKET_ASCII_CHARACTER)
                         {
                             objenumCheckElementStateType = csProcessOpenBracketState;
                         }
                         else
                         {
                             objenumCheckElementStateType = csCheckChildrenStartNameChar;
                         }
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidOperator;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Operator");
                     }
                 }
                 else if(*pu8Element == COMMA_ASCII_CHARACTER)
                 {
                     if((u8CurrentOperator == NULL_ASCII_CHARACTER) || (u8CurrentOperator == COMMA_ASCII_CHARACTER))
                     {
                         u8CurrentOperator = COMMA_ASCII_CHARACTER;
                         pstrParserInstanceType->u32Length           = 1U; /* update character length */
                         if(pu8Element[1] == OPEN_ROUND_BRACKET_ASCII_CHARACTER)
                         {
                             objenumCheckElementStateType = csProcessOpenBracketState;
                         }
                         else
                         {
                             objenumCheckElementStateType = csCheckChildrenStartNameChar;
                         }
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidOperator;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Operator");
                     }
                 }
                 else if((*pu8Element == PLUS_ASCII_CHARACTER) ||\
                         (*pu8Element == ASTERISK_ASCII_CHARACTER) ||\
                         (*pu8Element == QUESTION_MARK_ASCII_CHARACTER))
                 {
                     pstrParserInstanceType->u32Length           = 1U; /* update character length */
                     objenumCheckElementStateType = csCkeckSeparator;
                 }
                 else
                 {
                     pstrParserInstanceType->u32Length = LwXMLP_u8ValidNameCharacter(pu8Element);
                     if((pstrParserInstanceType->u32Length == 0U) || (bCloseBracketFound == STD_TRUE))
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidNameCharacter;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Name character");
                     }
                     else
                     {
                         /*
                          * Nothing to do
                          */
                     }

                 }
                 break;
            case csCkeckSeparator:
                 if(LwXMLP_bIsWhiteSpace(pu8Element) == STD_TRUE)
                 {
                     pstrParserInstanceType->u32Length = 1U; /* update character length */
                 }
                 else if(*pu8Element == CLOSE_ROUND_BRACKET_ASCII_CHARACTER)
                 {
                     pstrParserInstanceType->u32Length = 1U; /* Update character length */
                     bCloseBracketFound = STD_TRUE;
                     u8NumberOfBrackets--;
                     if(u8StackDepth > 0U)
                     {
                         --u8StackDepth;
                         u8CurrentOperator = pstrParserInstanceType->u8OperatorStack[u8StackDepth];
                     }
                     else
                     {
                         /*
                          * Nothing to do
                          */
                     }
                     objenumCheckElementStateType = csCheckNewName;
                 }
                 else if(*pu8Element == OR_ASCII_CHARACTER)
                 {
                     if((u8CurrentOperator == NULL_ASCII_CHARACTER) || (u8CurrentOperator == OR_ASCII_CHARACTER))
                     {
                         u8CurrentOperator = OR_ASCII_CHARACTER;
                         pstrParserInstanceType->u32Length           = 1U; /* update character length */
                         if(pu8Element[1] == OPEN_ROUND_BRACKET_ASCII_CHARACTER)
                         {
                             objenumCheckElementStateType = csProcessOpenBracketState;
                         }
                         else
                         {
                             objenumCheckElementStateType = csCheckChildrenStartNameChar;
                         }
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidOperator;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Operator");
                     }
                 }
                 else if(*pu8Element == COMMA_ASCII_CHARACTER)
                 {
                     if((u8CurrentOperator == NULL_ASCII_CHARACTER) || (u8CurrentOperator == COMMA_ASCII_CHARACTER))
                     {
                         u8CurrentOperator = COMMA_ASCII_CHARACTER;
                         pstrParserInstanceType->u32Length           = 1U; /* update character length */
                         if(pu8Element[1] == OPEN_ROUND_BRACKET_ASCII_CHARACTER)
                         {
                            objenumCheckElementStateType = csProcessOpenBracketState;
                         }
                         else
                         {
                             objenumCheckElementStateType = csCheckChildrenStartNameChar;
                         }
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidOperator;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Operator");
                     }
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = csInvalidOperator;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Operator");
                 }
                 break;
            case csCheckNewName:
                 if(LwXMLP_bIsWhiteSpace(pu8Element) == STD_TRUE)
                 {
                     pstrParserInstanceType->u32Length = 1U; /* update character length */
                 }
                 else if((*pu8Element == COMMA_ASCII_CHARACTER) || (*pu8Element == OR_ASCII_CHARACTER))
                 {
                     pstrParserInstanceType->objenumErrorType = csInvalidOperator;
                 }
                 else if(*pu8Element == CLOSE_ROUND_BRACKET_ASCII_CHARACTER)
                 {
                     pstrParserInstanceType->u32Length = 1U; /* update character length */
                     u8CurrentOperator  = NULL_ASCII_CHARACTER;
                     u8NumberOfBrackets--;
                     if(u8StackDepth > 0U)
                     {
                         --u8StackDepth;
                         u8CurrentOperator = pstrParserInstanceType->u8OperatorStack[u8StackDepth];
                     }
                     else
                     {
                         /*
                          * Nothing to do
                          */
                     }
                 }
                 else if(*pu8Element == OPEN_ROUND_BRACKET_ASCII_CHARACTER)
                 {
                     pstrParserInstanceType->u32Length           = 0U;
                     objenumCheckElementStateType = csProcessOpenBracketState;
                 }
                 else if(*pu8Element == ASTERISK_ASCII_CHARACTER)
                 {
                     if(LwXMLP_bIsWhiteSpace(pu8LastCharacter) == STD_TRUE)
                     {
                         pstrParserInstanceType->objenumErrorType = csIllegalWhiteSpace;
                     }
                     else
                     {
                         pstrParserInstanceType->u32Length   = 1U; /* Update character length */
                         objenumCheckElementStateType = csCkeckSeparator;
                     }
                 }
                 else if(*pu8Element == QUESTION_MARK_ASCII_CHARACTER)
                 {
                     if(LwXMLP_bIsWhiteSpace(pu8LastCharacter) == STD_TRUE)
                     {
                         pstrParserInstanceType->objenumErrorType = csIllegalWhiteSpace;
                     }
                     else
                     {
                         pstrParserInstanceType->u32Length   = 1U; /* update character length */
                         objenumCheckElementStateType = csCkeckSeparator;
                     }
                 }
                 else if(*pu8Element == PLUS_ASCII_CHARACTER)
                 {
                     if(LwXMLP_bIsWhiteSpace(pu8LastCharacter) == STD_TRUE)
                     {
                         pstrParserInstanceType->objenumErrorType = csIllegalWhiteSpace;
                     }
                     else
                     {
                         pstrParserInstanceType->u32Length   = 1U; /* update character length */
                         objenumCheckElementStateType = csCkeckSeparator;
                     }
                 }
                 else
                 {
                     pstrParserInstanceType->u32Length           = 0U; /* update character length */
                     objenumCheckElementStateType = csCheckChildrenStartNameChar;
                 }
                 break;
            case csCheckPCData:
                if(LwXMLP_bIsWhiteSpace(pu8Element) == STD_TRUE)
                {
                     pstrParserInstanceType->u32Length = 1U; /* update character length */
                }
                else if(*pu8Element == CLOSE_ROUND_BRACKET_ASCII_CHARACTER)
                {
                    objenumCheckElementStateType = csCheckAsterisk;
                    pstrParserInstanceType->u32Length           = 1U; /* update character length */
                    u8NumberOfBrackets--;
                    if(u8StackDepth > 0U)
                    {
                        --u8StackDepth;
                        u8CurrentOperator = pstrParserInstanceType->u8OperatorStack[u8StackDepth];
                    }
                    else
                    {
                        /*
                         * Nothing to do
                         */
                    }
                }
                else if(*pu8Element == OR_ASCII_CHARACTER)
                {
                    pstrParserInstanceType->u32Length   = 1U; /* update character length */
                    bGroupPCDATA         = STD_TRUE;
                    bSeparatorIsRequired = STD_FALSE;
                }
                else if(*pu8Element == COMMA_ASCII_CHARACTER)
                {
                    pstrParserInstanceType->objenumErrorType = csInvalidOperator;
                    LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Operator");
                }
                else if(*pu8Element == ASTERISK_ASCII_CHARACTER)
                {
                    pstrParserInstanceType->objenumErrorType = csInvalidOperator;
                    LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Operator");
                }
                else
                {
                    if(bSeparatorIsRequired == STD_TRUE)
                    {
                        pstrParserInstanceType->objenumErrorType = csMissingSeparator;
                    }
                    else
                    {
                        /*
                         * Nothing to do
                         */
                    }
                }
                break;
            case csCheckAsterisk:
                 if(*pu8Element == ASTERISK_ASCII_CHARACTER)
                 {
                     objenumCheckElementStateType = csCheckDone;
                 }
                 else if(bGroupPCDATA == STD_TRUE)
                 {
                     pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                 }
                 else if(LwXMLP_bIsWhiteSpace(pu8Element) == STD_TRUE)
                 {
                      pstrParserInstanceType->u32Length           = 1U; /* update character length */
                      objenumCheckElementStateType = csCheckDone;
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                 }
                 break;
            default:
                /*
                 * Nothing to do
                 */
                 break;
        }
        pu8LastCharacter = pu8Element;
        pu8Element       = &pu8Element[pstrParserInstanceType->u32Length];
    }
    if(pstrParserInstanceType->objenumErrorType == csNoError)
    {
        switch(objenumCheckElementStateType)
        {
            case csCheckName:
                 break;
            case csCheckContentSpec:
                 pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                 break;
            case csCheckMixedChildren:
                 break;
            case csCheckChildrenStartNameChar:
                 break;
            case csCheckChildrenEndName:
                 break;
            case csCheckNewName:
                 break;
            case csCkeckSeparator:
                 break;
            case csCheckAsterisk:
                 if(bGroupPCDATA == STD_TRUE)
                 {
                     pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                 }
                 else
                 {
                     /*
                      * Nothing to do
                      */
                 }
                 break;
            case csProcessOpenBracketState:
                 break;
            case csCheckPCData:
                 break;
            case csCheckDone:
            default:
                /*
                 * Nothing to do
                 */
                 break;
        }
    }
    if((pstrParserInstanceType->objenumErrorType == csNoError) && (u8NumberOfBrackets != 0U))
    {
        pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
    }
    else
    {
        /*
         * Nothing to do
         */
    }
}
static void LwXMLP_vRemoveWSInsideElement(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    BOOLEAN bBracketDetected = STD_FALSE, bFoundWhiteSpace = STD_FALSE;
    INT8U  *pu8Element = &pstrParserInstanceType->u8XMLworkingBuffer[9];
    INT32U  u32CharacterLength = 0U;
    INT8U  const *pu8LastNonWhiteSpaceCharacter = pu8Element;

    do
    {
        if(LwXMLP_bIsWhiteSpace(pu8Element) == STD_TRUE)
        {
            bFoundWhiteSpace   = STD_TRUE;
            /*
             * will escape the removing if we didn't find open bracket
             */
            if(bBracketDetected == STD_TRUE)
            {
                switch(pu8Element[1U])
                {
                    case PLUS_ASCII_CHARACTER:
                    case ASTERISK_ASCII_CHARACTER:
                    case QUESTION_MARK_ASCII_CHARACTER:
                         pstrParserInstanceType->objenumErrorType = csIllegalWhiteSpace;
                         break;
                    default:
                         /*
                          * Nothing to do
                          */
                         break;
                }
                LwXMLP_vMoveStringBlockLeft(pu8Element, 1U);
                u32CharacterLength = 0U;
            }
            else
            {
                u32CharacterLength = 1U;
            }
        }
        else if(strncmp((INT8S const *)pu8Element, "#PCDATA", strlen("#PCDATA")) == 0) /* check mixed content */
        {
            u32CharacterLength            = strlen("#PCDATA");
            pu8LastNonWhiteSpaceCharacter = pu8Element;
            bFoundWhiteSpace              = STD_FALSE;
        }
        else if(*pu8Element == OPEN_ROUND_BRACKET_ASCII_CHARACTER)
        {
            bBracketDetected   = STD_TRUE;
            u32CharacterLength = 1U;
            pu8LastNonWhiteSpaceCharacter = pu8Element;
            bFoundWhiteSpace              = STD_FALSE;
        }
        else
        {
            switch(*pu8LastNonWhiteSpaceCharacter)
            {
                case PLUS_ASCII_CHARACTER:
                case QUESTION_MARK_ASCII_CHARACTER:
                case ASTERISK_ASCII_CHARACTER:
                case COMMA_ASCII_CHARACTER:
                case OR_ASCII_CHARACTER:
                case CLOSE_ROUND_BRACKET_ASCII_CHARACTER:
                case OPEN_ROUND_BRACKET_ASCII_CHARACTER:
                     break;
                default:
                     switch(*pu8Element)
                     {
                         case PLUS_ASCII_CHARACTER:
                         case QUESTION_MARK_ASCII_CHARACTER:
                         case ASTERISK_ASCII_CHARACTER:
                         case COMMA_ASCII_CHARACTER:
                         case OR_ASCII_CHARACTER:
                         case CLOSE_ROUND_BRACKET_ASCII_CHARACTER:
                         case OPEN_ROUND_BRACKET_ASCII_CHARACTER:
                              break;
                         default:
                              if((bFoundWhiteSpace == STD_TRUE) && (bBracketDetected == STD_TRUE))
                              {
                                  pstrParserInstanceType->objenumErrorType = csMissingSeparator;
                              }
                              else
                              {
                                   /*
                                   * Nothing to do
                                   */
                              }
                              break;

                     }
                     break;
            }
            u32CharacterLength = LwXMLP_u8ValidCharacter(pu8Element);
            if(u32CharacterLength == 0U)
            {
                pstrParserInstanceType->objenumErrorType = csInvalidXMLCharacter;
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
            bFoundWhiteSpace              = STD_FALSE;
            pu8LastNonWhiteSpaceCharacter = pu8Element;
        }
        pu8Element       = &pu8Element[u32CharacterLength];
    }while((*pu8Element != NULL_ASCII_CHARACTER) && (pstrParserInstanceType->objenumErrorType == csNoError));
}
static void LwXMLP_vValidateAttributeList(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    typedef enum
    {
        csCheckElementName,
        csCheckAttributeName,
        csCheckAttributeType,
        csCheckAttributeValueStartQuot,
        csCheckAttributeValueEndQuot,
        csCheckNotation,
        csCheckDefaultDecleration,
        csCheckDone
    }enumCheckAttributeStateType;
    BOOLEAN               bChecMandatoryWhiteSpace = STD_FALSE, bValueIsEmpty = STD_FALSE;
    INT8U                *pu8AttributeList, u8AttributeEndCharacter = 0U, *pu8TempAttributeValue;
    INT8U                 u8NumberOfBrackets = 0U, u8EntityReferenceUpdateTrials, u8LastReferencesCountFound = 0U, u8ReferenceIndex;
    INT8U                 u8EntityIndex;
    INT8U   const        *pu8ElementName, *pu8EntityValue;
    INT8U   const        *pu8AttributeListEnd;
    enumCheckAttributeStateType objenumCheckAttributeStateType = csCheckElementName;

    pu8AttributeList                                             = &pstrParserInstanceType->u8XMLworkingBuffer[9];
    pu8AttributeListEnd                                          = &pstrParserInstanceType->u8XMLworkingBuffer[pstrParserInstanceType->u32DirectiveLength - 1U];
    pu8AttributeList[pstrParserInstanceType->u32DirectiveLength] = NULL_ASCII_CHARACTER;
    pu8ElementName                                               = pu8AttributeList; /* init value */
    pu8TempAttributeValue                                        = pu8AttributeList; /* init value */
    /* will escape <!ATTLIST  > from checking in the for loop */
    pstrParserInstanceType->u32Length  = 0U;
    for(;((pu8AttributeList != pu8AttributeListEnd) && (pstrParserInstanceType->objenumErrorType == csNoError)); )
    {
        switch(objenumCheckAttributeStateType)
        {
            case csCheckDone:
                 if(LwXMLP_bIsWhiteSpace(pu8AttributeList) == STD_FALSE) /* check for unwanted characters*/
                 {
                     pstrParserInstanceType->objenumErrorType = csWrongDirectiveEnd;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Wrong directive end");
                 }
                 else
                 {
                     pstrParserInstanceType->u32Length = 1U; /* update character length */
                 }
                 break;
            case csCheckElementName:
                 pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidateName(pu8AttributeList, &pstrParserInstanceType->pu8StartOfName, &pstrParserInstanceType->u32Length);
                 if(pstrParserInstanceType->objenumErrorType == csNoError)
                 {
                     objenumCheckAttributeStateType = csCheckAttributeName;
                     if(strlen((INT8S *)pstrParserInstanceType->pu8StartOfName) > MAX_ELEMENT_NAME_LENGTH)
                     {
                         pstrParserInstanceType->objenumErrorType = csLargeElementNameProperty;
                     }
                     else if(pstrParserInstanceType->u8AvaliableAttibuteListCount > (MAX_ATTRIBUTES_LIST - 1U))
                     {
                         pstrParserInstanceType->objenumErrorType = csLargeNumberOfAttributeList;
                     }
                     else
                     {

                         pu8ElementName = pstrParserInstanceType->pu8StartOfName;
                     }
                 }
                 else
                 {
                     /*
                      * Nothing to do
                      */
                 }
                 break;
            case csCheckAttributeName:
                 if((LwXMLP_bIsWhiteSpace(pu8AttributeList) == STD_FALSE) && (bChecMandatoryWhiteSpace == STD_TRUE))
                 {
                      pstrParserInstanceType->objenumErrorType = csMissingWhiteSpaceCharacter;
                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing WhiteSpace");
                 }
                 else if(LwXMLP_bIsWhiteSpace(pu8AttributeList) == STD_TRUE)
                 {
                      pstrParserInstanceType->u32Length         = 1U; /* update character length */
                      bChecMandatoryWhiteSpace = STD_FALSE;
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidateName(pu8AttributeList, &pstrParserInstanceType->pu8StartOfName, &pstrParserInstanceType->u32Length);
                     if(pstrParserInstanceType->objenumErrorType == csNoError)
                     {
                         objenumCheckAttributeStateType = csCheckAttributeType;
                         if(pstrParserInstanceType->u32Length < MAX_ATTRIBUTES_NAME_LENGTH)
                         {
                            if(pstrParserInstanceType->u8AvaliableAttibuteListCount < (MAX_ATTRIBUTES_LIST - 1U))
                             {
                                 (void)strncpy((INT8S *)pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].u8ElementName, \
                                         (INT8S const *)pu8ElementName, \
                                         MAX_ELEMENT_NAME_LENGTH);
                                 (void)strncpy((INT8S *)pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].u8AttributeName, \
                                         (INT8S const *)pstrParserInstanceType->pu8StartOfName, \
                                         MAX_ATTRIBUTES_NAME_LENGTH);
                             }
                             else
                             {
                                 pstrParserInstanceType->objenumErrorType = csLargeNumberOfAttributeList;
                             }
                         }
                         else
                         {
                             pstrParserInstanceType->objenumErrorType = csLargeAttributeNameProperty;
                         }
                     }
                     else
                     {
                         /*
                          * Nothing to do
                          */
                     }
                 }
                 break;
            case csCheckAttributeType:
                 if(LwXMLP_bIsWhiteSpace(pu8AttributeList) == STD_TRUE)
                 {
                      pstrParserInstanceType->u32Length = 1U; /* update character length */
                 }
                 else if(strncmp((INT8S const *)pu8AttributeList, "CDATA", strlen("CDATA")) == 0)
                 {
                     pstrParserInstanceType->u32Length             = strlen("CDATA");
                     objenumCheckAttributeStateType = csCheckDefaultDecleration;
                     pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].objenumAttibuteTypeType = csCDATAType;
                     bChecMandatoryWhiteSpace       = STD_TRUE;
                 }
                 else if(strncmp((INT8S const *)pu8AttributeList, "IDREFS", strlen("IDREFS")) == 0)
                 {
                     pstrParserInstanceType->u32Length             = strlen("IDREFS");
                     objenumCheckAttributeStateType = csCheckDefaultDecleration;
                     pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].objenumAttibuteTypeType = csIFREFSType;
                     bChecMandatoryWhiteSpace       = STD_TRUE;
                 }
                 else if(strncmp((INT8S const *)pu8AttributeList, "IDREF", strlen("IDREF")) == 0)
                 {
                     pstrParserInstanceType->u32Length             = strlen("IDREF");
                     pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].objenumAttibuteTypeType = csIFREFType;
                     objenumCheckAttributeStateType = csCheckDefaultDecleration;
                     bChecMandatoryWhiteSpace       = STD_TRUE;
                 }
                 else if(strncmp((INT8S const *)pu8AttributeList, "ID", strlen("ID")) == 0)
                 {
                     pstrParserInstanceType->u32Length             = strlen("ID");
                     objenumCheckAttributeStateType = csCheckDefaultDecleration;
                     pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].objenumAttibuteTypeType = csIDType;
                     bChecMandatoryWhiteSpace       = STD_TRUE;
                 }
                 else if(strncmp((INT8S const *)pu8AttributeList, "ENTITY", strlen("ENTITY")) == 0)
                 {
                     pstrParserInstanceType->u32Length             = strlen("ENTITY");
                     objenumCheckAttributeStateType = csCheckDefaultDecleration;
                     pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].objenumAttibuteTypeType = csENTITYType;
                     bChecMandatoryWhiteSpace       = STD_TRUE;
                 }
                 else if(strncmp((INT8S const *)pu8AttributeList, "ENTITIES", strlen("ENTITIES")) == 0)
                 {
                     pstrParserInstanceType->u32Length             = strlen("ENTITIES");
                     objenumCheckAttributeStateType = csCheckDefaultDecleration;
                     pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].objenumAttibuteTypeType = csENTITIESType;
                     bChecMandatoryWhiteSpace       = STD_TRUE;
                 }
                 else if(strncmp((INT8S const *)pu8AttributeList, "NMTOKENS", strlen("NMTOKENS")) == 0)
                 {
                     pstrParserInstanceType->u32Length             = strlen("NMTOKENS");
                     objenumCheckAttributeStateType = csCheckDefaultDecleration;
                     pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].objenumAttibuteTypeType = csNMTOKENSType;
                     bChecMandatoryWhiteSpace       = STD_TRUE;
                 }
                 else if(strncmp((INT8S const *)pu8AttributeList, "NMTOKEN", strlen("NMTOKEN")) == 0)
                 {
                     pstrParserInstanceType->u32Length             = strlen("NMTOKEN");
                     objenumCheckAttributeStateType = csCheckDefaultDecleration;
                     pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].objenumAttibuteTypeType = csNMTOKENType;
                     bChecMandatoryWhiteSpace       = STD_TRUE;
                 }
                 else if(strncmp((INT8S const *)pu8AttributeList, "NOTATION", strlen("NOTATION")) == 0)
                 {
                     pstrParserInstanceType->u32Length = strlen("NOTATION");
                     objenumCheckAttributeStateType = csCheckNotation;
                     pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].objenumAttibuteTypeType = csNOTATIONType;
                     bChecMandatoryWhiteSpace       = STD_TRUE;
                 }
                 else if(*pu8AttributeList == OPEN_ROUND_BRACKET_ASCII_CHARACTER)
                 {
                     pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].objenumAttibuteTypeType = csEnumertaedValueListType;
                     pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidateEnumeration(pu8AttributeList, &pstrParserInstanceType->u32Length);
                     if(pstrParserInstanceType->objenumErrorType == csNoError)
                     {
                         objenumCheckAttributeStateType = csCheckDefaultDecleration;
                         bChecMandatoryWhiteSpace       = STD_TRUE;
                     }
                     else
                     {
                          /*
                          * Nothing to do
                          */
                     }
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = csDTDInvalidAttributeType;
                 }
                 break;
            case csCheckNotation:
                 if((LwXMLP_bIsWhiteSpace(pu8AttributeList) == STD_FALSE) && (bChecMandatoryWhiteSpace == STD_TRUE))
                 {
                      pstrParserInstanceType->objenumErrorType = csMissingWhiteSpaceCharacter;
                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing WhiteSpace");
                 }
                 else if(LwXMLP_bIsWhiteSpace(pu8AttributeList) == STD_TRUE)
                 {
                      pstrParserInstanceType->u32Length         = 1U; /* update character length */
                      bChecMandatoryWhiteSpace = STD_FALSE;
                 }
                 else if(*pu8AttributeList == OPEN_ROUND_BRACKET_ASCII_CHARACTER)
                 {
                     pstrParserInstanceType->u32Length             = 1U; /* update character length */
                     bValueIsEmpty                  = STD_TRUE;
                     u8NumberOfBrackets++;
                 }
                 else if(*pu8AttributeList == CLOSE_ROUND_BRACKET_ASCII_CHARACTER)
                 {
                     if(u8NumberOfBrackets > 0U)
                     {
                         if(bValueIsEmpty == STD_TRUE)
                         {
                             pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                             LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
                         }
                         else
                         {
                             pstrParserInstanceType->u32Length             = 1U; /* update character length */
                             objenumCheckAttributeStateType = csCheckDefaultDecleration;
                             bChecMandatoryWhiteSpace       = STD_TRUE;
                             u8NumberOfBrackets--;
                         }
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                     }
                 }
                 else if(*pu8AttributeList == OR_ASCII_CHARACTER)
                 {
                     pstrParserInstanceType->u32Length         = 1U; /* update character length */
                 }
                 else
                 {
                     pstrParserInstanceType->u32Length = LwXMLP_u8ValidNameCharacter(pu8AttributeList);
                     if(pstrParserInstanceType->u32Length == 0U)
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidNameCharacter;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Start Name character");
                     }
                     else
                     {/* will be used to check the existence of the value */
                         bValueIsEmpty = STD_FALSE;
                     }
                 }
                 break;
            case csCheckDefaultDecleration:
                 if((LwXMLP_bIsWhiteSpace(pu8AttributeList) == STD_FALSE) && (bChecMandatoryWhiteSpace == STD_TRUE))
                 {
                      pstrParserInstanceType->objenumErrorType = csDTDMissingSystemIDError;
                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing WhiteSpace");
                 }
                 else if(LwXMLP_bIsWhiteSpace(pu8AttributeList) == STD_TRUE)
                 {
                      pstrParserInstanceType->u32Length       = 1U; /* update character length */
                      bChecMandatoryWhiteSpace = STD_FALSE;
                 }
                 else if(strncmp((INT8S const *)pu8AttributeList, "#REQUIRED", strlen("#REQUIRED")) == 0)
                 {
                     pstrParserInstanceType->u32Length = strlen("#REQUIRED");
                     objenumCheckAttributeStateType = csCheckAttributeName;
                     pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].objenumAttributeBehaviourType = csRequired;
                     pstrParserInstanceType->u8AvaliableAttibuteListCount++;
                 }
                 else if(strncmp((INT8S const *)pu8AttributeList, "#IMPLIED", strlen("#IMPLIED")) == 0)
                 {
                     pstrParserInstanceType->u32Length = strlen("#IMPLIED");
                     objenumCheckAttributeStateType = csCheckAttributeName;
                     pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].objenumAttributeBehaviourType = csImplied;
                     pstrParserInstanceType->u8AvaliableAttibuteListCount++;
                 }
                 else if(strncmp((INT8S const *)pu8AttributeList, "#FIXED", strlen("#FIXED")) == 0)
                 {
                     pstrParserInstanceType->u32Length             = strlen("#FIXED");
                     pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].objenumAttributeBehaviourType = csFixed;
                     objenumCheckAttributeStateType = csCheckAttributeValueStartQuot;
                     bChecMandatoryWhiteSpace       = STD_TRUE;
                 }
                 else if((*pu8AttributeList == DOUBLE_QUOT_ASCII_CHARACTER) || \
                         (*pu8AttributeList == SINGLE_QUOT_ASCII_CHARACTER))
                 {
                     u8AttributeEndCharacter        = *pu8AttributeList;
                     pu8TempAttributeValue          = &pu8AttributeList[1U];
                     *pu8AttributeList              = NULL_ASCII_CHARACTER;
                     pstrParserInstanceType->u32Length             = 1U; /* update character length */
                     objenumCheckAttributeStateType = csCheckAttributeValueEndQuot;
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = csDTDInvalidDefaultDeclaration;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Attribute Default Declaration");
                 }
                 break;

            case csCheckAttributeValueStartQuot:
                 if((LwXMLP_bIsWhiteSpace(pu8AttributeList) == STD_FALSE) && (bChecMandatoryWhiteSpace == STD_TRUE))
                 {
                      pstrParserInstanceType->objenumErrorType = csMissingWhiteSpaceCharacter;
                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing WhiteSpace");
                 }
                 else if(LwXMLP_bIsWhiteSpace(pu8AttributeList) == STD_TRUE)
                 {
                      pstrParserInstanceType->u32Length       = 1U; /* update character length */
                      bChecMandatoryWhiteSpace = STD_FALSE;
                 }
                 else if((*pu8AttributeList == DOUBLE_QUOT_ASCII_CHARACTER) || \
                         (*pu8AttributeList == SINGLE_QUOT_ASCII_CHARACTER))
                 {
                     u8AttributeEndCharacter        = *pu8AttributeList;
                     pu8TempAttributeValue          = &pu8AttributeList[1U];
                     *pu8AttributeList              = NULL_ASCII_CHARACTER;
                     pstrParserInstanceType->u32Length             = 1U; /* update character length */
                     objenumCheckAttributeStateType = csCheckAttributeValueEndQuot;
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = csMissingQuote;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing quote");
                 }
                 break;
            case csCheckAttributeValueEndQuot:
                 if((*pu8AttributeList == DOUBLE_QUOT_ASCII_CHARACTER) || \
                   (*pu8AttributeList == SINGLE_QUOT_ASCII_CHARACTER))
                 {
                     pstrParserInstanceType->u32Length = 1U; /* update character length */
                     if(*pu8AttributeList == u8AttributeEndCharacter)
                     {
                         *pu8AttributeList = NULL_ASCII_CHARACTER;
                         (void)strncpy((INT8S *)pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].u8AttributeValue, \
                                 (INT8S const *)pu8TempAttributeValue, \
                                 MAX_ATTRIBUTES_VALUE_LENGTH);
                         pu8TempAttributeValue = pstrParserInstanceType->objstrAttributeListType[pstrParserInstanceType->u8AvaliableAttibuteListCount].u8AttributeValue;
                         for(u8EntityReferenceUpdateTrials = 0; \
                             (u8EntityReferenceUpdateTrials < MAX_ENTITY_RESOLVING_DEPTH) && \
                             (pstrParserInstanceType->objenumErrorType == csNoError); \
                             ++u8EntityReferenceUpdateTrials)
                         {
                             pstrParserInstanceType->objenumErrorType = LwXMLP_enumUpdateEntityReference(pstrParserInstanceType, \
                                                                                                         pu8TempAttributeValue,\
                                                                                                         strlen((INT8S const *)pu8TempAttributeValue),\
                                                                                                         UPDATE_ENTITY_REFERENCE);
                             if((pstrParserInstanceType->u8FoundReference > 0U) &&\
                                (pstrParserInstanceType->objenumErrorType == csNoError))
                             {
                                 u8LastReferencesCountFound = pstrParserInstanceType->u8FoundReference;
                             }
                             else
                             {
                                 break;
                             }
                         }
                         if((u8LastReferencesCountFound > 0U) && (pstrParserInstanceType->objenumErrorType == csNoError))
                         {
                             for(u8ReferenceIndex = 0U; \
                                 (u8ReferenceIndex < u8LastReferencesCountFound) && \
                                 (pstrParserInstanceType->objenumErrorType == csNoError); \
                                 ++u8ReferenceIndex)
                             {
                                 if(pstrParserInstanceType->objstrFoundReferenceType[u8ReferenceIndex].objenumReferenceTypeType == csEntityReference)
                                 {   /* check that the found reference had been checked before if it is began in one entity and end in another */
                                     /* Rule 4.3.2 */
                                     u8EntityIndex  = pstrParserInstanceType->objstrFoundReferenceType[u8ReferenceIndex].u8EntityIndex;
                                     pu8EntityValue = pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityValue;
                                     if(pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityValueLength > 0U)
                                     {
                                         /* check if the still have unresolved entity, it may referee to itself */
                                         if(pstrParserInstanceType->u8FoundReference > 0U)
                                         {
                                             if(strstr((INT8S const *)pu8TempAttributeValue, (INT8S const *)pu8EntityValue) != STD_NULL)
                                             {
                                                 pstrParserInstanceType->objenumErrorType = csEntityRefereToItself;
                                             }
                                             else
                                             {
                                                 pstrParserInstanceType->objenumErrorType = csLargeResolvingTrialsProperty;
                                             }
                                         }
                                         else
                                         {
                                             break;
                                         }
                                     }
                                     else
                                     {
                                         /*
                                          * Nothing to do
                                          */
                                     }
                                 }
                                 else
                                 {
                                     /*
                                      * Nothing to do
                                      */
                                 }
                             }
                         }
                         else /* there is no more references */
                         {
                             /*TODO: erro */
                         }
                         objenumCheckAttributeStateType = csCheckAttributeName;
                         bChecMandatoryWhiteSpace       = STD_TRUE;
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csQuotesMissmatch;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                                         "Quotes miss match");
                     }
                 }
                 else
                 {
                     pstrParserInstanceType->u32Length = LwXMLP_u8ValidCharacter(pu8AttributeList);
                     if(pstrParserInstanceType->u32Length == 0U)
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidXMLCharacter;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid XML character");
                     }
                     else/* valid Name character */
                     {
                         /*
                          * Nothing to do
                          */
                     }
                 }
                 break;
            default:
                /*
                 * Nothing to do
                 */
                 break;
        }
        pu8AttributeList = &pu8AttributeList[pstrParserInstanceType->u32Length];
    }
    if(pstrParserInstanceType->objenumErrorType == csNoError)
    {
        switch(objenumCheckAttributeStateType)
        {
            case csCheckElementName:
            case csCheckAttributeValueStartQuot:
            case csCheckDefaultDecleration:
                 pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                 LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
                 break;
            case csCheckAttributeName:
                 break;
            case csCheckAttributeType:
                 break;
            case csCheckAttributeValueEndQuot:
                break;
            case csCheckNotation:
                break;
            case csCheckDone:
            default:
                /*
                 * Nothing to do
                 */
                 break;
        }
    }
}
static void LwXMLP_vValidateNotation(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    typedef enum
    {
        csCheckNotationName,
        csCheckID,
        csCheckDone
    }enumCheckNotationStateType;
    INT8U                     *pu8Notation;
    INT8U      const          *pu8NotationEnd;
    enumCheckNotationStateType objenumCheckNotationStateType = csCheckNotationName;

    pu8Notation                                             = &pstrParserInstanceType->u8XMLworkingBuffer[10];
    pu8NotationEnd                                          = &pstrParserInstanceType->u8XMLworkingBuffer[pstrParserInstanceType->u32DirectiveLength - 1U];
    pu8Notation[pstrParserInstanceType->u32DirectiveLength] = NULL_ASCII_CHARACTER;
    /* will escape <!NOTATION  > from checking in the for loop */
    pstrParserInstanceType->u32Length  = 0U;
    for(;((pu8Notation != pu8NotationEnd) && (pstrParserInstanceType->objenumErrorType == csNoError)); )
    {
        switch(objenumCheckNotationStateType)
        {
            case csCheckDone:
                 if(LwXMLP_bIsWhiteSpace(pu8Notation) == STD_FALSE) /* check for unwanted characters*/
                 {
                     pstrParserInstanceType->objenumErrorType = csWrongDirectiveEnd;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Wrong directive end");
                 }
                 else
                 {
                     pstrParserInstanceType->u32Length = 1U; /* update character length */
                 }
                 break;
            case csCheckNotationName:
                 pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidateName(pu8Notation, &pstrParserInstanceType->pu8StartOfName, &pstrParserInstanceType->u32Length);
                 if(pstrParserInstanceType->objenumErrorType == csNoError)
                 {
                     objenumCheckNotationStateType = csCheckID;
                     pu8Notation[pstrParserInstanceType->u32Length - 1U] = SPACE_ASCII_CHARACTER;
                 }
                 else
                 {
                     /*
                      * Nothing to do
                      */
                 }
                 break;
            case csCheckID:
                 pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidateExternalID(pu8Notation, &pstrParserInstanceType->u32Length, STD_TRUE);
                 if(pstrParserInstanceType->objenumErrorType == csNoError)
                 {
                     objenumCheckNotationStateType = csCheckDone;
                 }
                 else
                 {
                     /*
                      * Nothing to do
                      */
                 }
                 break;
            default:
                /*
                 * Nothing to do
                 */
                 break;
        }
        pu8Notation          = &pu8Notation[pstrParserInstanceType->u32Length];
    }
    if(pstrParserInstanceType->objenumErrorType == csNoError)
    {
        switch(objenumCheckNotationStateType)
        {
            case csCheckNotationName:
            case csCheckID:
                 pstrParserInstanceType->objenumErrorType = csDTDMissingPortionError;
                 LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
                 break;
            case csCheckDone:
                 break;
            default:
                 /*
                  * Nothing to do
                  */
                 break;
        }
    }
    if(pstrParserInstanceType->objenumErrorType == csNoError)
    {
#if 0
        pstrParserInstanceType->pu8ParsedNotaion = &pstrParserInstanceType->u8XMLworkingBuffer[0];
        strcat((INT8S *)pstrParserInstanceType->pu8ParsedNotaion, "[ ");
        strncat((INT8S *)pstrParserInstanceType->pu8ParsedNotaion, (INT8S const *)pu8Notation, 10);
        strcat((INT8S *)pstrParserInstanceType->pu8ParsedNotaion, "]>");

        LwXMLP_vMoveStringBlockRight(pstrParserInstanceType->pu8ParsedNotaion, strlen(pstrParserInstanceType->pu8ParsedNotaion));
#endif
    }
}
static LwXMLP_enumErrorType LwXMLP_enumValidateEnumeration(INT8U *pu8Enumeration, INT32U *pu32EnumerationLength)
{
    typedef enum
    {
        csCheckOpenBrackets,
        csCheckNmtoken,
        csCheckDone
    }enumCheckEnumerationStateType;
    BOOLEAN              bEnumerationFound = STD_FALSE;
    INT8U                u32CharacterLength;
    INT8U               *pu8CurrentEnumeration = pu8Enumeration;
    LwXMLP_enumErrorType objenumErrorType = csNoError;
    enumCheckEnumerationStateType objenumCheckEnumerationStateType = csCheckOpenBrackets;

    *pu32EnumerationLength = 0;
    u32CharacterLength  = 0;
    do
    {
        if(*pu8CurrentEnumeration != GREATER_THAN_ASCII_CHARACTER)
        {
            switch(objenumCheckEnumerationStateType)
            {
                case csCheckOpenBrackets:
                     if(*pu8CurrentEnumeration == OPEN_ROUND_BRACKET_ASCII_CHARACTER)
                     {
                         u32CharacterLength              = 1U; /* update character length */
                         objenumCheckEnumerationStateType = csCheckNmtoken;
                     }
                     else if(LwXMLP_bIsWhiteSpace(pu8CurrentEnumeration) == STD_FALSE)
                     {
                         objenumCheckEnumerationStateType = csCheckDone; /* not external ID */
                     }
                     else
                     {
                         /*
                          * Nothing to do
                          */
                     }
                     break;
                case csCheckNmtoken:
                     if(LwXMLP_bIsWhiteSpace(pu8CurrentEnumeration) == STD_TRUE)
                     {
                         u32CharacterLength = 1U; /* update character length */
                     }
                     else if(*pu8CurrentEnumeration == OR_ASCII_CHARACTER)
                     {
                         u32CharacterLength = 1U; /* update character length */
                     }
                     else if(*pu8CurrentEnumeration == CLOSE_ROUND_BRACKET_ASCII_CHARACTER)
                     {
                         u32CharacterLength                 = 1U; /* update character length */
                         objenumCheckEnumerationStateType   = csCheckDone;
                     }
                     else
                     {
                         u32CharacterLength = LwXMLP_u8ValidNameCharacter(pu8CurrentEnumeration);
                         if(u32CharacterLength == 0U)
                         {
                             objenumErrorType = csInvalidXMLCharacter;
                             LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid XML character");
                         }
                         else /* Valid character */
                         {
                             bEnumerationFound = STD_TRUE;
                         }
                     }
                     break;
                case csCheckDone:
                     break;
                default:
                    /*
                     * Nothing to do
                     */
                     break;
            }
            pu8CurrentEnumeration          = &pu8CurrentEnumeration[u32CharacterLength];
            *pu32EnumerationLength += u32CharacterLength;
        }
        else
        {
            objenumErrorType = csDTDMissingPortionError; /* reach the end early */
        }
    }
    while((objenumCheckEnumerationStateType != csCheckDone) && (objenumErrorType == csNoError));
    if(bEnumerationFound == STD_FALSE)
    {
        objenumErrorType = csDTDMissingPortionError;
    }
    else
    {
        /*
         * Nothing to do
         */

    }

    return objenumErrorType;
}
static LwXMLP_enumErrorType LwXMLP_enumValidateExternalID(INT8U   *pu8Name,
                                                          INT32U  *pu32PublicIDLength,
                                                          BOOLEAN  bPublicLiteralDiffrentLength)
{
    typedef enum
    {
        csCheckType,
        csCheckPubidLiteralStartQuot,
        csCheckPubidLiteralEndQuot,
        csCheckSystemLiteralStartQuot,
        csCheckSystemLiteralEndQuot,
        csCheckLitrealWhiteSpace,
        csCheckDone
    }enumCheckExternalIDStateType;
    BOOLEAN              bChecMandatoryWhiteSpace = STD_FALSE;
    INT8U                u8LiteralEndCharacter = 0U;
    INT8U               *pu8CurrentName = pu8Name;
    INT32U               u32CharacterLength;
    LwXMLP_enumErrorType objenumErrorType = csNoError;
    enumCheckExternalIDStateType objenumCheckExternalIDStateType = csCheckType;

    *pu32PublicIDLength = 0;
    u32CharacterLength  = 0;
    do
    {
        switch(objenumCheckExternalIDStateType)
        {
            case csCheckType:
                 if(strncmp((INT8S const *)pu8CurrentName, "SYSTEM", strlen("SYSTEM")) == 0)
                 {
                     u32CharacterLength              = strlen("SYSTEM");
                     objenumCheckExternalIDStateType = csCheckSystemLiteralStartQuot;
                     bChecMandatoryWhiteSpace        = STD_TRUE;
                 }
                 else if(strncmp((INT8S const *)pu8CurrentName, "PUBLIC", strlen("PUBLIC")) == 0)
                 {
                     u32CharacterLength              = strlen("PUBLIC");
                     objenumCheckExternalIDStateType = csCheckPubidLiteralStartQuot;
                     bChecMandatoryWhiteSpace        = STD_TRUE;
                 }
                 else if(LwXMLP_bIsWhiteSpace(pu8CurrentName) == STD_FALSE)
                 {
                     objenumCheckExternalIDStateType = csCheckDone; /* not external ID */
                 }
                 else if(LwXMLP_bIsWhiteSpace(pu8CurrentName) == STD_TRUE)
                 {
                     u32CharacterLength = 1U; /* update character length */
                 }
                 else
                 {
                     /*
                      * Nothing to do
                      */
                 }
                 break;
            case csCheckPubidLiteralStartQuot:
                 if((LwXMLP_bIsWhiteSpace(pu8CurrentName) == STD_FALSE) && (bChecMandatoryWhiteSpace == STD_TRUE))
                 {
                      objenumErrorType = csMissingWhiteSpaceCharacter;
                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing WhiteSpace");
                 }
                 else if(LwXMLP_bIsWhiteSpace(pu8CurrentName) == STD_TRUE)
                 {
                      u32CharacterLength         = 1U; /* update character length */
                      bChecMandatoryWhiteSpace = STD_FALSE;
                 }
                 else if((*pu8CurrentName == DOUBLE_QUOT_ASCII_CHARACTER) || \
                    (*pu8CurrentName == SINGLE_QUOT_ASCII_CHARACTER))
                 {
                     u8LiteralEndCharacter    = *pu8CurrentName;
                     u32CharacterLength       = 1U; /* update character length */
                     objenumCheckExternalIDStateType = csCheckPubidLiteralEndQuot;
                 }
                 else
                 {
                     objenumErrorType = csMissingQuote;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing quote");
                 }
                 break;
            case csCheckPubidLiteralEndQuot:
                 if((*pu8CurrentName == DOUBLE_QUOT_ASCII_CHARACTER) || \
                   (*pu8CurrentName == SINGLE_QUOT_ASCII_CHARACTER))
                 {
                     u32CharacterLength = 1U; /* update character length */
                     if(*pu8CurrentName == u8LiteralEndCharacter)
                     {
                         objenumCheckExternalIDStateType = csCheckLitrealWhiteSpace;
                     }
                     else
                     {
                         if(*pu8CurrentName == DOUBLE_QUOT_ASCII_CHARACTER)
                         {
                             objenumErrorType = csQuotesMissmatch;
                             LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                                             "Quotes miss match");
                         }
                         else
                         {
                             u32CharacterLength = 1U; /* update character length */
                         }
                     }
                 }
                 else
                 {
                     u32CharacterLength = LwXMLP_u8ValidPublicIDCharacter(pu8CurrentName);
                     if(u32CharacterLength == 0U)
                     {
                         objenumErrorType = csInvalidXMLCharacter;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid XML character");
                     }
                     else/* valid Name character */
                     {
                         /*
                          * Nothing to do
                          */
                     }
                 }
                 break;
            case csCheckLitrealWhiteSpace:
                 if(LwXMLP_bIsWhiteSpace(pu8CurrentName) == STD_TRUE)
                 {
                     u32CharacterLength = 1U; /* update character length */
                     objenumCheckExternalIDStateType = csCheckSystemLiteralStartQuot;
                 }
                 else
                 {
                     objenumErrorType = csDTDMissingPortionError;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
                 }
                 break;
            case csCheckSystemLiteralStartQuot:
                 if((LwXMLP_bIsWhiteSpace(pu8CurrentName) == STD_FALSE) && (bChecMandatoryWhiteSpace == STD_TRUE))
                 {
                      objenumErrorType = csMissingWhiteSpaceCharacter;
                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing WhiteSpace");
                 }
                 else if(LwXMLP_bIsWhiteSpace(pu8CurrentName) == STD_TRUE)
                 {
                      u32CharacterLength         = 1U; /* update character length */
                      bChecMandatoryWhiteSpace = STD_FALSE;
                 }
                 else if((*pu8CurrentName == DOUBLE_QUOT_ASCII_CHARACTER) || \
                    (*pu8CurrentName == SINGLE_QUOT_ASCII_CHARACTER))
                 {
                     u8LiteralEndCharacter    = *pu8CurrentName;
                     u32CharacterLength       = 1U; /* update character length */
                     objenumCheckExternalIDStateType = csCheckSystemLiteralEndQuot;
                 }
                 else
                 {
                     objenumErrorType = csMissingQuote;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing quote");
                 }
                 break;
            case csCheckSystemLiteralEndQuot:
                 if((*pu8CurrentName == DOUBLE_QUOT_ASCII_CHARACTER) || \
                    (*pu8CurrentName == SINGLE_QUOT_ASCII_CHARACTER))
                 {
                     u32CharacterLength = 1U; /* update character length */
                     if(*pu8CurrentName == u8LiteralEndCharacter)
                     {
                         objenumCheckExternalIDStateType = csCheckDone;
                     }
                     else
                     {
                         objenumErrorType = csQuotesMissmatch;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                                         "Quotes miss match");
                     }
                 }
                 else
                 {
                     u32CharacterLength = LwXMLP_u8ValidCharacter(pu8CurrentName);
                     if(u32CharacterLength == 0U)
                     {
                         objenumErrorType = csInvalidXMLCharacter;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid XML character");
                     }
                     else if(*pu8CurrentName == HASH_ASCII_CHARACTER)
                     {
                         objenumErrorType = csInvalidXMLCharacter;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid XML character");
                       }
                     else/* valid Name character */
                     {
                         /*
                          * Nothing to do
                          */
                     }
                 }
                 break;
            case csCheckDone:
            default:
                /*
                 * Nothing to do
                 */
                 break;
        }
        pu8CurrentName              = &pu8CurrentName[u32CharacterLength];
        *pu32PublicIDLength += u32CharacterLength;
        u32CharacterLength   = 0;
    }
    while((objenumCheckExternalIDStateType != csCheckDone) && \
          (*pu8CurrentName != GREATER_THAN_ASCII_CHARACTER) &&\
          (objenumErrorType == csNoError));

    if(objenumErrorType == csNoError)
    {
        switch(objenumCheckExternalIDStateType)
        {
            case csCheckType:
                 break;
            case csCheckPubidLiteralStartQuot:
                 objenumErrorType = csDTDMissingPortionError;
                 LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
                 break;
            case csCheckPubidLiteralEndQuot:
                 break;
            case csCheckLitrealWhiteSpace:
            case csCheckSystemLiteralStartQuot:
                 if(bPublicLiteralDiffrentLength == STD_FALSE)
                 {
                     objenumErrorType = csDTDMissingPortionError;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "DTD is missing portion");
                 }
                 break;
            case csCheckSystemLiteralEndQuot:
                 break;
            case csCheckDone:
                 break;
            default:
                /*
                 * Nothing to do
                 */
                break;
        }
    }

    return objenumErrorType;
}
static LwXMLP_enumErrorType LwXMLP_enumValidateName(INT8U *pu8Name, INT8U **pu8StartOfName, INT32U *pu32NameLength)
{
    typedef enum
    {
        csCheckNameStart,
        csCheckName,
        csCheckDone
    }enumCheckNameStateType;
    INT32U                 u32CharacterLength = 0U;
    INT8U                  *pu8CurrentName = pu8Name;
    LwXMLP_enumErrorType   objenumErrorType = csNoError;
    enumCheckNameStateType objenumCheckNameStateType = csCheckNameStart;

    *pu32NameLength = 0;
    do
    {
        switch(objenumCheckNameStateType)
        {
            case csCheckNameStart:
                 if(LwXMLP_bIsWhiteSpace(pu8CurrentName) == STD_TRUE)
                 {
                     u32CharacterLength = 1U;
                 }
                 else
                 {
                     u32CharacterLength = LwXMLP_u8ValidNameStartCharacter(pu8CurrentName);
                     if(u32CharacterLength > 0U)
                     {
                         objenumCheckNameStateType = csCheckName;
                         *pu8StartOfName = pu8CurrentName;
                     }
                     else
                     {
                           objenumErrorType = csInvalidStartNameCharacter;
                          LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Start Name character");
                     }
                 }
                 break;
            case csCheckName:
                 if((LwXMLP_bIsWhiteSpace(pu8CurrentName) == STD_TRUE) || (*pu8CurrentName == GREATER_THAN_ASCII_CHARACTER))
                 {
                     if(*pu8CurrentName != GREATER_THAN_ASCII_CHARACTER) /* it will be left to the upper caller to decide */
                     {
                         *pu8CurrentName           = NULL_ASCII_CHARACTER;
                         u32CharacterLength = 1U;
                     }
                     else
                     {
                         u32CharacterLength = 0U;
                     }
                     objenumCheckNameStateType = csCheckDone;
                 }
                 else
                 {
                     u32CharacterLength = LwXMLP_u8ValidNameCharacter(pu8CurrentName);
                     if(u32CharacterLength == 0U)
                     {
                         objenumErrorType = csInvalidNameCharacter;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Start Name character");
                     }
                     else /* valid Name character */
                      {
                         /*
                          * Nothing to do
                          */
                     }
                 }
                 break;
            case csCheckDone:
                 break;
            default:
                /*
                 * Nothing to do
                 */
                 break;
        }
        pu8CurrentName = &pu8CurrentName[u32CharacterLength];
        *pu32NameLength += u32CharacterLength;
    }
    while((objenumCheckNameStateType != csCheckDone) && (objenumErrorType == csNoError));

    return objenumErrorType;
}
/**********************************************************************************************************************/
/*!\fn         INT8U LwXMLP_u8ValidPublicIDCharacter(INT8U const *pu8Character)

@brief         Validate the character if it is Char according to W3C definition which should be
               #x20 | #xD | #xA | [a-zA-Z0-9] | [-'()+,./:=?;!*#@$_%]

@param[in]       pu8Character : Pointer to the location of the character

@return         ??????????????

@note
***********************************************************************************************************************/
static INT8U LwXMLP_u8ValidPublicIDCharacter(INT8U const *pu8Character)
{
    INT8U   u8CharacterLength;

    switch(pu8Character[0])
    {
        case SPACE_ASCII_CHARACTER:
        case CARRIAGE_RETURN_ASCII_CHARACTER:
        case LINE_FEED_ASCII_CHARACTER:
        case HYPHEN_ASCII_CHARACTER:
        case SINGLE_QUOT_ASCII_CHARACTER:
        case OPEN_ROUND_BRACKET_ASCII_CHARACTER:
        case CLOSE_ROUND_BRACKET_ASCII_CHARACTER:
        case PLUS_ASCII_CHARACTER:
        case COMMA_ASCII_CHARACTER:
        case DOT_ASCII_CHARACTER:
        case SLASH_ASCII_CHARACTER:
        case COLON_ASCII_CHARACTER:
        case EQUAL_ASCII_CHARACTER:
        case QUESTION_MARK_ASCII_CHARACTER:
        case SEMICOLON_ASCII_CHARACTER:
        case EXCLAM_MARK_ASCII_CHARACTER:
        case ASTERISK_ASCII_CHARACTER:
        case HASH_ASCII_CHARACTER:
        case AT_SYMBOLE_ASCII_CHARACTER:
        case DOLLAR_SIGN_ASCII_CHARACTER:
        case UNDERSCORE_ASCII_CHARACTER:
        case PERCENT_ASCII_CHARACTER:
             u8CharacterLength = 1U;
             break;
        default:
            if(isalpha((INT8U)(*pu8Character)) != 0)
            {
                u8CharacterLength = 1U;
            }
            else if(isdigit((INT32S)(*pu8Character)) != 0)
            {
                u8CharacterLength = 1U;
            }
            else
            {
                u8CharacterLength = 0U;
            }
            break;
    }

    return u8CharacterLength;
}
#endif
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_vCheckWellFormedDecl                                        <LwXMLP_CORE>

SYNTAX:         StdReturnType LwXMLP_vCheckWellFormedDecl(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)

DESCRIPTION :   this API will parse/validate the declaration

PARAMETER1  :   pstrParserInstanceType : pointer to the XML instance

RETURN VALUE:   E_NOT_OK: if parsing the Declaration is failed
                E_OK    : if parsing the comment is succeeded

Note        :   The API will fail to load the buffer in the following cases:
                - double -- are found inside the comment

Note        :   This API assume that the directive is exist in the working buffer <!--  -->

END DESCRIPTION *******************************************************************************************************/
static void LwXMLP_vCheckWellFormedDecl(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    typedef enum
    {
        csUnkownDeclaration,
        csVersionDeclaration,
        csStandAloneDeclaration,
        csEncodingDeclaration
    }enumDeclarationTypeType;
    typedef enum
    {
        csAttributeName,\
        csCheckEquality,\
        csCheckAttributeValueStartQuot,\
        csCheckAttributeValueEndQuot
    }enumCheckTagStateType;
    BOOLEAN                  bVersionFound = STD_FALSE, bEncodingFound = STD_FALSE, bSDDelFound = STD_FALSE, bChecMandatoryWhiteSpace = STD_FALSE;
    INT8U                   *pu8WorkingBuffer, u8AttributeEndCharacter = SPACE_ASCII_CHARACTER, u8AttributeLength = 0;
    INT8U    const          *pu8TempAttributeValue;
    INT32U                   u32CheckingDataAmount, u32BufferIndex, u32CharacterLength = 0U;
    enumCheckTagStateType    objenumCheckCommentStateType = csAttributeName;
    enumDeclarationTypeType  objenumDeclarationTypeType = csUnkownDeclaration;

    pu8WorkingBuffer      = &pstrParserInstanceType->u8XMLworkingBuffer[6];
    u32CheckingDataAmount = pstrParserInstanceType->u32DirectiveLength;
    u32CheckingDataAmount -= 8U;/* remove the <?xml ?> length */
    pu8TempAttributeValue  = pu8WorkingBuffer; /* Intial value */
    /* escape <?xml   */
    for(u32BufferIndex = 0U; \
       (u32BufferIndex < u32CheckingDataAmount) && (pstrParserInstanceType->objenumErrorType == csNoError);\
       u32BufferIndex += u32CharacterLength)
    {
        switch(objenumCheckCommentStateType)
        {
            case csAttributeName:
                 if((LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[u32BufferIndex]) == STD_FALSE) && (bChecMandatoryWhiteSpace == STD_TRUE))
                 {
                      pstrParserInstanceType->objenumErrorType = csMissingWhiteSpaceCharacter;
                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing WhiteSpace");
                 }
                 else if(LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[u32BufferIndex]) == STD_TRUE)
                 {
                      u32CharacterLength       = 1U; /* update character length */
                      bChecMandatoryWhiteSpace = STD_FALSE;
                 }
                 else if(strncmp((INT8S const *)&pu8WorkingBuffer[u32BufferIndex], "version", strlen("version")) == 0)
                 {
                     if(bVersionFound == STD_FALSE)
                     {
                         bVersionFound                = STD_TRUE;
                         u32CharacterLength           = strlen("version");
                         objenumCheckCommentStateType = csCheckEquality;
                         objenumDeclarationTypeType   = csVersionDeclaration;
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csRepeatedAttributeName;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                                         "Repeated attribute Name");
                     }
                 }
                 else if(strncmp((INT8S const *)&pu8WorkingBuffer[u32BufferIndex], "standalone", strlen("standalone")) == 0)
                 {
                     if(bVersionFound == STD_TRUE)
                     {
                         if(bSDDelFound == STD_FALSE)
                         {
                             bSDDelFound                  = STD_TRUE;
                             u32CharacterLength           = strlen("standalone");
                             objenumCheckCommentStateType = csCheckEquality;
                             objenumDeclarationTypeType   = csStandAloneDeclaration;
                         }
                         else
                         {
                             pstrParserInstanceType->objenumErrorType = csRepeatedAttributeName;
                             LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                                             "Repeated attribute Name");
                         }
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidVersionOrder;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                                         "Invalid version attribute order");
                     }
                 }
                 else if(strncmp((INT8S const *)&pu8WorkingBuffer[u32BufferIndex], "encoding", strlen("encoding")) == 0)
                 {
                     if(bVersionFound == STD_TRUE)
                     {
                         if(bSDDelFound == STD_FALSE)
                         {
                             if(bEncodingFound == STD_FALSE)
                             {
                                 bEncodingFound               = STD_TRUE;
                                 u32CharacterLength           = strlen("encoding");
                                 objenumCheckCommentStateType = csCheckEquality;
                                 objenumDeclarationTypeType   = csEncodingDeclaration;
                             }
                             else
                             {
                                 pstrParserInstanceType->objenumErrorType = csRepeatedAttributeName;
                                 LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                                                 "Repeated attribute Name");
                             }
                         }
                         else
                         {
                             pstrParserInstanceType->objenumErrorType = csInvalidSDeclOrder;
                             LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid SDecl Order");
                         }
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csInvalidVersionOrder;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                                         "Invalid version attribute order");

                     }
                 }
                 else
                 {
                      pstrParserInstanceType->objenumErrorType = csInvalidStartNameCharacter;
                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Start Name character");
                 }
                 break;
            case csCheckEquality:
                 if(pu8WorkingBuffer[u32BufferIndex] == EQUAL_ASCII_CHARACTER)
                 {
                     u32CharacterLength           = 1U; /* update character length */
                     objenumCheckCommentStateType = csCheckAttributeValueStartQuot;
                 }
                 else if(LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[u32BufferIndex]) == STD_TRUE)
                 {
                     u32CharacterLength = 1U; /* update character length */
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = csMissingEqual;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                                       "The = is missing between the attribute name and the attribute value");
                 }
                 break;
            case csCheckAttributeValueStartQuot:
                 if((pu8WorkingBuffer[u32BufferIndex] == DOUBLE_QUOT_ASCII_CHARACTER) || \
                    (pu8WorkingBuffer[u32BufferIndex] == SINGLE_QUOT_ASCII_CHARACTER))
                 {
                     u32CharacterLength           = 1U; /* update character length */
                     u8AttributeEndCharacter      = pu8WorkingBuffer[u32BufferIndex];
                     pu8TempAttributeValue        = &pu8WorkingBuffer[u32BufferIndex + 1U];
                     objenumCheckCommentStateType = csCheckAttributeValueEndQuot;
                 }
                 else if(LwXMLP_bIsWhiteSpace(&pu8WorkingBuffer[u32BufferIndex]) == STD_TRUE)
                 {
                     u32CharacterLength = 1U; /* update character length */
                 }
                 else
                 {
                     pstrParserInstanceType->objenumErrorType = csMissingQuote;
                     LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Missing quote");
                 }
                 break;
            case csCheckAttributeValueEndQuot:
                 if((pu8WorkingBuffer[u32BufferIndex] == DOUBLE_QUOT_ASCII_CHARACTER) || \
                    (pu8WorkingBuffer[u32BufferIndex] == SINGLE_QUOT_ASCII_CHARACTER))
                 {
                     if(pu8WorkingBuffer[u32BufferIndex] == u8AttributeEndCharacter)
                     {
                         pu8WorkingBuffer[u32BufferIndex] = NULL_ASCII_CHARACTER;
                         if((strncmp((INT8S const *)pu8TempAttributeValue, "UTF-16BE", u8AttributeLength) == 0) &&\
                             (objenumDeclarationTypeType == csEncodingDeclaration))
                         {
                             u32CharacterLength                              = 1U; /* update character length */
                             bChecMandatoryWhiteSpace                        = STD_TRUE;
                             objenumCheckCommentStateType                    = csAttributeName;
                             if((pstrParserInstanceType->objenumEncodingTypeType != csEncoding_UTF16BE) &&
                                (pstrParserInstanceType->objenumEncodingTypeType != csEncoding_NONE))
                             {
                                 pstrParserInstanceType->objenumErrorType = csEncodingDiscrepancy;
                             }
                             else
                             {
                                 pstrParserInstanceType->objenumEncodingTypeType = csEncoding_UTF16BE;
                             }
                         }
                         if((strncmp((INT8S const *)pu8TempAttributeValue, "UTF-16LE", u8AttributeLength) == 0) &&\
                             (objenumDeclarationTypeType == csEncodingDeclaration))
                         {
                             u32CharacterLength                              = 1U; /* update character length */
                             bChecMandatoryWhiteSpace                        = STD_TRUE;
                             objenumCheckCommentStateType                    = csAttributeName;
                             if((pstrParserInstanceType->objenumEncodingTypeType != csEncoding_UTF16LE) &&
                                (pstrParserInstanceType->objenumEncodingTypeType != csEncoding_NONE))
                             {
                                 pstrParserInstanceType->objenumErrorType = csEncodingDiscrepancy;
                             }
                             else
                             {
                                 pstrParserInstanceType->objenumEncodingTypeType = csEncoding_UTF16LE;
                             }
                         }
                         if((strncmp((INT8S const *)pu8TempAttributeValue, "UTF-16", u8AttributeLength) == 0) &&\
                             (objenumDeclarationTypeType == csEncodingDeclaration))
                         {
                             u32CharacterLength                              = 1U; /* update character length */
                             bChecMandatoryWhiteSpace                        = STD_TRUE;
                             objenumCheckCommentStateType                    = csAttributeName;
                             if((pstrParserInstanceType->objenumEncodingTypeType != csEncoding_UTF16LE) &&
                                (pstrParserInstanceType->objenumEncodingTypeType != csEncoding_NONE))
                             {
                                 pstrParserInstanceType->objenumErrorType = csEncodingDiscrepancy;
                             }
                             else
                             {
                                 pstrParserInstanceType->objenumEncodingTypeType = csEncoding_UTF16LE;
                             }

                         }
                         else if((strncmp((INT8S const *)pu8TempAttributeValue, "UTF-8", u8AttributeLength) == 0) &&\
                                 (objenumDeclarationTypeType == csEncodingDeclaration))
                         {
                             u32CharacterLength                              = 1U; /* update character length */
                             bChecMandatoryWhiteSpace                        = STD_TRUE;
                             objenumCheckCommentStateType                    = csAttributeName;
                             if((pstrParserInstanceType->objenumEncodingTypeType != csEncoding_UTF8) &&
                                (pstrParserInstanceType->objenumEncodingTypeType != csEncoding_NONE))
                             {
                                 pstrParserInstanceType->objenumErrorType = csEncodingDiscrepancy;
                             }
                             else
                             {
                                 pstrParserInstanceType->objenumEncodingTypeType = csEncoding_UTF8;
                             }
                         }
                         else if((strncmp((INT8S const *)pu8TempAttributeValue, "ISO-8859-1", u8AttributeLength) == 0) &&\
                                 (objenumDeclarationTypeType == csEncodingDeclaration))
                         {
                             u32CharacterLength                              = 1U; /* update character length */
                             bChecMandatoryWhiteSpace                        = STD_TRUE;
                             objenumCheckCommentStateType                    = csAttributeName;
                             if((pstrParserInstanceType->objenumEncodingTypeType != csEncoding_8859_1) &&
                                (pstrParserInstanceType->objenumEncodingTypeType != csEncoding_NONE))
                             {
                                 pstrParserInstanceType->objenumErrorType = csEncodingDiscrepancy;
                             }
                             else
                             {
                                 pstrParserInstanceType->objenumEncodingTypeType = csEncoding_8859_1;
                             }

                         }
                         else if((strncmp((INT8S const *)pu8TempAttributeValue, "1.0", u8AttributeLength) == 0) &&\
                                 (objenumDeclarationTypeType == csVersionDeclaration))
                         {
                             u32CharacterLength           = 1U; /* update character length */
                             bChecMandatoryWhiteSpace     = STD_TRUE;
                             objenumCheckCommentStateType = csAttributeName;
                         }
                         else if(((strncmp((INT8S const *)pu8TempAttributeValue, "yes", u8AttributeLength) == 0)) &&\
                                 (objenumDeclarationTypeType == csStandAloneDeclaration))
                         {
                             u32CharacterLength                          = 1U; /* update character length */
                             bChecMandatoryWhiteSpace                    = STD_TRUE;
                             objenumCheckCommentStateType                = csAttributeName;
                             pstrParserInstanceType->bStandAloneDocument = STD_TRUE;
                         }
                         else if((strncmp((INT8S const *)pu8TempAttributeValue, "no", u8AttributeLength) == 0) &&\
                                 (objenumDeclarationTypeType == csStandAloneDeclaration))
                         {
                             u32CharacterLength                          = 1U; /* update character length */
                             bChecMandatoryWhiteSpace                    = STD_TRUE;
                             objenumCheckCommentStateType                = csAttributeName;
                             pstrParserInstanceType->bStandAloneDocument = STD_FALSE;
                         }
                         else
                         {
                             switch(objenumDeclarationTypeType)
                             {
                                 case csStandAloneDeclaration:
                                      pstrParserInstanceType->objenumErrorType = csInvalidAttributeValue;
                                      break;
                                 case csEncodingDeclaration:
                                      pstrParserInstanceType->objenumErrorType = csUnsupportedEncodingScheme;
                                      break;
                                 case csVersionDeclaration:
                                      pstrParserInstanceType->objenumErrorType = csInvalidAttributeValue;
                                      break;
                                 case csUnkownDeclaration:
                                 default:
                                      pstrParserInstanceType->objenumErrorType = csInvalidAttributeValue;
                                      LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid Attribute Value");
                                      break;
                             }
                         }
                     }
                     else
                     {
                         pstrParserInstanceType->objenumErrorType = csQuotesMissmatch;
                         LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                                         "Quotes miss match");
                     }
                 }
                 else/* valid Name character */
                 {
                     u8AttributeLength++;
                     /* convert to upper for comparison */
                     if(objenumDeclarationTypeType == csEncodingDeclaration)
                     {
                         pu8WorkingBuffer[u32BufferIndex] = (INT8U)toupper((INT32S)pu8WorkingBuffer[u32BufferIndex]);
                     }
                 }
                 break;
            default:
                /*
                 * Nothing to do
                 */
                 break;
        }
    }
    /* validation part, such that the state machine shall terminate on specific states, otherwise, it shall be error */
    if(pstrParserInstanceType->objenumErrorType  == csNoError)
    {
        switch(objenumCheckCommentStateType)
        {
            case csCheckEquality:
                 pstrParserInstanceType->objenumErrorType = csMissingAttributeValue;
                 LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "an attribute value is missing");
                 break;
            case csCheckAttributeValueStartQuot:
            case csCheckAttributeValueEndQuot:
                 pstrParserInstanceType->objenumErrorType = csMissingQuote;
                 LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "an attribute value is missing its quote");
                 break;
                 /* valid exit states */
            case csAttributeName:
            default:
                /*
                 * Nothing to do
                 */
                 break;
        }
    }
}
#if(ENABLE_COMMENT_WELL_FORM_CHECK == STD_ON)
/* START FUNCTION DESCRIPTION ******************************************************************************************
LwXMLP_srParseComment                                        <LwXMLP_CORE>

SYNTAX:         StdReturnType LwXMLP_srParseComment(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)

DESCRIPTION :   this API will validate the comment not to contain -- or invalid XML character beside it will point to
                start of the comment location and terminate the comment by NULL

PARAMETER1  :   pstrParserInstanceType : pointer to the XML instance


RETURN VALUE:   E_NOT_OK: if parsing the comment is failed
                E_OK    : if parsing the comment is succeeded

Note        :   The API will fail to load the buffer in the following cases:
                - double -- are found inside the comment

Note        :   This API assume that the directive is comment is exist in the working buffer <!--  -->

END DESCRIPTION *******************************************************************************************************/
static void LwXMLP_vCheckWellFormedComment(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    INT8U  *pu8WorkingBuffer;
    INT32U u32CommentCharacterIndex, u32CharacterLength;
    BOOLEAN bFoundHyphenCharacter = STD_FALSE;

    pu8WorkingBuffer                                             = pstrParserInstanceType->u8XMLworkingBuffer;
    pu8WorkingBuffer[pstrParserInstanceType->u32DirectiveLength] = NULL_ASCII_CHARACTER;
    /* will escape <!--  --> from checking in the for loop */
    for(u32CommentCharacterIndex = 4U; \
        ((u32CommentCharacterIndex < (pstrParserInstanceType->u32DirectiveLength - 2U)) &&\
        (pstrParserInstanceType->objenumErrorType == csNoError));\
        u32CommentCharacterIndex += u32CharacterLength)
    {
        if(pu8WorkingBuffer[u32CommentCharacterIndex] == HYPHEN_ASCII_CHARACTER)
        {
            if(bFoundHyphenCharacter == STD_TRUE)/* if the previous character was hyphen */
            {
                pstrParserInstanceType->objenumErrorType = csDoubleHyphenInComment;

            }
            else
            {
                bFoundHyphenCharacter = STD_TRUE;
            }
        }
        else
        {
            bFoundHyphenCharacter = STD_FALSE;
        }
        u32CharacterLength = LwXMLP_u8ValidCharacter(&pu8WorkingBuffer[u32CommentCharacterIndex]);
        if(u32CharacterLength == 0U)
        {
            pstrParserInstanceType->objenumErrorType = csInvalidXMLCharacter;
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Invalid XML character");
        }
        else
        {
            /*
             * Nothing to do
             */
        }
    }
}
#endif
static BOOLEAN LwXMLP_bNewAttribute(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType, \
                                           INT8U                          const *pu8NewAttributeString)
{
    BOOLEAN bNewAttribute = STD_TRUE;
    INT16S  s16AttributeIndex;

    for(s16AttributeIndex = 0; s16AttributeIndex < pstrParserInstanceType->s16AttributeCounts; s16AttributeIndex++)
    {
        if(strcmp((INT8S const *)pstrParserInstanceType->pu8AttributeNameArray[s16AttributeIndex], \
                  (INT8S const *)pu8NewAttributeString) == 0)
        {
            bNewAttribute = STD_FALSE;
            break;
        }
        else
        {
            /*
             * Nothing to do
             */
        }
    }
    return bNewAttribute;
}
static void LwXMLP_vUpdatePathWithStartTag(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    INT32U u32CurrentTagLength;

    LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, \
                    "Updating Path with TAG: %s", \
                    pstrParserInstanceType->pu8ElementName);
    u32CurrentTagLength  = strlen((INT8S const*)pstrParserInstanceType->pu8ElementName);
    if((pstrParserInstanceType->u32PathLength + u32CurrentTagLength) < MAX_XML_PATH_LENGTH)
    {
        if(pstrParserInstanceType->u32PathLength != 0U)
        {
            LwXMLP_vCopy((INT8U *)&pstrParserInstanceType->u8XMLPathBuffer[pstrParserInstanceType->u32PathLength], \
                         (INT8U const*)"\\", \
                         sizeof("\\"));
            pstrParserInstanceType->u32PathLength++; /* Increase the path length due to \ */
        }
        else
        {
            /*
             * Nothing to do
             */
        }
        LwXMLP_vCopy((INT8U *)&pstrParserInstanceType->u8XMLPathBuffer[pstrParserInstanceType->u32PathLength], \
                     (INT8U const *)pstrParserInstanceType->pu8ElementName,\
                     u32CurrentTagLength);
        pstrParserInstanceType->u32PathLength += u32CurrentTagLength;
        pstrParserInstanceType->u8XMLPathBuffer[pstrParserInstanceType->u32PathLength] = 0U;
        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "New Path is: %s", pstrParserInstanceType->u8XMLPathBuffer);
    }
    else
    {
        pstrParserInstanceType->objenumErrorType = csFailedToAddElementToPath;
        LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Failed to add new TAG to the path");
    }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    if(pstrParserInstanceType->bSecondLevelBufferEnabled == STD_TRUE)
    {
        if((pstrParserInstanceType->u32SecondLevelPathLength + u32CurrentTagLength) < MAX_XML_PATH_LENGTH)
        {
            if(pstrParserInstanceType->u32SecondLevelPathLength != 0U)
            {
                LwXMLP_vCopy((INT8U *)&pstrParserInstanceType->u8SecondLevelPathBuffer[pstrParserInstanceType->u32SecondLevelPathLength], \
                             (INT8U const*)"\\", \
                             sizeof("\\"));
                pstrParserInstanceType->u32SecondLevelPathLength++; /* increase the path length due to \ */
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
            LwXMLP_vCopy((INT8U *)&pstrParserInstanceType->u8SecondLevelPathBuffer[pstrParserInstanceType->u32SecondLevelPathLength], \
                         (INT8U const *)pstrParserInstanceType->pu8ElementName,\
                         u32CurrentTagLength);
            pstrParserInstanceType->u32SecondLevelPathLength += u32CurrentTagLength;
            pstrParserInstanceType->u8SecondLevelPathBuffer[pstrParserInstanceType->u32SecondLevelPathLength] = 0U;
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "New Path is: %s", pstrParserInstanceType->u8SecondLevelPathBuffer);
        }
        else
        {
            pstrParserInstanceType->objenumErrorType = csFailedToAddElementToPath;
            LwXMLP_mVerbose(VERBOSE_MESSAGE_ON|VERBOSE_LEVEL_DEBUGGING, "Failed to add new TAG to the path");
        }
    }
#endif
}
/*
 * This method has two assumptions
 * the pu8Buffer contain a UTF-8 Data
 * it is a string with NULL terminator
 * this is a complete XML structure so it should have the complete end of line presentation like \r\n
 */
/* Normalize all line breaks before parsing, by translating both the two-character sequence #xD #xA and
     * any #xD that is not followed by #xA to a single #xA character.
*/
/*
 * the method will normalize the content of the working buffer
 */
#if(ENABLE_EOL_NORMALIZATION == STD_ON)
static void LwXMLP_vNormalizeEndOfLine(INT8U *pu8StringBuffer)
{
    INT32U  u32WorkingBufferIndex, u32MoveWorkingBufferIndex;

    for(u32WorkingBufferIndex = 0U; \
        (pu8StringBuffer[u32WorkingBufferIndex] != NULL_ASCII_CHARACTER);\
        ++u32WorkingBufferIndex)
    {
        if(pu8StringBuffer[u32WorkingBufferIndex] == CARRIAGE_RETURN_ASCII_CHARACTER) /* 0x0D  \r*/
        {   /* Is the next character is LINE_FEED_ASCII_CHARACTER? */
            if(pu8StringBuffer[u32WorkingBufferIndex + 1U] == LINE_FEED_ASCII_CHARACTER)
            {
                /* Now move all the buffer */
                for(u32MoveWorkingBufferIndex = u32WorkingBufferIndex; \
                (pu8StringBuffer[u32MoveWorkingBufferIndex] != NULL_ASCII_CHARACTER); \
                    ++u32MoveWorkingBufferIndex)
                {
                    pu8StringBuffer[u32MoveWorkingBufferIndex] = pu8StringBuffer[u32MoveWorkingBufferIndex + 1U];
                }
            }
            else
            {
                //pu8StringBuffer[u32WorkingBufferIndex] = LINE_FEED_ASCII_CHARACTER;
            }
        }
        else
        {
           /* leave any other character including LINE_FEED_ASCII_CHARACTER */
        }
    }
}
#endif
#if(ENABLE_ATTRIBUTE_Normalization == STD_ON)
static void LwXMLP_vNormalizeAttributes(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    INT8U const *pu8EntityValue;
    INT8U  u8LastReferencesCountFound = 0U, u8ReferenceIndex, u8EntityIndex, u8EntityReferenceUpdateTrials;
    INT8U *pu8AttributeName;
    INT16S  s16AdjustedAttributeIndex;
    INT32U  u32ReferenceUpdateMask, u32AttributeLength, u32AdjustedLength, u32CurrentTagLength;
#endif
    INT8U   u8LastCharacter = 0U;
    INT8U  *pu8AttributeValue;
    INT16S  s16Attributeindex;
    INT8U   u8AtributeListIndex;
    enumAttibuteTypeType objenumAttibuteTypeType = csCDATAType;
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    u32CurrentTagLength = pstrParserInstanceType->u32CurrentTagLength;
#endif
    for(s16Attributeindex = 0;
        (s16Attributeindex < pstrParserInstanceType->s16AttributeCounts) &&
        (pstrParserInstanceType->objenumErrorType == csNoError);\
         ++s16Attributeindex)
    {
        /* round one,Normalize end of line first */
#if(ENABLE_EOL_NORMALIZATION == STD_ON)
        LwXMLP_vNormalizeEndOfLine(pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex]);
#endif
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
        for(u8EntityReferenceUpdateTrials = 0U; \
            (u8EntityReferenceUpdateTrials < MAX_ENTITY_RESOLVING_DEPTH) && \
            (pstrParserInstanceType->objenumErrorType == csNoError); ++u8EntityReferenceUpdateTrials)
        {
            if(u8EntityReferenceUpdateTrials == 0U)
            {
                u32ReferenceUpdateMask = UPDATE_ENTITY_REFERENCE | DONT_DEREF_AS_GENERAL_ENTITY;
            }
            else
            {
                u32ReferenceUpdateMask = UPDATE_ENTITY_REFERENCE | UPDATE_CHARACTER_REFERENCE | DONT_DEREF_AS_GENERAL_ENTITY;
            }

            u32AttributeLength = strlen((INT8S const *)pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex]);
            pstrParserInstanceType->objenumErrorType = LwXMLP_enumUpdateEntityReference(pstrParserInstanceType, \
                                                                                        pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex],\
                                                                                        u32CurrentTagLength,\
                                                                                        u32ReferenceUpdateMask);
            /* check entity value for direct < reference */
            if(pstrParserInstanceType->u8FoundReference > 0U)
            {
                for(u8ReferenceIndex = 0; \
                    (u8ReferenceIndex < pstrParserInstanceType->u8FoundReference) && \
                    (pstrParserInstanceType->objenumErrorType == csNoError); \
                    ++u8ReferenceIndex)
                {
                    if(pstrParserInstanceType->objstrFoundReferenceType[u8ReferenceIndex].objenumReferenceTypeType == csEntityReference)
                    {
                        u8EntityIndex  = pstrParserInstanceType->objstrFoundReferenceType[u8ReferenceIndex].u8EntityIndex;
                        pstrParserInstanceType->objenumErrorType = LwXMLP_enumValidateAttributeValue(pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityValue);
                    }
                    else
                    {
                        /*
                         * Nothing to do
                         */
                    }
                }
                if(u32AttributeLength < strlen((INT8S const *)pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex]))
                {/* Check if the attribute value is increased, so we need to adjust the rest of attributes pointers name and value */
                    u32AdjustedLength = strlen((INT8S const *)pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex]) - u32AttributeLength;
                    for(s16AdjustedAttributeIndex = (s16Attributeindex + 1);
                        (s16AdjustedAttributeIndex < pstrParserInstanceType->s16AttributeCounts);\
                         ++s16AdjustedAttributeIndex)
                    {
                        pu8AttributeValue = pstrParserInstanceType->pu8AttributeValueArray[s16AdjustedAttributeIndex];
                        pstrParserInstanceType->pu8AttributeValueArray[s16AdjustedAttributeIndex] = \
                                &pu8AttributeValue[u32AdjustedLength];
                        pu8AttributeName = pstrParserInstanceType->pu8AttributeNameArray[s16AdjustedAttributeIndex];
                        pstrParserInstanceType->pu8AttributeNameArray[s16AdjustedAttributeIndex] = \
                                &pu8AttributeName[u32AdjustedLength];
                    }
                    u32CurrentTagLength += u32AdjustedLength;
                }
                else
                {
                    pu8AttributeValue = pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex];
                    u32AdjustedLength = u32AttributeLength - strlen((INT8S const *)pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex]);
                    LwXMLP_vMoveBlockRight(&pu8AttributeValue[strlen((INT8S const *)pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex])],\
                                            u32AdjustedLength,\
                                            u32CurrentTagLength);
                }
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
            if(pstrParserInstanceType->u8FoundReference > 0U)
            {
                u8LastReferencesCountFound = pstrParserInstanceType->u8FoundReference;
            }
            else
            {
                break;
            }
        }
        if((u8LastReferencesCountFound > 0U) && (pstrParserInstanceType->objenumErrorType == csNoError))
        {
            for(u8ReferenceIndex = 0U; \
                (u8ReferenceIndex < u8LastReferencesCountFound) && \
                (pstrParserInstanceType->objenumErrorType == csNoError); \
                ++u8ReferenceIndex)
            {
                if(pstrParserInstanceType->objstrFoundReferenceType[u8ReferenceIndex].objenumReferenceTypeType == csEntityReference)
                {   /* check that the found reference had been checked before if it is began in one entity and end in another */
                    /* Rule 4.3.2 */
                    u8EntityIndex  = pstrParserInstanceType->objstrFoundReferenceType[u8ReferenceIndex].u8EntityIndex;
                    pu8EntityValue = pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityValue;
                    if(pstrParserInstanceType->objstrEntityType[u8EntityIndex].u8EntityValueLength > 0U)
                    {
                        /* Check if the still have unresolved entity, it may referee to itself */
                        if(pstrParserInstanceType->u8FoundReference > 0U)
                        {
                            if(strstr((INT8S const *)pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex],\
                                      (INT8S const *)pu8EntityValue) != STD_NULL)
                            {
                                pstrParserInstanceType->objenumErrorType = csEntityRefereToItself;
                            }
                            else
                            {
                                pstrParserInstanceType->objenumErrorType = csLargeResolvingTrialsProperty;
                            }
                        }
                        else
                        {
                            /*
                             * Nothing to do
                             */
                        }
                    }
                    else
                    {
                        /*
                         * Nothing to do
                         */
                    }
                }
                else if((pstrParserInstanceType->objstrFoundReferenceType[u8ReferenceIndex].objenumReferenceTypeType == csCharacterEntityReference) &&
                        (u8EntityReferenceUpdateTrials > 1U))
                {
                    pstrParserInstanceType->objenumErrorType = csPartialCharacterReference;
                }
                else
                {
                    /*
                     * Nothing to do
                     */
                }
            }
        }
        else /* there is no more references */
        {
            /*TODO: erro */
        }
#endif
        if(pstrParserInstanceType->objenumErrorType == csNoError)
        {
            /* round two, Replace any  (#x20, #xD, #xA, #x9) by (#x20U)*/
            pu8AttributeValue = pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex];
            while(*pu8AttributeValue != NULL_ASCII_CHARACTER)
            {
                if((*pu8AttributeValue == HTAB_ASCII_CHARACTER)|| \
                   (*pu8AttributeValue == LINE_FEED_ASCII_CHARACTER)|| \
                   (*pu8AttributeValue == CARRIAGE_RETURN_ASCII_CHARACTER))
                {
                    *pu8AttributeValue = SPACE_ASCII_CHARACTER;
                }
                ++pu8AttributeValue;
            }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
            pstrParserInstanceType->objenumErrorType = \
                    LwXMLP_enumUpdateEntityReference(pstrParserInstanceType, \
                            pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex],
                            u32CurrentTagLength,\
                            UPDATE_CHARACTER_REFERENCE);
#endif
            for(u8AtributeListIndex = 0; u8AtributeListIndex < MAX_ATTRIBUTES_LIST; ++u8AtributeListIndex)
            {
                if(strncmp((INT8S const *)pstrParserInstanceType->pu8AttributeNameArray[s16Attributeindex], \
                           (INT8S const *)pstrParserInstanceType->objstrAttributeListType[u8AtributeListIndex].u8AttributeName, \
                           MAX_ATTRIBUTES_NAME_LENGTH) == 0)
                {
                    objenumAttibuteTypeType = pstrParserInstanceType->objstrAttributeListType[u8AtributeListIndex].objenumAttibuteTypeType;
                    break;
                }
            }
            if(objenumAttibuteTypeType != csCDATAType)
            {
                /* round three, replacing sequences of space (#x20U) characters by a single space (#x20U)  */
                pu8AttributeValue = pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex];
                while(*pu8AttributeValue != NULL_ASCII_CHARACTER)
                {
                    if((u8LastCharacter == SPACE_ASCII_CHARACTER) && (*pu8AttributeValue == SPACE_ASCII_CHARACTER))
                    {
                        LwXMLP_vMoveStringBlockLeft(pu8AttributeValue, 1U);
                        u8LastCharacter = *pu8AttributeValue;
                    }
                    else
                    {
                        u8LastCharacter = *pu8AttributeValue;
                        ++pu8AttributeValue;
                    }
                }
                /* round four, discarding any leading space (#x20U) characters*/
                pu8AttributeValue = pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex];
                while(*pu8AttributeValue == SPACE_ASCII_CHARACTER)
                {
                   LwXMLP_vMoveStringBlockLeft(pu8AttributeValue, 1U);
                }
                /* round five, discarding any trailing space (#x20U) characters*/
                pu8AttributeValue = pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex];
                pu8AttributeValue = &pu8AttributeValue[strlen((INT8S const *)pu8AttributeValue) - 1U];
                while(*pu8AttributeValue == SPACE_ASCII_CHARACTER)
                {
                    *pu8AttributeValue = NULL_ASCII_CHARACTER;
                    pu8AttributeValue--;
                }
            }
            else
            {
                /*
                 * Do nothing
                 */
            }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
            for(u8EntityReferenceUpdateTrials = 0U; \
                (u8EntityReferenceUpdateTrials < MAX_ENTITY_RESOLVING_DEPTH) && \
                (pstrParserInstanceType->objenumErrorType == csNoError); ++u8EntityReferenceUpdateTrials)
            {
                pstrParserInstanceType->objenumErrorType = LwXMLP_enumUpdateEntityReference(pstrParserInstanceType, \
                                                                                            pstrParserInstanceType->pu8AttributeValueArray[s16Attributeindex],\
                                                                                            u32CurrentTagLength,\
                                                                                            UPDATE_CHARACTER_REFERENCE | UPDATE_ENTITY_REFERENCE);
                if((pstrParserInstanceType->objenumErrorType == csMissingSemicolon) && (u8EntityReferenceUpdateTrials > 0U))
                { /* may be & appears after second round, so it shouldn't consider an error */
                    pstrParserInstanceType->objenumErrorType = csNoError;
                }
                else
                {
                    /*
                     * Nothing to do
                     */
                }
                if(pstrParserInstanceType->u8FoundReference == 0U)/* No Entity is found */
                {
                    break;
                }
                else
                {
                    /*
                     * Nothing to do
                     */
                }
            }
#endif
        }
        else
        {
            /*
             * Nothing to do
             */
        }
    }
}
#if((ENABLE_ENTITY_SUPPORT == STD_ON) && (ENABLE_ATTRIBUTE_Normalization == STD_ON))
static LwXMLP_enumErrorType LwXMLP_enumValidateAttributeValue(INT8U const *pu8AttributeValue)
{
    LwXMLP_enumErrorType objenumErrorType = csNoError;

    for(;*pu8AttributeValue != NULL_ASCII_CHARACTER;)
    {
        if(*pu8AttributeValue == LESS_THAN_ASCII_CHARACTER)
        {
            objenumErrorType = csInvalidDirectReference;
            break;
        }
        else
        {
            /*
             * Nothing to do
             */
        }
        pu8AttributeValue++;
    }

    return objenumErrorType;
}
#endif
static void LwXMLP_vMoveStringBlockLeft(INT8U *pu8StringBlockAddress, INT32U u32MoveLength)
{
    INT8U  const *pu8StringEnd = pu8StringBlockAddress;
    INT32U u32BlockLength = 0U;
    INT32U u32ByteIndex;

    while(*pu8StringEnd != NULL_ASCII_CHARACTER)
    {
        u32BlockLength++;
        pu8StringEnd++;
    }
    for(u32ByteIndex = 0; u32ByteIndex < u32BlockLength; ++u32ByteIndex)
    {
        pu8StringBlockAddress[u32ByteIndex] = pu8StringBlockAddress[u32ByteIndex + u32MoveLength];
    }
}
#endif
static BOOLEAN LwXMLP_bMoreCharacterAtEndOfXML(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    BOOLEAN  bIsCharactersAtEndOfXML = STD_FALSE;
    INT8U   const *pu8CurrentXMLCharacter;
    INT32U   u32CharacterIndex, u32CheckingDataAmount;


    u32CheckingDataAmount  = LwXMLP_u32GetReadyData(pstrParserInstanceType);
    pu8CurrentXMLCharacter = pstrParserInstanceType->pu8LastParserLocation; /* we need to to escape < */
    for(u32CharacterIndex = 0U; u32CharacterIndex < u32CheckingDataAmount;++u32CharacterIndex)
    {
        if(LwXMLP_bIsWhiteSpace(pu8CurrentXMLCharacter) == STD_FALSE)
        {
            bIsCharactersAtEndOfXML = STD_TRUE;
            break;
        }
        else
        {   /* consume any white space */
            LwXMLP_vConsumeCharacter(pstrParserInstanceType);
        }
        pu8CurrentXMLCharacter++;
    }

    return bIsCharactersAtEndOfXML;
}
static BOOLEAN LwXMLP_bXMLPathIsEmpty(LwXMLP_strXMLParseInstanceType const *pstrParserInstanceType)
{
    BOOLEAN bIsEndOfXML;

    if(pstrParserInstanceType->u8XMLPathBuffer[0] == NULL_ASCII_CHARACTER)
    {
        bIsEndOfXML = STD_TRUE;
    }
    else
    {
        bIsEndOfXML = STD_FALSE;
    }

    return bIsEndOfXML;
}
static void LwXMLP_vRemoveTagfromPath(LwXMLP_strXMLParseInstanceType *pstrParserInstanceType)
{
    INT8U         *pu8SlashLocation;
    INT32U         u32TempPathLength;
    INT8U         *pu8TempPath;

    pu8SlashLocation  = STD_NULL;
    u32TempPathLength = pstrParserInstanceType->u32PathLength;
    pu8TempPath       = &pstrParserInstanceType->u8XMLPathBuffer[u32TempPathLength];
    while(u32TempPathLength != 0U)
    {
        if(*pu8TempPath == BACK_SLASH_ASCII_CHARACTER)
        {
            pu8SlashLocation                      = pu8TempPath;
            pstrParserInstanceType->u32PathLength = u32TempPathLength;
            break;
        }
        pu8TempPath--;
        u32TempPathLength--;
    }
    if(pu8SlashLocation == STD_NULL)
    {
        if(strcmp((INT8S const*)pstrParserInstanceType->u8XMLPathBuffer, \
                (INT8S const*)pstrParserInstanceType->pu8ElementName) == 0)
        {
            *(pstrParserInstanceType->u8XMLPathBuffer) = NULL_ASCII_CHARACTER;
            pstrParserInstanceType->u32PathLength      = 0U;
        }
        else /* trying to remove unexist TAG */
        {
            pstrParserInstanceType->objenumErrorType = csWrongNesting;
        }
    }
    else
    {
        if(strcmp((INT8S const*)(&pu8SlashLocation[1U]), \
                  (INT8S const*)pstrParserInstanceType->pu8ElementName) == 0)
        {
            *pu8SlashLocation = NULL_ASCII_CHARACTER;
            pstrParserInstanceType->u32PathLength = u32TempPathLength;
        }
        else /* trying to remove unexist TAG */
        {
            pstrParserInstanceType->objenumErrorType = csWrongNesting;
        }
    }
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    if(pstrParserInstanceType->bSecondLevelBufferEnabled == STD_TRUE)
    {
        pu8SlashLocation  = STD_NULL;
        u32TempPathLength = pstrParserInstanceType->u32SecondLevelPathLength;
        pu8TempPath       = &pstrParserInstanceType->u8SecondLevelPathBuffer[u32TempPathLength];
        while(u32TempPathLength != 0U)
        {
            if(*pu8TempPath == BACK_SLASH_ASCII_CHARACTER)
            {
                pu8SlashLocation = pu8TempPath;
                pstrParserInstanceType->u32SecondLevelPathLength = u32TempPathLength;
                break;
            }
            pu8TempPath--;
            u32TempPathLength--;
        }
        if(pu8SlashLocation == STD_NULL)
        {
            if(strcmp((INT8S const*)pstrParserInstanceType->u8SecondLevelPathBuffer, \
                    (INT8S const*)pstrParserInstanceType->pu8ElementName) == 0)
            {
                *(pstrParserInstanceType->u8SecondLevelPathBuffer) = NULL_ASCII_CHARACTER;
                pstrParserInstanceType->u32SecondLevelPathLength   = 0U;
            }
            else /* trying to remove unexist TAG */
            {
                pstrParserInstanceType->objenumErrorType = csWrongNesting;
            }
        }
        else
        {
            if(strcmp((INT8S const*)(&pu8SlashLocation[1U]), \
                      (INT8S const*)pstrParserInstanceType->pu8ElementName) == 0)
            {
                *pu8SlashLocation = NULL_ASCII_CHARACTER;
                pstrParserInstanceType->u32SecondLevelPathLength = u32TempPathLength;
            }
            else /* trying to remove unexist TAG */
            {
                pstrParserInstanceType->objenumErrorType = csWrongNesting;
            }
        }
    }
#endif
}
/*
********************************************************************************************************************
*                                                      MODULE END
********************************************************************************************************************
*/




