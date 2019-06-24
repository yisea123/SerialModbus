/**
 * @file hmserio.hpp
 *
 * Serial port I/O header file
 *
 * @if NOTICE
 *
 * $Id: hmserio.hpp,v 1.26 2012/03/28 05:57:55 henrik Exp $
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


#ifndef _HMSERIO_H_INCLUDED
#define _HMSERIO_H_INCLUDED

#ifndef __cplusplus
#  error Must use C++ to compile this module!
#endif

// HM platform detection
#include "hmplatf.h"

// Platform header
#include <stdlib.h>
#if defined(__WIN32__)
#  include <windows.h>
#elif defined(__VXWORKS__)
#  include <ioLib.h>
#  include <selectLib.h>
#elif defined(__UNIX__)
#  include <sys/select.h> // Needed by QNX4
#  include <termios.h>
#else
#  error "Platform not yet supported. Please add a new condition to this #if block or update hmplatf.h"
#endif


/*****************************************************************************
 * Serial Port I/O interface
 *****************************************************************************/

/**
 * Serial Port class. This class implements a platform independant access
 * to serial ports.
 */
class HmSerialPort
{
  public:

   enum
   {
      SER_SUCCESS = 0,       ///< No error
      SER_ERR_CLASS = 0x1100,///< Serial error class
      SER_INVALID_PARAMETER, ///< Parameter invalid or not supported
      SER_API_ERROR,         ///< Underlaying API returns an error
      SER_PORT_NOT_FOUND,    ///< Serial port not found
      SER_ALREADY_OPEN,      ///< Serial port already open
      SER_NOT_OPEN,          ///< Port not opened
      SER_TIME_OUT,          ///< Operation timed out
      SER_LINE_ERROR,        ///< RS232 line error
      /// Operation not supported in current configuration
      SER_UNSUPPORTED_OPERATION,
      SER_PORT_NO_ACCESS     ///< No access rights for port (Try as root)
   };

   enum
   {
      SER_DATABITS_7 = 7, ///< 7 data bits
      SER_DATABITS_8 = 8  ///< 8 data bits
   };

   enum
   {
      SER_STOPBITS_1 = 1, ///< 1 stop bit
      SER_STOPBITS_2 = 2  ///< 2 stop bits
   };

   enum
   {
      SER_PARITY_NONE = 0, ///< No parity
      SER_PARITY_ODD  = 1, ///< Odd parity
      SER_PARITY_EVEN = 2  ///< Even parity
   };

   enum
   {
      SER_HANDSHAKE_NONE    = 0, ///< No handshake
#if defined(__WIN32__)
      SER_HANDSHAKE_RTS_TOGGLE = 2, ///< RTS half-duplex toggle for RS-485 mode
#endif
      SER_HANDSHAKE_RTS_CTS = 1 ///< RTS/CTS hardware handshake
   };


   HmSerialPort();

   ~HmSerialPort();

   int openPort(const char * const name);

#if defined (__WIN32__)
   int openHandle(HANDLE portHandle);
#endif

   void closePort();

   int config(long baudRate, int dataBits,
              int stopBits, int parity, int flowControl);

   int send(const char *bufPtr, size_t len, long timeOut);

   int receive(const char *bufPtr, size_t len, long timeOut);

   int receiveByte(const char *charBuf, unsigned long microTimeOut);

   int flush();

   int drain();

   int setRts();

   int clearRts();

   int isOpen();


  protected:

   int baudRate;
   int flowControl;
#if defined (__WIN32__)
   HANDLE port;
#if !defined(_WIN32_WCE)
   OVERLAPPED ovlStruct;
#endif
   COMMTIMEOUTS commTimeOutBlock;
   DCB savedDevCtrlBlock;
#elif defined(__VXWORKS__)
   int port;
   int channel;
#else
   int port;
   struct termios savedTermioAttr;
#ifdef __LINUX__
   int savedSerialFlags;
#endif
#endif


  private:

   // Disable default operator and copy constructor
   HmSerialPort &operator=(HmSerialPort &);
   HmSerialPort(const HmSerialPort &);

};


#endif // ifdef ..._H_INCLUDED
