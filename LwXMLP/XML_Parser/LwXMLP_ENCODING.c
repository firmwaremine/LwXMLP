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
 * @file LwXMLP_ENCODING.C
 * @brief This is a source code file for the different encoding schemes
 * @author Mohammed Fawzy
 * @version 1.0.0
 **********************************************************************************************************************/

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
#include "LwXMLP_ENCODING.h"
#include <string.h>
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
#define LITTLE_ENDIAN   0U              /*!< Little ENDIAN option */
#define BIG_ENDIAN      1U              /*!< BIG ENDIAN option */
#define ENDIAN_TYPE     LITTLE_ENDIAN   /*!< Define the microcontroller ENDIAN type */

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
static StdReturnType LwXMLP_srUTF16LEToUTF8(const INT8U  *pu8InputBuffer, \
                                                  INT32U *pu32InputBufferLength, \
                                                  INT8U  *pu8OutputBuffer, \
                                                  INT32U *pu32OutputBufferLength);
static StdReturnType LwXMLP_srUTF16BEToUTF8(const INT8U  *pu8InputBuffer,\
                                                  INT32U *pu32InputBufferLength,\
                                                  INT8U  *pu8OutputBuffer,\
                                                  INT32U *pu32OutputBufferLength);
static StdReturnType LwXMLP_srASCIIToUTF8(const INT8U  *pu8InputBuffer, \
                                                INT32U *pu32InputBufferLength, \
                                                INT8U  *pu8OutputBuffer,\
                                                INT32U *pu32OutputBufferLength);
static StdReturnType LwXMLP_srUTF8ToUTF8(const INT8U  *pu8InputBuffer, \
                                               INT32U *pu32InputBufferLength, \
                                               INT8U  *pu8OutputBuffer,
                                               INT32U *pu32OutputBufferLength);
static StdReturnType LwXMLP_srIsolat1ToUTF8(const INT8U  *pu8InputBuffer, \
                                                INT32U *pu32InputBufferLength, \
                                                INT8U  *pu8OutputBuffer, \
                                                INT32U *pu32OutputBufferLength);
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
/*!\fn        StdReturnType LwXMLP_srEncodeXMLportion(      LwXMLP_enumEncodingTypeType  objenumEncodingTypeType,
                                                      const INT8U                       *pu8InputBuffer,
                                                            INT32U                      *pu32InputBufferLength,
                                                            INT8U                       *pu8OutputBuffer,
                                                            INT32U                      *pu32OutputBufferLength)


@brief         This API is invoking the propore encoding converter function received by objenumEncodingTypeType, so
               the received protion will be converted from its encoding type to UTF-8 type

@param[in]       objenumEncodingTypeType : the current encoding type of the data inside the pu8InputBuffer

@param[in]       pu8InputBuffer          : pointer to the data to be encoded

@param[in,out]   pu32InputBufferLength   : the length of the data to be encodded and it will be used to return the
                                           the amount of the encodded length out of the actual received length

@param[out]      pu8OutputBuffer         : buffer where the output encodded data will be stored in

@param[out]      pu32OutputBufferLength  : the encoodded data length

@return        E_NOT_OK: if the encodding process is failed
               E_OK    : if the encodding process is succedded

@note          the XML parser enigne shall be continusly invoked till parsing done or the parsing encountered an error
***********************************************************************************************************************/
StdReturnType LwXMLP_srEncodeXMLportion(       LwXMLP_enumEncodingTypeType  objenumEncodingTypeType,
                                         const INT8U                       *pu8InputBuffer,
                                               INT32U                      *pu32InputBufferLength,
                                               INT8U                       *pu8OutputBuffer,
                                               INT32U                      *pu32OutputBufferLength)
{
    StdReturnType srEncodingStatus = E_OK;

    switch(objenumEncodingTypeType)
    {
        case csEncoding_ASCII: /*!< pure ASCII */
             srEncodingStatus = LwXMLP_srASCIIToUTF8(pu8InputBuffer, \
                                                     pu32InputBufferLength, \
                                                     pu8OutputBuffer,\
                                                     pu32OutputBufferLength);

             break;
        case csEncoding_UTF16LE:
             srEncodingStatus = LwXMLP_srUTF16LEToUTF8(pu8InputBuffer, \
                                                       pu32InputBufferLength, \
                                                       pu8OutputBuffer, \
                                                       pu32OutputBufferLength);

             break;
        case csEncoding_UTF16BE:
             srEncodingStatus = LwXMLP_srUTF16BEToUTF8(pu8InputBuffer,\
                                                       pu32InputBufferLength,\
                                                       pu8OutputBuffer,\
                                                       pu32OutputBufferLength);

             break;
        case csEncoding_NONE:
        case csEncoding_UTF8:
             srEncodingStatus = LwXMLP_srUTF8ToUTF8(pu8InputBuffer,\
                                                    pu32InputBufferLength,\
                                                    pu8OutputBuffer,\
                                                    pu32OutputBufferLength);
             break;
        case csEncoding_8859_1: /*!< ISO-8859-1U ISO Latin 1U */
             srEncodingStatus = LwXMLP_srIsolat1ToUTF8(pu8InputBuffer,\
                                                       pu32InputBufferLength,\
                                                       pu8OutputBuffer,\
                                                       pu32OutputBufferLength);
             break;
        case csEncoding_EBCDIC:
        case csEncoding_UCS4_2143:
        case csEncoding_UCS4_3412:
        case csEncoding_UCS4LE:
        case csEncoding_UCS4BE:
        case csEncoding_UCS2:
        case csEncoding_8859_2: /*!< ISO-8859-2 ISO Latin 2 */
        case csEncoding_8859_3: /*!< ISO-8859-3 */
        case csEncoding_8859_4: /*!< ISO-8859-4 */
        case csEncoding_8859_5: /*!< ISO-8859-5 */
        case csEncoding_8859_6: /*!< ISO-8859-6 */
        case csEncoding_8859_7: /*!< ISO-8859-7 */
        case csEncoding_8859_8: /*!< ISO-8859-8 */
        case csEncoding_8859_9: /*!< ISO-8859-9 */
        case csEncoding_2022_JP: /*!< ISO-2022-JP */
        case csEncoding_SHIFT_JIS: /*!< Shift_JIS */
        case csEncoding_EUC_JP: /*!< EUC-JP */
        case csEncoding_ERROR:
        default:
            srEncodingStatus = E_NOT_OK;
            /* Not supported encoding schemes */
            break;
    }

    return srEncodingStatus;
}
/*
 ********************************************************************************************************************
 ************************************************************************************************************************
 *                                               L O C A L   F U N C T I O N S
 ************************************************************************************************************************
 ********************************************************************************************************************
 */
/**********************************************************************************************************************/
/*!\fn         StdReturnType LwXMLP_srUTF8ToUTF8(const INT8U  *pu8InputBuffer, \
                                                 INT32U *pu32InputBufferLength, \
                                                 INT8U  *pu8OutputBuffer, \
                                                 INT32U *pu32OutputBufferLength)

@brief         This API is responsible to convert the UTF8 unicode in pu8InputBuffer to UTF8 unicode pu8OutputBuffer

@param[in]      pu8InputBuffer        : the pointer to the buffer with contain the data encoded in UTF16LE
@param[in,out]  pu32InputBufferLength : the length in bytes of the input data with UTF16LE encoding format, and it will
                                        be updated with the actually encoded data length
@param[in]      pu8OutputBuffer       : the pointer to the buffer that will be loaded with the enconded data in UTF8
@param[in,out]  pu32OutputBufferLength: the length in bytes of the out data buffer, and it will be updated with the
                                        actually encoded data length in UTF8 format

@return        E_OK    : if the convention is succeeded
@return        E_NOT_OK: if the convention is failed

@note
***********************************************************************************************************************/
static StdReturnType LwXMLP_srUTF8ToUTF8(const INT8U  *pu8InputBuffer, \
                                               INT32U *pu32InputBufferLength, \
                                               INT8U  *pu8OutputBuffer,\
                                               INT32U *pu32OutputBufferLength)
{
    StdReturnType srEncodingStatus = E_OK;
    INT32U u32Templength;

    if((pu8OutputBuffer != STD_NULL) && \
        (pu8InputBuffer != STD_NULL) && \
        (pu32OutputBufferLength != STD_NULL) && \
        (pu32InputBufferLength != STD_NULL))
    {
        if(*pu32OutputBufferLength > *pu32InputBufferLength)
        {
            u32Templength = *pu32InputBufferLength;
        }
        else
        {
            u32Templength = *pu32OutputBufferLength;
        }
        memcpy(pu8OutputBuffer, (const INT8U *)pu8InputBuffer, u32Templength);
        *pu32OutputBufferLength = u32Templength;
        *pu32InputBufferLength  = u32Templength;
    }
    else
    {
        srEncodingStatus = E_NOT_OK;
    }

    return srEncodingStatus;
}
/**********************************************************************************************************************/
/*!\fn         StdReturnType LwXMLP_srUTF16LEToUTF8(const INT8U  *pu8InputBuffer, \
                                                          INT32U *pu32InputBufferLength, \
                                                          INT8U  *pu8OutputBuffer, \
                                                          INT32U *pu32OutputBufferLength)

@brief         This API is responsible to convert the UTF16LE unicode in pu8InputBuffer to UTF8 unicode pu8OutputBuffer

@param[in]      pu8InputBuffer        : the pointer to the buffer with contain the data encoded in UTF16LE
@param[in,out]  pu32InputBufferLength : the length in bytes of the input data with UTF16LE encoding format, and it will
                                        be updated with the actually encoded data length
@param[in]      pu8OutputBuffer       : the pointer to the buffer that will be loaded with the enconded data in UTF8
@param[in,out]  pu32OutputBufferLength: the length in bytes of the out data buffer, and it will be updated with the
                                        actually encoded data length in UTF8 format

@return        E_OK    : if the convention is succeeded
@return        E_NOT_OK: if the convention is failed

@note
***********************************************************************************************************************/
static StdReturnType LwXMLP_srUTF16LEToUTF8(const INT8U  *pu8InputBuffer, \
                                                  INT32U *pu32InputBufferLength, \
                                                  INT8U  *pu8OutputBuffer, \
                                                  INT32U *pu32OutputBufferLength)
{
          StdReturnType  srEncodingStatus = E_OK;
          INT8U         const *pu8TempBuffer = pu8InputBuffer;
          INT32U         u32TempCharacter, u32SurrogatePair, u32CheckedLength = 0U, u32EncodingLength = 0U;
          INT32S         s32Bits;

          memset(pu8OutputBuffer, 0x00, *pu32OutputBufferLength);
    if((*pu32InputBufferLength) > 5U) /* Guarantee that the incoming buffer contain complete character */
    {
        if((*pu32InputBufferLength % 2U) == 1U)
        {
            (*pu32InputBufferLength)--;
        }
        while(((u32EncodingLength + 5U) < *pu32OutputBufferLength) && (u32CheckedLength < ((*pu32InputBufferLength)/2U)))
        {
#if(ENDIAN_TYPE == LITTLE_ENDIAN)
            u32TempCharacter = *pu8TempBuffer;
            pu8TempBuffer++;
            u32TempCharacter |= ((INT32U)*pu8TempBuffer << 8U);
            pu8TempBuffer++;
#endif
#if(ENDIAN_TYPE == BIG_ENDIAN)
            u32TempCharacter = (((INT32U) *pu8TempBuffer) << 8U);
            pu8TempBuffer++;
            u32TempCharacter |= *pu8TempBuffer;
            pu8TempBuffer++;
#endif
            if((u32TempCharacter & 0xFC00U) == 0xD800U)
            {
#if(ENDIAN_TYPE == LITTLE_ENDIAN)
                u32SurrogatePair = *pu8TempBuffer;
                pu8TempBuffer++;
                u32SurrogatePair |= ((INT32U)*pu8TempBuffer << 8U);
                pu8TempBuffer++;
#endif
#if(ENDIAN_TYPE == BIG_ENDIAN)
                u32SurrogatePair = (((INT32U) *pu8TempBuffer) << 8U);
                pu8TempBuffer++;
                u32SurrogatePair |= *pu8TempBuffer;
                pu8TempBuffer++;
#endif
                if((u32SurrogatePair & 0xFC00U) == 0xDC00U)
                {
                    u32TempCharacter &= 0x03FFU;
                    u32TempCharacter <<= 10U;
                    u32TempCharacter |= u32SurrogatePair & 0x03FFU;
                    u32TempCharacter += 0x10000U;
                }
                else
                {
                    srEncodingStatus = E_NOT_OK;
                    break;
                }
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
            if(u32TempCharacter < 0x80U)
            {
                *pu8OutputBuffer = (INT8U)u32TempCharacter;
                pu8OutputBuffer++;
                s32Bits = -6;
                u32EncodingLength++;
            }
            else if(u32TempCharacter < 0x800U)
            {
                *pu8OutputBuffer = (INT8U)(((u32TempCharacter >> 6U) & 0x1FU) | 0xC0U);
                pu8OutputBuffer++;
                u32EncodingLength++;
                s32Bits = 0;
            }
            else if(u32TempCharacter < 0x10000U)
            {
                *pu8OutputBuffer = (INT8U)(((u32TempCharacter >> 12U) & 0x0FU) | 0xE0U);
                pu8OutputBuffer++;
                u32EncodingLength++;
                s32Bits = 6;
            }
            else
            {
                *pu8OutputBuffer = (INT8U)(((u32TempCharacter >> 18U) & 0x07U) | 0xF0U);
                pu8OutputBuffer++;
                u32EncodingLength++;
                s32Bits = 12;
            }
            for(; s32Bits >= 0; s32Bits -= 6)
            {
                *pu8OutputBuffer = (INT8U)(((u32TempCharacter >> (INT32U)s32Bits) & 0x3FU) | 0x80U);
                pu8OutputBuffer++;
                u32EncodingLength++;
            }
            u32CheckedLength++;
        }
        *pu32InputBufferLength  = (u32CheckedLength * 2U);
        *pu32OutputBufferLength = u32EncodingLength;
    }
    else
    {
        *pu32OutputBufferLength = 0U;
        *pu32InputBufferLength  = 0U;
    }
    return srEncodingStatus;
}
/**********************************************************************************************************************/
/*!\fn         StdReturnType LwXMLP_srUTF16BEToUTF8(const INT8U  *pu8InputBuffer, \
                                                 INT32U *pu32InputBufferLength, \
                                                 INT8U  *pu8OutputBuffer, \
                                                 INT32U *pu32OutputBufferLength)

@brief         This API is responsible to convert the UTF16BE unicode in pu8InputBuffer to UTF8 unicode pu8OutputBuffer

@param[in]      pu8InputBuffer        : the pointer to the buffer with contain the data encoded in UTF16BE
@param[in,out]  pu32InputBufferLength : the length in bytes of the input data with UTF16BE encoding format, and it will
                                        be updated with the actually encoded data length
@param[in]      pu8OutputBuffer       : the pointer to the buffer that will be loaded with the enconded data in UTF8
@param[in,out]  pu32OutputBufferLength: the length in bytes of the out data buffer, and it will be updated with the
                                        actually encoded data length in UTF8 format

@return        E_OK    : if the convention is succeeded
@return        E_NOT_OK: if the convention is failed

@note
***********************************************************************************************************************/
static StdReturnType LwXMLP_srUTF16BEToUTF8(const INT8U  *pu8InputBuffer, \
                                                  INT32U *pu32InputBufferLength, \
                                                  INT8U  *pu8OutputBuffer, \
                                                  INT32U *pu32OutputBufferLength)
{
          StdReturnType  srEncodingStatus = E_OK;
          BOOLEAN        bReachEncodingEnd = STD_FALSE;
    const INT8U         *pu8TempInputBuffer = pu8InputBuffer;
          INT32U         u32UTF8LowerWord, u32UTF8UpperWord;
          INT32S         s32Bits = 0;
          INT32U         u32AvaliableOutputLength = *pu32OutputBufferLength, u32AvaliableInputLength;

    if(((*pu32InputBufferLength) > 2U) && (*pu32OutputBufferLength > 0U))/* Guarantee that the incoming buffer contain complete character */
    {

        if((*pu32InputBufferLength % 2U) == 1U)
        {
            (*pu32InputBufferLength)--;
        }
        else
        {
            /*
             * Nothing to do
             */
        }
        u32AvaliableInputLength = *pu32InputBufferLength / 2U;
        while((u32AvaliableInputLength > 0U) && (u32AvaliableOutputLength > 0U) && (bReachEncodingEnd == STD_FALSE))
        {
#if(ENDIAN_TYPE == LITTLE_ENDIAN)
                u32UTF8LowerWord = *pu8TempInputBuffer;
                pu8TempInputBuffer++;
                u32UTF8LowerWord = u32UTF8LowerWord << 8U;
                u32UTF8LowerWord |= (INT32U) *pu8TempInputBuffer;
                pu8TempInputBuffer++;
#endif
#if(ENDIAN_TYPE == BIG_ENDIAN)
                u32UTF8LowerWord |= (((INT32U) *pu8TempInputBuffer) << 8U);
                pu8TempInputBuffer++;
                u32UTF8LowerWord = *pu8TempInputBuffer;
                pu8TempInputBuffer++;
#endif
            if((u32UTF8LowerWord & 0xFC00U) == 0xD800U)
            {
#if(ENDIAN_TYPE == LITTLE_ENDIAN)
                u32UTF8UpperWord = *pu8TempInputBuffer;
                pu8TempInputBuffer++;
                u32UTF8UpperWord = u32UTF8UpperWord << 8U;
                u32UTF8UpperWord |= (INT32U) *pu8TempInputBuffer;
                pu8TempInputBuffer++;
#endif
#if(ENDIAN_TYPE == BIG_ENDIAN)
                u32UTF8UpperWord |=(((INT32U) *pu8TempInputBuffer) << 8U);
                pu8TempInputBuffer++;
                u32UTF8UpperWord = *pu8TempInputBuffer;
                pu8TempInputBuffer++;
#endif
                if((u32UTF8UpperWord & 0xFC00U) == 0xDC00U)
                {
                    u32UTF8LowerWord &= 0x03FFU;
                    u32UTF8LowerWord <<= 10;
                    u32UTF8LowerWord |= u32UTF8UpperWord & 0x03FFU;
                    u32UTF8LowerWord += 0x10000U;
                }
                else
                {
                    srEncodingStatus = E_NOT_OK;
                    break;
                }
            }
            if(u32UTF8LowerWord < 0x80U)
            {
                if(u32AvaliableOutputLength >= 1U)
                {
                    *pu8OutputBuffer = (INT8U)u32UTF8LowerWord;
                    pu8OutputBuffer++;
                    s32Bits = -6;
                    u32AvaliableOutputLength -= 1U;
                }
                else
                {
                    bReachEncodingEnd = STD_TRUE;
                }
            }
            else if(u32UTF8LowerWord < 0x800U)
            {
                if(u32AvaliableOutputLength >= 2U)
                {
                    *pu8OutputBuffer = (INT8U)(((u32UTF8LowerWord >> 6U) & 0x1FU) | 0xC0U);
                    pu8OutputBuffer++;
                    s32Bits = 0;
                    u32AvaliableOutputLength -= 2U;
                }
                else
                {
                    bReachEncodingEnd = STD_TRUE;
                }
            }
            else if(u32UTF8LowerWord < 0x10000U)
            {
                if(u32AvaliableOutputLength >= 3U)
                {
                    *pu8OutputBuffer = (INT8U)(((u32UTF8LowerWord >> 12U) & 0x0FU) | 0xE0U);
                    pu8OutputBuffer++;
                    s32Bits = 6;
                    u32AvaliableOutputLength -= 3U;
                }
                else
                {
                    bReachEncodingEnd = STD_TRUE;
                }
            }
            else
            {
                if(u32AvaliableOutputLength >= 4U)
                {
                    *pu8OutputBuffer = (INT8U)(((u32UTF8LowerWord >> 18U) & 0x07U) | 0xF0U);
                    pu8OutputBuffer++;
                    s32Bits = 12;
                    u32AvaliableOutputLength -= 4U;
                }
                else
                {
                    bReachEncodingEnd = STD_TRUE;
                }

            }
            for(; (s32Bits >= 0) && (bReachEncodingEnd == STD_FALSE); s32Bits -= 6)
            {
                *pu8OutputBuffer = (INT8U)(((u32UTF8LowerWord >> (INT32U)s32Bits) & 0x3FU) | 0x80U);
                pu8OutputBuffer++;
            }
            if(bReachEncodingEnd == STD_FALSE)
            {
                u32AvaliableInputLength--;
            }
            else
            {
                /*
                 * Nothing to do
                 */
            }
        }
        *pu32OutputBufferLength -= u32AvaliableOutputLength;
        *pu32InputBufferLength  -= (u32AvaliableInputLength * 2U);
    }
    else
    {
        *pu32OutputBufferLength = 0;
        *pu32InputBufferLength  = 0;
    }

    return srEncodingStatus;
}
/**********************************************************************************************************************/
/*!\fn         StdReturnType LwXMLP_srUTF16BEToUTF8(const INT8U  *pu8InputBuffer, \
                                                    INT32U *pu32InputBufferLength, \
                                                    INT8U  *pu8OutputBuffer, \
                                                    INT32U *pu32OutputBufferLength)

@brief         This API is responsible to convert the ASCII in pu8InputBuffer to UTF8 unicode pu8OutputBuffer

@param[in]      pu8InputBuffer        : the pointer to the buffer with contain the data encoded in ASCII
@param[in,out]  pu32InputBufferLength : the length in bytes of the input data with ASCII encoding format, and it will
                                        be updated with the actually encoded data length
@param[in]      pu8OutputBuffer       : the pointer to the buffer that will be loaded with the enconded data in UTF8
@param[in,out]  pu32OutputBufferLength: the length in bytes of the out data buffer, and it will be updated with the
                                        actually encoded data length in UTF8 format

@return        E_OK    : if the convention is succeeded
@return        E_NOT_OK: if the convention is failed

@note
***********************************************************************************************************************/
static StdReturnType LwXMLP_srASCIIToUTF8(const INT8U  *pu8InputBuffer, \
                                                INT32U *pu32InputBufferLength, \
                                                INT8U  *pu8OutputBuffer, \
                                                INT32U *pu32OutputBufferLength)
{
    INT32U u32CheckedLength = 0U, u32EncodingLength = 0U;
    StdReturnType  srEncodingStatus = E_OK;

    while((u32EncodingLength < (*pu32OutputBufferLength)) && (u32CheckedLength < (*pu32InputBufferLength)))
    {
        if(pu8InputBuffer[u32CheckedLength] < 0x80U)
        {
            pu8OutputBuffer[u32EncodingLength] = pu8InputBuffer[u32CheckedLength];
        }
        else
        {
            srEncodingStatus = E_NOT_OK;
            break;
        }

        u32EncodingLength++;
        u32CheckedLength++;
    }
    *pu32OutputBufferLength = u32EncodingLength;
    *pu32InputBufferLength  = u32CheckedLength;

    return srEncodingStatus;
}
/**********************************************************************************************************************/
/*!\fn         StdReturnType LwXMLP_srIsolat1ToUTF8(const INT8U  *pu8InputBuffer, \
                                                    INT32U *pu32InputBufferLength, \
                                                    INT8U  *pu8OutputBuffer, \
                                                    INT32U *pu32OutputBufferLength)

@brief         This API is responsible to convert the ISO-8859-1 in pu8InputBuffer to UTF8 unicode pu8OutputBuffer

@param[in]      pu8InputBuffer        : the pointer to the buffer with contain the data encoded in ASCII
@param[in,out]  pu32InputBufferLength : the length in bytes of the input data with ASCII encoding format, and it will
                                        be updated with the actually encoded data length
@param[in]      pu8OutputBuffer       : the pointer to the buffer that will be loaded with the enconded data in UTF8
@param[in,out]  pu32OutputBufferLength: the length in bytes of the out data buffer, and it will be updated with the
                                        actually encoded data length in UTF8 format

@return        E_OK    : if the convention is succeeded
@return        E_NOT_OK: if the convention is failed

@note
***********************************************************************************************************************/
static StdReturnType LwXMLP_srIsolat1ToUTF8(const INT8U  *pu8InputBuffer, \
                                                INT32U *pu32InputBufferLength, \
                                                INT8U  *pu8OutputBuffer, \
                                                INT32U *pu32OutputBufferLength)
{
    INT32U u32CheckedLength = 0U, u32EncodingLength = 0U;
    StdReturnType  srEncodingStatus = E_OK;

    while((u32EncodingLength < (*pu32OutputBufferLength - 1U)) && (u32CheckedLength < (*pu32InputBufferLength)))
    {
        if(pu8InputBuffer[u32CheckedLength] < 0x80U)
        {
            pu8OutputBuffer[u32EncodingLength] = pu8InputBuffer[u32CheckedLength];
            u32EncodingLength++;
            u32CheckedLength++;
        }
        else
        {
            pu8OutputBuffer[u32EncodingLength] = ((pu8InputBuffer[u32CheckedLength] >> 6U) & 0x1FU) | 0xC0U;
            u32EncodingLength++;
            pu8OutputBuffer[u32EncodingLength] = (pu8InputBuffer[u32CheckedLength] & 0x3FU) | 0x80U;
            u32EncodingLength++;
            u32CheckedLength++;
        }
    }
    *pu32OutputBufferLength = u32EncodingLength;
    *pu32InputBufferLength  = u32CheckedLength;

    return srEncodingStatus;
}

/*
 ********************************************************************************************************************
 *                                                      MODULE END
 ********************************************************************************************************************
 */

