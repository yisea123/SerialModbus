/**
 * @file MfcMbusScannerDlg.cpp
 *
 * Sample dialog
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
#include "MfcMbusScannerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/*****************************************************************************
 * CModbusScanner class implementation
 *****************************************************************************/

BEGIN_MESSAGE_MAP(CMfcMbusScannerDlg, CDialog)
	//{{AFX_MSG_MAP(CMfcMbusScannerDlg)
	ON_WM_CLOSE()
	ON_WM_TIMER()
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
   ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * Constructor and init
 *
 * @param pParent window
 */
CMfcMbusScannerDlg::CMfcMbusScannerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMfcMbusScannerDlg::IDD, pParent)
   , m_modbusChannel(&globalDataLock)
   , m_ipAddress(_T("127.0.0.1"))
   , m_startReg(_T("1"))
   , m_regCnt(_T("8"))
   , m_timerId(0)
{
   memset(readData, 0, sizeof(readData));
}


/**
 * Data exchange method (managed by VC++)
 */
void CMfcMbusScannerDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMfcMbusScannerDlg)
   DDX_Text(pDX, IDC_EDIT1, m_ipAddress);
   DDX_Text(pDX, IDC_EDIT2, m_startReg);
   DDX_Text(pDX, IDC_EDIT3, m_regCnt);
   DDX_Control(pDX, IDC_LIST3, m_valueList);
   DDX_Control(pDX, IDC_EDIT5, m_statusBar);
	//}}AFX_DATA_MAP
}


/**
 * Dialog shows up
 */
BOOL CMfcMbusScannerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
   m_timerId = SetTimer(1, 1000, 0);
   configureModbus();
   m_modbusChannel.CreateThread();
   updateView();
	return (TRUE);
}


/**
 * Dialog is removed from screen
 */
void CMfcMbusScannerDlg::OnClose()
{
	if (m_timerId)
	{
		KillTimer(m_timerId);
		m_timerId = NULL;
	}
	CDialog::OnClose();
}


/**
 * The Re-Start button is pressed. Stops scanner thread, re-configures Modbus
 * and re-starts scanner thread with new configuration parameters.
 */
void CMfcMbusScannerDlg::OnBnClickedOk()
{
   m_modbusChannel.StopThread();
   configureModbus();
   updateView();
   m_modbusChannel.CreateThread();
}


/**
 * Terminates application
 */
void CMfcMbusScannerDlg::OnBnClickedCancel()
{
   CDialog::OnCancel();
}


/**
 * One second screen update
 */
void CMfcMbusScannerDlg::OnTimer(UINT_PTR nIDEvent)
{
   updateView();
   CDialog::OnTimer(nIDEvent);
}


/**
 * Configures Modbus scanner thread with data taken from dialog form
 */
void CMfcMbusScannerDlg::configureModbus(void)
{
   UpdateData(true);
   m_modbusChannel.SetGlobalReadArray(readData);
   m_modbusChannel.SetReadStartReg(m_startReg);
   m_modbusChannel.SetReadCnt(m_regCnt);
   m_modbusChannel.SetSlaveIpAddress(m_ipAddress);
   m_valueList.DeleteAllItems();
   viewCnt = atoi(m_regCnt);
   viewStart = atoi(m_startReg);
   for (int i = 0; i != viewCnt; i++)
   {
      CString itemTextStr;

      itemTextStr.Format("%4i: %8.0f", viewStart + i, readData[i]);
      m_valueList.InsertItem(i, itemTextStr);
   }
}


/**
 * Updates data shown in the form
 */
void CMfcMbusScannerDlg::updateView()
{
   static long lastScanCounter = 0;
   long scanRate;
   char buffer[100];

   for (int i = 0; i != viewCnt; i++)
   {
      CString itemTextStr;

      itemTextStr.Format("%4i: %8.0f", viewStart + i, readData[i]);
      m_valueList.SetItemText(i, 0, itemTextStr);
   }
   scanRate = m_modbusChannel.GetScanCounter() - lastScanCounter;
   lastScanCounter = m_modbusChannel.GetScanCounter();
   m_statusBar.SetWindowText(m_modbusChannel.GetErrorText() + ", Scan rate: " + ltoa(scanRate, buffer, 10));
   m_statusBar.UpdateData(true);
   m_valueList.RedrawItems(0, viewCnt);
}
