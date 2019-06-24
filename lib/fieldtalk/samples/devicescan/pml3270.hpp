/**
 * @file pml3270.cpp
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


/*****************************************************************************
 * PML 3720 series power meter Data Tables
 *****************************************************************************/

class Pml3720DataTable1: public MbusReadMod10000Table
{

  public:

   /*
    * PML 3720 Series Power Meter Modbus Data Table Definition
    *
    * Modbus register table #1
    * Base address 40011
    * Type: 32-bit signed M10K
    */
   struct
   {
      long voltageAn;    // 40011
      long voltageBn;    // 40012
      long voltageCn;    // 40013
      long voltageAvrN;  // 40014
      long voltageAb;    // 40015
      long voltageBc;    // 40016
      long voltageCa;    // 40017
      long voltageAvr;   // 40018
      long gap1;
      long voltageAux;   // 40020
      long currentA;     // 40021
      long currentB;     // 40022
      long currentC;     // 40023
      long currentAvr;   // 40024
      long gap2;
      long currentN;     // 40026
   } reg;


  public:

   Pml3720DataTable1(int deviceIdx):
      MbusReadMod10000Table(1000, // Scan every 1 s
                            11,   // Start register 40011
                            (short *) &reg, sizeof(reg))
   {
      (void) deviceIdx;
   }


   void beforeScan()
   {
      printf("Scanning device %d @ node %d table %d ...",
                getDeviceIndex(), getSlaveAddr(), getTableStartRef());
   }


   void afterScan()
   {
      printf("\b\b\bOK\n");
   }


   void failedScan(int errCode)
   {
      errCode = errCode; // Avoid compiler warning
      memset(&reg, 0, sizeof(reg)); // Clear data
      printf("\b\b\bFailed: %s\n", getBusProtocolErrorText(errCode));
   }

};


class Pml3720DataTable2: public MbusReadMod10000Table
{

  public:

   /*
    * PML 3720 Series Power Meter Modbus Data Table Definition
    *
    * Modbus register table #2
    * Base address 40034
    * Type: 32-bit signed M10K
    */
   struct
   {
      long kwTot;      // 40034
      long kvarA;      // 40035
      long kvarB;      // 40036
      long kvarC;      // 40037
      long kvarTot;    // 40038
      long pwrFactA;   // 40039
      long pwrFactB;   // 40040
      long pwrFactC;   // 40041
      long pwrFactTot; // 40042
      long kVaA;       // 40043
      long kVaB;       // 40044
      long kVaC;       // 40045
      long kVaTot;     // 40046
      long gap1;
      long freqA;      // 40048
   } reg;


  public:

   Pml3720DataTable2(int deviceIdx):
      MbusReadMod10000Table(1000, // Scan every 1 s
                            34,   // Start register 40034
                            (short *) &reg, sizeof(reg))
   {
      (void) deviceIdx;
   }


   void beforeScan()
   {
      printf("Scanning device %d @ node %d table %d ...",
                getDeviceIndex(), getSlaveAddr(), getTableStartRef());
   }


   void afterScan()
   {
      printf("\b\b\bOK\n");
   }


   void failedScan(int errCode)
   {
      errCode = errCode; // Avoid compiler warning
      memset(&reg, 0, sizeof(reg)); // Clear data
      printf("\b\b\bFailed: %s\n", getBusProtocolErrorText(errCode));
   }

};
