/**
 * @file MbusRtuMasterProtocol.cpp
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
#include <string.h>

// Package header
#include "hmtracelog.h"
#include "hmcrc16.h"
#include "hmtimer.hpp"
#include "hmserio.hpp"
#include "MbusRtuMasterProtocol.hpp"


/*****************************************************************************
 * MbusRtuMasterProtocol class implementation
 *****************************************************************************/

/**
 * Constructs a MbusRtuMasterProtocol object and initialises its data.
 */
MbusRtuMasterProtocol::MbusRtuMasterProtocol()
{
   frameSilence = 5UL; // Default  for 9600 bps
}


/**
 * Opens a Modbus RTU protocol and the associated serial port with
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
 * @param dataBits Must be SER_DATABITS_8 for RTU
 * @param stopBits SER_STOPBITS_1: 1 stop bit, SER_STOPBITS_2: 2 stop bits
 * @param parity SER_PARITY_NONE: no parity, SER_PARITY_ODD: odd parity,
 * SER_PARITY_EVEN: even parity
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 */
int MbusRtuMasterProtocol::openProtocol(const char * const portName,
                                        long baudRate,
                                        int dataBits,
                                        int stopBits,
                                        int parity)
{
   int result;

   // 8 databits + startbit +  stopbits = 11
   // 3.5 char time is specified, we use 4 to be safe
   frameSilence = (unsigned long) ((4.0F * (11.0F * 1000.0F /
                                            (float) baudRate)) + 0.5F);

   // Smallest resolution we can resolve is 1 ms
   if (frameSilence <= 1UL)
      frameSilence = 1UL;
   if (dataBits != SER_DATABITS_8)
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   result = MbusSerialMasterProtocol::openProtocol(portName, baudRate,
                                                   dataBits, stopBits, parity);
   TRACELOG2("Frame silence: %d\n", frameSilence);
   return (result);
}


#ifdef __WIN32__
// Required as the inherited overloaded method is not seen
int MbusRtuMasterProtocol::openProtocol(const wchar_t * const portName,
                                        long baudRate,
                                        int dataBits, int stopBits,
                                        int parity)
{
   return MbusSerialMasterProtocol::openProtocol(portName,
                                                 baudRate, dataBits,
                                                 stopBits, parity);
}
#endif


/**
 * Sends a Modbus RTU query frame and receives the corresponding reply
 * frame.
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
int MbusRtuMasterProtocol::transceiveMessage(int address, int function,
                                             char sendDataArr[],
                                             int sendDataLen,
                                             char rcvDataArr[],
                                             int rcvDataLen,
                                             int *actualRcvdPtr)
{
   int i;
   int result;
   int bytesReceived;
   int bytesExpected = rcvDataLen + FRAME_LEN;
   unsigned short checkSum;
   char garbageBuffer[1];
   int idleRetry;

   totalCounter++;

   if (actualRcvdPtr)
     *actualRcvdPtr = 0;

   //
   // Assemble MODBUS RTU message
   //
   bufferArr[0]  = (char) address;
   bufferArr[1]  = (char) function;
   // Copy data into send buffer
   memcpy(&(bufferArr[2]), sendDataArr, sendDataLen);

   //
   // Calculate CRC
   //
   checkSum = calcCrc16(bufferArr, sendDataLen + 2, 0xFF);
   bufferArr[sendDataLen + 2] = (char) (checkSum & 0xFF);
   bufferArr[sendDataLen + 3] = (char) (checkSum >> 8);

   // Clear buffers from garbage
   serialPort.flush();

   //
   // Do poll delay
   //
   if (pollDelay > 0)
      silenceTimer.sleepUntilExpiry();
   silenceTimer.startMillis(pollDelay);

   //
   // Wait for line idle time of minimum 3.5 character times
   //
   idleRetry = 0;
   while (serialPort.receive(garbageBuffer, 1, frameSilence) == HmSerialPort::SER_SUCCESS)
   {
	   idleRetry++;
	   serialPort.flush();
	   if(idleRetry > 100)
         return FTALK_LINE_BUSY_ERROR;
   };

   //
   // Send request to slave
   //
   TRACELOG2("Send(%d): ", sendDataLen + FRAME_LEN);
   for  (i = 0; i < sendDataLen + FRAME_LEN; i++)
      TRACEADD2("%02X ", (int) bufferArr[i] & 0xFF);
   TRACEADD1("\n");

   if (serialMode == SER_RS485)
      serialPort.setRts();
   result = serialPort.send((char *) bufferArr, sendDataLen + FRAME_LEN, timeOut);
   serialPort.drain();
   if (serialMode == SER_RS485)
   {
      HmTimer::sleepMillis(rtsDelay);
      serialPort.clearRts();
   }
   switch (result)
   {
      case HmSerialPort::SER_SUCCESS:
         // OK
      break;
      case HmSerialPort::SER_TIME_OUT:
      case HmSerialPort::SER_LINE_ERROR:
      return (FTALK_SEND_TIMEOUT_ERROR);
      default:
         TRACELOG2("serialPort.send failed with result=%d\n", result);
      return (FTALK_IO_ERROR);
   }

   // Return if broadcast address 0 is used
   if ((address == 0) || (rcvDataLen <= 0))
   {
      successCounter++;
      return (FTALK_SUCCESS);
   }

   //
   // Receive first part of the message
   //
   result = serialPort.receive((char *) bufferArr, EXC_MSG_LEN, timeOut);
   switch (result)
   {
      case HmSerialPort::SER_SUCCESS:
         bytesReceived = EXC_MSG_LEN;
      break;
      case HmSerialPort::SER_TIME_OUT:
         TRACELOG1("Recv: time-out\n");
      return (FTALK_REPLY_TIMEOUT_ERROR);
      case HmSerialPort::SER_LINE_ERROR:
         TRACELOG1("Recv: line error\n");
      return (FTALK_REPLY_TIMEOUT_ERROR);
      default:
         TRACELOG2("serialPort.receive failed with result=%d\n", result);
      return (FTALK_IO_ERROR);
   }
   // If no exception message received, read the remainder of the message
   if (((signed char) bufferArr[1] >= 0) && (bytesExpected - EXC_MSG_LEN > 0))
   {
      result  = serialPort.receive((char *) &bufferArr[EXC_MSG_LEN],
                                   bytesExpected - EXC_MSG_LEN, timeOut);
      switch (result)
      {
         case HmSerialPort::SER_SUCCESS:
            bytesReceived = bytesExpected;
         break;
         case HmSerialPort::SER_TIME_OUT:
         case HmSerialPort::SER_LINE_ERROR:
         return (FTALK_REPLY_TIMEOUT_ERROR);
         default:
            TRACELOG2("serialPort.receive2 failed with result=%d\n", result);
         return (FTALK_IO_ERROR);
      }
   }

   TRACELOG2("Recv(%d): ", bytesReceived);
   for  (i = 0; i < bytesReceived; i++)
      TRACEADD2("%02X ", (int) bufferArr[i] & 0xFF);
   TRACEADD1("\n");

   //
   // Check frame
   // - Checksum ok
   // - Function code ok
   // - Address ok
   //
   checkSum = (unsigned short) ((bufferArr[bytesReceived - 1] << 8) |
                                (bufferArr[bytesReceived - 2] & 0xFF));
   if (checkSum != calcCrc16(bufferArr, bytesReceived - 2, 0xFF))
      return(FTALK_CHECKSUM_ERROR);
   if (((bufferArr[1] & (char) 0x7F) != (char) function) ||
       (bufferArr[0] != (char) address))
      return (FTALK_INVALID_FRAME_ERROR);

   //
   // Handle exception responses
   //
   if ((signed char) bufferArr[1] < 0)
      return (FTALK_MBUS_EXCEPTION_RESPONSE + bufferArr[2]);

   // Copy data into result buffer
   memcpy(rcvDataArr, &bufferArr[2], rcvDataLen);
   if (actualRcvdPtr)
     *actualRcvdPtr = bytesReceived;

   successCounter++;
   return (FTALK_SUCCESS);
}

