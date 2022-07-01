The LwXMLP features:
-	It is a non-validating XML processor.
-	It is C language based.
-	It follows all the requirements provided by the W3C (check the LwXMLP test tool and W3C compliance matrix).
-	It targets the embedded system area with many limitations on the code, data and processing speed.
-	It is online streaming processor (incremental processor) such that it can generate correct partial output from partial input and this gives a degree of freedom on the XML document size, i.e. there is no restriction on the XML document size. 
-	It is SAX or event-based processor. Such that it reads an XML document from beginning to end, and each time it encounters a syntax construction, it notifies the application that is running it, and the application must implement the appropriate methods to handle the callbacks and get the functionality needed.
-	It can receive the XML document from different stream type like file stream and IP stream (TCP/UDP), serial communication, so on.
-	The code/data size can be customize to enable/disable some of the feature on the expense of the code/data size using some macro definitions.
-	Thread-safe (more precisely: reentrant).
-	Support two models of memory allocation, static memory allocation and dynamic memory allocation.
-	The LWXML processor shall be able to detect the xml fetal errors which will cause the reporting for the error type and the XML line where the error is occurred then it will cause the termination for reading process. 
-	Supports only for encoding types UTF-8, UTF-16BE and UTF-16LE.
-	It complies with MISRA-2012 (Check Compliance matrix)...

*** Resources ***
- Firmware Mine Website (https://firmwaremine.com/)
- LwXMLP Manual(https://firmwaremine.com/lwxmlp_doxy/)
- LwXMLP API Reference (https://firmwaremine.com/lwxmlp_doxy/)
