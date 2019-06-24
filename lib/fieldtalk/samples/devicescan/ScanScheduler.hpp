/**
 * @file ScanScheduler.hpp
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


#ifndef _SCANSCHEDULER_H_INCLUDED
#define _SCANSCHEDULER_H_INCLUDED

#ifndef __cplusplus
#  error Must use C++ to compile this module!
#endif

// Platform header
#include <stdlib.h>

// Package header
#include "hmtimer.hpp"
#include "MbusMasterFunctions.hpp"


class MbusDeviceScanList;
class MbusDevice;
class MbusDeviceDataTable;


/**
 * @brief This base class implements ...
 *
 *
 * @ingroup mbusscanscheduler
 */
class
#if defined (_WINDLL) || defined(__DLL__)
   __declspec(dllexport)
#endif
MbusDeviceDataTable
{

  protected:

   MbusDeviceDataTable(unsigned long scanFreq, int startRef,
                       void *dataBufPtr, size_t dataBufSize);


  public:

   virtual ~MbusDeviceDataTable();


   unsigned long schedule();

   MbusDeviceDataTable *getPrev() { return (prevDataTablePtr); }

   void setPrev(MbusDeviceDataTable *prevDataTablePtr)
   {
      this->prevDataTablePtr = prevDataTablePtr;
   }

   void setTableIndex(int tableIdx) { this->tableIdx = tableIdx; }

   void setParent(MbusDevice *parentDevicePtr)
   {
      this->parentDevicePtr = parentDevicePtr;
   }

   int getLastResult()
   {
      return (lastResult);
   }

  protected:

   int getDeviceIndex();

   int getTableIndex() { return (tableIdx); }

   int getTableStartRef() { return (startRef); }

   int getSlaveAddr();


  private:

   virtual void beforeScan() { }

   virtual void afterScan() { }

   virtual void failedScan(int result) { result = result; }


   virtual int performScan(MbusMasterFunctions *mbusProtocol,
                           int slaveAddr, int startRef,
                           void *bufPtr, size_t bufSize) = 0;


  private:

   MbusDevice *parentDevicePtr;
   MbusDeviceDataTable *prevDataTablePtr;

   int startRef;
   size_t dataBufSize;
   void *dataBufPtr;
   int tableIdx;
   unsigned long scanFreq;
   HmTimer scanTimer;
   int lastResult;


  private:

   // Disable default operator and copy constructor
   MbusDeviceDataTable &operator= (MbusDeviceDataTable &);
   MbusDeviceDataTable (const MbusDeviceDataTable &);

};


/**
 * @brief This class implements ...
 *
 *
 * @ingroup mbusscanscheduler
 */
class
#if defined (_WINDLL) || defined(__DLL__)
   __declspec(dllexport)
#endif
MbusReadRegisterTable:
    public MbusDeviceDataTable
{

  protected:

   MbusReadRegisterTable(unsigned long scanFreq, int startRef,
                         void *dataBufPtr, size_t dataBufSize):
      MbusDeviceDataTable(scanFreq, startRef, dataBufPtr, dataBufSize) { }


  private:

   int performScan(MbusMasterFunctions *mbusProtocol,
                   int slaveAddr, int startRef,
                   void *bufPtr, size_t bufSize)
   {
      return (mbusProtocol->readMultipleRegisters(slaveAddr, startRef,
                                                  (short *) bufPtr,
                                                  (int) (bufSize / 2)));
   }


  private:

   // Disable default operator and copy constructor
   MbusReadRegisterTable &operator= (MbusReadRegisterTable &);
   MbusReadRegisterTable (const MbusReadRegisterTable &);

};


/**
 * @brief This class implements ...
 *
 *
 * @ingroup mbusscanscheduler
 */
class
#if defined (_WINDLL) || defined(__DLL__)
   __declspec(dllexport)
#endif
MbusReadLongIntTable:
    public MbusDeviceDataTable
{

  protected:

   MbusReadLongIntTable(unsigned long scanFreq, int startRef,
                         void *dataBufPtr, size_t dataBufSize):
      MbusDeviceDataTable(scanFreq, startRef, dataBufPtr, dataBufSize) { }


  private:

   int performScan(MbusMasterFunctions *mbusProtocol,
                   int slaveAddr, int startRef,
                   void *bufPtr, size_t bufSize)
   {
      return (mbusProtocol->readMultipleLongInts(slaveAddr, startRef,
                                                 (int *) bufPtr,
                                                 (int) (bufSize / 4)));
   }


  private:

   // Disable default operator and copy constructor
   MbusReadLongIntTable &operator= (MbusReadLongIntTable &);
   MbusReadLongIntTable (const MbusReadLongIntTable &);

};


/**
 * @brief This class implements ...
 *
 *
 * @ingroup mbusscanscheduler
 */
class
#if defined (_WINDLL) || defined(__DLL__)
   __declspec(dllexport)
#endif
MbusReadMod10000Table:
    public MbusDeviceDataTable
{

  protected:

   MbusReadMod10000Table(unsigned long scanFreq, int startRef,
                         void *dataBufPtr, size_t dataBufSize):
      MbusDeviceDataTable(scanFreq, startRef, dataBufPtr, dataBufSize) { }


  private:

   int performScan(MbusMasterFunctions *mbusProtocol,
                   int slaveAddr, int startRef,
                   void *bufPtr, size_t bufSize)
   {
      return (mbusProtocol->readMultipleMod10000(slaveAddr, startRef,
                                                 (int *) bufPtr,
                                                 (int) (bufSize / 4)));
   }


  private:

   // Disable default operator and copy constructor
   MbusReadMod10000Table &operator= (MbusReadMod10000Table &);
   MbusReadMod10000Table (const MbusReadMod10000Table &);

};


/**
 * @brief This class implements ...
 *
 *
 * @ingroup mbusscanscheduler
 */
class
#if defined (_WINDLL) || defined(__DLL__)
   __declspec(dllexport)
#endif
MbusDevice
{

  protected:

   MbusDevice(int slaveAddr);


  public:

   void addDataTable(MbusDeviceDataTable *newDataTablePtr);

   unsigned long schedule();

   virtual void beforeScan() { }

   virtual void afterScan() { }

   virtual void failedScan(int result) { result = result; }

   int checkForErrors();

   MbusDevice *getPrev() { return (prevDevicePtr); }


   void setPrev (MbusDevice *prevDevicePtr)
   {
      this->prevDevicePtr = prevDevicePtr;
   }


   MbusMasterFunctions *getProtocol();

   int getSlaveAddr() { return (slaveAddr); }

   void setSlaveAddr(int slaveAddr) { this->slaveAddr = slaveAddr; }

   int getDeviceIndex() { return (deviceIdx); }

   void setDeviceIndex(int deviceIdx) { this->deviceIdx = deviceIdx; }


   void setParent(MbusDeviceScanList *parentDeviceScanListPtr)
   {
      this->parentDeviceScanListPtr = parentDeviceScanListPtr;
   }


  private:

   MbusDeviceScanList *parentDeviceScanListPtr;
   MbusDevice *prevDevicePtr;
   MbusDeviceDataTable *lastDataTablePtr;

   int slaveAddr;
   int deviceIdx;
   int numberOfTables;


  private:

   // Disable default operator and copy constructor
   MbusDevice &operator= (MbusDevice &);
   MbusDevice (const MbusDevice &);

};


/**
 * @brief This base class implements ...
 *
 *
 * @ingroup mbusscanscheduler
 */
class
#if defined (_WINDLL) || defined(__DLL__)
   __declspec(dllexport)
#endif
MbusDeviceScanList
{
  protected:

   MbusDeviceScanList(MbusMasterFunctions *mbusProtocolPtr);


  public:

   void addDevice(MbusDevice *newDevicePtr);

   unsigned long schedule();

   MbusMasterFunctions *getProtocol() { return (mbusProtocolPtr); }


  private:

   MbusMasterFunctions *mbusProtocolPtr;
   MbusDevice *lastDevicePtr;

   int numberOfDevices;


  private:

   // Disable default operator and copy constructor
   MbusDeviceScanList &operator= (MbusDeviceScanList &);
   MbusDeviceScanList (const MbusDeviceScanList &);

};


#endif // ifdef ..._H_INCLUDED

