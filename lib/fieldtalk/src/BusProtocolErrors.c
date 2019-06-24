/**
 * @file BusProtocolErrors.c
 *
 * @if NOTICE
 *
 * Copyright (c) proconX Pty Ltd. All rights reserved.
 *
 * THIS IS PROPRIETARY SOFTWARE AND YOU NEED A LICENSE TO USE OR REDISTRIBUTE.
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


/* HM platform detection */
#include "hmplatf.h"

/* Platform header */
#include <stdlib.h>

/* Include FieldTalk package header */
#include "BusProtocolErrors.h"

/* UNICODE support (MS VC++, Win CE) */
#if defined (_UNICODE)
#  define STR_TYPE L
#else
#  define STR_TYPE
#endif


#define FTALK_PORT_ALREADY_BOUND_TEXT       STR_TYPE"TCP Port alread bound"
#define FTALK_SUCCESS_TEXT                  STR_TYPE"Operation was successful"
#define FTALK_ILLEGAL_ARGUMENT_ERROR_TEXT   STR_TYPE"Illegal argument error"
#define FTALK_ILLEGAL_STATE_ERROR_TEXT      STR_TYPE"Illegal state error"
#define FTALK_EVALUATION_EXPIRED_TEXT       STR_TYPE"Evaluation expired"
#define FTALK_NO_DATA_TABLE_ERROR_TEXT      STR_TYPE"No data table configured"
#define FTALK_ILLEGAL_SLAVE_ADDRESS_TEXT    STR_TYPE"Slave address 0 is illegal in this context"
#define FTALK_IO_ERROR_CLASS_TEXT           STR_TYPE"I/O error class"
#define FTALK_IO_ERROR_TEXT                 STR_TYPE"I/O error"
#define FTALK_OPEN_ERR_TEXT                 STR_TYPE"Port or socket open error"
#define FTALK_PORT_ALREADY_OPEN_TEXT        STR_TYPE"Serial port already open"
#define FTALK_TCPIP_CONNECT_ERR_TEXT        STR_TYPE"TCP/IP connection error"
#define FTALK_CONNECTION_WAS_CLOSED_TEXT    STR_TYPE"TCP/IP connection was closed by remote peer"
#define FTALK_SOCKET_LIB_ERROR_TEXT         STR_TYPE"Socket library error"
#define FTALK_LISTEN_FAILED_TEXT            STR_TYPE"Listen failed"
#define FTALK_FILEDES_EXCEEDED_TEXT         STR_TYPE"File descriptors exceeded"
#define FTALK_PORT_NO_ACCESS_TEXT           STR_TYPE"No permission to access serial or TCP port"
#define FTALK_PORT_NOT_AVAIL_TEXT           STR_TYPE"TCP Port not available"
#define FTALK_BUS_PROTOCOL_ERROR_CLASS_TEXT STR_TYPE"Fieldbus protocol error class"
#define FTALK_CHECKSUM_ERROR_TEXT           STR_TYPE"Checksum error"
#define FTALK_INVALID_FRAME_ERROR_TEXT      STR_TYPE"Invalid frame error"
#define FTALK_INVALID_REPLY_ERROR_TEXT      STR_TYPE"Invalid reply error"
#define FTALK_REPLY_TIMEOUT_ERROR_TEXT      STR_TYPE"Reply time-out"
#define FTALK_SEND_TIMEOUT_ERROR_TEXT       STR_TYPE"Send time-out"
#define FTALK_INVALID_MBAP_ID_TEXT          STR_TYPE"Invalid MPAB indentifer"

#define FTALK_MBUS_EXCEPTION_RESPONSE_TEXT  STR_TYPE"Modbus exception response"
#define FTALK_MBUS_ILLEGAL_FUNCTION_RESPONSE_TEXT STR_TYPE"Illegal Function exception response"
#define FTALK_MBUS_ILLEGAL_ADDRESS_RESPONSE_TEXT  STR_TYPE"Illegal Data Address exception response"
#define FTALK_MBUS_ILLEGAL_VALUE_RESPONSE_TEXT    STR_TYPE"Illegal Data Value exception response"
#define FTALK_MBUS_SLAVE_FAILURE_RESPONSE_TEXT    STR_TYPE"Slave Device Failure exception response"
#define FTALK_MBUS_GW_PATH_UNAVAIL_RESPONSE_TEXT  STR_TYPE"Gateway Path Unavailable exception response"
#define FTALK_MBUS_GW_TARGET_FAIL_RESPONSE_TEXT   STR_TYPE"Gateway Target Device Failed exception response"

#define FTALK_UNKNOWN_ERROR                       STR_TYPE"Unknown error"


struct TextEntry
{
   int errorNo;
   const TCHAR *textMsg;
};


static const struct TextEntry entryListArr[] =
{
   {FTALK_SUCCESS, FTALK_SUCCESS_TEXT},
   {FTALK_ILLEGAL_ARGUMENT_ERROR, FTALK_ILLEGAL_ARGUMENT_ERROR_TEXT},
   {FTALK_ILLEGAL_STATE_ERROR, FTALK_ILLEGAL_STATE_ERROR_TEXT},
   {FTALK_EVALUATION_EXPIRED, FTALK_EVALUATION_EXPIRED_TEXT},
   {FTALK_NO_DATA_TABLE_ERROR, FTALK_NO_DATA_TABLE_ERROR_TEXT},
   {FTALK_ILLEGAL_SLAVE_ADDRESS, FTALK_ILLEGAL_SLAVE_ADDRESS_TEXT},

   {FTALK_IO_ERROR_CLASS, FTALK_IO_ERROR_CLASS_TEXT},
   {FTALK_IO_ERROR, FTALK_IO_ERROR_TEXT},
   {FTALK_OPEN_ERR, FTALK_OPEN_ERR_TEXT},
   {FTALK_PORT_ALREADY_OPEN, FTALK_PORT_ALREADY_OPEN_TEXT},
   {FTALK_TCPIP_CONNECT_ERR, FTALK_TCPIP_CONNECT_ERR_TEXT},
   {FTALK_CONNECTION_WAS_CLOSED, FTALK_CONNECTION_WAS_CLOSED_TEXT},
   {FTALK_SOCKET_LIB_ERROR, FTALK_SOCKET_LIB_ERROR_TEXT},
   {FTALK_PORT_ALREADY_BOUND, FTALK_PORT_ALREADY_BOUND_TEXT},
   {FTALK_LISTEN_FAILED, FTALK_LISTEN_FAILED_TEXT},
   {FTALK_FILEDES_EXCEEDED, FTALK_FILEDES_EXCEEDED_TEXT},
   {FTALK_PORT_NO_ACCESS, FTALK_PORT_NO_ACCESS_TEXT},
   {FTALK_PORT_NOT_AVAIL, FTALK_PORT_NOT_AVAIL_TEXT},

   {FTALK_BUS_PROTOCOL_ERROR_CLASS, FTALK_BUS_PROTOCOL_ERROR_CLASS_TEXT},
   {FTALK_CHECKSUM_ERROR, FTALK_CHECKSUM_ERROR_TEXT},
   {FTALK_INVALID_FRAME_ERROR, FTALK_INVALID_FRAME_ERROR_TEXT},
   {FTALK_INVALID_REPLY_ERROR, FTALK_INVALID_REPLY_ERROR_TEXT},
   {FTALK_REPLY_TIMEOUT_ERROR, FTALK_REPLY_TIMEOUT_ERROR_TEXT},
   {FTALK_SEND_TIMEOUT_ERROR, FTALK_SEND_TIMEOUT_ERROR_TEXT},
   {FTALK_INVALID_MBAP_ID , FTALK_INVALID_MBAP_ID_TEXT},
   {FTALK_MBUS_EXCEPTION_RESPONSE, FTALK_MBUS_EXCEPTION_RESPONSE_TEXT},
   {FTALK_MBUS_ILLEGAL_FUNCTION_RESPONSE, FTALK_MBUS_ILLEGAL_FUNCTION_RESPONSE_TEXT},
   {FTALK_MBUS_ILLEGAL_ADDRESS_RESPONSE, FTALK_MBUS_ILLEGAL_ADDRESS_RESPONSE_TEXT},
   {FTALK_MBUS_ILLEGAL_VALUE_RESPONSE, FTALK_MBUS_ILLEGAL_VALUE_RESPONSE_TEXT},
   {FTALK_MBUS_SLAVE_FAILURE_RESPONSE, FTALK_MBUS_SLAVE_FAILURE_RESPONSE_TEXT},
   {FTALK_MBUS_GW_PATH_UNAVAIL_RESPONSE, FTALK_MBUS_GW_PATH_UNAVAIL_RESPONSE_TEXT},
   {FTALK_MBUS_GW_TARGET_FAIL_RESPONSE, FTALK_MBUS_GW_TARGET_FAIL_RESPONSE_TEXT},
   {-1, NULL} /* Last entry mark */
};


/**
 * Translates a numeric error code into a description string.
 *
 * @param errCode FieldTalk error code
 * @return Error text string
 * @ingroup buserror
 */
const TCHAR * getBusProtocolErrorText(int errCode)
{
   int i = 0;

   while (entryListArr[i].textMsg != NULL)
   {
      if (entryListArr[i].errorNo == errCode)
         return (entryListArr[i].textMsg);
      i++;
   }
   return (FTALK_UNKNOWN_ERROR);
}

#  if defined(_NATIVE_WCHAR_T_DEFINED)
// Create an entry point for eVC++ 4.0 and VC++ 6.0 using unsigned short*
// instead of the native wchar_t*. eVC++ and VC++ 6 have no native
// wchar_t and uses unsigned char* instead which breaks binary
// compatibility if compiled under VS2005. Refer to:
// http://msdn.microsoft.com/en-us/library/ms838254.aspx
// http://msdn.microsoft.com/en-us/library/dh8che7s%28v=vs.80%29.aspx
unsigned short * getBusProtocolErrorText(int errCode)
{
   return (unsigned short *) ((TCHAR *) getBusProtocolErrorText(errCode));
}
#endif

