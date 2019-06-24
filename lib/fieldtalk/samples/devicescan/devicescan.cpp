/**
 * @file devicescan.cpp
 *
 * A simple scheduler based device scanner which polls cyclically
 * two PML power meters.
 *
 * @if NOTICE
 *
 * Copyright (c) proconX Pty Ltd. All rights reserved.
 *
 * The following source file constitutes example program code and is
 * intended merely to illustrate useful programming techniques.  The user
 * is responsible for applying the code correctly.
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
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#endif

// Include FieldTalk package header
#include "ScanScheduler.hpp"
#include "MbusRtuMasterProtocol.hpp"


/*****************************************************************************
 * Gobal configuration data
 *****************************************************************************/

#if defined(__WIN32__)
   TCHAR *portName = TEXT("COM1");
#elif defined(__LINUX__)
   char *portName = "/dev/ttyS0";
#elif defined(__MACOSX__)
   char *portName = "/dev/ttys0";
#elif defined(__FREEBSD__) || defined(__NETBSD__) || defined(__OPENBSD__)
   char *portName = "/dev/ttyd0";
#elif defined(__QNX__)
   char *portName = "/dev/ser1";
#elif defined(__VXWORKS__)
   char *portName = "/tyCo/0";
#elif defined(__SOLARIS__)
   char *portName = "/dev/ttya";
#elif defined(__IRIX__)
   char *portName = "/dev/ttyf1";
#elif defined(__OSF__)
   char *portName = "/dev/tty00";
#else
#  error Unknown serial port name, please add an entry for portName
#endif

long baudRate = 19200;
int stopBits = 1;
int retries = 2;
int parity = MbusSerialMasterProtocol::SER_PARITY_EVEN;


/*****************************************************************************
 * PML power meter modbus Data Tables
 *****************************************************************************/

#include "pml3270.hpp"
#include "pml7300.hpp"


/*****************************************************************************
 * PML 7300 series power meter Device Definition
 *****************************************************************************/

class Pml7300Device: public MbusDevice
{

  public:

   Pml7300Device(int deviceIdx, int slaveAddr):
      MbusDevice(slaveAddr)
   {
      addDataTable(new Pml7300DataTable1(deviceIdx));
      addDataTable(new Pml7300DataTable2(deviceIdx));
      addDataTable(new Pml7300DataTable4(deviceIdx));
      addDataTable(new Pml7300DataTableExtBool(deviceIdx));
   }

 };


/*****************************************************************************
 * PML 3720 series power meter Device Definition
 *****************************************************************************/

class Pml3720Device: public MbusDevice
{

  public:

   Pml3720Device(int deviceIdx, int slaveAddr):
      MbusDevice(slaveAddr)
   {
      addDataTable(new Pml3720DataTable1(deviceIdx));
      addDataTable(new Pml3720DataTable2(deviceIdx));
   }

 };


/*****************************************************************************
 * Application's Scanlist Definition
 *****************************************************************************/

class MyMbusScanList: public MbusDeviceScanList
{

  public:

   MyMbusScanList(MbusMasterFunctions *mbusProtocolPtr):
      MbusDeviceScanList(mbusProtocolPtr)
   {
      addDevice(new Pml7300Device(1, 51));
      addDevice(new Pml7300Device(2, 52));
      addDevice(new Pml3720Device(3, 53));
      addDevice(new Pml3720Device(4, 54));
   }

};


/*****************************************************************************
 * Gobal data
 *****************************************************************************/

MbusRtuMasterProtocol mbusProtocol;
MyMbusScanList mbusScanList(&mbusProtocol);


/*****************************************************************************
 * Function implementation
 *****************************************************************************/

/**
 * Opens protocol
 */
void openProtocol()
{
   int result;

   result = mbusProtocol.setRetryCnt(retries);
   if (result != FTALK_SUCCESS)
   {
      fprintf(stderr, "Error setting retry count: %s!\n",
                       getBusProtocolErrorText(result));
      exit(EXIT_FAILURE);
   }
   result = mbusProtocol.openProtocol(portName, baudRate,
                                      MbusSerialMasterProtocol::SER_DATABITS_8,
                                      stopBits, parity);
   if (result != FTALK_SUCCESS)
   {
      fprintf(stderr, "Error opening serial port: %s!\n",
                       getBusProtocolErrorText(result));
      exit(EXIT_FAILURE);
   }
}


/**
 * Closes protocol
 */
void closeProtocol()
{
   mbusProtocol.closeProtocol();
}


/**
 * Cyclic loop which polls all data table from all devices
 */
void runPollLoop()
{
   for (;;)
   {
      unsigned long waitTimeMillis;

      waitTimeMillis = mbusScanList.schedule();
      HmTimer::sleepMillis(waitTimeMillis);
   }
}


/**
 * Main function.
 *
 * @return Error code: 0 = OK, else error
 */
int main()
{
   openProtocol();

   runPollLoop();

   closeProtocol();
   return (EXIT_SUCCESS);
}
