/**
 * @file MbusMasterCwrapper.cpp
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

// Package header
#include "MbusRtuMasterProtocol.hpp"
#include "MbusAsciiMasterProtocol.hpp"
#include "MbusTcpMasterProtocol.hpp"
#include "MbusRtuOverTcpMasterProtocol.hpp"
#include "MbusMasterCexports.h" // Last include


/*****************************************************************************
 * C wrapper functions to create and destroy protocol objects
 *****************************************************************************/

extern "C" EXPORT MbusMasterProtocolHdl STDCALL
mbusMaster_createTcpProtocol(void)
{
   return(new MbusTcpMasterProtocol());
}

extern "C" EXPORT MbusMasterProtocolHdl STDCALL
mbusMaster_createRtuOverTcpProtocol(void)
{
   return(new MbusRtuOverTcpMasterProtocol());
}

extern "C" EXPORT MbusMasterProtocolHdl STDCALL
mbusMaster_createRtuProtocol(void)
{
   return(new MbusRtuMasterProtocol());
}


extern "C" EXPORT MbusMasterProtocolHdl STDCALL
mbusMaster_createAsciiProtocol(void)
{
   return(new MbusAsciiMasterProtocol());
}


extern "C" EXPORT void STDCALL
mbusMaster_destroyProtocol(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl != NULL)
      delete (MbusMasterFunctions *) mbusHdl;
}


/*****************************************************************************
 * C wrapper function for Bit Access Modbus Function Codes
 *****************************************************************************/

extern "C" EXPORT int STDCALL
mbusMaster_readCoils(MbusMasterProtocolHdl mbusHdl,
                     int slaveAddr, int startRef,
                     int bitArr[], int refCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          readCoils(slaveAddr, startRef, bitArr, refCnt));
}


extern "C" EXPORT int STDCALL
mbusMaster_readInputDiscretes(MbusMasterProtocolHdl mbusHdl,
                              int slaveAddr, int startRef,
                              int bitArr[], int refCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          readInputDiscretes(slaveAddr, startRef, bitArr, refCnt));
}


extern "C" EXPORT int STDCALL
mbusMaster_writeCoil(MbusMasterProtocolHdl mbusHdl,
                     int slaveAddr, int bitAddr, int bitVal)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          writeCoil(slaveAddr, bitAddr, bitVal));
}


extern "C" EXPORT int STDCALL
mbusMaster_forceMultipleCoils(MbusMasterProtocolHdl mbusHdl,
                              int slaveAddr, int startRef,
                              const int bitArr[], int refCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          forceMultipleCoils(slaveAddr, startRef, bitArr, refCnt));
}


/*****************************************************************************
 * C wrapper function for 16-bit Access Modbus Function Codes
 *****************************************************************************/

extern "C" EXPORT int STDCALL
mbusMaster_readMultipleRegisters(MbusMasterProtocolHdl mbusHdl,
                                 int slaveAddr,
                                 int startRef,
                                 short regArr[], int refCnt)

{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          readMultipleRegisters(slaveAddr, startRef, regArr, refCnt));
}


extern "C" EXPORT int STDCALL
mbusMaster_readInputRegisters(MbusMasterProtocolHdl mbusHdl,
                              int slaveAddr, int startRef,
                              short regArr[], int refCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          readInputRegisters(slaveAddr, startRef, regArr, refCnt));
}


extern "C" EXPORT int STDCALL
mbusMaster_writeSingleRegister(MbusMasterProtocolHdl mbusHdl,
                               int slaveAddr,
                               int regAddr,
                               short regVal)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          writeSingleRegister(slaveAddr, regAddr, regVal));
}


extern "C" EXPORT int STDCALL
mbusMaster_writeMultipleRegisters(MbusMasterProtocolHdl mbusHdl,
                                  int slaveAddr,
                                  int startRef,
                                  const short regArr[],
                                  int refCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          writeMultipleRegisters(slaveAddr, startRef, regArr, refCnt));
}


extern "C" EXPORT int STDCALL
mbusMaster_maskWriteRegister(MbusMasterProtocolHdl mbusHdl,
                             int slaveAddr,
                             int regAddr,
                             short andMask,
                             short orMask)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          maskWriteRegister(slaveAddr, regAddr, andMask, orMask));
}


extern "C" EXPORT int STDCALL
mbusMaster_readWriteRegisters(MbusMasterProtocolHdl mbusHdl,
                              int slaveAddr,
                              int readRef,
                              short readArr[], int readCnt,
                              int writeRef,
                              const short writeArr[],
                              int writeCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          readWriteRegisters(slaveAddr, readRef, readArr, readCnt,
                             writeRef, writeArr, writeCnt));
}


/*****************************************************************************
 * C wrapper functions for 32-bit Access Modbus Function Codes
 *****************************************************************************/

extern "C" EXPORT int STDCALL
mbusMaster_readMultipleLongInts(MbusMasterProtocolHdl mbusHdl,
                                int slaveAddr,
                                int startRef,
                                int int32Arr[], int refCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          readMultipleLongInts(slaveAddr, startRef, int32Arr, refCnt));
}


extern "C" EXPORT int STDCALL
mbusMaster_readInputLongInts(MbusMasterProtocolHdl mbusHdl,
                             int slaveAddr,
                             int startRef,
                             int int32Arr[], int refCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          readInputLongInts(slaveAddr, startRef, int32Arr, refCnt));
}


extern "C" EXPORT int STDCALL
mbusMaster_writeMultipleLongInts(MbusMasterProtocolHdl mbusHdl,
                                 int slaveAddr,
                                 int startRef,
                                 const int int32Arr[],
                                 int refCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          writeMultipleLongInts(slaveAddr, startRef, int32Arr, refCnt));
}


extern "C" EXPORT int STDCALL
mbusMaster_readMultipleFloats(MbusMasterProtocolHdl mbusHdl,
                              int slaveAddr,
                              int startRef,
                              float float32Arr[], int refCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          readMultipleFloats(slaveAddr, startRef, float32Arr, refCnt));
}


extern "C" EXPORT int STDCALL
mbusMaster_readInputFloats(MbusMasterProtocolHdl mbusHdl,
                           int slaveAddr,
                           int startRef,
                           float float32Arr[], int refCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          readInputFloats(slaveAddr, startRef, float32Arr, refCnt));
}


extern "C" EXPORT int STDCALL
mbusMaster_writeMultipleFloats(MbusMasterProtocolHdl mbusHdl,
                               int slaveAddr,
                               int startRef,
                               const float float32Arr[],
                               int refCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          writeMultipleFloats(slaveAddr, startRef, float32Arr, refCnt));
}


extern "C" EXPORT int STDCALL
mbusMaster_readMultipleMod10000(MbusMasterProtocolHdl mbusHdl,
                                int slaveAddr,
                                int startRef,
                                int int32Arr[], int refCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          readMultipleMod10000(slaveAddr, startRef, int32Arr, refCnt));
}


extern "C" EXPORT int STDCALL
mbusMaster_readInputMod10000(MbusMasterProtocolHdl mbusHdl,
                             int slaveAddr,
                             int startRef,
                             int int32Arr[], int refCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          readInputMod10000(slaveAddr, startRef, int32Arr, refCnt));
}


extern "C" EXPORT int STDCALL
mbusMaster_writeMultipleMod10000(MbusMasterProtocolHdl mbusHdl,
                                 int slaveAddr,
                                 int startRef,
                                 const int int32Arr[],
                                 int refCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          writeMultipleMod10000(slaveAddr, startRef, int32Arr, refCnt));
}


/*****************************************************************************
 * C wrapper function for Diagnostics Function Codes
 *****************************************************************************/

extern "C" EXPORT int STDCALL
mbusMaster_readExceptionStatus(MbusMasterProtocolHdl mbusHdl,
                               int slaveAddr,
                               unsigned char *statusByte)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          readExceptionStatus(slaveAddr, statusByte));
}


extern "C" EXPORT int STDCALL
mbusMaster_returnQueryData(MbusMasterProtocolHdl mbusHdl,
                           int slaveAddr,
                           const unsigned char queryArr[],
                           unsigned char echoArr[], int len)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          returnQueryData(slaveAddr, queryArr, echoArr, len));
}


extern "C" EXPORT int STDCALL
mbusMaster_restartCommunicationsOption(MbusMasterProtocolHdl mbusHdl,
                                       int slaveAddr,
                                       int clearEventLog)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->
          restartCommunicationsOption(slaveAddr, clearEventLog));
}


/*****************************************************************************
 * C wrapper function for User Defined Function Codes (65-72 and 100-110)
 *****************************************************************************/

extern "C" EXPORT int STDCALL
mbusMaster_customFunction(MbusMasterProtocolHdl mbusHdl,
                          int slaveAddr,
                          int functionCode,
                          void* requestData, size_t requestLen,
                          void* responseData, size_t *responseLenPtr)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusTcpMasterProtocol *) mbusHdl)->
          customFunction(slaveAddr, functionCode,
                         requestData, requestLen,
                         responseData, responseLenPtr));
}


/*****************************************************************************
 * C wrapper functions for Protocol Configuration
 *****************************************************************************/

extern "C" EXPORT int STDCALL
mbusMaster_setTimeout(MbusMasterProtocolHdl mbusHdl, int timeOut)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->setTimeout(timeOut));
}


extern "C" EXPORT int STDCALL
mbusMaster_getTimeout(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return(0);
   return(((MbusMasterFunctions *) mbusHdl)->getTimeout());
}


extern "C" EXPORT int STDCALL
mbusMaster_setPollDelay(MbusMasterProtocolHdl mbusHdl, int pollDelay)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->setPollDelay(pollDelay));
}


extern "C" EXPORT int STDCALL
mbusMaster_getPollDelay(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return(0);
   return(((MbusMasterFunctions *) mbusHdl)->getPollDelay());
}


extern "C" EXPORT int STDCALL
mbusMaster_setRetryCnt(MbusMasterProtocolHdl mbusHdl, int retryCnt)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusMasterFunctions *) mbusHdl)->setRetryCnt(retryCnt));
}


extern "C" EXPORT int STDCALL
mbusMaster_getRetryCnt(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return(0);
   return(((MbusMasterFunctions *) mbusHdl)->getRetryCnt());
}


/*****************************************************************************
* C wrapper functions for Transmission Statistic Functions
*****************************************************************************/

extern "C" EXPORT long STDCALL
mbusMaster_getTotalCounter(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return(0);
   return(((MbusMasterFunctions *) mbusHdl)->getTotalCounter());
}


extern "C" EXPORT void STDCALL
mbusMaster_resetTotalCounter(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl != NULL)
      ((MbusMasterFunctions *) mbusHdl)->resetTotalCounter();
}


extern "C" EXPORT long STDCALL
mbusMaster_getSuccessCounter(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return(0);
   return(((MbusMasterFunctions *) mbusHdl)->getSuccessCounter());
}


extern "C" EXPORT void STDCALL
mbusMaster_resetSuccessCounter(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl != NULL)
      ((MbusMasterFunctions *) mbusHdl)->resetSuccessCounter();
}


/*****************************************************************************
 * C wrapper functions for Slave Configuration
 *****************************************************************************/

extern "C" EXPORT void STDCALL
mbusMaster_configureBigEndianInts(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return;
   ((MbusMasterFunctions *) mbusHdl)->configureBigEndianInts();
}


extern "C" EXPORT void STDCALL
mbusMaster_configureLittleEndianInts(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return;
   ((MbusMasterFunctions *) mbusHdl)->configureLittleEndianInts();
}


extern "C" EXPORT void STDCALL
mbusMaster_configureIeeeFloats(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return;
   ((MbusMasterFunctions *) mbusHdl)->configureIeeeFloats();
}


extern "C" EXPORT void STDCALL
mbusMaster_configureSwappedFloats(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return;
   ((MbusMasterFunctions *) mbusHdl)->configureSwappedFloats();
}


extern "C" EXPORT void STDCALL
mbusMaster_configureStandard32BitMode(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return;
   ((MbusMasterFunctions *) mbusHdl)->configureStandard32BitMode();
}


extern "C" EXPORT void STDCALL
mbusMaster_configureEnron32BitMode(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return;
   ((MbusMasterFunctions *) mbusHdl)->configureEnron32BitMode();
}


extern "C" EXPORT void STDCALL
mbusMaster_configureCountFromOne(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return;
   ((MbusMasterFunctions *) mbusHdl)->configureCountFromOne();
}


extern "C" EXPORT void STDCALL
mbusMaster_configureCountFromZero(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return;
   ((MbusMasterFunctions *) mbusHdl)->configureCountFromZero();
}

//
// Per slave functions
//

extern "C" EXPORT int STDCALL
mbusMaster_configSlaveBigEndianInts(MbusMasterProtocolHdl mbusHdl, int slaveAddr)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return ((MbusMasterFunctions *) mbusHdl)->configureBigEndianInts(slaveAddr);
}


extern "C" EXPORT int STDCALL
mbusMaster_configSlaveLittleEndianInts(MbusMasterProtocolHdl mbusHdl, int slaveAddr)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return ((MbusMasterFunctions *) mbusHdl)->configureLittleEndianInts(slaveAddr);
}


extern "C" EXPORT int STDCALL
mbusMaster_configSlaveIeeeFloats(MbusMasterProtocolHdl mbusHdl, int slaveAddr)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return ((MbusMasterFunctions *) mbusHdl)->configureIeeeFloats(slaveAddr);
}


extern "C" EXPORT int STDCALL
mbusMaster_configSlaveSwappedFloats(MbusMasterProtocolHdl mbusHdl, int slaveAddr)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return ((MbusMasterFunctions *) mbusHdl)->configureSwappedFloats(slaveAddr);
}


extern "C" EXPORT int STDCALL
mbusMaster_configSlaveStandard32BitMode(MbusMasterProtocolHdl mbusHdl, int slaveAddr)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return ((MbusMasterFunctions *) mbusHdl)->configureStandard32BitMode(slaveAddr);
}


extern "C" EXPORT int STDCALL
mbusMaster_configSlaveEnron32BitMode(MbusMasterProtocolHdl mbusHdl, int slaveAddr)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return ((MbusMasterFunctions *) mbusHdl)->configureEnron32BitMode(slaveAddr);
}


extern "C" EXPORT int STDCALL
mbusMaster_configSlaveCountFromOne(MbusMasterProtocolHdl mbusHdl, int slaveAddr)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return ((MbusMasterFunctions *) mbusHdl)->configureCountFromOne(slaveAddr);
}


extern "C" EXPORT int STDCALL
mbusMaster_configSlaveCountFromZero(MbusMasterProtocolHdl mbusHdl, int slaveAddr)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return ((MbusMasterFunctions *) mbusHdl)->configureCountFromZero(slaveAddr);
}


/*****************************************************************************
 * C wrapper functions for Utility routines
 *****************************************************************************/

extern "C" EXPORT void STDCALL
mbusMaster_closeProtocol(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl != NULL)
      ((MbusMasterFunctions *) mbusHdl)->closeProtocol();
}


extern "C" EXPORT int STDCALL
mbusMaster_isOpen(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return(0);
   return(((MbusMasterFunctions *) mbusHdl)->isOpen());
}


extern "C" EXPORT const TCHAR * STDCALL
mbusMaster_getPackageVersion()
{
   return(MbusMasterFunctions::getPackageVersion());
}


extern "C" EXPORT const TCHAR * STDCALL
mbusMaster_getErrorText(int errCode)
{
   return(getBusProtocolErrorText(errCode));
}


/*****************************************************************************
 * C wrapper functions for MbusTcpMasterProtocol class
 *****************************************************************************/

extern "C" EXPORT int STDCALL
mbusMaster_openTcpProtocol(MbusMasterProtocolHdl mbusHdl,
                           const TCHAR * const hostName)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusTcpMasterProtocol *) mbusHdl)->openProtocol(hostName));
}


extern "C" EXPORT int STDCALL
mbusMaster_setTcpPort(MbusMasterProtocolHdl mbusHdl, unsigned short portNo)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusTcpMasterProtocol *) mbusHdl)->setPort(portNo));
}


extern "C" EXPORT unsigned short  STDCALL
mbusMaster_getTcpPort(MbusMasterProtocolHdl mbusHdl)
{
   if (mbusHdl == NULL)
      return(0);
   return(((MbusTcpMasterProtocol *) mbusHdl)->getPort());
}


extern "C" EXPORT int STDCALL
mbusMaster_adamSendReceiveAsciiCmd(MbusMasterProtocolHdl mbusHdl,
                                   const char * const commandSz,
                                   char* responseSz)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusTcpMasterProtocol *) mbusHdl)->
          adamSendReceiveAsciiCmd(commandSz, responseSz));
}


/*****************************************************************************
 * C wrapper functions for MbusSerialMasterProtocol class
 *****************************************************************************/

extern "C" EXPORT int STDCALL
mbusMaster_openSerialProtocol(MbusMasterProtocolHdl mbusHdl,
                              const TCHAR * const portName,
                              long baudRate, int dataBits,
                              int stopBits, int parity)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusSerialMasterProtocol *) mbusHdl)->openProtocol(portName,
                                                               baudRate,
                                                               dataBits,
                                                               stopBits,
                                                               parity));
}


#if defined (__WIN32__)
extern "C" EXPORT int STDCALL
mbusMaster_openUseExistingConnection(MbusMasterProtocolHdl mbusHdl,
                                     HANDLE cnxnHandle)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusSerialMasterProtocol *) mbusHdl)->openUseExistingConnection(cnxnHandle));
}
#endif


extern "C" EXPORT int STDCALL
mbusMaster_enableRs485Mode(MbusMasterProtocolHdl mbusHdl, int rtsDelay)
{
   if (mbusHdl == NULL)
      return(FTALK_ILLEGAL_STATE_ERROR);
   return(((MbusSerialMasterProtocol *) mbusHdl)->enableRs485Mode(rtsDelay));
}



/*****************************************************************************
 * DLL entry point
 *****************************************************************************/

#if defined(_WIN32) || defined(__WIN32__)

/**
 * DLL entry point for Windows 32 bit API
 *
 * @param hModule A handle to the DLL. The value is the base address of
 * the DLL. The HINSTANCE of a DLL is the same as the HMODULE of the DLL,
 * so hinstDLL can be used in subsequent calls to the GetModuleFileName
 * function and other functions that require a module handle.
 * @param ul_reason_for_call Specifies a flag indicating why the DLL entry-point
 * function is being called. See Win32 API.
 * @param lpReserved Specifies further aspects of DLL initialization and
 * cleanup.  If fdwReason is DLL_PROCESS_ATTACH, lpvReserved is NULL for
 * dynamic loads and non-NULL for static loads.  If fdwReason is
 * DLL_PROCESS_DETACH, lpvReserved is NULL if  DllEntryPoint has been called
 * by using FreeLibrary and non-NULL if DllEntryPoint has been called
 * during process termination.
 * @return TRUE = Successful, FALSE = Init error
 */
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
   (void) hModule; (void) ul_reason_for_call; (void) lpReserved; // Silence warning

   return TRUE;
}


#endif
