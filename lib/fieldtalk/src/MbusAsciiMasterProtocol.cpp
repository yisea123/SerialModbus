/**
 * @file MbusAsciiMasterProtocol.cpp
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

// Package header
#include "hmtracelog.h"
#include "hmhex.h"
#include "hmserio.hpp"
#include "hmtimer.hpp"
#include "MbusAsciiMasterProtocol.hpp"


/*****************************************************************************
 * MbusAsciiMasterProtocol class implementation
 *****************************************************************************/


/**
 * Constructs a MbusAsciiMasterProtocol object and initialises its data.
 */
MbusAsciiMasterProtocol::MbusAsciiMasterProtocol()
{
}


/**
 * Sends a Modbus ASCII query frame and receives the corresponding reply
 * frame.
 *
 * @param address Modbus slave address used for data transfer
 * @param function Modbus function the data transfer is performed for
 * @param sendDataArr Data to be sent. This data is function specific.
 * @param sendDataLen Number of data bytes to be sent
 * @param rcvDataArr Data to receive. This data is function specific.
 * @param rcvDataLen Number of data bytes expected to be received
 * @return Error code. See @ref buserror for a list of error codes.
 */
int MbusAsciiMasterProtocol::transceiveMessage(int address, int function,
                                               char sendDataArr[],
                                               int sendDataLen,
                                               char rcvDataArr[],
                                               int rcvDataLen,
                                               int *actualRcvdPtr)
{
   int i, result, idx;
   int bytesReceived;
   int bytesExpected = rcvDataLen * 2 + FRAME_LEN;
   int bytesToDecode;
   char checkSum;
   int decodedByte; // Temp var must an int to distinguish -1 from 0xFF

   totalCounter++;

   if (actualRcvdPtr)
     *actualRcvdPtr = 0;

   //
   // Assemble MODBUS ASCII message
   //
   checkSum = 0;
   idx = 0;
   bufferArr[idx++]  = ':';
   bufferArr[idx++]  = hiNibbletoHex((char) address);
   bufferArr[idx++]  = loNibbletoHex((char) address);
   checkSum = checkSum + (char) address;
   bufferArr[idx++]  = hiNibbletoHex((char) function);
   bufferArr[idx++]  = loNibbletoHex((char) function);
   checkSum = checkSum + (char) function;

   //
   // Copy and encode data into send buffer
   //
   for (i = 0; i < sendDataLen; i++)
   {
      bufferArr[idx++] = hiNibbletoHex(sendDataArr[i]);
      bufferArr[idx++] = loNibbletoHex(sendDataArr[i]);
      checkSum = checkSum + sendDataArr[i];
   }

   //
   // Calculate LRC
   //
   checkSum = (char) -checkSum;
   bufferArr[idx++] = hiNibbletoHex((char) checkSum);
   bufferArr[idx++] = loNibbletoHex((char) checkSum);
   bufferArr[idx++] = 0x0D;
   bufferArr[idx++] = 0x0A;

   // Clear buffers from garbage
   serialPort.flush();

   //
   // Do poll delay
   //
   if (pollDelay > 0)
      silenceTimer.sleepUntilExpiry();
   silenceTimer.startMillis(pollDelay);

   //
   // Send request to slave
   //
   TRACELOG2("Send(%d): ", idx);
   for  (i = 0; i < idx; i++)
      TRACEADD2("%02X ", (int) bufferArr[i] & 0xFF);
   TRACEADD1("\n");

   if (serialMode == SER_RS485)
      serialPort.setRts();
   result = serialPort.send((char *) bufferArr, idx, timeOut);
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
      case HmSerialPort::SER_LINE_ERROR:
      return (FTALK_REPLY_TIMEOUT_ERROR);
      default:
         TRACELOG2("serialPort.receive failed with result=%d\n", result);
      return (FTALK_IO_ERROR);
   }
   // If no exception message received, read the remainder of the message
   // Any function code > 0x80 is an exception msg
   if ((bufferArr[3] < '8')  && (bytesExpected - EXC_MSG_LEN > 0))
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
   // Encode buffer
   //
   checkSum = 0;
   // bytesToDecode: Deduct 3 for colon, cr & lf, divide it by two because of
   // hex coding and deduct one for the checksum
   bytesToDecode = (bytesReceived - 3) / 2 - 1;
   if (bufferArr[0] != ':')
   {
      TRACELOG2("No colon: %x\n", bufferArr[0]);
      return (FTALK_INVALID_FRAME_ERROR);
   }
   idx = 1; // Drop the colon, so start at 1
   for (i = 0; i < bytesToDecode; i++, idx += 2)
   {
      decodedByte = hexToByte(bufferArr[idx], bufferArr[idx + 1]);
      if (decodedByte == -1)
         return (FTALK_INVALID_FRAME_ERROR);
      bufferArr[i] = (char) decodedByte;
      checkSum = checkSum + bufferArr[i];
   }
   // Checksum is handled seperatly here, we don't add it to checkSum
   decodedByte = hexToByte(bufferArr[idx], bufferArr[idx + 1]);
   if (decodedByte == -1)
      return (FTALK_INVALID_FRAME_ERROR);
   bufferArr[i] = (char) decodedByte;

   //
   // Check frame
   // - Checksum ok
   // - Function code ok
   // - Address ok
   // - No exception: Bytes received = bytes expected
   // - Exception: Bytes received
   //
   checkSum = (char) -checkSum;
   if (checkSum !=  bufferArr[bytesToDecode])
      return (FTALK_CHECKSUM_ERROR);
   if (((bufferArr[1] & (char) 0x7F) != (char) function) ||
       (bufferArr[0] != (char) address))
      return (FTALK_INVALID_FRAME_ERROR);

   //
   // Handle exception responses
   //
   if ((signed char) bufferArr[1] < 0)
      return (FTALK_MBUS_EXCEPTION_RESPONSE + bufferArr[2]);

   // Copy data into result buffer, strip address and function code
   memcpy(rcvDataArr, &bufferArr[2], rcvDataLen);
   if (actualRcvdPtr)
     *actualRcvdPtr = bytesReceived;

   successCounter++;
   return (FTALK_SUCCESS);
}
