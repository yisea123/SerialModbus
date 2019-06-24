/**
 * @file MfcMbusScannerDlg.h
 *
 * Sample dialog = header file
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

#include "ModbusScanner.h"
#include "afxwin.h"
#include "afxcmn.h"


/*****************************************************************************
 * CMfcMbusScannerDlg class definition
 *****************************************************************************/

/**
 * Modbus scanner dialog
 *
 * User interface of main application.
 */
class CMfcMbusScannerDlg : public CDialog
{
  public:

	CMfcMbusScannerDlg(CWnd* pParent = NULL);

   // Dialog Data
	enum { IDD = IDD_MFCMBUSSCANNER_DIALOG };


  protected:

   void configureModbus(void);
   void updateView(void);


  protected:

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//{{AFX_MSG(CMfcMbusScannerDlg)
	afx_msg void OnClose();
   afx_msg void OnBnClickedOk();
   afx_msg void OnBnClickedCancel();
   afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG


 private:

   UINT_PTR m_timerId;
   double readData[125];
   int viewCnt;
   int viewStart;
   CModbusScanner m_modbusChannel;
   CString m_ipAddress;
   CString m_startReg;
   CString m_regCnt;
   CListCtrl m_valueList;
   CEdit m_statusBar;

	DECLARE_MESSAGE_MAP()
};
