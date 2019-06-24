/**
 * @file MbusRtuOverTcpMasterProtocol.cpp
 *
 * @if NOTICE
 *
 * Copyright (c) proconX Pty Ltd. All rights reserved.
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
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Package header
#include "hmtracelog.h"
#include "hmcrc16.h"
#include "hmtimer.hpp"
#include "MbusRtuOverTcpMasterProtocol.hpp"


/*****************************************************************************
 * MbusRtuOverTcpMasterProtocol class implementation
 *****************************************************************************/

/**
 * Constructs a MbusRtuOverTcpMasterProtocol object and initialises its data.
 */
MbusRtuOverTcpMasterProtocol::MbusRtuOverTcpMasterProtocol():
   MbusTcpMasterProtocol()
{
   portNo = 1100; // TCP port number, defaults to 1100
}


// Implementation is here for documentation purposes mainly.
/**
 * Connects to a Encapsulated Modbus RTU slave.
 *
 * This function establishes a logical network connection between master
 * and slave. After a connection has been established data and control
 * functions can be used. A TCP/IP connection should be closed if it is no
 * longer needed.
 *
 * @note The default time-out for the connection is 1000 ms.
 * @note The default TCP port number is 1100.
 * @param hostName String with IP address or host name
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 */
int MbusRtuOverTcpMasterProtocol::openProtocol(const char * const hostName)
{
   return (MbusTcpMasterProtocol::openProtocol(hostName));
}


#ifdef __WIN32__
// Required as the inherited overloaded method is not seen
int MbusRtuOverTcpMasterProtocol::openProtocol(const wchar_t * const hostName)
{
   return (MbusTcpMasterProtocol::openProtocol(hostName));
}
#endif


// Implementation is here for documentation purposes mainly.
/**
 * Sets the TCP port number to be used by the protocol.
 *
 * @remark Usually the port number remains unchanged and defaults to
 * 1100. In this case no call to this function is necessary. However if
 * the port number has to be different from 1100 this function must be
 * called <i>before</i> opening the connection with openProtocol().
 *
 * @param portNo Port number to be used when opening the connection
 * @retval FTALK_SUCCESS Success
 * @retval FTALK_ILLEGAL_STATE_ERROR Protocol already open
 */
int MbusRtuOverTcpMasterProtocol::setPort(unsigned short portNo)
{
   return (MbusTcpMasterProtocol::setPort(portNo));
}


/**
 * Sends a Encapsulated Modbus RTU query frame and receives the corresponding
 * reply frame.
 *
 * @param address Modbus slave address used for data transfer
 * @param function Modbus function the data transfer is performed for
 * @param sendDataArr Data to be sent. This data is function specific.
 * @param sendDataLen Number of data bytes to be sent
 * @param rcvDataArr Data to received sent. This data is function specific.
 * @param rcvDataLen Number of data bytes expected to be received
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 */
int MbusRtuOverTcpMasterProtocol::transceiveMessage(int address, int function,
                                                    char sendDataArr[],
                                                    int sendDataLen,
                                                    char rcvDataArr[],
                                                    int rcvDataLen,
                                                    int *actualRcvdPtr)
{
   int i;
   int result;
   int bytesReceived = 0;
   int bytesExpected = (int) rcvDataLen + FRAME_LEN;
   unsigned short checkSum;
   fd_set fdSet;
   timeval timeVal;

   totalCounter++;

   if (actualRcvdPtr)
     *actualRcvdPtr = 0;

   //
   // Clear input buffer from garbage
   //
   timeVal.tv_sec = 0;
   timeVal.tv_usec = 0;
   FD_ZERO(&fdSet);
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4127)
#endif
   FD_SET(tcpSocket, &fdSet);
#ifdef _MSC_VER
#  pragma warning(pop)
#endif
   while (select((int) tcpSocket + 1, &fdSet, NULL, NULL, &timeVal) == 1)
   {
      result = recv(tcpSocket, (char *) bufferArr, sizeof (bufferArr), 0);
      if ((result == SOCKET_ERROR) || (result == 0))
         return (FTALK_CONNECTION_WAS_CLOSED);
      // Prepare fd_set for next select call
      FD_ZERO(&fdSet);
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4127)
#endif
      FD_SET(tcpSocket, &fdSet);
#ifdef _MSC_VER
#  pragma warning(pop)
#endif
      timeVal.tv_sec = 0;
      timeVal.tv_usec = 0;
   }

   //
   // Assemble MODBUS/TCP message
   //
   bufferArr[0] = (char) address;
   bufferArr[1] = (char) function;
   // Copy data into send buffer
   memcpy(&(bufferArr[HDR_LEN]), sendDataArr, sendDataLen);

   //
   // Calculate CRC
   //
   checkSum = calcCrc16(bufferArr, sendDataLen + 2, 0xFF);
   bufferArr[sendDataLen + 2] = (char) (checkSum & 0xFF);
   bufferArr[sendDataLen + 3] = (char) (checkSum >> 8);

   //
   // Poll delay
   //
   if (pollDelay > 0)
      silenceTimer.sleepUntilExpiry();
   silenceTimer.startMillis(pollDelay);

   //
   // Send request to slave
   //
   TRACELOG2("Send(%d): ", sendDataLen + HDR_LEN);
   for  (i = 0; i < sendDataLen + HDR_LEN; i++)
      TRACEADD2("%02X ", (int) bufferArr[i] & 0xFF);
   TRACEADD1("\n");

   timeVal.tv_sec = (long) timeOut / 1000L;
   timeVal.tv_usec = ((long) timeOut % 1000L) * 1000L;
   FD_ZERO(&fdSet);
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4127)
#endif
   FD_SET(tcpSocket, &fdSet);
#ifdef _MSC_VER
#  pragma warning(pop)
#endif
   if (timeOut == 0)
      result = select((int) tcpSocket + 1, NULL, &fdSet, NULL, NULL);
   else
      result = select((int) tcpSocket + 1, NULL, &fdSet, NULL, &timeVal);
   if (result != 1)
   {
      TRACELOG3("Send select failed with result=%d, socketErrno=%d\n",
                result, socketErrno);
      return (FTALK_IO_ERROR);
   }
   result = send(tcpSocket, (char *) bufferArr,
                 sendDataLen + FRAME_LEN, 0);
   if (result == SOCKET_ERROR)
      return (FTALK_CONNECTION_WAS_CLOSED);
   if (result != sendDataLen + FRAME_LEN)
   {
      TRACELOG3("Send failed with result=%d, socketErrno=%d\n",
                result, socketErrno);
      return (FTALK_IO_ERROR);
   }

   //
   // Receive first part of the message
   //
   while (bytesReceived  < EXC_MSG_LEN)
   {
      timeVal.tv_sec = (long) timeOut / 1000L;
      timeVal.tv_usec = ((long) timeOut % 1000L) * 1000L;
      FD_ZERO(&fdSet);
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4127)
#endif
      FD_SET((SOCKET) tcpSocket, &fdSet);
      FD_SET(tcpSocket, &fdSet);
#ifdef _MSC_VER
#  pragma warning(pop)
#endif
      if (timeOut == 0)
         result = select((int) tcpSocket + 1, &fdSet, NULL, NULL, NULL);
      else
         result = select((int) tcpSocket + 1, &fdSet, NULL, NULL, &timeVal);
      if (result == 0)
         return (FTALK_REPLY_TIMEOUT_ERROR);
      if ((result == SOCKET_ERROR) && (socketErrno == SOCKET_EINTR))
         continue;
      if (result != 1)
      {
         TRACELOG3("Recv select failed with result=%d, socketErrno=%d\n",
                   result, socketErrno);
         return (FTALK_IO_ERROR);
      }
      result  = recv(tcpSocket, (char *) &bufferArr[bytesReceived],
                     EXC_MSG_LEN - bytesReceived, 0);
      if ((result == SOCKET_ERROR) || (result == 0))
         return (FTALK_CONNECTION_WAS_CLOSED);
      bytesReceived += result;
   }
   // If no exception message received, read the remainder of the message
   if ((signed char) bufferArr[1] >= 0)
   {
      while (bytesReceived  < rcvDataLen + FRAME_LEN)
      {
         timeVal.tv_sec = (long) timeOut / 1000L;
         timeVal.tv_usec = ((long) timeOut % 1000L) * 1000L;
         FD_ZERO(&fdSet);
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4127)
#endif
		 FD_SET(tcpSocket, &fdSet);
#ifdef _MSC_VER
#  pragma warning(pop)
#endif
         if (timeOut == 0)
            result = select((int) tcpSocket + 1, &fdSet, NULL, NULL, NULL);
         else
            result = select((int) tcpSocket + 1, &fdSet, NULL, NULL, &timeVal);
         if (result == 0)
            return (FTALK_REPLY_TIMEOUT_ERROR);
         if ((result == SOCKET_ERROR) && (socketErrno == SOCKET_EINTR))
            continue;
         if (result != 1)
         {
            TRACELOG3("Recv Select2 failed with result=%d, socketErrno=%d\n",
                      result, socketErrno);
            return (FTALK_IO_ERROR);
         }
         result  = recv(tcpSocket, (char *) &bufferArr[bytesReceived],
                        bytesExpected - bytesReceived, 0);
         if ((result == SOCKET_ERROR) || (result == 0))
            return (FTALK_CONNECTION_WAS_CLOSED);
         bytesReceived += result;
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
