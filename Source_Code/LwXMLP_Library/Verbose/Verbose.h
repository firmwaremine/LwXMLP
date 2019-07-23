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
 * @file VERBOSE.h
 * @brief This is the header file which define the verbose function and the verbose levels
 * @author Mohammed Fawzy
 * @version 1.0.0
 **********************************************************************************************************************/
#ifndef  __VERBOSE_H
#define  __VERBOSE_H



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
/** lower two bits indicate verbose level
 * - 0 all
 * - 1 warning
 * - 2 serious
 * - 3 severe
 */
#define VERBOSE_LEVEL_OFF               0x00U     /*!< No message information. This level of messaging costs the least
                                                       amount of processing resources. You won't be able to display
                                                       any messages*/
#define VERBOSE_LEVEL_DEBUGGING         0x01U     /*!< Debug information is displayed as well as warnings and critical
                                                       errors */
#define VERBOSE_LEVEL_WARNING           0x02U     /*!< Warnings are displayed as well as critical errors */
#define VERBOSE_LEVEL_SERIOUS           0x03U     /*!< Only critical errors are displayed */

#define VERBOSE_MASK_LEVEL              0x03U    /*!< The location of the level mask bits*/
#define VERBOSE_MESSAGE_ON              0x80U    /*!< flag to enable that message to be displayed*/
#define VERBOSE_MESSAGE_OFF             0x00U    /*!< flag to disable that message to be displayed */
#define VERBOSE_HALT                    0x08U    /*!< flag to halt after displaying this debug message */
#define VERBOSE_TRACE                   0x40U    /*!< flag indicating a tracing message (to follow program flow) */
#define VERBOSE_STATE                   0x20U    /*!< flag indicating a state debug message (to follow module states) */
#define VERBOSE_FRESH                   0x10U    /*!< flag  indicating newly added code, not thoroughly tested yet */
#define NEW_LINE_CARRIAGE_RETRUN        "\n\r"   /*!< two successive characters  newline 0x0A and carriage return 0x0D*/
#define NEW_LINE                        "\n"     /*!< newline 0x0A*/
#define VERBOSE_END_OF_LINE             NEW_LINE_CARRIAGE_RETRUN /*!< Specify the end of line type*/


extern BOOLEAN VERBOSE_bSTarted;                /*!< flag  to indicate that the standrd output had been init or not */


/*!\fn         mVerbose(MIN_LEVEL, REQUIRED_LEVEL,FORMAT, ...)

@brief         This Macro is used to send the log event to the avaliable standard output, the current file name
               and the line number of the event will be added to the event

@param[in]     MIN_LEVEL         : the mimimum allowed level to be sent to the standard output

@param[in]     REQUIRED_LEVEL    : the level of importance of the log event

@param[in]     FORMAT            : the string that describe the evenet

@return

@note
*/
/* Info 778: Constant expression evaluates to 0 in operation '&' */

/* Info 831: Reference cited in prior message, PCLint didn't find anything that actually uses this function,
 *           so it could be dead code/candidate for removal.*/
/* Warning 506:   Constant value Boolean [MISRA 2012 Rule 2.1, required] */
/* Info 774: Boolean within 'if' always evaluates to True [MISRA 2012 Rule 14.3, required]*/
/* Note 9026: Function-like  macro, defined [MISRA 2012 Directive 4.9, advisory]*/
/* Note 9024: '#/##' operator used in macro 'mVerbose' [MISRA 2012 Rule 20.10, advisory] */
/*lint -estring(9024,VERBOSE_printf)*/
/*lint -estring(9026,mVerbose)*/
/*lint -estring(9024,mVerbose)*/
/*lint -estring(9026,mVerboseNoLine)*/
/*lint -estring(9024,mVerboseNoLine)*/
#define mVerbose(MIN_LEVEL, REQUIRED_LEVEL,FORMAT, ...)\
do/*lint -save -e831 -e506 -e774 -e778 */\
    {\
       if(((REQUIRED_LEVEL) & VERBOSE_MESSAGE_ON) != 0U)\
       {\
            if(((REQUIRED_LEVEL) & VERBOSE_MASK_LEVEL) >= MIN_LEVEL) \
            {\
                 if(VERBOSE_bSTarted == STD_TRUE)\
                 {\
                     (VERBOSE_printf("%s:%d: " FORMAT, __FILE__, __LINE__, ##__VA_ARGS__));\
                     (VERBOSE_printf(VERBOSE_END_OF_LINE));\
                 }\
            } \
       }\
    }\
    while(1U == 0U)/*lint -restore */
/*!\fn         mVerboseNoLine(MIN_LEVEL, REQUIRED_LEVEL,FORMAT, ...)

@brief         This Macro is used to send the log event to the avaliable standard output, the current file name
               and the line number of the event won't be added to the event

@param[in]     MIN_LEVEL         : the mimimum allowed level to be sent to the standard output

@param[in]     REQUIRED_LEVEL    : the level of importance of the log event

@param[in]     FORMAT            : the string that describe the evenet

@return

@note
*/
#define mVerboseNoLine(MIN_LEVEL, REQUIRED_LEVEL,FORMAT, ...)\
/*lint -save -e831 -e506 -e774 -e778 */  do\
    {\
       if(((REQUIRED_LEVEL) & VERBOSE_MESSAGE_ON) != 0U)\
       {\
            if(((REQUIRED_LEVEL) & VERBOSE_MASK_LEVEL) >= MIN_LEVEL) \
            {\
                 if(VERBOSE_bSTarted == STD_TRUE)\
                 {\
                     (VERBOSE_printf(FORMAT, ##__VA_ARGS__));\
                 }\
            } \
       }\
    }\
    while(1U == 0U)/*lint -restore */




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
/*!\fn         void VERBOSE_vInit(void)

@brief         invoked by the upper layer to init the standard output
*/
void VERBOSE_vInit(void);
/*!\fn         void VERBOSE_printf(const char *pcString, ...)

@brief         the print function which used to send the log/string to the standard output

@param[in]     pcString         : pointer to the log/string
*/
void VERBOSE_printf(const char *pcString, ...);
#ifdef __cplusplus
}
#endif


/*
********************************************************************************************************************
*                                                     MODULE END
********************************************************************************************************************
*/
#endif



