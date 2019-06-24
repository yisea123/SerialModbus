/**
 * @file MbusSerialMasterProtocol.hpp
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


#ifndef _MBUSSERIALMASTERPROTOCOL_H_INCLUDED
#define _MBUSSERIALMASTERPROTOCOL_H_INCLUDED

#ifndef __cplusplus
#  error Must use C++ to compile this module!
#endif

// Platform header
#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN32_WCE)
#  include <windows.h> /* Required for HANDLE */
#endif

// Package header
#include "hmplatf.h"
#include "MbusMasterFunctions.hpp"


/*****************************************************************************
 * Forward declaration
 *****************************************************************************/

class HmSerialPort;


/*****************************************************************************
 * MbusSerialMasterProtocol class declaration
 *****************************************************************************/

/**
 * @brief Base class for serial serial master protocols
 *
 * This base class realises the Modbus serial master protocols. It provides
 * functions to open and to close serial port as well as data and control
 * functions which can be used at any time after the protocol has been
 * opened. The data and control functions are organized different
 * conformance classes. For a more detailed description of the data and
 * control functions see section @ref mbusmaster.
 *
 * It is possible to instantiate multiple instances for establishing
 * multiple connections on different serial ports (They should be executed
 * in separate threads).
 *
 * @see mbusmaster, mbusmasterserial
 * @see MbusMasterFunctions,
 *      MbusRtuMasterProtocol, MbusAsciiMasterProtocol,
 *      MbusTcpMasterProtocol, MbusRtuOverTcpMasterProtocol
 */
class
#if defined (_WINDLL) || defined(__DLL__)
   __declspec(dllexport)
#endif
MbusSerialMasterProtocol: public MbusMasterFunctions
{

  public:

   enum
   {
      SER_DATABITS_7 = 7,    ///< 7 data bits
      SER_DATABITS_8 = 8     ///< 8 data bits
   };

   enum
   {
      SER_STOPBITS_1 = 1,    ///< 1 stop bit
      SER_STOPBITS_2 = 2     ///< 2 stop bits
   };

   enum
   {
      SER_PARITY_NONE = 0,   ///< No parity
      SER_PARITY_EVEN = 2,   ///< Even parity
      SER_PARITY_ODD = 1     ///< Odd parity
   };


  protected:

   HmSerialPort &serialPort;

   enum
   {
      SER_RS232, // RS232 mode w/o RTS/CTS handshake
      SER_RS485  // RS485 mode: RTS enables/disables transmitter
   };
   int serialMode;
   int rtsDelay;

   MbusSerialMasterProtocol();

   ~MbusSerialMasterProtocol();


  public:

   virtual int openProtocol(const char * const portName,
                            long baudRate, int dataBits,
                            int stopBits, int parity);

#ifdef __WIN32__
   int openProtocol(const wchar_t * const portName,
                    long baudRate, int dataBits,
                    int stopBits, int parity);

#  if defined(_NATIVE_WCHAR_T_DEFINED) // eVC++/VC++ 6.0 backwards-compatibility
   int openProtocol(const unsigned short * const portName, long baudRate,
                    int dataBits, int stopBits, int parity);
#  endif

   int openUseExistingConnection(HANDLE cnxnHandle);
#endif

   virtual void closeProtocol();

   virtual int isOpen();

   virtual int enableRs485Mode(int rtsDelay);


  protected:

   virtual int deliverMessage(int address, int function,
                              char sendDataArr[], int sendDataLen,
                              char rcvDataArr[], int rcvDataLen,
                              int *actualRcvdPtr = NULL);

   virtual int transceiveMessage(int address, int function,
                                 char sendDataArr[], int sendDataLen,
                                 char rcvDataArr[], int rcvDataLen,
                                 int *actualRcvdPtr) = 0;


  private:

   // Disable default operator and copy constructor
   MbusSerialMasterProtocol &operator= (MbusSerialMasterProtocol &);
   MbusSerialMasterProtocol (const MbusSerialMasterProtocol &);

};


#endif // ifdef ..._H_INCLUDED

