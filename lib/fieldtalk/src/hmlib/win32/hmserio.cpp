/**
 * @internal
 * @file win32/hmserio.cpp
 *
 * Serial port I/O for the Windows 32-bit platform
 *
 * @if NOTICE
 *
 * $Id: hmserio.cpp,v 1.18 2012/03/28 05:57:55 henrik Exp $
 *
 * Copyright (c) 2002-2012 proconX Pty Ltd. All rights reserved.
 *
 * THIS IS PROPRIETARY SOFTWARE AND YOU NEED A LICENSE TO USE OR REDISTRIBUTE.
 *
 * THIS SOFTWARE IS PROVIDED BY PROCONX AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PROCONX OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @endif
 */


// Platform header
#include <stdio.h>
#include <stddef.h>
#include <windows.h>

// HM C/C++ library header
#include "hmserio.hpp"
#include "hmtracelog.h"

// Version identifier
static char ver[] = "$Id: hmserio.cpp,v 1.18 2012/03/28 05:57:55 henrik Exp $";


/*****************************************************************************
 * Serial Port I/O implementation
 *****************************************************************************/

/**
 * @defgroup hm_serio_win32 Serial Port I/O for Windows 32-bit platform
 *
 * These functions form a Windows specific implementation of a platform
 * independant interface layer for access to serial port functions.
 */
//@{


/**
 * Creates a SerialPort object and initialises internal data.
 */
HmSerialPort::HmSerialPort()
{
   (void) ver; // Avoid compiler warning

   port = INVALID_HANDLE_VALUE;
#if !defined(_WIN32_WCE)
   memset(&ovlStruct, 0, sizeof(ovlStruct));
#endif
}


/**
 * Disposes a SerialPort object. The port is closed before disposal.
 */
HmSerialPort::~HmSerialPort()
{
   closePort();
}


/**
 * Opens a serial port. The port will be configured with 9600 bps, 8
 * databits, 1 stopbit, no parity, no flow control. After a port has been
 * opened send/receive and control functions can be used.
 *
 * @param portName String with port identifier (This is a
 *                 DOS-style COM port name like COM1 or a device
 *                 name like \\.\COM1 on Windows or
 *                 \$device\COM1 on Windows CE.)
 * @retval SER_SUCCESS No error
 * @retval SER_PORT_NOT_FOUND Port could not be found.
 * @retval SER_ALREADY_OPEN Port has alreeady been opened by somebody else
 * @retval SER_API_ERROR The underlaying API returned an error
 */
int HmSerialPort::openPort(const char * const portName)
{
#if defined(_WIN32_WCE)
   static char COM_DEVICE_NAME[] = "\\$device\\COM"; // Refer to http://msdn.microsoft.com/en-us/library/aa447463.aspx
#else
   static char COM_DEVICE_NAME[] = "\\\\.\\COM"; // Refer to http://support.microsoft.com/kb/115831
#endif
   int result;
   wchar_t *bufPtr;
   unsigned int portNumber = 0;
#if _MSC_VER >= 1400
#  pragma warning(disable:4996) // Disable deprecation warning in VC++ 2005
#endif

   if (isOpen())
      closePort();
   result = sscanf(portName, "COM%u", &portNumber);
   if ((result == 1) && (portNumber <= 0xFFFF))
   {
      bufPtr = new wchar_t[sizeof(COM_DEVICE_NAME) + 5]; // Plus the length of an unsigned short
      swprintf(bufPtr, L"%S%u", COM_DEVICE_NAME, portNumber);
   }
   else
   {
      size_t len = mbstowcs(NULL, portName, 0) + 1; // Add space for termination \0 char
      if (len == 0) // Handle conversion error
         return SER_PORT_NOT_FOUND;
      bufPtr = new wchar_t[len];
      mbstowcs(bufPtr, portName, len);
   }
   port = CreateFileW(bufPtr, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                      OPEN_EXISTING, 0, NULL);
   delete [] bufPtr;

   if (port == INVALID_HANDLE_VALUE)
   {
      switch (GetLastError())
      {
         case ERROR_FILE_NOT_FOUND:
         return (SER_PORT_NOT_FOUND);
         case ERROR_ACCESS_DENIED:
         return (SER_ALREADY_OPEN);
         default:
         return (SER_API_ERROR);
      }
   }
   // Save current device control block
   if (!GetCommState(port, &savedDevCtrlBlock))
      return (SER_API_ERROR);
   this->flush();
   return (config(9600, SER_DATABITS_8, SER_STOPBITS_1,
           SER_PARITY_NONE, SER_HANDSHAKE_NONE));
}


/**
 * Opens a serial port using an already open file handle. No
 * configuration is performed.
 *
 * @param portHandle Win32 API handle of already opened serial
 *                   port
 * @retval SER_SUCCESS No error
 * @retval SER_API_ERROR The underlaying API returned an error
 */
int HmSerialPort::openHandle(HANDLE portHandle)
{
   if (isOpen())
      return (SER_ALREADY_OPEN);
   port = portHandle;

   // Save current device control block
   if (!GetCommState(port, &savedDevCtrlBlock))
      return (SER_API_ERROR);
   this->flush();
   return (SER_SUCCESS);
}


/**
 * Configures a serial port.
 *
 * @param baudRate The port baudRate in bps (typically 300 - 115200).
 * The supported baudrates are OS dependant.
 * @param dataBits SER_DATABITS_7: 7 data bits, SER_DATABITS_8: data bits
 * @param stopBits SER_STOPBITS_1: 1 stop bit, SER_STOPBITS_2: 2 stop bits
 * @param parity SER_PARITY_NONE: no parity, SER_PARITY_ODD: odd parity,
 * PARITY_EVEN: SER_even parity
 * @param flowControl SER_HANDSHAKE_NONE: no handshake,
 * SER_HANDSHAKE_RTS_CTS: RTS/CTS hardware handshake
 * @retval SER_SUCCESS No error
 * @retval SER_NOT_OPEN Port has not been opened
 * @retval SER_INVALID_PARAMETER A parameter is not supported
 * @retval SER_API_ERROR The underlaying API returned an error
 */
int HmSerialPort::config(long baudRate, int dataBits,
                       int stopBits, int parity, int flowControl)
{
   DCB devCtrlBlock;

   if (!isOpen())
      return (SER_NOT_OPEN);

   //
   // Retrieve current device control block
   //
   if (!GetCommState(port, &devCtrlBlock))
      return (SER_API_ERROR);

   //
   // Modify device control block
   //
   devCtrlBlock.BaudRate = baudRate;
   switch (dataBits)
   {
      case SER_DATABITS_7:
         devCtrlBlock.ByteSize = 7;
      break;
      case SER_DATABITS_8:
         devCtrlBlock.ByteSize = 8;
      break;
      default:
      return (SER_INVALID_PARAMETER);
   }
   switch (stopBits)
   {
      case SER_STOPBITS_1:
         devCtrlBlock.StopBits = ONESTOPBIT;
      break;
      case SER_STOPBITS_2:
         devCtrlBlock.StopBits = TWOSTOPBITS;
      break;
      default:
      return (SER_INVALID_PARAMETER);
   }
   switch (parity)
   {
      case SER_PARITY_NONE:
         devCtrlBlock.fParity = FALSE;
         devCtrlBlock.Parity = NOPARITY;
      break;
      case SER_PARITY_EVEN:
         devCtrlBlock.fParity = TRUE;
         devCtrlBlock.Parity = EVENPARITY;
      break;
      case SER_PARITY_ODD:
         devCtrlBlock.fParity = TRUE;
         devCtrlBlock.Parity = ODDPARITY;
      break;
      default:
      return (SER_INVALID_PARAMETER);
   }
   switch (flowControl)
   {
      case SER_HANDSHAKE_RTS_CTS:
         devCtrlBlock.fOutX = FALSE; // Disable output X-ON/X-OFF
         devCtrlBlock.fInX = FALSE; // Disable input X-ON/X-OFF
         devCtrlBlock.fOutxCtsFlow = TRUE;
         devCtrlBlock.fOutxDsrFlow = FALSE;
         devCtrlBlock.fRtsControl = RTS_CONTROL_HANDSHAKE;
         devCtrlBlock.fDtrControl = DTR_CONTROL_ENABLE;
         devCtrlBlock.fDsrSensitivity = FALSE;
      break;
      case SER_HANDSHAKE_NONE:
         devCtrlBlock.fOutX = FALSE; // Disable output X-ON/X-OFF
         devCtrlBlock.fInX = FALSE; // Disable input X-ON/X-OFF
         devCtrlBlock.fOutxCtsFlow = FALSE;
         devCtrlBlock.fOutxDsrFlow = FALSE;
         devCtrlBlock.fRtsControl = RTS_CONTROL_DISABLE;
         devCtrlBlock.fDtrControl = DTR_CONTROL_ENABLE;
         devCtrlBlock.fDsrSensitivity = FALSE;
      break;
      // Note: SER_HANDSHAKE_RTS_TOGGLE only availabe for Windows NT
      // Does not work w/ Win95, Win98 and ME
      // Refer to http://support.microsoft.com/kb/q140030/
      case SER_HANDSHAKE_RTS_TOGGLE:
         devCtrlBlock.fOutX = FALSE; // Disable output X-ON/X-OFF
         devCtrlBlock.fInX = FALSE; // Disable input X-ON/X-OFF
         devCtrlBlock.fOutxCtsFlow = FALSE;
         devCtrlBlock.fOutxDsrFlow = FALSE;
         devCtrlBlock.fRtsControl = RTS_CONTROL_TOGGLE;
         devCtrlBlock.fDtrControl = DTR_CONTROL_ENABLE;
         devCtrlBlock.fDsrSensitivity = FALSE;
      break;
      default:
      return (SER_INVALID_PARAMETER);
   }
   devCtrlBlock.fBinary = TRUE;
   devCtrlBlock.fErrorChar = FALSE;
   devCtrlBlock.fNull = FALSE;
   devCtrlBlock.fAbortOnError = FALSE;

   //
   // Store device control block
   //
   if (!SetCommState(port, &devCtrlBlock))
      return (SER_INVALID_PARAMETER);
   this->baudRate = baudRate;
   this->flowControl = flowControl;
   return (SER_SUCCESS);
}


void HmSerialPort::closePort()
{
   this->flush(); // Clear buffer in case OS won't do it for us
   // Restore saved device control block
   SetCommState(port, &savedDevCtrlBlock);
   CloseHandle(port);
   port = INVALID_HANDLE_VALUE;
}


/**
 * Sends a block of data.
 *
 * @param bufPtr Pointer to data to send
 * @param len Length of data buffer in bytes
 * @param timeOut Timeout value in milliseconds for operation
 * @retval SER_SUCCESS No error
 * @retval SER_NOT_OPEN Port has not been opened
 * @retval SER_TIME_OUT Unable to send data within the specified timeout period
 * @retval SER_API_ERROR The underlaying API returned an error
 */
int HmSerialPort::send(const char *bufPtr, size_t len, long timeOut)
{
   BOOL result;
   DWORD bytesSent = 0;

   if (!isOpen())
      return (SER_NOT_OPEN);
   memset(&commTimeOutBlock, 0, sizeof (commTimeOutBlock));
   commTimeOutBlock.WriteTotalTimeoutConstant = timeOut;
   commTimeOutBlock.WriteTotalTimeoutMultiplier = 0;
   if (!SetCommTimeouts(port, &commTimeOutBlock))
      return (SER_API_ERROR);
   result = WriteFile(port, bufPtr, (DWORD) len, &bytesSent,
#if !defined(_WIN32_WCE)
      &ovlStruct
#else
      NULL
#endif
      );
#if !defined(_WIN32_WCE)
   // Check for special case that we have been opened in overlappe mode
   if (!result && (GetLastError() == ERROR_IO_PENDING))
      result = GetOverlappedResult(port, &ovlStruct, &bytesSent, TRUE);
#endif
#ifdef TRACELOG
   TRACELOG2("RawSend(%d): ", bytesSent);
   for  (DWORD i = 0; i < bytesSent; i++)
      TRACEADD2("%02X ", (int) bufPtr[i] & 0xFF);
   TRACEADD1("\n");
#endif
   if (bytesSent != len)
      return (SER_TIME_OUT);
   if (!result)
      return (SER_API_ERROR);
   return (SER_SUCCESS);
}


/**
 * Receives a specified number of bytes.
 *
 * @param bufPtr Pointer to buffer where received data is to be returned
 * @param len Length of data buffer in bytes. This specifies the number
 * of bytes to receive
 * @param timeOut Timeout value in milliseconds for operation
 * @retval SER_SUCCESS No error
 * @retval SER_NOT_OPEN Port has not been opened
 * @retval SER_TIME_OUT Unable to receive data within the specified
 * timeout period
 * @retval SER_API_ERROR The underlaying API returned an error
 */
int HmSerialPort::receive(const char *bufPtr, size_t len, long timeOut)
{
   BOOL result;
   DWORD bytesRead;

   if (!isOpen())
      return (SER_NOT_OPEN);
   memset(&commTimeOutBlock, 0, sizeof (commTimeOutBlock));
   commTimeOutBlock.ReadTotalTimeoutConstant = timeOut;
   if (!SetCommTimeouts(port, &commTimeOutBlock))
      return (SER_API_ERROR);
   result = ReadFile(port, (void *) bufPtr, (DWORD) len, &bytesRead,
#if !defined(_WIN32_WCE)
      &ovlStruct
#else
      NULL
#endif
      );
#if !defined(_WIN32_WCE)
   // Check for special case that we have been opened in overlappe mode
   if (!result && (GetLastError() == ERROR_IO_PENDING))
      result = GetOverlappedResult(port, &ovlStruct, &bytesRead, TRUE);
#endif
#ifdef TRACELOG
   TRACELOG2("RawRead(%d): ", bytesRead);
   for  (DWORD i = 0; i < bytesRead; i++)
      TRACEADD2("%02X ", (int) bufPtr[i] & 0xFF);
   TRACEADD1("\n");
#endif
   if (bytesRead != len)
      return (SER_TIME_OUT);
   if (!result)
      return (SER_API_ERROR);
   return (SER_SUCCESS);
}


/**
 * Receives a single byte
 *
 * @param charBuf Pointer where received byte is to be returned
 * @param timeOut Timeout value in microseconds for operation
 * @retval SER_SUCCESS No error
 * @retval SER_NOT_OPEN Port has not been opened
 * @retval SER_TIME_OUT Unable to receive data within the specified
 * timeout period
 * @retval SER_API_ERROR The underlaying API returned an error
 */
int HmSerialPort::receiveByte(const char *charBuf, unsigned long microTimeOut)
{
   BOOL result;
   DWORD bytesRead;

   if (!isOpen())
      return (SER_NOT_OPEN);
   memset(&commTimeOutBlock, 0, sizeof (commTimeOutBlock));
   commTimeOutBlock.ReadTotalTimeoutConstant = (microTimeOut + 999UL) / 1000UL;
   if (!SetCommTimeouts(port, &commTimeOutBlock))
      return (SER_API_ERROR);
   result = ReadFile(port, (void *) charBuf, 1, &bytesRead,
#if !defined(_WIN32_WCE)
      &ovlStruct
#else
      NULL
#endif
      );
#if !defined(_WIN32_WCE)
   // Check for special case that we have been opened in overlappe mode
   if (!result && (GetLastError() == ERROR_IO_PENDING))
      result = GetOverlappedResult(port, &ovlStruct, &bytesRead, TRUE);
#endif
   if (bytesRead != 1)
      return (SER_TIME_OUT);
   if (!result)
      return (SER_API_ERROR);
   return (SER_SUCCESS);
}


/**
 * Purges send and receive buffer. Any unsent data is discarded.
 *
 * @retval SER_SUCCESS No error
 * @retval SER_NOT_OPEN Port has not been opened
 * @retval SER_API_ERROR The underlaying API returned an error
 */
int HmSerialPort::flush()
{
   if (!isOpen())
      return (SER_NOT_OPEN);
   if (!PurgeComm(port, PURGE_TXCLEAR | PURGE_RXCLEAR))
        return (SER_API_ERROR);
   return (SER_SUCCESS);
}


/**
 * Waits until all previous data has been transferred from the buffer
 * to the UART chip. This function is only permitted when
 * flow control is switched off.
 *
 * @retval SER_SUCCESS No error
 * @retval SER_UNSUPPORTED_OPERATION Operation is not permitted
 * because handshake is enabled
 * @retval SER_NOT_OPEN Port has not been opened
 * @retval SER_API_ERROR The underlaying API returned an error
 */
int HmSerialPort::drain()
{
   if (!isOpen())
      return (SER_NOT_OPEN);
   if (flowControl != SER_HANDSHAKE_NONE)
      return (SER_UNSUPPORTED_OPERATION);
   memset(&commTimeOutBlock, 0, sizeof (commTimeOutBlock));
   if (!SetCommTimeouts(port, &commTimeOutBlock))
      return (SER_API_ERROR);
   if (!FlushFileBuffers(port))
      return (SER_API_ERROR);
   return (SER_SUCCESS);
}


/**
 * Sets the RTS line. This function is only permitted when
 * flow control is switched off.
 *
 * @retval SER_SUCCESS No error
 * @retval SER_UNSUPPORTED_OPERATION Operation is not permitted
 * because handshake is enabled or underlaying API does
 * not support this function
 * @retval SER_NOT_OPEN Port has not been opened
 * @retval SER_API_ERROR The underlaying API returned an error
 */
int HmSerialPort::setRts()
{
   if (!isOpen())
      return (SER_NOT_OPEN);
   if (flowControl != SER_HANDSHAKE_NONE)
      return (SER_UNSUPPORTED_OPERATION);
   if (!EscapeCommFunction(port, SETRTS))
      return (SER_API_ERROR);
   return (SER_SUCCESS);
}


/**
 * Clears the RTS line. This function is only permitted when
 * flow control is switched off.
 *
 * @retval SER_SUCCESS No error
 * @retval SER_UNSUPPORTED_OPERATION Operation is not permitted
 * because handshake is enabled or underlaying API does
 * not support this function
 * @retval SER_NOT_OPEN Port has not been opened
 * @retval SER_API_ERROR The underlaying API returned an error
 */
int HmSerialPort::clearRts()
{
   if (!isOpen())
      return (SER_NOT_OPEN);
   if (flowControl != SER_HANDSHAKE_NONE)
      return (SER_UNSUPPORTED_OPERATION);
   if (!EscapeCommFunction(port, CLRRTS))
      return (SER_API_ERROR);
   return (SER_SUCCESS);
}


/**
 * Tests if port has been opened.
 *
 * @return true if port is open else false
 */
int HmSerialPort::isOpen()
{
   return (port != INVALID_HANDLE_VALUE);
}


//@}


/*****************************************************************************
 * Test case
 *****************************************************************************/

#ifdef TEST_RUN

#include <stdio.h>

int main()
{
   char testString[] = "The quick brown fox jumps over the lazy dog.\n\r";
   char buf[10];
   HmSerialPort port;
   int i;
   int result;

   printf("Test Run for hmserio.cpp\n\n");

   result = port.openPort("COM1");
   printf("open = 0x%X\n", result);

   result = port.config(9600, port.SER_DATABITS_8,
                        port.SER_STOPBITS_1,
                        port.SER_PARITY_NONE,
                        port.SER_HANDSHAKE_NONE);
   printf("config = 0x%X\n", result);

   for (i = 0; i < 10; i++)
   {
      result = port.send(testString, strlen(testString), 1000);
      HmTimer::sleepMillis(100);
      printf("send = 0x%X\n", result);
   }

   result = port.receive(buf, sizeof(buf), 10000);
   printf("receive = 0x%X\n", result);

   return (0);
}


#endif
