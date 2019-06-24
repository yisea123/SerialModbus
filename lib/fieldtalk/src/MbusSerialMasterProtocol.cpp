/**
 * @file MbusSerialMasterProtocol.cpp
 *
 * @if NOTICE
 *
 * THIS FILE IS PART OF PROPRIETARY SOFTWARE COPYRIGHTED BY PROCONX PTY LTD
 * AND YOU NEED A LICENSE OR WRITTEN PERMISSION TO USE OR REDISTRIBUTE.
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
#include <stdlib.h>
#include <math.h>

// Package header
#include "hmplatf.h"
#include "hmtracelog.h"
#include "hmserio.hpp"
#include "MbusSerialMasterProtocol.hpp"


/*****************************************************************************
 * MbusSerialMasterProtocol class implementation
 *****************************************************************************/

/**
 * Constructs a MbusSerialMasterProtocol object and initialises its data.
 */
MbusSerialMasterProtocol::MbusSerialMasterProtocol(): serialPort(*(new HmSerialPort()))
{
   serialMode = SER_RS232;
   rtsDelay = 0UL;
}


// Refer to super class documentation
MbusSerialMasterProtocol::~MbusSerialMasterProtocol()
{
   closeProtocol();
   delete &serialPort;
}


/**
 * Opens a serial Modbus protocol and the associated serial port with
 * specific port parameters.
 *
 * This function opens the serial port. After a port has been
 * opened, data and control functions can be used.
 *
 * @note The default time-out for the data transfer is 1000 ms.
 * @note The default poll delay is 0 ms.
 * @note Automatic retries are switched off (retry count is 0).
 * @note The Modbus standard requires two stop bits if no parity is chosen.
 *       This library is not enforcing this but it is a recommended configuration.
 * @param portName Serial port identifier
 * (e.g. "COM1", "/dev/ser1" or "/dev/ttyS0")
 * @param baudRate The port baudRate in bps (typically 1200 - 115200,
 * maximum value depends on UART hardware)
 * @param dataBits SER_DATABITS_7: 7 data bits (ASCII protocol only),
 * SER_DATABITS_8: data bits
 * @param stopBits SER_STOPBITS_1: 1 stop bit, SER_STOPBITS_2: 2 stop bits
 * @param parity SER_PARITY_NONE: no parity, SER_PARITY_ODD: odd parity,
 * SER_PARITY_EVEN: even parity
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 */
int MbusSerialMasterProtocol::openProtocol(const char * const portName,
                                           long baudRate, int dataBits,
                                           int stopBits, int parity)
{
   int result;

   TRACELOG2("Open port: %s \n", portName);
   TRACELOG5("Parameters: %d, %d, %d, %d\n",
             baudRate, dataBits, stopBits, parity);
   TRACELOG4("Configuration: %d, %d, %d\n", timeOut, pollDelay, retryCnt);

   if (isOpen())
      return (FTALK_ILLEGAL_STATE_ERROR);

   if ((dataBits != HmSerialPort::SER_DATABITS_7) &&
       (dataBits != HmSerialPort::SER_DATABITS_8))
      return(FTALK_ILLEGAL_ARGUMENT_ERROR);

   result = serialPort.openPort(portName);
   if (result == HmSerialPort::SER_PORT_NO_ACCESS)
      return(FTALK_PORT_NO_ACCESS);
   if (result == HmSerialPort::SER_ALREADY_OPEN)
      return(FTALK_PORT_ALREADY_OPEN);
   if (result != HmSerialPort::SER_SUCCESS)
   {
      TRACELOG2("serialPort.openPort failed with result=%d\n", result);
      return(FTALK_OPEN_ERR);
   }
   result = serialPort.config(baudRate, dataBits,
                              stopBits, parity,
                              HmSerialPort::SER_HANDSHAKE_NONE);
   if (result != HmSerialPort::SER_SUCCESS)
      return(FTALK_ILLEGAL_ARGUMENT_ERROR);
   if (serialMode == SER_RS485)
      serialPort.clearRts();
   else
      serialPort.setRts();
   return (FTALK_SUCCESS);
}

#ifdef __WIN32__
// Create a second entry point for wchar_t strings
int MbusSerialMasterProtocol::openProtocol(const wchar_t * const portName,
                                           long baudRate, int dataBits,
                                           int stopBits, int parity)
{
   int result;

   //
   // Convert UNICODE hostName to ANSI string
   //
   size_t len = wcslen(portName) + 1;
   char *bufPtr = new char[len];
#if _MSC_VER >= 1400
#  pragma warning(disable:4996) // Disable deprecation warning in VC++ 2005
#endif
   wcstombs(bufPtr, portName, len);

   result = openProtocol(bufPtr, baudRate,
                         dataBits, stopBits, parity);

   delete [] bufPtr;
   return result;
}


#  if defined(_NATIVE_WCHAR_T_DEFINED)
// Create an entry point for eVC++ 4.0 and VC++ 6.0 using unsigned short*
// instead of the native wchar_t*. eVC++ and VC++ 6 have no native
// wchar_t and uses unsigned char* instead which breaks binary
// compatibility if compiled under VS2005. Refer to:
// http://msdn.microsoft.com/en-us/library/ms838254.aspx
// http://msdn.microsoft.com/en-us/library/dh8che7s%28v=vs.80%29.aspx
int MbusSerialMasterProtocol::openProtocol(const unsigned short * const portName,
                                           long baudRate,
                                           int dataBits,
                                           int stopBits,
                                           int parity)
{
   return openProtocol((wchar_t *) portName, baudRate,
                        dataBits, stopBits, parity);
}
#  endif


/**
 * Opens a serial Modbus protocol using an existing and open
 * handle. Useful for using the protocol over a modem link.
 *
 * @param cnxnHandle Win32 API handle pointing to the existing
 *                   and open connection.
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 */
int MbusSerialMasterProtocol::openUseExistingConnection(HANDLE cnxnHandle)
{
   int result;

   if (isOpen())
      return (FTALK_ILLEGAL_STATE_ERROR);

   result = serialPort.openHandle(cnxnHandle);
   if (result != HmSerialPort::SER_SUCCESS)
   {
      TRACELOG2("serialPort.openPort failed with result=%d\n", result);
      return(FTALK_OPEN_ERR);
   }
   return (FTALK_SUCCESS);
}
#endif


// Refer to super class documentation
void MbusSerialMasterProtocol::closeProtocol()
{
   TRACELOG1("Close port\n");
   serialPort.closePort();
}


// Refer to super class documentation
int MbusSerialMasterProtocol::isOpen()
{
   return (serialPort.isOpen());
}


/**
 * @internal
 * Sends a Modbus query frame and receives the corresponding reply frame.
 * The method will do an automatic error recovery using retries.
 *
 * @param address Modbus slave address used for data transfer
 * @param function Modbus function the data transfer is performed for
 * @param sendDataArr Data to be sent. This data is function specific.
 * @param sendDataLen Number of data bytes to be sent
 * @param rcvDataArr Data to receive. This data is function specific.
 * @param rcvDataLen Number of data bytes expected to be received
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 */
int MbusSerialMasterProtocol::deliverMessage(int address, int function,
                                             char sendDataArr[],
                                             int sendDataLen,
                                             char rcvDataArr[],
                                             int rcvDataLen,
                                             int *actualRcvdPtr)
{
   int retries = retryCnt;
   int result;
#ifdef EVAL_VERSION
   static unsigned int msgCnt = 0;

   if (sqrt((double) msgCnt) >= 9.9785)
      return (FTALK_EVALUATION_EXPIRED);
   else
      msgCnt++;
#endif

   // Ensure that connection has been opened
   if (!isOpen())
      return (FTALK_ILLEGAL_STATE_ERROR);

   for (;;)
   {
      result = transceiveMessage(address, function,
                              sendDataArr, sendDataLen,
                              rcvDataArr, rcvDataLen, actualRcvdPtr);
      // If we have this kind of error refresh and re-try
      if ((result == FTALK_INVALID_FRAME_ERROR) ||
          (result == FTALK_REPLY_TIMEOUT_ERROR) ||
          (result == FTALK_CHECKSUM_ERROR))
      {
         if (retries <= 0)
            return (result);
      }
      else
         return (result);
      TRACELOG1("Retrying...\n");
      retries--;
   }
}


/**
 * Enables RS485 mode.
 *
 * In RS485 mode the RTS signal can be used to enable and disable the
 * transmitter of a RS232/RS485 converter. The RTS signal is asserted
 * before sending data. It is cleared after the transmit buffer has been
 * emptied and in addition the specified delay time has elapsed. The delay
 * time is necessary because even the transmit buffer is already empty, the
 * UART's FIFO will still contain unsent characters.
 *
 * @warning The use of RTS controlled RS232/RS485 converters should be
 * avoided if possible. It is difficult to determine the exact time when to
 * switch off the transmitter with non real-time operating systems like
 * Windows and Linux. If it is switched off to early characters might still
 * sit in the FIFO or the transmit register of the UART and these
 * characters will be lost. Hence the slave will not recognize the
 * message. On the other hand if it is switched off too late then the
 * slave's message is corrupted and the master will not recognize the
 * message.
 *
 * @remark The delay value is indicative only and not guaranteed to be
 * maintained. How precise it is followed depends on the operating system
 * used, it's scheduling priority and it's system timer resolution.
 * @note A protocol must be closed in order to configure it.
 * @param rtsDelay Delay time in ms (Range: 0 - 100000) which applies after
 * the transmit buffer is empty. 0 disables this mode.
 * @retval FTALK_SUCCESS Success
 * @retval FTALK_ILLEGAL_ARGUMENT_ERROR Argument out of range
 * @retval FTALK_ILLEGAL_STATE_ERROR Protocol is already open
 */
int MbusSerialMasterProtocol::enableRs485Mode(int rtsDelay)
{
   if ((rtsDelay < 0) || (rtsDelay > 100000))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if (isOpen())
      return (FTALK_ILLEGAL_STATE_ERROR);
   this->rtsDelay = rtsDelay;
   if (rtsDelay == 0)
      serialMode = SER_RS232;
   else
      serialMode = SER_RS485;
   return (FTALK_SUCCESS);
}
