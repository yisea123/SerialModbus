/**
 * @file ModbusScanner.cpp
 *
 * Modbus scanner thread
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


#include "stdafx.h"
#include "MfcMbusScannerApp.h"
#include "ModbusScanner.h"


/*****************************************************************************
 * CModbusScanner class implementation
 *****************************************************************************/

BEGIN_MESSAGE_MAP(CModbusScanner, CWinThread)
END_MESSAGE_MAP()


/**
 * Constructor. Creates object and initialises data. The thread is not started
 * yet. Use CreateThread() to start it.
 *
 * @param dataLockPtr Pointer to a global critical section which synchronises
 * access to global data array.
 */
CModbusScanner::CModbusScanner(CRITICAL_SECTION *dataLockPtr):
   m_dataLockPtr(dataLockPtr),
   m_exitRunLoop(false),
   m_pollRate(0), // in ms
   m_errorCode(0),
   m_isConnected(false),
   m_readArrayPtr(NULL),
   m_writeArrayPtr(NULL),
   m_readCnt(0),
   m_writeCnt(0),
   m_readStartReg(0),
   m_writeStartReg(0),
   m_scanCounter(0)
{
   TRACE0("CModbusScanner::CModbusScanner construct\n");
   m_bAutoDelete = false; // Don't delete object when thread terminates
}


/**
 * Destructor
 */
CModbusScanner::~CModbusScanner()
{
   TRACE0("CModbusScanner::~CModbusScanner destroy\n");
}


/**
 * Initialises thread
 *
 * @note This thread is a worker thread without GUI message loop!
 */
BOOL CModbusScanner::InitInstance()
{
   TRACE1("CModbusScanner::InitInstance for thread 0x%X\n", m_hThread);
   SetThreadPriority(THREAD_PRIORITY_IDLE);
   m_exitRunLoop = false;
   return (TRUE);
}


/**
 * Thread clean-up
 *
 * @note This thread is a worker thread without GUI message loop!
 */
int CModbusScanner::ExitInstance()
{
   TRACE1("CModbusScanner::ExitInstance for thread 0x%X\n", m_hThread);
   // Note: This thread is a worker thread without GUI message loop!
   CloseHandle(m_hThread);
   m_hThread = NULL; // Allow re-start with CreateThread
   return (0);
}


/**
 * Configure where the Modbus data to be read goes to
 *
 * @param readArrayPtr Pointer to global data array (NULL = reads disabled)
 */
void CModbusScanner::SetGlobalReadArray(double *readArrayPtr)
{
   ASSERT(m_hThread == NULL); // Thread must be stopped
   TRACE0("CModbusScanner::SetGlobalReadArray\n");
   m_readArrayPtr = readArrayPtr;
}


/**
 * Configute where the Modbus data to be written comes from
 *
 * @param writeArrayPtr Pointer to global data array (NULL = writes disabled)
 */
void CModbusScanner::SetGlobalWriteArray(double *writeArrayPtr)
{
   ASSERT(m_hThread == NULL); // Thread must be stopped
   TRACE0("CModbusScanner::SetGlobalWriteArray\n");
   m_writeArrayPtr = writeArrayPtr;
}


/**
 * IP address of slave
 *
 * @param valueStr IP address like 127.0.0.1
 */
void CModbusScanner::SetSlaveIpAddress(CString valueStr)
{
   ASSERT(m_hThread == NULL); // Thread must be stopped
   m_slaveHostName = valueStr;
   TRACE1("CModbusScanner::SetSlaveIpAddress = %s\n", m_slaveHostName);
}


/**
 * First register to be read
 *
 * @param valueStr String with integer value
 */
void CModbusScanner::SetReadStartReg(CString valueStr)
{
   ASSERT(m_hThread == NULL); // Thread must be stopped
   m_readStartReg = atoi(valueStr);
   TRACE1("CModbusScanner::SetReadStartReg = %d\n", m_readStartReg);
}


/**
 * First register to be written
 *
 * @param valueStr String with integer value
 */
void CModbusScanner::SetWriteStartReg(CString valueStr)
{
   ASSERT(m_hThread == NULL); // Thread must be stopped
   m_writeStartReg = atoi(valueStr);
   TRACE1("CModbusScanner::SetWriteStartReg = %d\n", m_writeStartReg);
}


/**
 * Number of values to read
 *
 * @param valueStr String with integer value (0 = disbled or 1 - 125)
 */
void CModbusScanner::SetReadCnt(CString valueStr)
{
   ASSERT(m_hThread == NULL); // Thread must be stopped
   m_readCnt = atoi(valueStr);
   TRACE1("CModbusScanner::SetReadCnt = %d\n", m_readCnt);
}


/**
 * Number of values to write
 *
 * @param valueStr String with integer value (0 = disbled or 1 - 125)
 */
void CModbusScanner::SetWriteCnt(CString valueStr)
{
   ASSERT(m_hThread == NULL); // Thread must be stopped
   m_writeCnt = atoi(valueStr);
   TRACE1("CModbusScanner::SetWriteCnt = %d\n", m_writeCnt);
}


/**
 * Poll rate in ms
 *
 * @param valueStr String with integer value (0 = poll as quick as possible)
 */
void CModbusScanner::SetPollRate(CString valueStr)
{
   ASSERT(m_hThread == NULL); // Thread must be stopped
   m_pollRate = atoi(valueStr);
   TRACE1("CModbusScanner::SetPollRate = %d\n", m_pollRate);
}


/**
 * If not connected, this code tells us what went wrong
 *
 * @return FieldTalk library error code
 */
int CModbusScanner::GetErrorCode()
{
   TRACE1("CModbusScanner::GetErrorCode = 0x%X\n", m_errorCode);
   return (m_errorCode); // Assuming access is atomic
}


/**
 * Text version of error code
 *
 * @return Error description as string
 */
CString CModbusScanner::GetErrorText()
{
   return (getBusProtocolErrorText(GetErrorCode()));
}


/**
 * Returns true if we are successfully connected to the slave
 *
 * @return true if connected
 */
bool CModbusScanner::IsConnected()
{
   return (m_isConnected); // Assuming access is atomic
}


/**
 * Returns the number of scanner cycles
 *
 * @return Scan sycles
 */
long CModbusScanner::GetScanCounter()
{
   return (m_scanCounter); // Assuming access is atomic
}


/**
 * Worker thread run loop
 */
int CModbusScanner::Run()
{
   int result;

   TRACE0("CModbusScanner::Run() started\n");
   while (!m_exitRunLoop) // Assuming access is atomic
   {
      if (!m_mbusProtocol.isOpen() && !m_slaveHostName.IsEmpty())
      {
         m_mbusProtocol.setPollDelay(m_pollRate);
         m_mbusProtocol.setTimeout(1000); // 1 s
         TRACE1("m_mbusProtocol.openProtocol to %s\n", m_slaveHostName);
         result = m_mbusProtocol.openProtocol(m_slaveHostName);
         if (result != FTALK_SUCCESS)
         {
            // Retry to open after 1 s
            if (!m_exitRunLoop)
               Sleep(1000);
            m_isConnected = false;
         }
         else
            m_isConnected = true;
         TRACE1("m_mbusProtocol.openProtocol: %s\n",
                getBusProtocolErrorText(result));

      }
      if (m_mbusProtocol.isOpen() &&
         (m_writeCnt > 0) &&  (m_writeArrayPtr != NULL))
      {
         EnterCriticalSection(m_dataLockPtr);
         for (int i = 0; i != m_writeCnt; i++)
            m_registerArr[i] = (short) m_writeArrayPtr[i];
         LeaveCriticalSection(m_dataLockPtr);
         result = m_mbusProtocol.writeMultipleRegisters(1,
                                                       m_writeStartReg,
                                                       m_registerArr,
                                                       m_writeCnt);
         TRACE1("m_mbusProtocol.writeMultipleRegisters: %s\n",
                getBusProtocolErrorText(result));
         if (result != FTALK_SUCCESS)
         {
            // Close connection in error case
            m_isConnected = false;
            m_mbusProtocol.closeProtocol();
         }
      }
      if (m_mbusProtocol.isOpen() &&
         (m_readCnt > 0) &&  (m_readArrayPtr != NULL))
      {
         result = m_mbusProtocol.readMultipleRegisters(1,
                                                       m_readStartReg,
                                                       m_registerArr,
                                                       m_readCnt);
         TRACE1("m_mbusProtocol.readMultipleRegisters: %s\n",
                getBusProtocolErrorText(result));
         if (result != FTALK_SUCCESS)
         {
            // Close connection in error case
            m_isConnected = false;
            m_mbusProtocol.closeProtocol();
            // Invalidate data
            memset(m_registerArr, 0, m_readCnt * sizeof(short));
         }
         EnterCriticalSection(m_dataLockPtr);
         for (int i = 0; i != m_readCnt; i++)
            m_readArrayPtr[i] = (double) m_registerArr[i];
         LeaveCriticalSection(m_dataLockPtr);
      }
      // Publich error/result status
      m_errorCode = result;
      m_scanCounter++;
   }
   m_mbusProtocol.closeProtocol();
   m_isConnected = false;
   TRACE0("CModbusScanner::Run() finished\n");
   return (ExitInstance());
}


/**
 * Terminates thread nicely
 */
void CModbusScanner::StopThread()
{
   TRACE0("CModbusScanner::StopThread() called\n");
   if (m_hThread != NULL)
   {
      m_exitRunLoop = true; // Assuming access is atomic
      TRACE0("CModbusScanner::StopThread() waiting to join...\n");
      ::WaitForSingleObject(m_hThread, INFINITE);
   }
   TRACE0("CModbusScanner::StopThread() finished\n");
}
