/**
 * @file MfcMbusScannerApp.cpp
 *
 * Defines the class behaviors for the application
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
 * Global data
 *****************************************************************************/

CMfcMbusScannerApp theApp;
CRITICAL_SECTION globalDataLock;


/*****************************************************************************
 * CMfcMbusScannerApp class implementation
 *****************************************************************************/

BEGIN_MESSAGE_MAP(CMfcMbusScannerApp, CWinApp)
END_MESSAGE_MAP()


/**
 * Constructor
 */
CMfcMbusScannerApp::CMfcMbusScannerApp()
{
}


/**
 * Starts demo application
 */
BOOL CMfcMbusScannerApp::InitInstance()
{
   CWinApp::InitInstance();

   InitializeCriticalSection(&globalDataLock);
   CMfcMbusScannerDlg dlg;
   m_pMainWnd = &dlg;
   INT_PTR nResponse = dlg.DoModal();
   return FALSE;
}


/**
 * Clean-up
 */
int CMfcMbusScannerApp::ExitInstance()
{
   DeleteCriticalSection(&globalDataLock);
   return CWinApp::ExitInstance();
}

