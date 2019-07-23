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
 * @file LwXMLP_FCFG.h
 * @brief This is the header file of the LwXMLP_CORE fixed configuration which contain all macros which control the
*   behavior of the LwXMLP either by enable/disable some of the LwXMLP features or control the size of some of the
*   elements which control the whole data size used by the parser
 * @author Mohammed Fawzy
 * @version 1.0.0
 **********************************************************************************************************************/
#ifndef  LwXMLP_FCFG_H
#define  LwXMLP_FCFG_H

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
/*
 ==================================================================================================================
 *                                                  Application Include
 ==================================================================================================================
 */

/*
********************************************************************************************************************
************************************************************************************************************************
*                                                   # D E F I N E S
************************************************************************************************************************
********************************************************************************************************************
*/
/*
 * Enable/Disable verbose and it shall be disabled in the production code
 */
#define LwXMLP_VERBOSE_ENABLE              STD_OFF                   /*!< Enable/Disable the verbose output, and it should
                                                                      be set to STD_OFF for production code */
#define LwXMLP_VERBOSE_MIN_LEVEL           VERBOSE_LEVEL_DEBUGGING   /*!< Verbose level for LwXMLP   There are three levels
                                                                         VERBOSE_LEVEL_DEBUGGING
                                                                         VERBOSE_LEVEL_WARNING
                                                                         VERBOSE_LEVEL_SERIOUS
                                                                      */
#define INTERNAL_STATIC_ALLOCATION            0U                     /*!< Internal Static allocation type, such that the
                                                                      LwXMLP will allocate the parser instance
                                                                      static internally*/
#define INTERNAL_DYNAMIC_ALLOCATION           1U                     /*!< Internal dynamic allocation type, such that the
                                                                      LwXMLP will allocate the parser instance
                                                                      dynamic internally and selecting such option
                                                                      will cause a violation for some MISRA:2012 rules
                                                                      like Rule 21.3, Rule 11.5, Rule 11.3, Rule 21.3
                                                                      and Rule 11.8*/
#define EXTERNAL_ALLOCATION                   2U                     /*!< Extarnal allocation type, such that the parser
                                                                       instance will be created by ivoker module*/

#define ALLOCATION_TYPE                       EXTERNAL_ALLOCATION    /*!< define the instance allocation type*/

#define LARGE_SIZE                            1U                      /*!< It supports all XML Features */
#define INTERMEDIATE_SIZE                     2U                      /*!< It supports some of the XML features*/
#define MINIMUM_SIZE                          3U                      /*!< It supports the least supported XML features
                                                                           which is adequate to achieve the minimum
                                                                           code and data size */

#define CONFIGURATION_TYPE                    INTERMEDIATE_SIZE              /*!< the type of configuration group */
#if(CONFIGURATION_TYPE == LARGE_SIZE)
    #define ENABLE_ENCODING                    STD_ON                 /*!< enable different encoding type rather than
                                                                            the UTF-8 */
    #define SUPPORT_ASCII_ONLY                 STD_OFF                /*!< Enable/Disable the support to XML with ASCII
                                                                           encoding format only */
    #define ENABLE_ENTITY_SUPPORT              STD_ON                 /*!< Enable/Disable the support to XML entity */
    #define ENABLE_EOL_NORMALIZATION           STD_ON                 /*!< Enable end of line normalization by Normalizing
                                                                             all line breaks before parsing, by translating
                                                                             both the two-character sequence #xD #xA and
                                                                             any #xD that is not followed by #xA to a
                                                                             single #xA character*/
    #define ENABLE_ATTRIBUTE_Normalization     STD_ON                 /*!< Enable/Disable the support for attribute
                                                                           normalization */
    #define SUPPORT_ERROR_STRING               STD_ON                 /*!< Enable/Disable the support for error string */
    #define ENABLE_DTD_RECEIPTION              STD_ON                 /*!< Enable the LwXML to  receive DTD */
    #if(ENABLE_DTD_RECEIPTION == STD_ON)
        #define ENABLE_DTD_WELL_FORM_CHECK     STD_ON                 /*!<Option to Enable/Disable well form for the DTD */
    #else
        #define ENABLE_DTD_WELL_FORM_CHECK     STD_OFF                /*!< Must disable well form checking for the DTD */
    #endif
    #define ENABLE_PI_WELL_FORM_CHECK          STD_ON                 /*!< Enable/Disable well form for the PI */
    #define ENABLE_COMMENT_WELL_FORM_CHECK     STD_ON                 /*!< Enable/Disable well form for the COMMENT */
    #define ENABLE_DECL_WELL_FORM_CHECK        STD_ON                 /*!< Enable/Disable well form for the DECLARATION */
    #define ENABLE_CDATA_WELL_FORM_CHECK       STD_ON                 /*!< Enable/Disable well form for the CDATA */
    #define MAX_ENTITY_VALUE_LENGTH            120U                   /*!< The maximum length of the entity value string,
                                                                            it shouldn't exceed 255 */
    #define MAX_XML_PATH_LENGTH                500U                   /*!< The Maximum path length of the XML element */
    #define MAX_WORKING_BUFFER_SIZE            7000U                  /*!< Maximum number of characters in the working
                                                                           buffer */
    #define MAX_CIRCULAR_BUFFER_SIZE           7000U                  /*!< Maximum number of characters in the circular
                                                                           buffer */
    #define TOTAL_STATIC_XML_INSTANCES         1U                     /*!< Total number of the allocated statically instances
                                                                          which should be set by the user, and it shouldn't
                                                                          be larger than excpeted to avoid unwanted memory
                                                                          waste*/
    #define MAX_ATTRIBUTES_LIST                100U                    /*!< Maximum number of attribute list declaration */
    #define MAX_OPERATORS_COUNT                10U                    /*!< Maximum operator counts inside the element
                                                                           content (children)*/
    #define MAX_ENTITY_NAME_LENGTH             30U                    /*!< The maximum length of the entity Name string
                                                                            including the Ampersand and the Semicolon,
                                                                            it shouldn't exceed 255*/
    #define MAX_ENTITIES                       102U                   /*!< The maximum number of entities, shouldn't
                                                                           exceed 255*/
    #define MAX_NUMBER_OF_ATTRIBUTES           40                     /*!< Maximum number of the supported attributes
                                                                           in the XML TAG*/

    #define MAX_ATTRIBUTES_NAME_LENGTH         30U                    /*!< Maximum number of characters per attribute
                                                                           name */
    #define MAX_ELEMENT_NAME_LENGTH            14U                    /*!< Maximum number of characters per element name */
    #define MAX_ATTRIBUTES_VALUE_LENGTH        100U                   /*!< Maximum number of characters per attribute
                                                                           value */
    #define MAX_ENTITY_RESOLVING_DEPTH         3U                     /*!< Maximum depth of resolving of the entity */
    #define MAX_FOUND_REFERENCE                4U                     /*!< Maximum allowed found references during
                                                                           entity resolving */
#elif(CONFIGURATION_TYPE == INTERMEDIATE_SIZE)
    #define ENABLE_ENCODING                    STD_OFF                /*!< enable different encoding type rather than
                                                                            the UTF-8 */
    #define SUPPORT_ASCII_ONLY                 STD_OFF                 /*!< Enable/Disable the support to XML with ASCII
                                                                           encoding format only */
    #define ENABLE_ENTITY_SUPPORT              STD_OFF                /*!< Enable/Disable the support to XML entity */
    #define ENABLE_EOL_NORMALIZATION           STD_OFF                /*!< Enable end of line normalization by Normalizing
                                                                             all line breaks before parsing, by translating
                                                                             both the two-character sequence #xD #xA and
                                                                             any #xD that is not followed by #xA to a
                                                                             single #xA character*/
    #define ENABLE_ATTRIBUTE_Normalization     STD_OFF                /*!< Enable/Disable the support for attribute
                                                                           normalization */
    #define SUPPORT_ERROR_STRING               STD_OFF                /*!< Enable/Disable the support for error string */
    #define ENABLE_DTD_RECEIPTION              STD_ON                 /*!< Enable the LwXML to  receive DTD */
    #if(ENABLE_DTD_RECEIPTION == STD_ON)
        #define ENABLE_DTD_WELL_FORM_CHECK     STD_OFF                 /*!<Option to Enable/Disable well form for the DTD */
    #else
        #define ENABLE_DTD_WELL_FORM_CHECK     STD_OFF                /*!< Must disable well form checking for the DTD */
    #endif
    #define ENABLE_PI_WELL_FORM_CHECK          STD_OFF                /*!< Enable/Disable well form for the PI */
    #define ENABLE_COMMENT_WELL_FORM_CHECK     STD_OFF                /*!< Enable/Disable well form for the COMMENT */
    #define ENABLE_DECL_WELL_FORM_CHECK        STD_OFF                /*!< Enable/Disable well form for the DECLARATION */
    #define ENABLE_CDATA_WELL_FORM_CHECK       STD_OFF                /*!< Enable/Disable well form for the CDATA */
    #define MAX_ENTITY_VALUE_LENGTH            120U                   /*!< The maximum length of the entity value string,
                                                                            it shouldn't exceed 255 */
    #define MAX_XML_PATH_LENGTH                100U                   /*!< The Maximum path length of the XML element */
    #define MAX_WORKING_BUFFER_SIZE            100U                   /*!< Maximum number of characters in the working
                                                                           buffer */
    #define MAX_CIRCULAR_BUFFER_SIZE           100U                   /*!< e */
    #define TOTAL_STATIC_XML_INSTANCES         1U                     /*!< Total number of the allocated statically instances
                                                                          which should be set by the user, and it shouldn't
                                                                          be larger than excepted to avoid unwanted memory
                                                                          waste*/
    #define MAX_ATTRIBUTES_LIST                16U                    /*!< Maximum number of attribute list declaration */
    #define MAX_OPERATORS_COUNT                10U                    /*!< Maximum operator counts inside the element
                                                                           content (children)*/
    #define MAX_ENTITY_NAME_LENGTH             30U                    /*!< The maximum length of the entity Name string
                                                                            including the Ampersand and the Semicolon,
                                                                            it shouldn't exceed 255*/
    #define MAX_ENTITIES                       102U                   /*!< The maximum number of entities, shouldn't
                                                                           exceed 255*/
    #define MAX_NUMBER_OF_ATTRIBUTES           8                      /*!< Maximum number of the supported attributes
                                                                           in the XML TAG*/

    #define MAX_ATTRIBUTES_NAME_LENGTH         30U                    /*!< Maximum number of characters per attribute
                                                                           name */
    #define MAX_ELEMENT_NAME_LENGTH            14U                    /*!< Maximum number of characters per element name */
    #define MAX_ATTRIBUTES_VALUE_LENGTH        100U                   /*!< Maximum number of characters per attribute
                                                                           value */
    #define MAX_ENTITY_RESOLVING_DEPTH         3U                     /*!< Maximum depth of resolving of the entity */
    #define MAX_FOUND_REFERENCE                4U                     /*!< Maximum allowed found references during
                                                                           entity resolving */
#elif(CONFIGURATION_TYPE == MINIMUM_SIZE)
    #define ENABLE_ENCODING                    STD_OFF                /*!< enable different encoding type rather than
                                                                            the UTF-8 */
    #define SUPPORT_ASCII_ONLY                 STD_ON                 /*!< Enable/Disable the support to XML with ASCII
                                                                           encoding format only */
    #define ENABLE_ENTITY_SUPPORT              STD_OFF                /*!< Enable/Disable the support to XML entity */
    #define ENABLE_EOL_NORMALIZATION           STD_OFF                /*!< Enable end of line normalization by Normalizing
                                                                             all line breaks before parsing, by translating
                                                                             both the two-character sequence #xD #xA and
                                                                             any #xD that is not followed by #xA to a
                                                                             single #xA character*/
    #define ENABLE_ATTRIBUTE_Normalization     STD_OFF                /*!< Enable/Disable the support for attribute
                                                                           normalization */
    #define SUPPORT_ERROR_STRING               STD_OFF                /*!< Enable/Disable the support for error string */
    #define ENABLE_DTD_RECEIPTION              STD_OFF                /*!< Enable the LwXML to  receive DTD */
    #if(ENABLE_DTD_RECEIPTION == STD_ON)
        #define ENABLE_DTD_WELL_FORM_CHECK     STD_ON                 /*!<Option to Enable/Disable well form for the DTD */
    #else
        #define ENABLE_DTD_WELL_FORM_CHECK     STD_OFF                /*!< Must disable well form checking for the DTD */
    #endif
    #define ENABLE_PI_WELL_FORM_CHECK          STD_OFF                /*!< Enable/Disable well form for the PI */
    #define ENABLE_COMMENT_WELL_FORM_CHECK     STD_OFF                /*!< Enable/Disable well form for the COMMENT */
    #define ENABLE_DECL_WELL_FORM_CHECK        STD_OFF                /*!< Enable/Disable well form for the DECLARATION */
    #define ENABLE_CDATA_WELL_FORM_CHECK       STD_OFF                /*!< Enable/Disable well form for the CDATA */
    #define MAX_XML_PATH_LENGTH                100U                   /*!< The Maximum path length of the XML element */
    #define MAX_WORKING_BUFFER_SIZE            100U                   /*!< Maximum number of characters in the working
                                                                           buffer */
    #define MAX_CIRCULAR_BUFFER_SIZE           100U                   /*!< Maximum number of characters in the circular
                                                                           buffer */
    #define TOTAL_STATIC_XML_INSTANCES         1U                     /*!< Total number of the allocated statically instances
                                                                          which should be set by the user, and it shouldn't
                                                                          be larger than excpeted to avoid unwanted memory
                                                                          waste*/
    #define MAX_ENTITY_VALUE_LENGTH            120U                   /*!< The maximum length of the entity value string,
                                                                            it shouldn't exceed 255 */
    #define MAX_ATTRIBUTES_LIST                16U                    /*!< Maximum number of attribute list declaration */
    #define MAX_OPERATORS_COUNT                10U                    /*!< Maximum operator counts inside the element
                                                                           content (children)*/
    #define MAX_ENTITY_NAME_LENGTH             30U                    /*!< The maximum length of the entity Name string
                                                                            including the Ampersand and the Semicolon,
                                                                            it shouldn't exceed 255*/
    #define MAX_ENTITIES                       102U                   /*!< The maximum number of entities, shouldn't
                                                                           exceed 255*/
    #define MAX_NUMBER_OF_ATTRIBUTES           8                      /*!< Maximum number of the supported attributes
                                                                           in the XML TAG*/

    #define MAX_ATTRIBUTES_NAME_LENGTH         30U                    /*!< Maximum number of characters per attribute
                                                                           name */
    #define MAX_ATTRIBUTES_VALUE_LENGTH        100U                   /*!< Maximum number of characters per attribute
                                                                           value */
    #define MAX_ELEMENT_NAME_LENGTH            14U                    /*!< Maximum number of characters per element name */
    #define MAX_ENTITY_RESOLVING_DEPTH         3U                     /*!< Maximum depth of resolving of the entity */
    #define MAX_FOUND_REFERENCE                4U                     /*!< Maximum allowed found references during
                                                                           entity resolving */
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

/*
********************************************************************************************************************
*                                                     MODULE END
********************************************************************************************************************
*/
#endif /* end #ifndef  LwXMLP_FCFG_H */


