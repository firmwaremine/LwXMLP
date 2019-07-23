Example Summary
---------------
Application that uses the WiFi driver and CC3X00 device to echo TCP packets.

Peripherals Exercised
---------------------
Board_LED0        Indicator LED
Board_LED1        AP Connection Indicator LED
Board_WIFI        Wireless driver instance
Board_WIFI_SPI    Interface to communicate with CC3X00
Board_BUTTON1     Used to put CC3X00 in Smart Config mode

Resources & Jumper Settings
---------------------------
Please refer to the development board's specific "Settings and Resources"
section in the Getting Started Guide. For convenience, a short summary is also
shown below.

| Development board | Notes                                                  |
| ================= | ====================================================== |
| CC3200_LAUNCHXL   | This example was designed to use a CC3200 launchpad    |
| DK-TM4C129X       | or a CC3100 evaluation module (CC3100BP).  A wireless  |
| EK-TM4C123GXL     | or access point (AP) is also required for this example.|
| EK-TM4C1294XL     | Note that the CC3X00 uses the 2.4GHz band. If your AP  |
| EK-TM4C129EXL     | is dual band (i.e. supports both 2.4GHz and 5GHz bands)|
| MSP-EXP430F5529LP | you must utilize the 2.4GHz band when running this     |
| MSP-EXP430FR5969  | application.                                           |
| MSP-EXP430FR6989  |                                                        |
| MSP-EXP432P401R   | This example uses SmartConfig technology to connect to |
|                   | a wireless network. A SmartConfig application will be  |
|                   | required to complete the connection process. For mobile|
|                   | devices, the application is called "Texas Instruments  |
|                   | SimpleLink Wi-Fi Starter." Again, the CC3X00 uses the  |
|                   | 2.4GHz band, so make sure the device that's running the|
|                   | SmartConfig app is connected to the AP on the 2.4GHz   |
|                   | band.                                                  |
|                   |                                                        |
|                   | Lastly, if using the CC3100 evaluation module, ensure  |
|                   | that you have powered the CC3100 itself, (in addition  |
|                   | to powering the base board) via its USB port.          |
| ----------------- | ------------------------------------------------------ |

Example Usage
-------------
Run the example. Board_LED0 turns ON to indicate TI-RTOS driver
initialization is complete.  Board_LED1 is used as a connection indicator during
the example.

If connecting the CC3X00 to the AP for the first time, Board_BUTTON1 must be
pressed to start the SmartConfig process.  Use another wireless device
(computer, phone, etc.) connected to the AP to launch a SmartConfig application.

Follow the application instructions to connect the CC3X00 to your AP.  Once
connected, the CC3X00's IP address is printed on the console and Board_LED1 is
ON.  The following message should be displayed:

        CC3X00 has connected to an AP and acquired an IP address.
        IP Address: xxx.xxx.xxx.xxx

Launch the tcpSendReceive Linux or Windows executable shipped with TI-RTOS. The
executable is found in:

  <tirtos_install_dir>\packages\examples\tools\tcpSendReceive

  Usage: ./tcpSendReceive <IP-addr> <port> <id> -l[length] -s[sleep in uS]

  <IP-addr> is the IP address of the CC3X00
  <port>    is the TCP port being listened to (1000)
  <id>      is a unique id for the executable. Printed out when 1000 packets are
            transmitted. It allows the user to run multiple instances
            of tcpSendReceive.

  Optional:
    -l[length]      size of the packet in bytes. Default is 1024 bytes.
                    Must be set to 256 bytes for MSP430 examples.
    -s[sleep in uS] usleep time to between sends. Default is 1000 uSecs.

  Example:
        tcpSendReceive 192.168.1.100 1000 1 -l256

Messages such as the following will begin to appear on the terminal window when
a TCP packet has been echoed back:

        Starting test with a 1000 uSec delay between transmits
        [id 1] count = 1000, time = 12
        [id 1] count = 2000, time = 24
        [id 1] count = 3000, time = 36

Application Design Details
--------------------------
This application uses two tasks:

* Note: The RX and TX payload sizes are configured to 1024 bytes.  This is the
  largest possible amount of data sent from the client application.

  'echoTask' performs the following actions:
      Opens a WIFI driver object.

      Waits until the CC3X00 is connected to the AP.  If connecting to AP
      for the first time, the SmartConfig procedure must be used.  The
      smartConfigFxn() initializes the configuration process on the CC3X00.

      The SimpleLinkWlanEventHandler and SimpleLinkNetAppEventHandler functions
      manage the connection events.  Execution continues once connected to
      the AP and DHCP has returned an IP address.

      A TCP socket is created, bound and listening for connections on port 1000.

      If a client is acquired, the application echoes back data received until
      the remote client closes the socket.

For GNU and IAR users, please read the following website for details about
semi-hosting:
    http://processors.wiki.ti.com/index.php/TI-RTOS_Examples_SemiHosting