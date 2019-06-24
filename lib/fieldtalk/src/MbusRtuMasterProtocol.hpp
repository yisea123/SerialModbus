/**
 * @file MbusRtuMasterProtocol.hpp
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


#ifndef _MBUSRTUMASTERPROTOCOL_H_INCLUDED
#define _MBUSRTUMASTERPROTOCOL_H_INCLUDED

#ifndef __cplusplus
#  error Must use C++ to compile this module!
#endif

// Package header
#include "MbusSerialMasterProtocol.hpp"


/*****************************************************************************
 * MbusRtuMasterProtocol class declaration
 *****************************************************************************/

/**
 * @brief Modbus RTU Master Protocol class
 *
 * This class realizes the Modbus RTU master protocol. It provides
 * functions to open and to close serial port as well as data and control
 * functions which can be used at any time after the protocol has been
 * opened. The data and control functions are organized different
 * conformance classes. For a more detailed description of the data and
 * control functions see section @ref mbusmaster.
 *
 * It is possible to instantiate multiple instances of this class for
 * establishing multiple connections on different serial ports (They should
 * be executed in separate threads).
 *
 * @ingroup mbusmasterserial
 * @see mbusmaster, mbusmasterserial
 * @see MbusMasterFunctions,
 *      MbusSerialMasterProtocol, MbusAsciiMasterProtocol,
 *      MbusTcpMasterProtocol, MbusRtuOverTcpMasterProtocol
 */
class
#if defined (_WINDLL) || defined(__DLL__)
   __declspec(dllexport)
#endif
MbusRtuMasterProtocol: public MbusSerialMasterProtocol
{

  private:

   enum
   {
      SER_DATABITS_7 = 7     // Hide this
   };

   enum
   {
      // RTU header length is 2 bytes address/function
      HDR_LEN = 2 ,
      // RTU frame length is header + 2 bytes crc
      FRAME_LEN = HDR_LEN + 2,
      // RTU exception message length is frame + 1 byte exception code
      EXC_MSG_LEN = FRAME_LEN + 1,
      // RTU max. message size is data size + frame length
      MAX_MSG_SIZE = MAX_DATA_SIZE + FRAME_LEN
   };

   unsigned long frameSilence;
   char bufferArr[MAX_MSG_SIZE];


  public:

   MbusRtuMasterProtocol();

   virtual int openProtocol(const char * const portName, long baudRate,
                            int dataBits, int stopBits, int parity);

#ifdef __WIN32__
   int openProtocol(const wchar_t * const portName, long baudRate,
                    int dataBits, int stopBits, int parity);
#endif

  private:

   virtual int transceiveMessage(int address, int function,
                                 char sendDataArr[], int sendDataLen,
                                 char rcvDataArr[], int rcvDataLen,
                                 int *actualRcvdPtr);


  private:

   // Disable default operator and copy constructor
   MbusRtuMasterProtocol &operator= (MbusRtuMasterProtocol &);
   MbusRtuMasterProtocol (const MbusRtuMasterProtocol &);

};


#endif // ifdef ..._H_INCLUDED
