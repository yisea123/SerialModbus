/**
 * @file ScanScheduler.cpp
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
#include <stdlib.h>
#include <limits.h>

// Package header
#include "ScanScheduler.hpp"


/*****************************************************************************
 * MbusDeviceDataTable class implementation
 *****************************************************************************/

MbusDeviceDataTable::
MbusDeviceDataTable(unsigned long scanFreq, int startRef,
                    void *dataBufPtr, size_t dataBufSize)
{
   this->scanFreq = scanFreq;
   this->startRef = startRef;
   this->dataBufSize = dataBufSize;
   this->dataBufPtr = dataBufPtr;
   parentDevicePtr = NULL;
   prevDataTablePtr = NULL;
   scanTimer.startMillis(scanFreq);
   lastResult = FTALK_SUCCESS;
}


MbusDeviceDataTable::~MbusDeviceDataTable()
{
}

unsigned long MbusDeviceDataTable::schedule()
{
   if (!scanTimer.isExpired())
      return (scanTimer.left());
   scanTimer.startMillis(scanFreq);
   if ((parentDevicePtr == NULL) || (dataBufPtr == NULL) ||
       (startRef <= 0) || (dataBufSize == 0))
      return (scanTimer.left());
   parentDevicePtr->beforeScan();
   if (getSlaveAddr() <= 0)
      return (scanTimer.left());
   beforeScan();
   lastResult = performScan(parentDevicePtr->getProtocol(),
                            getSlaveAddr(), startRef,
                            dataBufPtr, dataBufSize);
   if (lastResult == FTALK_SUCCESS)
   {
      parentDevicePtr->afterScan();
      afterScan();
    }
   else
   {
      parentDevicePtr->failedScan(lastResult);
      failedScan(lastResult);
   }
   return (scanTimer.left());
}


int MbusDeviceDataTable::getDeviceIndex()
{
   if (parentDevicePtr == NULL)
      return (0);
   return (parentDevicePtr->getDeviceIndex());
}

int MbusDeviceDataTable::getSlaveAddr()
{
   if (parentDevicePtr == NULL)
      return (0);
   return (parentDevicePtr->getSlaveAddr());
}


/*****************************************************************************
 * MbusDevice class implementation
 *****************************************************************************/

MbusDevice::MbusDevice(int slaveAddr)
{
   this->slaveAddr = slaveAddr;
   parentDeviceScanListPtr = NULL;
   lastDataTablePtr = NULL;
   prevDevicePtr = NULL;
   numberOfTables = 0;
}


void MbusDevice::addDataTable(MbusDeviceDataTable *newDataTablePtr)
{
   if (newDataTablePtr != NULL)
   {
      // First entry in list?
      if (lastDataTablePtr == NULL)
         lastDataTablePtr = newDataTablePtr;
      else
      {
         newDataTablePtr->setPrev(lastDataTablePtr);
         lastDataTablePtr = newDataTablePtr;
      }
      newDataTablePtr->setParent(this);
      numberOfTables++;
      newDataTablePtr->setTableIndex(numberOfTables);
   }
}


unsigned long MbusDevice::schedule()
{
   MbusDeviceDataTable *indexPtr = lastDataTablePtr;
   unsigned long maxTimeMillis = ULONG_MAX;
   unsigned long remainTimeMillis;

   while (indexPtr != NULL)
   {
      remainTimeMillis = indexPtr->schedule();
      if (remainTimeMillis < maxTimeMillis)
         maxTimeMillis = remainTimeMillis;
      indexPtr = indexPtr->getPrev();
   }
   return (maxTimeMillis);
}


int MbusDevice::checkForErrors()
{
   MbusDeviceDataTable *indexPtr = lastDataTablePtr;
   int errors = 0;


   while (indexPtr != NULL)
   {
      if (indexPtr->getLastResult() != FTALK_SUCCESS)
         errors = 1;
      indexPtr = indexPtr->getPrev();
   }
   return (errors);
}


MbusMasterFunctions *MbusDevice::getProtocol()
{
   if (parentDeviceScanListPtr == NULL)
      return (NULL);
   return (parentDeviceScanListPtr->getProtocol());
}


/*****************************************************************************
 * MbusDeviceScanList class implementation
 *****************************************************************************/

MbusDeviceScanList::
MbusDeviceScanList(MbusMasterFunctions *mbusProtocolPtr)
{
   this->mbusProtocolPtr = mbusProtocolPtr;
   this->lastDevicePtr = NULL;
}


void MbusDeviceScanList::addDevice(MbusDevice *newDevicePtr)
{
   if (newDevicePtr != NULL)
   {
      // First entry in list?
      if (lastDevicePtr == NULL)
         lastDevicePtr = newDevicePtr;
      else
      {
         newDevicePtr->setPrev(lastDevicePtr);
         lastDevicePtr = newDevicePtr;
      }
      newDevicePtr->setParent(this);
      numberOfDevices++;
      newDevicePtr->setDeviceIndex(numberOfDevices);
   }
}


unsigned long MbusDeviceScanList::schedule()
{
   MbusDevice *indexPtr = lastDevicePtr;
   unsigned long maxTimeMillis = ULONG_MAX;
   unsigned long remainTimeMillis;


   while (indexPtr != NULL)
   {
      remainTimeMillis = indexPtr->schedule();
      if (remainTimeMillis < maxTimeMillis)
         maxTimeMillis = remainTimeMillis;
      indexPtr = indexPtr->getPrev();
   }
   return (maxTimeMillis);
}
