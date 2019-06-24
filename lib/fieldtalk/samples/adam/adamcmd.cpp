/**
 * @file adamcmd.cpp
 *
 * A command line utility to demonstrate the use of the Advantech specific ADAM
 * 5000/6000 series commands. These commands allow module configuration and
 * other enhanced functions.
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

// Include FieldTalk package header
#include "MbusTcpMasterProtocol.hpp"


/*****************************************************************************
 * Gobal data
 *****************************************************************************/

MbusTcpMasterProtocol mbusProtocol;


/*****************************************************************************
 * Function implementation
 *****************************************************************************/

/**
 * Opens protocol
 */
void openProtocol(char *hostName)
{
   int result;

   result = mbusProtocol.openProtocol(hostName);
   if (result != FTALK_SUCCESS)
   {
      fprintf(stderr, "Error opening connection to %s: %s!\n",
              hostName, getBusProtocolErrorText(result));
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
 * Execute ASCII command
 */
void execCmd(char *cmdSz)
{
   char responseBufSz[256];
   int result;

   result = mbusProtocol.adamSendReceiveAsciiCmd(cmdSz, responseBufSz);
   if (result == FTALK_SUCCESS)
   {
       printf("ADAM replied: %s\n", responseBufSz);
   }
   else
   {
       fprintf(stderr, "%s!\n", getBusProtocolErrorText(result));
       // Stop for fatal errors
       if (!(result & FTALK_BUS_PROTOCOL_ERROR_CLASS))
           return;
   }
}


/**
 * Main function
 *
 * @param argc Command line argument count
 * @param argv Command line argument value string array
 * @return Error code: 0 = OK, else error
 */
int main (int argc, char **argv)
{

   if (argc != 3)
   {
       fprintf(stderr, "Usage: adamcd ipaddr asciicmd\n");
       return (EXIT_FAILURE);
   }
   openProtocol(argv[1]);

   execCmd(argv[2]);

   closeProtocol();
   return (EXIT_SUCCESS);
}
