/**
 * @file ModbusScanner.h
 *
 * Modbus scanner thread - header file
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


#pragma once

#include "MbusTcpMasterProtocol.hpp"  // FieldTalk library


/*****************************************************************************
 * CModbusScanner class definition
 *****************************************************************************/

/**
 * Modbus Scanner class
 *
 * This class implements a scanning thread for Modbus/TCP devices.
 */
class CModbusScanner: public CWinThread
{
  public:

	CModbusScanner(CRITICAL_SECTION *);
	virtual ~CModbusScanner();


  /**************************************************************************
   * Thread control functions
   **************************************************************************/

  public:

   void StopThread();


  protected:

	virtual int Run();
	virtual BOOL InitInstance();
	virtual int ExitInstance();


  /**************************************************************************
   * Status of the communication
   **************************************************************************/

  public:

   bool IsConnected();
   int GetErrorCode();
   CString GetErrorText();


  /**************************************************************************
   * Configuration methods
   *
   * Thread has to be stopped before making any changes using these methods!
   **************************************************************************/

  public:

   void SetGlobalReadArray(double *);
   void SetGlobalWriteArray(double *);
   void SetSlaveIpAddress(CString);
   void SetReadStartReg(CString);
   void SetWriteStartReg(CString);
   void SetReadCnt(CString);
   void SetWriteCnt(CString);
   void SetPollRate(CString);
   long GetScanCounter();


  private:

	CRITICAL_SECTION *m_dataLockPtr;
   volatile bool m_exitRunLoop; // Assuming access is atomic
   volatile int m_errorCode;    // Assuming access is atomic
   volatile bool m_isConnected; // Assuming access is atomic
   volatile long m_scanCounter; // Assuming access is atomic
   short m_registerArr[125];
   CString m_slaveHostName;
   int m_pollRate;
   double *m_readArrayPtr;
   double *m_writeArrayPtr;
   int m_readCnt;
   int m_writeCnt;
   int m_readStartReg;
   int m_writeStartReg;
   MbusTcpMasterProtocol m_mbusProtocol;

	DECLARE_MESSAGE_MAP()
};
