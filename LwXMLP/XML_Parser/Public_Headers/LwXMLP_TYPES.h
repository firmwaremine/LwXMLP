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
 * @file LwXMLP_TYPES.h
 * @brief This is the header file contain the global data types
 * @author Mohammed Fawzy
 * @version 1.0.0
 **********************************************************************************************************************/
#ifndef  __LwXMLP_TYPES_H
#define  __LwXMLP_TYPES_H

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
/*! \enum LwXMLP_enumErrorType
    \brief the errors types that may occur during the parsing
*/
typedef enum
{
    csNoError  = 0,                                  /*!< No error is detected */
    csMissingEqual = 1,                             /*!< The "=" is missing between the attribute name and the attribute
                                                     value */
    csQuotesMissmatch = 2,                          /*!< Quotes miss match*/
    csMissingQuote = 3,                             /*!< missing quote */
    csInvalidXMLCharacter = 4,                      /*!< Invalid XML Character */
    csInvalidStartNameCharacter = 5,                /*!< Invalid Start Name Character */
    csInvalidNameCharacter = 6,                     /*!< Invalid Name Character */
    csCallBackErrorOnPIDirective = 7,               /*!< User Callback Error on the Process Instruction */
    csCallBackErrorOnStartElement = 8,              /*!< User Callback Error on the start element */
    csCallBackErrorOnData = 9,                      /*!< User Callback Error on the text node */
    csCallBackErrorOnEndElement = 10,                /*!< User Callback Error on the End element */
    csCallBackErrorOnComment = 11,                   /*!< User Callback Error on the comment */
    csUnexpectedElementType = 12,                    /*!< Unexpected element Type */
    csFailedToGetElementInfo = 13,                   /*!< Failed to Extract element info */
    csFailedToAddElementToPath = 14,                 /*!< Failed to add new element to the Path buffer */
    csFailedToExtractAttributes = 15,                /*!< Failed to Extract the attributes */
    csWrongNesting = 16,                             /*!< Wrong nesting */
    csLargeElementLength = 17,                       /*!< Element length is larger than available buffer */
    csLargeElementNameProperty = 18,                 /*!< Can't load element name, increase MAX_ELEMENT_NAME_LENGTH */
    csLargeNumberOfAttributeList = 19,               /*!< Can't load Attribute list declarations, increase MAX_ATTRIBUTES_LIST */
    csWrongAttributeFormat = 20,                     /*!< Missing attribute value */
    csWrongDeclarationLocation = 21,                 /*!< Wrong Deceleration Location */
    csUnsupportedEncodingScheme = 22,                /*!< Unsupported encoding scheme */
    csUnExpectedDirectiveType = 23,                  /*!< Received unexpected directive type */
    csDoubleHyphenInComment = 24,                    /*!< Double Hyphen inside the comment */
    csWrongDirectiveEnd = 25,                        /*!< Can't find the end of the directive */
    csMissingAttributeValue = 26,                    /*!< Attribute value is missing */
    csEncodingError = 27,                            /*!< Problem occurred during encoding XML portion */
    csMissingSemicolon = 28,                         /*!< Missing semi-colon from entity reference */
    csExtraContentAtTheEnd = 29,                     /*!< Extra content at the end of the document  */
    csRepeatedAttributeName = 30,                    /*!< Repeated attribute Name */
    csLargeNumberOfAttributes = 31,                  /*!< Can't Process all received attributes, increase MAX_NUMBER_OF_ATTRIBUTES */
    csLargeAttributeNameProperty = 32,               /*!< Can't load attribute name, increase MAX_ATTRIBUTES_NAME_LENGTH */
    csLargeDatalength = 33,                          /*!< Load the data into working buffer, increase MAX_CIRCULAR_BUFFER_SIZE & MAX_WORKING_BUFFER_SIZE */
    csLargeDTDPortionlength = 34,                    /*!< Can't load the DTD portion into working buffer, increase MAX_CIRCULAR_BUFFER_SIZE & MAX_WORKING_BUFFER_SIZE */
    csLargeDirectiveProperty = 35,                   /*!< Can't load the Directive into working buffer, increase MAX_CIRCULAR_BUFFER_SIZE & MAX_WORKING_BUFFER_SIZE */
    csLargeElementProperty = 36,                     /*!< Can't load the element into working buffer, increase MAX_CIRCULAR_BUFFER_SIZE & MAX_WORKING_BUFFER_SIZE */
    csLargeResolvingTrialsProperty = 37,             /*!< More entities need to be resolved, increase MAX_ENTITY_RESOLVING_DEPTH */
     csLargeChildrenOperatorsProperty = 38,          /*!< Can't process all received attributes, increase MAX_OPERATORS_COUNT */
    csLargeEntityCountProperty = 39,                 /*!< Can't load entity, increase MAX_ENTITIES */
    csLargeEntityNameProperty = 40,                  /*!< Can't load entity name, increase MAX_ENTITY_NAME_LENGTH */
    csLargeEntityValueProperty = 41,                 /*!< Can't load entity name, increase MAX_ENTITY_VALUE_LENGTH */
    csDataWithoutStartTag = 42,                      /*!< Found data without being included in a start TAG */
    csInvalidReference = 43,                         /*!< Invalid Character Entity */
    csInvalidVersionOrder = 44,                      /*!< Invalid Version Attribute Order */
    csInvalidSDeclOrder = 45,                        /*!< Invalid SDecl Order */
    csInvalidPIName = 46,                            /*!< Invalid PI name */
    csMissingWhiteSpaceCharacter = 47,               /*!< Missing WhiteSpace */
    csIllegalWhiteSpace = 48,                        /*!< Illegal WhiteSpace */
    csDTDMissingSystemIDError = 49,                  /*!< DTD is missing System ID */
    csDTDMissingPortionError = 50,                   /*!< DTD is missing portion */
    csIncompleteEntityContent = 51,                  /*!< Incomplete entity content */
    csDTDInvalidAttributeType = 52,                  /*!< DTD Invalid Attribute Type */
    csDTDInvalidDefaultDeclaration = 53,             /*!< DTD Invalid Attribute Default Declaration */
    csDTDCapitalPortionError = 54,                   /*!< DTD small letter portion is used*/
    csParsedEntityError = 55,                        /*!< Parsed entity isn't well-formed */
    csMissingPITarget = 56,                          /*!< Missing PI Target */
    csInvalidParameterEntity = 57,                   /*!< Invalid parameter entity */
    csInvalidRootToken = 58,                         /*!< Invalid Token at the root document */
    csMissingEndOfEntity = 59,                       /*!< Missing end of entity */
    csGeneralEntityInDTD = 60,                       /*!< Found General entity in DTD */
    csInvalidAttributeValue = 61,                    /*!< Invalid Attribute Value */
    csInvalidOperator = 62,                          /*!< Invalid Operator */
    csMissingSeparator = 63,                         /*!< Missing separator in choice */
    csPEReferencesinInInternalSubset = 64,           /*!< PEReferences forbidden in internal subset */
    csPartialCharacterReference = 65,                /*!< Found Partial Character Reference*/
    csInvalidDirectReference = 66,                   /*!< Direct reference to an entity with &lt; */
    csEntityRefereToItself = 67,                     /*!< It isn't allowed to let the entity to refer to itself */
    csAttributeInEndTag = 68,                        /*!< Attribute in End TAG isn't allowed */
    csFeatureNotSupported = 69,                      /*!< Feature isn't supported by the parser */
    csInternalError = 70,                            /*!< Error detected in the internal buffer during parsing */
    csInvalidPassingParameter = 71,                  /*!< The parser received invalid parser instance */
    csEncodingDiscrepancy = 72,                      /*!< The parser detected Discrepancy between BOM and encoding type
                                                          in deceleration */
    csMaximumXMLError = 73                           /*!< Maximum available number of errors */
}LwXMLP_enumErrorType;
/*! \enum LwXMLP_enumXMLEventTypes
    \brief The events that shall be fired up on detecting one of the XML document structure like start, end, text node,
    comment, processing instruction, and Notation
*/
typedef enum
{
    csUnkownEvent,                              /*!< Event isn't known */
    csStartElementEvent,                        /*!< Event that should be fired up on detecting start element*/
    csEndElementEvent,                          /*!< Event that should be fired up on detecting End element*/
    csProcessInstructionEvent,                  /*!< Event that should be fired up on detecting Processing Instruction*/
    csTextNodeEvent,                            /*!< Event that should be fired up on detecting text node*/
    csCommentEvent,                             /*!< Event that should be fired up on detecting comment*/
    csCDataEvent,                               /*!< Event that should be fired up on detecting CData*/
    csNotationEvent                             /*!< Event that should be fired up on detecting Notation */
}LwXMLP_enumXMLEventTypes;
/*! \enum LwXMLP_enumParserStatusType
    \brief the LwXMLp parser return status
*/
typedef enum
{
    csUnkownParserStatus,                       /*!< Couldn't detect XML structure */
    csContinueAddingXMLData,                    /*!< Allow the application to add more XML portions to be parsed*/
    csContinueParsingXMLData,                   /*!< Allow the application to invoke the parser engine */
    csParserError,                              /*!< Error is encountered during the parsing process*/
    csParsingFinished                           /*!< The parser detected the end of the XML document*/
}LwXMLP_enumParserStatusType;
/*! brief LwXMLP_strXMLParseInstanceType
    the XML parser instance which shall be created/allocated when the parser is started and it will be used
           to post the XML document result
*/
typedef struct LwXMLP_strXMLParseInstanceType LwXMLP_strXMLParseInstanceType;
/** \brief LwXLMP_psrFuncCallBackType
 * \param pstrXMLParseInstanceType The only parameter which is the XML parser instance
 *
 * Callback function that should be assigned by the upper layer and it will be invoked by the LwXMLP up on detecting
 * XML structure
 */
typedef StdReturnType(*LwXLMP_psrFuncCallBackType)(LwXMLP_strXMLParseInstanceType *pstrXMLParseInstanceType);



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
#endif /* end #ifndef  LwXMLP_TYPES_H */


