/**
 * @file tcpsimple.cpp
 *
 * A simple example using FieldTalk in Modbus/TCP master mode.
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
#include <stdlib.h>
#ifdef _WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#endif

// Include FieldTalk package header
#include "MbusTcpMasterProtocol.hpp"


/*****************************************************************************
 * Gobal data
 *****************************************************************************/

#if defined(_WIN32) && defined (_UNICODE)
wchar_t *hostName = L"127.0.0.1";
#else
char *hostName = "127.0.0.1";
#endif
MbusTcpMasterProtocol mbusProtocol;


/*****************************************************************************
 * Function implementation
 *****************************************************************************/

/**
 * Opens protocol
 */
void openProtocol()
{
   int result;

   result = mbusProtocol.openProtocol(hostName);
   if (result != FTALK_SUCCESS)
   {
      fprintf(stderr, "Error opening protocol: %s!\n",
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
 * Cyclic loop which polls every one second 10 registers starting at
 * reference 100 from slave # 1
 */
void runPollLoop()
{
   short dataArr[10];

   for (;;)
   {
      int i;
      int result;

      result = mbusProtocol.readMultipleRegisters(1, 100,
                                                  dataArr,
                                                  sizeof(dataArr) / 2);
      if (result == FTALK_SUCCESS)
         for (i = 0; i < int(sizeof(dataArr) / 2); i++)
            printf("[%d]: %hd\n", 100 + i, dataArr[i]);
      else
      {
         fprintf(stderr, "%s!\n", getBusProtocolErrorText(result));
         // Stop for fatal errors
         if (!(result & FTALK_BUS_PROTOCOL_ERROR_CLASS))
            return;
      }

#ifdef __WIN32__
      Sleep(1000);
#else
      sleep(1);
#endif
   }
}


/**
 * Main function.
 *
 * @return Error code: 0 = OK, else error
 */
#if defined(_WIN32_WCE)
int wmain()
#else
int main()
#endif
{
   openProtocol();

   runPollLoop();

   closeProtocol();
   return (EXIT_SUCCESS);
}
