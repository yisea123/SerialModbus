/**
 * @file MbusTcpMasterProtocol.cpp
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
#include <math.h>

// Package header
#include "hmplatf.h"
#include "hmtcpip.h"
#include "hmtracelog.h"
#include "hmtimer.hpp"
#include "MbusTcpMasterProtocol.hpp"


/*****************************************************************************
 * MbusTcpMasterProtocol class implementation
 *****************************************************************************/

/**
 * Constructs a MbusTcpMasterProtocol object and initialises its data.
 */
MbusTcpMasterProtocol::MbusTcpMasterProtocol()
{
   portNo = 502; // TCP port number, defaults to 502
   tcpSocket = INVALID_SOCKET;
   transactionId = 0;

#ifdef __WIN32__
   //
   // Special treatment for the Win32 platform, needs to load WinSock DLL
   //
   WSADATA wsaData;

   WSAStartup(0x0101, &wsaData);
#endif

}


// Refer to super class documentation
MbusTcpMasterProtocol::~MbusTcpMasterProtocol()
{
   closeProtocol();
#ifdef __WIN32__
   // Unload the Winsock API
   WSACleanup();
#endif
}


/**
 * Connects to a MODBUS/TCP slave.
 *
 * This function establishes a logical network connection between master
 * and slave. After a connection has been established data and control
 * functions can be used. A TCP/IP connection should be closed if it is no
 * longer needed.
 *
 * @note The default time-out for the connection is 1000 ms.
 * @note The default TCP port number is 502.
 * @param hostName String with IP address or host name
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 */
int MbusTcpMasterProtocol::openProtocol(const char * const hostName)
{
   TRACELOG2("Open connection to %s\n", hostName);
   TRACELOG4("Configuration: %d, %d, %d\n", timeOut, pollDelay, retryCnt);

   if (isOpen())
      return (FTALK_ILLEGAL_STATE_ERROR);

   //
   // Evaluate host address
   //
   hostAddress.sin_family = AF_INET;
   hostAddress.sin_port = htons(portNo);
   hostAddress.sin_addr.s_addr = inet_addr((char *) hostName);
#if !defined(__VXWORKS__) // We don't support host name resolving with VxWorks
   if (hostAddress.sin_addr.s_addr == INADDR_NONE)
   {
      struct hostent *hostInfo;

      hostInfo = gethostbyname((char *) hostName);

      if (hostInfo == NULL)
         return (FTALK_TCPIP_CONNECT_ERR);
      hostAddress.sin_addr = *(struct in_addr *) hostInfo->h_addr;
   }
#endif
   return (openProtocol());
}


#ifdef __WIN32__
// Create a second entry point for wchar_t strings
int MbusTcpMasterProtocol::openProtocol(const wchar_t * const hostName)
{
   int result;

   //
   // Convert UNICODE hostName to ANSI string
   //
   size_t len = wcslen(hostName) + 1;
   char *bufPtr = new char[len];
#if _MSC_VER >= 1400
#  pragma warning(disable:4996) // Disable deprecation warning in VC++ 2005
#endif
   wcstombs(bufPtr, hostName, len);

   result = openProtocol(bufPtr);

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
int MbusTcpMasterProtocol::openProtocol(const unsigned short * const hostName)
{
   return openProtocol((wchar_t *) hostName);
}
#  endif
#endif


/**
 * Closes a TCP/IP connection to a slave and releases
 * any system resources associated with the connection.
 */
void MbusTcpMasterProtocol::closeProtocol()
{
   TRACELOG1("Close connection\n");
   if (tcpSocket == INVALID_SOCKET)
      return;
   closesocket(tcpSocket);
   tcpSocket = INVALID_SOCKET;
}


/**
 * Sets the TCP port number to be used by the protocol.
 *
 * @remark Usually the port number remains unchanged and defaults to
 * 502. In this case no call to this function is necessary. However if
 * the port number has to be different from 502 this function must be
 * called <i>before</i> opening the connection with openProtocol().
 *
 * @param portNo Port number to be used when opening the connection
 * @retval FTALK_SUCCESS Success
 * @retval FTALK_ILLEGAL_STATE_ERROR Protocol already open
 */
int MbusTcpMasterProtocol::setPort(unsigned short portNo)
{
   TRACELOG2("Set port %d\n", portNo);
   if (isOpen())
      return (FTALK_ILLEGAL_STATE_ERROR);
   this->portNo = (unsigned short) portNo;
   return (FTALK_SUCCESS);
}


// Refer to super class documentation
int MbusTcpMasterProtocol::isOpen()
{
   return (tcpSocket != INVALID_SOCKET);
}


/**
 * @internal
 * Connects to a slave.
 *
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 */
int MbusTcpMasterProtocol::openProtocol()
{
   int result;
   int socketOption;
   SOCK_LEN_TYPE sockOptLen;
#ifdef __WIN32__
   u_long ioctlOpt;
#else
   int ioctlOpt;
#endif
   fd_set fdSet;
   timeval timeVal;
   refreshFlag = 0; // Flag to avoid double timeout problem

   if (isOpen())
      closeProtocol();

   //
   // Open socket
   //
   tcpSocket = socket(PF_INET, SOCK_STREAM, 0);
   if (tcpSocket == INVALID_SOCKET)
   {
      closeProtocol();
      return (FTALK_OPEN_ERR);
   }

   //
   // Make socket non-blocking
   //
   ioctlOpt = 1;
#if defined(__VXWORKS__)
   result = ioctlsocket(tcpSocket, FIONBIO, (int) &ioctlOpt);
#else
   result = ioctlsocket(tcpSocket, FIONBIO, &ioctlOpt);
#endif
   if (result == SOCKET_ERROR)
   {
      TRACELOG3("ioctlsocket failed with result=%d, socketErrno=%d\n",
                result, socketErrno);
      closeProtocol();
      return (FTALK_OPEN_ERR);
   }

   //
   // Connect
   //
   result = connect(tcpSocket, (struct sockaddr *) &hostAddress,
                    sizeof(hostAddress));
   if ((result == SOCKET_ERROR) && ((socketErrno != SOCKET_EWOULDBLOCK) &&
                                    (socketErrno != SOCKET_EINPROGRESS)))
   {
      TRACELOG2("Connect failed with sockErrno=%d\n", socketErrno);
      closeProtocol();
      return (FTALK_TCPIP_CONNECT_ERR);
   }
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
   if (result == 0)
   {
      closeProtocol();
      return (FTALK_TCPIP_CONNECT_ERR);
   }
   if (result != 1)
   {
      TRACELOG3("Connect select failed with result=%d, socketErrno=%d\n",
                result, socketErrno);
      closeProtocol();
      return (FTALK_IO_ERROR);
   }

   //
   // Validate connection (must be done on Linux)
   //
   socketOption = 0;
   sockOptLen = sizeof (socketOption);
   getsockopt(tcpSocket, SOL_SOCKET, SO_ERROR,
              (char *) &socketOption, &sockOptLen);
   if (socketOption != 0)
   {
      TRACELOG2("SO_ERROR = %d\n", socketOption);
      closeProtocol();
      return (FTALK_TCPIP_CONNECT_ERR);
   }

   //
   // Configure socket options (we ignore errors here, not critical)
   //
#ifdef TCP_NODELAY
   socketOption = 1; // Enable option
   setsockopt(tcpSocket, IPPROTO_TCP, TCP_NODELAY,
              (char *) &socketOption, sizeof (socketOption));
#endif
#ifdef SO_SNDBUF
   socketOption = MAX_MSG_SIZE;
   setsockopt(tcpSocket, SOL_SOCKET, SO_SNDBUF,
              (char *) &socketOption, sizeof (socketOption));
#endif
#ifdef SO_RCVBUF
   socketOption = MAX_MSG_SIZE;
   setsockopt(tcpSocket, SOL_SOCKET, SO_RCVBUF,
              (char *) &socketOption, sizeof (socketOption));
#endif
#ifdef SO_KEEPALIVE
   socketOption = 1; // Enable option
   setsockopt(tcpSocket, SOL_SOCKET, SO_KEEPALIVE,
              (char *) &socketOption, sizeof (socketOption));
#endif

   return (FTALK_SUCCESS);
}


/**
 * @internal
 * Refreshes a TCP/IP connection.
 *
 * A connection should be refreshed after corrupted data has been received.
 * Refreshing a connection closes and re-opens the TCP/IP connection. This
 * operation is recommended by the MODBUS/TCP specification.
 *
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 */
int MbusTcpMasterProtocol::refreshConnection()
{
   TRACELOG1("Refresh connection\n");
   closeProtocol();
   return (openProtocol());
}


/**
 * @internal
 * Sends a Modbus query frame and receives the corresponding reply frame.
 * The method will do an automatic error recovery using retries
 * and closing and re-opening the TCP/IP connection.
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
int MbusTcpMasterProtocol::deliverMessage(int address, int function,
                                          char sendDataArr[],
                                          int sendDataLen,
                                          char rcvDataArr[],
                                          int rcvDataLen,
                                          int *actualRcvdPtr)
{
   int retries = retryCnt;
   int err;
   int result;
#ifdef EVAL_VERSION
   static unsigned int msgCnt = 0;

   if (sqrt((double) msgCnt) >= 9.9876)
      return (FTALK_EVALUATION_EXPIRED);
   else
      msgCnt++;
#endif

   // Ensure that connection has been opened
   if (!isOpen())
      return (FTALK_ILLEGAL_STATE_ERROR);

   // Manage delayed refreshing of connection
   if (refreshFlag == 1)
   {
      result = refreshConnection();
      if (result != FTALK_SUCCESS)
         return (result);
   }

   for (;;)
   {
       err = transceiveMessage(address, function,
                               sendDataArr, sendDataLen,
                               rcvDataArr, rcvDataLen, actualRcvdPtr);
       // If we have this kind of error refresh and re-try
       if ((err == FTALK_INVALID_FRAME_ERROR) ||
           (err == FTALK_INVALID_REPLY_ERROR) ||
           (err == FTALK_REPLY_TIMEOUT_ERROR))
       {
           if ((err == FTALK_REPLY_TIMEOUT_ERROR) && (retries == 0))
           {
               // Avoid double time-out, delay refreshing of connection to next call
               refreshFlag = 1;
               return(FTALK_REPLY_TIMEOUT_ERROR);
           }
           else
               result = refreshConnection();
           if (result != FTALK_SUCCESS)
           {
               // Connection broken, we can't continue
               return (result);
           }
           if (retries <= 0)
               return (err);
       }
       else
           return (err);
       TRACELOG2("Retrying %d...\n", retries);
       retries--;
   }
}


/**
 * Sends a MODBUS/TCP query frame and receives the corresponding reply
 * frame.
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
int MbusTcpMasterProtocol::transceiveMessage(int address, int function,
                                             char sendDataArr[],
                                             int sendDataLen,
                                             char rcvDataArr[],
                                             int rcvDataLen,
                                             int *actualRcvdPtr)
{
   int i;
   int result;
   int bytesReceived = 0;
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
      if (result == SOCKET_ERROR)
         return (FTALK_IO_ERROR);
      if (result == 0)
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
   bufferArr[0] = (char) (++transactionId >> 8);
   bufferArr[1] = (char) (transactionId & 0xFF);
   bufferArr[2] = 0; // Protocol indentifier, 0 indicates Modbus
   bufferArr[3] = 0; // Protocol indentifier, 0 indicates Modbus
   bufferArr[4] = (char) ((sendDataLen + 2) >> 8);
   bufferArr[5] = (char) ((sendDataLen + 2) & 0xFF);
   bufferArr[6] = (char) address;
   bufferArr[7] = (char) function;
   // Copy data into send buffer
   memcpy(&(bufferArr[HDR_LEN]), sendDataArr, sendDataLen);

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
   result = send(tcpSocket, (char *) bufferArr, sendDataLen + FRAME_LEN, 0);
   if (result == SOCKET_ERROR)
      return (FTALK_CONNECTION_WAS_CLOSED);
   if (result != sendDataLen + FRAME_LEN)
   {
      TRACELOG3("Send failed with result=%d, socketErrno=%d\n",
                result, socketErrno);
      return (FTALK_IO_ERROR);
   }

   //
   // Receive first part of the message to check MBAP header and for an
   // exception
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
      FD_SET(tcpSocket, &fdSet);
#ifdef _MSC_VER
#  pragma warning(pop)
#endif
      if (timeOut == 0)
         result = select((int) tcpSocket + 1, &fdSet, NULL, NULL, NULL);
      else
         result = select((int) tcpSocket + 1, &fdSet, NULL, NULL, &timeVal);
      if (result == 0)
      {
         TRACELOG2("Recv(%d): ", bytesReceived);
         for  (i = 0; i < bytesReceived; i++)
            TRACEADD2("%02X ", (int) bufferArr[i] & 0xFF);
         TRACEADD1("\n");
         TRACELOG3("Recv select returns 0, received so far = %d, expected = %d\n",
                   bytesReceived, EXC_MSG_LEN);
         return (FTALK_REPLY_TIMEOUT_ERROR);
      }
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
      if (result == SOCKET_ERROR)
         return (FTALK_IO_ERROR);
      if (result == 0)
         return (FTALK_CONNECTION_WAS_CLOSED);
      bytesReceived += result;
   }

   //
   // Validate protocol and transaction indentifier
   //
   if ((bufferArr[2] != 0) || (bufferArr[3] != 0))
      return (FTALK_INVALID_MBAP_ID);
   if (((unsigned short) (bufferArr[0] << 8) + (bufferArr[1] & 0xFF)) != transactionId)
      return (FTALK_INVALID_MBAP_ID);

   //
   // If no exception message received, read the remainder of the message
   //
   if ((signed char) bufferArr[7] >= 0)
   {
      int bytesExpected = (bufferArr[4] << 8) + (bufferArr[5] & 0xFF) + PREFIX_LEN;

      while (bytesReceived  < bytesExpected)
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
         {
            TRACELOG2("Recv(%d): ", bytesReceived);
            for  (i = 0; i < bytesReceived; i++)
               TRACEADD2("%02X ", (int) bufferArr[i] & 0xFF);
            TRACEADD1("\n");
            TRACELOG3("Recv select2 returns 0, received so far = %d, expected = %d\n",
                      bytesReceived, rcvDataLen + FRAME_LEN);
            return (FTALK_REPLY_TIMEOUT_ERROR);
         }
         if ((result == SOCKET_ERROR) && (socketErrno == SOCKET_EINTR))
            continue;
         if (result != 1)
         {
            TRACELOG3("Recv select2 failed with result=%d, socketErrno=%d\n",
                      result, socketErrno);
            return (FTALK_IO_ERROR);
         }
         result  = recv(tcpSocket, (char *) &bufferArr[bytesReceived],
                        bytesExpected - bytesReceived, 0);
         if (result == SOCKET_ERROR)
            return (FTALK_IO_ERROR);
         if (result == 0)
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
   // - Function code ok
   // - Address ok
   // Note:
   //   Some Schneider PLC's reply with address field zero in case of an exception.
   if (((bufferArr[7] & (char) 0x7F) != (char) function) ||
       ((bufferArr[6] != (char) address) && (bufferArr[6] != 0)))
      return (FTALK_INVALID_FRAME_ERROR);

   //
   // Handle exception responses
   //
   if ((signed char) bufferArr[7] < 0)
      return (FTALK_MBUS_EXCEPTION_RESPONSE + bufferArr[8]);

   // Adjust to payload count
   bytesReceived -= FRAME_LEN;

   // Copy data into result buffer. We copy only up to rcvDataLen.
   if (rcvDataLen > bytesReceived)
      rcvDataLen = bytesReceived;
   memcpy(rcvDataArr, &bufferArr[8], rcvDataLen);
   if (actualRcvdPtr)
     *actualRcvdPtr = bytesReceived;

   successCounter++;
   return (FTALK_SUCCESS);
}


/*****************************************************************************
 * User Defined Function Codes (65-72 and 100-110)
 *****************************************************************************/

/**
 * Send/Receive ADAM 5000/6000 ASCII command.
 * @ingroup devicespecific
 *
 * Sends an ADAM 5000/6000 ASCII command to the device and receives the reply
 * as ASCII string. (e.g. "$01M" to retrieve the module name)
 *
 * @param commandSz Buffer which holds command string. Must not be longer
 * than 255 characters.
 * @param responseSz Buffer which holds response string. Must be
 * a buffer of 256 bytes. A possible trailing CR is removed.
 *
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note No broadcast supported
 */
int MbusTcpMasterProtocol::adamSendReceiveAsciiCmd(const char * const commandSz,
                                                   char* responseSz)
{
   int i;
   int result;
   int charCnt;
   char myBufferArr [5 + 256 + 1];

   TRACELOG2("adamSendReceiveAsciiCmd send:\n%s\n", commandSz);

   charCnt = (int) strlen(commandSz);
   if ((charCnt > 255) || (charCnt == 0))
       return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   //
   // Copy data into buffer
   //
   memcpy(&myBufferArr[5], commandSz, charCnt);
   if (commandSz[charCnt - 1] != 13)
   {
       // Add CR
       myBufferArr[5 + charCnt] = 13;
       charCnt++;
   }
   // Padding to even byte count
   if ((charCnt % 2) != 0)
   {
       myBufferArr[5 + charCnt] = 0;
       charCnt++;
   }

   //
   // Transmit - receive first message with ASCII command
   //
   myBufferArr[0] = (char) 0x27;
   myBufferArr[1] = (char) 0x0F;
   myBufferArr[2] = (char) ((charCnt / 2)  >> 8);
   myBufferArr[3] = (char) ((charCnt / 2) & 0xFF);
   myBufferArr[4] = (char) (charCnt);
   result = transceiveMessage(1, 0x10, myBufferArr,
                              5 + charCnt, myBufferArr, 4, NULL);

   if (result != FTALK_SUCCESS)
      return (result);

   //
   // Transmit - receive second message to retrieve ASCII cmd response
   //
   myBufferArr[0] = (char) 0x27;
   myBufferArr[1] = (char) 0x0F;
   myBufferArr[2] = (char) 0x00;
   myBufferArr[3] = (char) 0x28; // 6000 series uses here 0x80, 5000 0x28
   result = transceiveMessage(1, 0x03, myBufferArr, 4, myBufferArr, 1, NULL);

   if (result != FTALK_SUCCESS)
      return (result);

   //
   // Now read appended data
   //
   result  = recv(tcpSocket, responseSz, 255, 0);
   if (result == SOCKET_ERROR)
      return (FTALK_IO_ERROR);
   if (result == 0)
      return (FTALK_CONNECTION_WAS_CLOSED);

   TRACELOG2("Recv(%d): ", result);
   for (i = 0; i < result; i++)
      TRACEADD2("%02X ", (int) responseSz[i] & 0xFF);
   TRACEADD1("\n");

   // Search end of string and strip CR
   responseSz[result] = 0; // Terminate string
   for (i = 0; i < result; i++)
      if (responseSz[i] == 13)
      {
         responseSz[i] = 0;
         break;
      }

   TRACELOG2("adamSendReceiveAsciiCmd recv: %s\n", responseSz);

   return (FTALK_SUCCESS);
}

