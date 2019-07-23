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
 * @file LwXMLP_PRIVATE_TYPES.h
 * @brief This header file contain all the private types definitions for the LwXMLP_CORE which will be converted to
 *        global if the allocation type for the instance is Set to be EXTERNAL_ALLOCATION
 * @author Mohammed Fawzy
 * @version 1.0.0
 **********************************************************************************************************************/


#ifndef  LwXMLP_PRIVATE_TYPES_H
#define  LwXMLP_PRIVATE_TYPES_H


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
#ifndef  LwXMLP_FCFG_H
    #include "LwXMLP_FCFG.h"
#endif
#ifndef  LwXMLP_ENCODING_H
    #include "LwXMLP_ENCODING.h"
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
 *                                                   VERSION NUMBER
 ==================================================================================================================
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
/*! \enum LwXMLP_enumReferenceTypeType
    \brief the types of entities
*/
typedef enum
{
    csUnkownEntity,                             /*!< Unknown entity type */
    csEntityReference,                          /*!< Entity reference */
    csCharacterEntityReference,                 /*!< Character entity reference */
    csParamterEntity,                           /*!< Parameter reference */
    csCharacterReference,                       /*!< Character reference */
    csDecimalCharcterReference,                 /*!< Character reference in decimal format*/
    csHexCharcterReference,                     /*!< Character reference in HEX format*/
    csPredefinedCharacterEntity,                /*!< Predefined Character reference*/
    csMixedContent                              /*!< Mixed Content*/
}LwXMLP_enumReferenceTypeType;
/*! \enum LwXMLP_enumEntityStoreTypeType
    \brief the types of entities
*/
typedef enum
{
    csInternalStorage,                          /*!< Internal entity type*/
    csExternalStorage                           /*!< External entity type*/
}LwXMLP_enumEntityStoreTypeType;
/*! \enum enumMarkupTypeType
    \brief the types of XML Mark-up
*/
typedef enum
{
    csCommentMarkup,                            /*!< Comment mark-up */
    csProcessingInstructionMarkup,              /*!< Processing instruction mark-up */
    csDeclarationMarkup,                        /*!< Declaration mark-up */
    csCDATAMarkup,                              /*!< CDATA mark-up */
    csDTDMarkup                                 /*!< DTD mark-up*/
#if(ENABLE_DTD_RECEIPTION == STD_ON)
    ,csDTDElementMarkup,                        /*!< DTD <!ELEMENT mark-up*/
    csDTDEntitytMarkup,                         /*!< DTD <!ENTITY mark-up*/
    csDTDNotationtMarkup,                       /*!< DTD <!NOTATION mark-up*/
    csDTDAttlistMarkup,                         /*!< DTD <!ATTLIST mark-up*/
    csInternalDTDMarkup,                        /*!< Internal DTD mark-up*/
    csExternalDTDMarkup                         /*!< External DTD mark-up*/
#endif
}enumMarkupTypeType;
/*! \enum enumXMLCheckStateType
    \brief The external state of the parser, such that the first state is checking the BOM and the next state is
           checking the internal XML file based of the BOM type
*/
typedef enum
{
    csBOMChecking,                              /*!< BOM checking state, which is the first state in the parsing */
    csInternalDocumentChecking                  /*!< Internal XML document checking state, which is the second */
}enumXMLCheckStateType;
/*! \enum enumTagTypeType
    \brief indication for the TAG type which may be start, end or empty TAG
*/
typedef enum
{
    csUnkownTagType,                            /*!< Unknown TAG type */
    csStartTagType,                             /*!< Start TAG type */
    csEndTagType,                               /*!< End TAG type */
    csEmptyTagType                              /*!< Empty TAG type */
}enumTagTypeType;
/*! \enum enumAtrributeExtractState
    \brief the states of attributes extraction which start with extracting the attribute name and end with attribute
           value
*/
typedef enum
{
    csGetAttributeName,                         /*!< extracting attribute name state  */
    csGetAttributeValue                         /*!< extracting attribute value state */
}enumAtrributeExtractState;
/*! \enum enumPISearchStateType
    \brief the states of Processing instruction (PI) extraction
*/
typedef enum
{
    csCheckEndOfPITarget,                       /*!< Looking for the end of PI state, which is the first state  */
    csLookForData,                              /*!< Looking for the data of PI state, which is the second state  */
    csLoadData                                  /*!< Load the PI data, which is the third state  */
}enumPISearchStateType;
/*! \enum enumAttibuteTypeType
    \brief the types of attribute
*/
typedef enum
{
    csUnkownType,                               /*!< Unknown attribute type */
    csCDATAType,                                /*!< CDATA attribute type */
    csNMTOKENType,                              /*!< NMTOKEN attribute type */
    csNMTOKENSType,                             /*!< NMTOKENS attribute type */
    csIDType,                                   /*!< ID attribute type */
    csIFREFType,                                /*!< IDREF attribute type */
    csIFREFSType,                               /*!< IDREFS attribute type */
    csENTITYType,                               /*!< ENTITY attribute type */
    csENTITIESType,                             /*!< ENTITIES attribute type */
    csEnumertaedValueListType,                  /*!< Enumerated value attribute type */
    csNOTATIONType                              /*!< NOTATION attribute type */
}enumAttibuteTypeType;
/*! \enum enumDefaultDeclarationTypeType
    \brief the types of default declaration
*/
typedef enum
{
    csImplied,                                  /*!< IMPLIED attribute Declaration */
    csRequired,                                 /*!< REQUIRED attribute Declaration */
    csFixed                                     /*!< FIXED attribute Declaration */
}enumDefaultDeclarationTypeType;
/*! \struct strEntityType
    \brief it the structure which hold the entities
    @var strEntityType::bParsedEntity
    Member 'bParsedEntity' it is STD_TRUE if the entity is parsed entity and STD_FALSE otherwise
    @var strEntityType::u8EntityValue
    Member 'u8EntityValue' it is an array contain the entity value
    @var strEntityType::u8EntityName
    Member 'u8EntityName' it is an array contain the entity name
    @var strEntityType::u8EntityNameLength
    Member 'u8EntityNameLength' it contains the length of the entity name string
    @var strEntityType::u8EntityValueLength
    Member 'u8EntityValueLength'it contains the length of the entity vlaue string
    @var strEntityType::objenumReferenceTypeType
    Member 'objenumReferenceTypeType' it is an enum to indicate the entity type
    @var strEntityType::objenumEntityStoreTypeType
    Member 'objenumEntityStoreTypeType' it is an enum contain the storage location of the entity internal/external
*/
typedef struct
{
    BOOLEAN                        bParsedEntity;
    INT8U                          u8EntityValue[MAX_ENTITY_VALUE_LENGTH];
    INT8U                          u8EntityName[MAX_ENTITY_NAME_LENGTH];
    INT8U                          u8EntityNameLength;
    INT8U                          u8EntityValueLength;
    LwXMLP_enumReferenceTypeType   objenumReferenceTypeType;
    LwXMLP_enumEntityStoreTypeType objenumEntityStoreTypeType;
}strEntityType;
/*! \struct LwXML_strFoundReferenceType
    \brief it the structure which hold the found entity index and the corresponding type
    @var LwXML_strFoundReferenceType::u8EntityIndex
    Member 'u8EntityIndex' it hold the found entity index in the global array of data structure of entity list
    @var LwXML_strFoundReferenceType::objenumReferenceTypeType
    Member 'objenumReferenceTypeType' it hold the type of the found entity
*/
typedef struct
{
    INT8U                        u8EntityIndex;
    LwXMLP_enumReferenceTypeType objenumReferenceTypeType;
}LwXML_strFoundReferenceType;
/*! \struct strAttributeListType
    \brief it the structure which hold the attributes name/behavior per element
    @var strAttributeListType::u8AttributeName
    Member 'u8AttributeName' array contain the name of the attribute
    @var strAttributeListType::u8ElementName
    Member 'u8ElementName' array contain the name of the element
    @var strAttributeListType::u8AttributeValue
    Member 'u8AttributeValue' array contain the value of the attibute
    @var strAttributeListType::objenumAttibuteTypeType
    Member 'objenumAttibuteTypeType' enum indicate the type of the attribute
    @var strAttributeListType::objenumAttributeBehaviourType
    Member 'objenumEncodingTypeType' enum to indicate the default declaration of the attribute
*/
typedef struct
{
    INT8U                      u8AttributeName[MAX_ATTRIBUTES_NAME_LENGTH];
    INT8U                      u8ElementName[MAX_ELEMENT_NAME_LENGTH];
    INT8U                      u8AttributeValue[MAX_ATTRIBUTES_VALUE_LENGTH];
    enumAttibuteTypeType       objenumAttibuteTypeType;
    enumDefaultDeclarationTypeType objenumAttributeBehaviourType;
}strAttributeListType;
/*! \struct strBOMDescriptorType
    \brief it the structure which hold the description of different encoding scheme
    @var strBOMDescriptorType::pu8EncodingBOM
    Member 'pu8EncodingBOM' Pointer to the encoding BOM array
    @var strBOMDescriptorType::u8EncodingBOMLength
    Member 'u8EncodingBOMLength' number of bytes that shall be used by the BOM
    @var strBOMDescriptorType::objenumEncodingTypeType
    Member 'objenumEncodingTypeType' type of the encoding BOM which may be UTF-8, UTF-16, UTF-16BE, UTF-32LE and 
    UTF-32BE
*/
typedef struct
{
    const INT8U                       *pu8EncodingBOM;
          INT8U                        u8EncodingBOMLength;
          LwXMLP_enumEncodingTypeType  objenumEncodingTypeType;
}strBOMDescriptorType;
/*! \struct LwXMLP_strXMLParseInstanceType
    \brief the XML parser instance which shall be created/allocated when the parser is started and it will be used
           to post the XML document result

    @var LwXMLP_strXMLParseInstanceType::bTagContainDirective
    Member 'bTagContainDirective' boolean indicating if the current portion of the XML which is a TAG is containing a
    a directive or not like Comment mark-up or Processing instruction mark-up or Declaration mark-upor CDATA mark-up or
    DTD mark-up
    @var LwXMLP_strXMLParseInstanceType::bEndOfDirectiveFound
    Member 'bEndOfDirectiveFound' boolean indicating if the current portion of the XML which is a directive is fully
    loaded into the working buffer or not
    @var LwXMLP_strXMLParseInstanceType::bEndOfDataIsFound
    Member 'bEndOfDataIsFound' boolean indicating if the current portion of the XML which is a text data is fully
    loaded into the working buffer or not
    @var LwXMLP_strXMLParseInstanceType::bTagCloserIsFound
    Member 'bTagCloserIsFound' boolean indicating if the current portion of the XML which is a TAG is fully
    loaded into the working buffer or not
    @var LwXMLP_strXMLParseInstanceType::pu8LastParserLocation
    Member 'pu8LastParserLocation' Pointer to the last parsed location in the circular buffer
    @var LwXMLP_strXMLParseInstanceType::pu8CircularBufferEnd
    Member 'pu8CircularBufferEnd' Pointer to the end of the circular buffer
    @var LwXMLP_strXMLParseInstanceType::pu8CircularBufferHead
    Member 'pu8CircularBufferEnd' Pointer to the start of the circular buffer
    @var LwXMLP_strXMLParseInstanceType::u8XMLPathBuffer
    Member 'pu8CircularBufferEnd' XML Path of the current element
    @var LwXMLP_strXMLParseInstanceType::pu8AttributeNameArray
    Member 'pu8AttributeNameArray' Array of pointer to the available attributes names string
    @var LwXMLP_strXMLParseInstanceType::pu8AttributeValueArray
    Member 'pu8AttributeValueArray' Array of pointer to the available attributes values string
    @var LwXMLP_strXMLParseInstanceType::pu8ParsedDataBuffer
    Member 'pu8ParsedDataBuffer' pointer to the current parsed text data string
    @var LwXMLP_strXMLParseInstanceType::pu8ParsedPITarget
    Member 'pu8ParsedPITarget' pointer to the current parsed processing instruction target string
    @var LwXMLP_strXMLParseInstanceType::pu8ParsedPIContent
    Member 'pu8ParsedPIContent' pointer to the current parsed processing instruction Data string
    @var LwXMLP_strXMLParseInstanceType::pu8ParsedComments
    Member 'pu8ParsedComments' pointer to the current comment string
    @var LwXMLP_strXMLParseInstanceType::u8XMLCircularBuffer
    Member 'u8XMLCircularBuffer' array of bytes which contains an XML portion to be parsed by the LwXMLP
    @var LwXMLP_strXMLParseInstanceType::u8XMLworkingBuffer
    Member 'u8XMLworkingBuffer' array of bytes which contains an XML structure piece, such that the working buffer
    shall be greater than any XML structure like the XML text or XML TAG
    @var LwXMLP_strXMLParseInstanceType::s16AttributeCounts
    Member 's16AttributeCounts' number of the current available attributes
    @var LwXMLP_strXMLParseInstanceType::u32FreeLocations
    Member 'u32FreeLocations' the number of free bytes in the circular buffer
    @var LwXMLP_strXMLParseInstanceType::u32PathLength
    Member 'u32PathLength' the number of bytes used by the current XML path
    @var LwXMLP_strXMLParseInstanceType::u32LineNumber
    Member 'u32LineNumber' the line number of the XML document under processing which will be used to get the error
    location if an error is encountered
    @var LwXMLP_strXMLParseInstanceType::u32CoulmnNumber
    Member 'u32CoulmnNumber' the coulmn number of the XML document under processing which will be used to get the error
    location if an error is encountered
    @var LwXMLP_strXMLParseInstanceType::u32CurrentTagLength
    Member 'u32CurrentTagLength'the string length of the current TAG contains all the attributes if any
    @var LwXMLP_strXMLParseInstanceType::objenumTagTypeType
    Member 'objenumTagTypeType' the current TAG type which may be start, end or Empty TAG
    @var LwXMLP_strXMLParseInstanceType::objpsrFuncCallBackType
    Member 'objpsrFuncCallBackType' pointer to the call back function which shall be assigned by the upper layer
    @var LwXMLP_strXMLParseInstanceType::objenumErrorType
    Member 'objenumErrorType' the current error number if an error is encountered
    @var LwXMLP_strXMLParseInstanceType::objenumXMLCheckStateType
    Member 'objenumXMLCheckStateType' the current state of the parser which may be BOM checking or actual parsing for
    the document
    @var LwXMLP_strXMLParseInstanceType::objenumXMLEventTypes
    Member 'objenumXMLEventTypes' the event type that shall be fired up on detecting XML structure which may be
    start, end, PI, text node or comment
    @var LwXMLP_strXMLParseInstanceType::objenumDirectiveTypeType
    Member 'objenumDirectiveTypeType' The type of the current detected mark up which may be comment, PI, deceleration,
    CDATA and DTD mark up
    @var LwXMLP_strXMLParseInstanceType::objenumEncodingTypeType
    Member 'objenumEncodingTypeType' The detected encoding type of the current XML document
    @var LwXMLP_strXMLParseInstanceType::pu8StartOfName
    Member 'pu8StartOfName' General variable will be used in a temp way instead of using auto variable which shall be
    passed to some function, because passing an auto variable to function will violate the MISRA rules
    @var LwXMLP_strXMLParseInstanceType::u8OperatorStack
    Member 'u8OperatorStack' General variable will be used in a temp way instead of using auto variable which shall be
    passed to some function, because passing an auto variable to function will violate the MISRA rules
    @var LwXMLP_strXMLParseInstanceType::u32Length
    Member 'u32Length' General variable will be used in a temp way instead of using auto variable which shall be
    passed to some function, because passing an auto variable to function will violate the MISRA rules
    @var LwXMLP_strXMLParseInstanceType::u32NewLength
    Member 'u32NewLength' General variable will be used in a temp way instead of using auto variable which shall be
    passed to some function, because passing an auto variable to function will violate the MISRA rules
    @var LwXMLP_strXMLParseInstanceType::u8SecondLevelPathBuffer
    Member 'u8SecondLevelPathBuffer' it is expression to indicate the location of the nodes in an XML document when
    event is fired during parsing XML portion inside the XML especially when the entity is XML element.
    These path expressions look very much like the expressions you see when you work with a traditional computer
    file system.
    @var LwXMLP_strXMLParseInstanceType::u8SecondLevelBuffer
    Member 'u8SecondLevelBuffer' it is an array contain part of XML file to be pasred in another domain rathar than the
    main XML file
    @var LwXMLP_strXMLParseInstanceType::u32SecondLevelPathLength
    Member 'u32SecondLevelPathLength' it is present the string length of the second level path
    @var LwXMLP_strXMLParseInstanceType::u32LastReferenceLine
    Member 'u32LastReferenceLine' it holds the last xml file line which contain a reference
    @var LwXMLP_strXMLParseInstanceType::objstrEntityType
    Member 'objstrEntityType' it is array of data structure which contain all the found entity references in DTD
    @var LwXMLP_strXMLParseInstanceType::objstrFoundReferenceType
    Member 'objstrFoundReferenceType' it is array of data structure which contain the found entity information which
    resolving the entity in the attribute value or in the text of the element
    @var LwXMLP_strXMLParseInstanceType::u32SecondLevelReadyAmount
    Member 'u32SecondLevelReadyAmount' it contains the length of XML portion which is available in the
    u8SecondLevelPathBuffer
    @var LwXMLP_strXMLParseInstanceType::bParserSilentModeIsEnabled
    Member 'bParserSilentModeIsEnabled' it is STD_TRUE, it will force the parser to parse the XML without firing any
    events, just to valdiate the XML file for well-formedness
    @var LwXMLP_strXMLParseInstanceType::u8XMLEncodingBuffer
    Member 'u8XMLEncodingBuffer' it is buffer which contain the encoded XML portion in UTF-8, if the transcoding between
    the othe encoding like UTF16 to UTF8 is allowed by ENABLE_ENCODING
    @var LwXMLP_strXMLParseInstanceType::bBOMIsChecked
    Member 'bBOMIsChecked' it is a flag to indicate if the BOM is check in the current XML is checked or not, if it was
    checked, it is value will be STD_TRUE
    @var LwXMLP_strXMLParseInstanceType::bReachXMLEnd
    Member 'bReachXMLEnd' it is a flag to indicate if the parser reach the last node in the XML files, if the end is
    reach, the value will be STD_TRUE
    @var LwXMLP_strXMLParseInstanceType::pu8ElementName
    Member 'pu8ElementName' it is a pointer to the current parsed element name
    @var LwXMLP_strXMLParseInstanceType::u8AvaliableEntities
    Member 'u8AvaliableEntities' it contains the avaliable number of entities that found while parsing the DTD
    @var LwXMLP_strXMLParseInstanceType::u32ParsedLength
    Member 'u32ParsedLength' it contains the length in bytes for parsed XML file
    @var LwXMLP_strXMLParseInstanceType::u32DirectiveLength
    Member 'u32DirectiveLength' it contains the string length of the current directive under parsing
    @var LwXMLP_strXMLParseInstanceType::objstrAttributeListType
    Member 'objstrAttributeListType' it array of data structure contain the parsed attribute information gotten from
    the DTD
    @var LwXMLP_strXMLParseInstanceType::u8AvaliableAttibuteListCount
    Member 'u8AvaliableAttibuteListCount' it reflect the available number of attribute gotten from the DTD
    @var LwXMLP_strXMLParseInstanceType::bFoundExternalEntity
    Member 'bFoundExternalEntity' if it was STD_TRUE, it means that the parser is detected the use of external entity
    in the DTD
    @var LwXMLP_strXMLParseInstanceType::bParsingDTDInProgress
    Member 'bParsingDTDInProgress'
    @var LwXMLP_strXMLParseInstanceType::bStandAloneDocument
    Member 'bStandAloneDocument' if it set to STD_TRUE, it means that the parser detected the stand alone type of the
    XML docuemnt
    @var LwXMLP_strXMLParseInstanceType::u8FoundReference
    Member 'u8FoundReference' It reflects the number of resolving level that made inside the attribute value/element text
    @var LwXMLP_strXMLParseInstanceType::bSecondLevelBufferEnabled
    Member 'bSecondLevelBufferEnabled' it reflect that second level buffer that used to parse the entity is in use
    @var LwXMLP_strXMLParseInstanceType::pu8ParsedNotaion
    Member 'pu8ParsedNotaion' it points to the buffer which hold the parsed notation
    @var LwXMLP_strXMLParseInstanceType::pu8SavedLastParserLocation
    Member 'pu8SavedLastParserLocation' it used to save the current location where parser will stop to jump to the next
    level buffer, so when the second level parsing is complete, the parser will pop the save location
    @var LwXMLP_strXMLParseInstanceType::pu8SavedCircularBufferEnd
    Member 'pu8SavedCircularBufferEnd' it used to save the end of the main buffer to be retrieved when the parser go back
    from the second level parsing
*/
struct LwXMLP_strXMLParseInstanceType
{
    /*
     * DTD fields
     */
    strAttributeListType          objstrAttributeListType[MAX_ATTRIBUTES_LIST];
    INT8U                         u8AvaliableAttibuteListCount;
    BOOLEAN                       bFoundExternalEntity;
#if(ENABLE_DTD_RECEIPTION == STD_ON)
    BOOLEAN                       bParsingDTDInProgress;
#endif
    BOOLEAN                       bStandAloneDocument;
    INT8U                        *pu8ParsedNotaion;
#if(ENABLE_ENTITY_SUPPORT == STD_ON)
    INT8U                         u8FoundReference;
    BOOLEAN                       bSecondLevelBufferEnabled;
    INT8U                        *pu8SavedLastParserLocation;
    INT8U                        *pu8SavedCircularBufferEnd;
    INT8U                         u8SecondLevelBuffer[MAX_CIRCULAR_BUFFER_SIZE];
    INT8U                         u8SecondLevelPathBuffer[MAX_XML_PATH_LENGTH];
    INT32U                        u32SecondLevelPathLength;
    INT32U                        u32LastReferenceLine;
    strEntityType                 objstrEntityType[MAX_ENTITIES];
    LwXML_strFoundReferenceType   objstrFoundReferenceType[MAX_FOUND_REFERENCE];
    INT32U                        u32SecondLevelReadyAmount;
    BOOLEAN                       bParserSilentModeIsEnabled;
#endif
    /*
     * XML Buffer
     */

    INT8U                        *pu8LastParserLocation;
    INT8U                        *pu8CircularBufferEnd;
    INT8U                        *pu8CircularBufferHead;
    INT32U                        u32FreeLocations;
    INT8U                         u8XMLCircularBuffer[MAX_CIRCULAR_BUFFER_SIZE];
#if(ENABLE_ENCODING == STD_ON)
    INT8U                         u8XMLEncodingBuffer[MAX_CIRCULAR_BUFFER_SIZE];
#endif
    /*
     * XML Parsing result
     */
    BOOLEAN                       bBOMIsChecked;
    BOOLEAN                       bTagContainDirective;
    BOOLEAN                       bEndOfDirectiveFound;
    BOOLEAN                       bEndOfDataIsFound;
    BOOLEAN                       bTagCloserIsFound;
    BOOLEAN                       bReachXMLEnd;
    INT8U                         u8XMLPathBuffer[MAX_XML_PATH_LENGTH];
    INT8U                        *pu8AttributeNameArray[MAX_NUMBER_OF_ATTRIBUTES];
    INT8U                        *pu8AttributeValueArray[MAX_NUMBER_OF_ATTRIBUTES];
    INT8U                        *pu8ElementName;
    INT8U                        *pu8ParsedDataBuffer;
    INT8U                        *pu8ParsedPITarget;
    INT8U                        *pu8ParsedPIContent;
    INT8U                        *pu8ParsedComments;
    INT8U                         u8AvaliableEntities;
    INT8U                         u8XMLworkingBuffer[MAX_WORKING_BUFFER_SIZE];
    INT16S                        s16AttributeCounts;
    INT32U                        u32PathLength;
    INT32U                        u32ParsedLength;
    INT32U                        u32LineNumber;
    INT32U                        u32CoulmnNumber;
    INT32U                        u32CurrentTagLength;
    INT32U                        u32DirectiveLength;
    enumTagTypeType               objenumTagTypeType;
    LwXLMP_psrFuncCallBackType    objpsrFuncCallBackType;
    LwXMLP_enumErrorType          objenumErrorType;
    enumXMLCheckStateType         objenumXMLCheckStateType;
    LwXMLP_enumXMLEventTypes      objenumXMLEventTypes;
    enumMarkupTypeType            objenumDirectiveTypeType;
    LwXMLP_enumEncodingTypeType   objenumEncodingTypeType;
    /*
     * Temporary variables to be used as while passing variable to functions not used auto variable to complay
     * with MISRA:2014 1.2
     */
    INT8U                        *pu8StartOfName;
    INT8U                         u8OperatorStack[MAX_OPERATORS_COUNT];
    INT32U                        u32Length;
    INT32U                        u32NewLength;
};


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


#ifdef __cplusplus
}
#endif


/*
********************************************************************************************************************
*                                                     MODULE END
********************************************************************************************************************
*/
#endif /* end #ifndef  LwXMLP_PRIVATE_TYPES_H */



