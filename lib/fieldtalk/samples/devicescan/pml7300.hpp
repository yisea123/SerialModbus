/**
 * @file pml7300.cpp
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
 * PML 7300 series power meter Data Tables
 *****************************************************************************/

class Pml7300DataTable1: public MbusReadRegisterTable
{

  public:

   /*
    * PML 7300 Series Power Meter Modbus Data Table Definition
    *
    * Modbus register table #1
    * Base address 40011
    * Type: 16-bit signed integer
    */
   struct
   {
      short voltageAn;    // 40011
      short voltageBn;    // 40012
      short voltageCn;    // 40013
      short voltageAvrN;  // 40014
      short voltageAb;    // 40015
      short voltageBc;    // 40016
      short voltageCa;    // 40017
      short voltageAvr;   // 40018
      short currentA;     // 40019
      short currentB;     // 40020
      short currentC;     // 40021
      short currentAvr;   // 40022
      short voltageUnbal; // 40023
      short currentUnbal; // 40024
      short frequency;    // 40025
   } reg;


  public:

   Pml7300DataTable1(int deviceIdx):
      MbusReadRegisterTable(1000, // Scan every 1 s
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


class Pml7300DataTable2: public MbusReadRegisterTable
{

  public:

   /*
    * PML 7300 Series Power Meter Modbus Data Table Definition
    *
    * Modbus register table #2
    * Base address 40033
    * Type: 32-bit signed integer
    */
   struct
   {
      long kwTot;     // 40033 & 40034
      long kvarA;     // 40035 & 40036
      long kvarB;     // 40037 & 40038
      long kvarC;     // 40039 & 40040
      long kvarTot;   // 40041 & 40042
      long kVaA;      // 40043 & 40044
      long kVaB;      // 40045 & 40046
      long kVaC;      // 40047 & 40048
      long kVaTot;    // 40049 & 40050
      long pfSignA;   // 40051 & 40052
      long pfSignB;   // 40053 & 40054
      long pfSignC;   // 40055 & 40056
      long pfSignTot; // 40057 & 40058
   } reg;


  public:

   Pml7300DataTable2(int deviceIdx):
      MbusReadRegisterTable(1000, // Scan every 1 s
                            33,   // Start register 40033
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


class Pml7300DataTable4: public MbusReadMod10000Table
{

  public:

   /*
    * PML 7300 Series Power Meter Modbus Data Table Definition
    *
    * Modbus register table #4
    * Base address 40089
    * Type: 32-bit signed M10K
    */
   struct
   {
      long kwhImp;   // 40089 & 40190
      long kwhExp;   // 40091 & 40192
      long kwhTot;   // 40093 & 40194
      long kwhNet;   // 40095 & 40196
      long kvarhImp; // 40097 & 40198
      long kvarhExp; // 40099 & 40100
      long kvarhTot; // 40001 & 40102
      long kvarhNet; // 40003 & 40104
      long kVAh;     // 40005 & 40106
      long v1ThdMax; // 40007 & 40108
      long v2ThdMax; // 40009 & 40110
      long v3ThdMax; // 40011 & 40112
      long i1ThdMax; // 40013 & 40114
      long i2ThdMax; // 40015 & 40116
      long i3ThdMax; // 40017 & 40118
   } reg;


  public:

   Pml7300DataTable4(int deviceIdx):
      MbusReadMod10000Table(10000, // Scan every 10 s
                            89,    // Start register 40089
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


class Pml7300DataTableExtBool: public MbusReadRegisterTable
{

  public:

   /*
    * PML 7300 Series Power Meter Modbus Data Table Definition
    *
    * External boolean registers
    * Base address 42201
    * Type: 16-bit bool
    */
   struct
   {
      short extBoolArr[8]; // 42201 - 42208
   } reg;


  public:

   Pml7300DataTableExtBool(int deviceIdx):
      MbusReadRegisterTable(1000,  // Scan every 1 s
                            2201,  // Start register 42201
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
