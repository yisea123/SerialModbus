/**
 * @file MbusMasterFunctions.cpp
 *
 * @if NOTICE
 *
 * THIS FILE IS PART OF PROPRIETARY SOFTWARE COPYRIGHTED BY PROCONX PTY LTD
 * AND YOU NEED A LICENSE OR WRITTEN PERMISSION TO USE OR REDISTRIBUTE.
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


// HM platform detection
#include "hmplatf.h"

// Platform header
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Package header
#include "hmtracelog.h"
#include "hmtimer.hpp"
#include "MbusMasterFunctions.hpp"

// Version identifier
#include "FieldTalkVersion.h"


/*****************************************************************************
 * Definitions
 *****************************************************************************/

//
// Slave configuration constants
//
#define REFERENCES_START_AT_ZERO 0x0001
#define FLOATS_ARE_BIG_ENDIAN    0x0002
#define INTS_ARE_BIG_ENDIAN      0x0004
#define DANIEL_ENRON_32_BIT_MODE 0x0008


/*****************************************************************************
 * Utility routines
 *****************************************************************************/

static void swap32BitWords(unsigned int valueArr[], size_t cnt)
{
   size_t i;
   unsigned short tempWord;
   struct SwapStruct
   {
      unsigned short wordA;
      unsigned short wordB;
   };


   for (i = 0; i < cnt; i++)
   {
      SwapStruct *valuePtr = (SwapStruct *) &valueArr[i];
      tempWord = valuePtr->wordB;
      valuePtr->wordB = valuePtr->wordA;
      valuePtr->wordA = tempWord;
   }
}


/*****************************************************************************
 * MbusMasterFunctions class implementation
 *****************************************************************************/

/**
 * Constructs a MbusMasterFunctions object and initialises its data. It
 * also detects the endianess of the machine it's running on and configures
 * byte swapping if necessary.
 */
MbusMasterFunctions::MbusMasterFunctions(): silenceTimer(*(new HmTimer(0)))
{
   union // Used for endian check, defines big endian layout
   {
      short int16Word;
      struct
      {
         char high;
         char low;
      } bytes;
   } intLayout;
   union // Used for float endian check, defines big endian layout
   {
      float float32Value;
      struct
      {
         unsigned int sign : 1;
         unsigned int exponent: 8;
         unsigned int fraction0: 7;
         unsigned int fraction1: 16;
      } number;
   } floatLayout;

   (void) banner; // Avoid compiler warning
   totalCounter = 0L;
   successCounter = 0L;
   retryCnt = 0; // Retry counter
   timeOut = 1000; // Time-out in ms
   pollDelay = 0; // Delay between two Modbus read/writes in ms
   intLayout.int16Word = 0x1234;  // Magic value to detect endianess
   floatLayout.float32Value = -1.0; // Magic value to detect endianess
   //
   // Endian check for integer words
   //
   if (intLayout.bytes.high == 0x34) // Little endian
   {
      bigEndianMachine = 0;
   }
   else // Big endian
   {
      bigEndianMachine = 1;
   }

   //
   // Endian check for floats
   //
   if (floatLayout.number.sign) // Big endian
   {
      bigEndianFloatMachine = 1;
   }
   else // Little endian
   {
      bigEndianFloatMachine = 0;
   }
   memset(slaveConfigFlagsArr, 0, sizeof(slaveConfigFlagsArr));
}


/**
 * Destructor. Does clean-up and closes an open protocol including any
 * associated communication resources (serial ports or sockets).
 */
MbusMasterFunctions::~MbusMasterFunctions()
{
   delete &silenceTimer;
}


/*****************************************************************************
 * Shared subroutines for Modbus functions
 *****************************************************************************/

/**
 * @internal
 * Subroutine for Modbus function 1 and 2 to read multiple coils/discretes.
 *
 * @param function Modbus function code to be used (1 or 2)
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255 for serial, 0 - 255 for TCP)
 * @param startRef Start reference (Range: 1 - 65536)
 * @param bitArr Buffer which will contain the data read
 * @param refCnt Number of coils to be read (Range: 1 - 2000)
 * @note More details see readCoils() and readInputDiscretes()
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 */
int MbusMasterFunctions::readBits(int function, int slaveAddr, int startRef,
                                  int bitArr[], int refCnt)
{
   int i;
   int result;
   int numBytes = (refCnt + 7) / 8;

   // Clear result array
   memset(bitArr, 0, sizeof(bitArr[0]) * refCnt);

   //
   // Check arguments
   //
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & REFERENCES_START_AT_ZERO) == 0)
      startRef--;
   if ((startRef < 0) || (startRef >= 0x10000))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((refCnt <= 0) || (refCnt > MAX_FC01_COILS))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   //
   // Assemble Modbus request packet
   //
   bufferArr[0] = (char) (startRef >> 8);
   bufferArr[1] = (char) (startRef & 0xFF);
   bufferArr[2] = (char) (refCnt >> 8);
   bufferArr[3] = (char) (refCnt & 0xFF);

   //
   // Transmit - receive
   //
   result = deliverMessage(slaveAddr, function,
                           bufferArr, 4,
                           bufferArr, 1 + numBytes);
   // Check for error result codes
   if (result != FTALK_SUCCESS)
      return (result);

   //
   // Verify reply
   //
   if (bufferArr[0] != (char) (numBytes))
      return (FTALK_INVALID_REPLY_ERROR);

   //
   // Copy data into buffer
   //
   {
      int byteIdx = 0;
      int bitIdx;
      for (i = 0; i < refCnt; i++)
      {
         bitIdx = i % 8;
         if (bitIdx == 0)
            byteIdx++;
            bitArr[i] = (((bufferArr[byteIdx] >> bitIdx) & 0x01) == 0x01);
      }
   }

   return (FTALK_SUCCESS);
}


/**
 * @internal
 * Subroutine for Modbus function 3 and 4 to read multiple registers.
 *
 * @param function Modbus function code to be used (3 or 4)
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255 for serial, 0 - 255 for TCP)
 * @param startRef Start register (Range: 1 - 65536)
 * @param regArr Buffer which will be filled with the data read
 * @param regArrLen Length of data array (Range: 1-125)
 * @param regCount Number of registers to read. For standard Modbus this
 * must be the same than the length of regArr. For the ENRON Modbus variant
 * this must be half of the length of regArr.
 * @note More details see readMultipleRegisters() and readInputRegister()
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 */
int MbusMasterFunctions::readRegisters(int function,
                                       int slaveAddr, int startRef,
                                       short regArr[], int regArrLen, int regCount)
{
   int i;
   int result;

   // Clear result array
   memset(regArr, 0, sizeof(short) * regArrLen);

   //
   // Check arguments
   //
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & REFERENCES_START_AT_ZERO) == 0)
      startRef--;
   if ((startRef < 0) || (startRef >= 0x10000))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((regArrLen <= 0) || (regArrLen > MAX_FC03_WORDS))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   //
   // Assemble Modbus request packet
   //
   bufferArr[0] = (char) (startRef >> 8);
   bufferArr[1] = (char) (startRef & 0xFF);
   bufferArr[2] = (char) (regCount >> 8);
   bufferArr[3] = (char) (regCount & 0xFF);

   //
   // Transmit - receive
   //
   result = deliverMessage(slaveAddr, function,
                           bufferArr, 4,
                           bufferArr,
                           1 + regArrLen * 2);
   // Check for error result codes
   if (result != FTALK_SUCCESS)
      return (result);

   //
   // Verify reply
   //
   if (bufferArr[0] != (char) (regArrLen * 2))
      return (FTALK_INVALID_REPLY_ERROR);

   //
   // Copy data into buffer
   //
   for (i = 0; i < regArrLen; i++)
      regArr[i] = (short) ((bufferArr[1 + i * 2] << 8) +
                           (bufferArr[2 + i * 2] & 0xFF));
   return (FTALK_SUCCESS);
}


/**
 * @internal
 * Subroutine for Modbus function 16 to write multiple registers.
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 0 - 255)
 * @param startRef Start register (Range: 1 - 65536)
 * @param regArr Buffer with the data to be sent.
 * @param regArrLen Length of data array (Range: 1-123)
 * @param regCount Number of registers to write. For standard Modbus this
 * must be the same than the length of regArr. For the ENRON Modbus variant
 * this must be half of the length of regArr.
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note Broadcast supported for serial protocols
 */
int MbusMasterFunctions::writeRegisters(int slaveAddr,
                                        int startRef,
                                        const short regArr[],
                                        int regArrLen, int regCount)
{
   int i;
   int result;

   //
   // Check arguments
   //
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & REFERENCES_START_AT_ZERO) == 0)
      startRef--;
   if ((startRef < 0) || (startRef >= 0x10000))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((regArrLen <= 0) || (regArrLen > MAX_FC16_WORDS))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   //
   // Assemble Modbus request packet
   //
   bufferArr[0] = (char) (startRef >> 8);
   bufferArr[1] = (char) (startRef & 0xFF);
   bufferArr[2] = (char) (regCount >> 8);
   bufferArr[3] = (char) (regCount & 0xFF);
   bufferArr[4] = (char) (regArrLen * 2);

   //
   // Copy data into buffer
   //
   for (i = 0; i < regArrLen; i++)
   {
      bufferArr[5 + i * 2] = (char) (regArr[i] >> 8);
      bufferArr[6 + i * 2] = (char) (regArr[i] & 0xFF);
   }

   //
   // Transmit - receive
   //
   result = deliverMessage(slaveAddr, 0x10,
                           bufferArr,
                           5 + regArrLen * 2,
                           bufferArr, 4);
   // Check for error result codes
   if (result != FTALK_SUCCESS)
      return (result);

   //
   // Verify reply. We don't check the reference number because
   // some slaves don't reply a valid reference number here.
   // No reply check for broadcasts (address 0).
   //
   if (((bufferArr[2] != (char) (regCount >> 8)) ||
       (bufferArr[3] != (char) (regCount & 0xFF))) && (slaveAddr != 0))
      return (FTALK_INVALID_REPLY_ERROR);

   return (FTALK_SUCCESS);
}


/*****************************************************************************
 * Bit Access function codes 1 and 2
 *****************************************************************************/

/**
 * Modbus function 1,
 * Read Coil Status/Read Coils.
 *
 * Reads the contents of the discrete outputs (coils, 0:00000 table).
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255)
 * @param startRef Start reference (Range: 1 - 65536)
 * @param bitArr Buffer which will contain the data read
 * @param refCnt Number of coils to be read (Range: 1-2000)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note No broadcast supported
 */
int MbusMasterFunctions::readCoils(int slaveAddr, int startRef,
                                   int bitArr[], int refCnt)
{
   TRACELOG4("Read coils %d %d %d\n", slaveAddr, startRef, refCnt);

   return (readBits(0x01, slaveAddr, startRef, bitArr, refCnt));
}


/**
 * Modbus function 2,
 * Read Inputs Status/Read Input Discretes.
 *
 * Reads the contents of the discrete inputs (input status, 1:00000 table).
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255)
 * @param startRef Start reference (Range: 1 - 65536)
 * @param bitArr Buffer which will contain the data read
 * @param refCnt Number of coils to be read (Range: 1-2000)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note No broadcast supported
 */
int MbusMasterFunctions::readInputDiscretes(int slaveAddr, int startRef,
                                            int bitArr[], int refCnt)
{
   TRACELOG4("Read input discretes %d %d %d\n", slaveAddr, startRef, refCnt);

   return (readBits(0x02, slaveAddr, startRef, bitArr, refCnt));
}


/*****************************************************************************
 * Modbus function code 3
 *****************************************************************************/

/**
 * Modbus function 3,
 * Read Holding Registers/Read Multiple Registers.
 *
 * Reads the contents of the output registers
 * (holding registers, 4:00000 table).
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255)
 * @param startRef Start register (Range: 1 - 65536)
 * @param regArr Buffer which will be filled with the data read
 * @param refCnt Number of registers to be read (Range: 1-125)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note No broadcast supported
 */
int MbusMasterFunctions::readMultipleRegisters(int slaveAddr,
                                               int startRef,
                                               short regArr[], int refCnt)

{
   TRACELOG3("Read multiple registers %d %d\n", slaveAddr, startRef);

   return (readRegisters(0x03, slaveAddr, startRef, regArr, refCnt, refCnt));
}


/**
 * Modbus function 3 for 32-bit long int data types,
 * Read Holding Registers/Read Multiple Registers as long int data.
 *
 * Reads the contents of pairs of consecutive output registers (holding
 * registers, 4:00000 table) into 32-bit long int values.
 *
 * @remark Depending on the 32-bit Mode setting, an int will
 * be transferred as two consecutive 16-bit registers (Standard) or
 * as one 32-bit register (Daniel/Enron).
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255)
 * @param startRef Start reference (Range: 1 - 65536)
 * @param int32Arr Buffer which will be filled with the data read
 * @param refCnt Number of long integers to be read (Range: 1-62)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note No broadcast supported
 */
int MbusMasterFunctions::readMultipleLongInts(int slaveAddr,
                                              int startRef,
                                              int int32Arr[], int refCnt)
{
   int result;
   int wordCnt;

   TRACELOG3("Read multiple long ints %d %d\n", slaveAddr, startRef);

   // Adjust register count in ENRON mode
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & DANIEL_ENRON_32_BIT_MODE) != 0)
      wordCnt = refCnt;
   else
      wordCnt = refCnt * 2;

   result = readRegisters(0x03, slaveAddr, startRef, (short *) int32Arr, refCnt * 2, wordCnt);

   if (((slaveConfigFlagsArr[slaveAddr] & INTS_ARE_BIG_ENDIAN) != 0) ^ bigEndianMachine)
      swap32BitWords((unsigned int *) int32Arr, refCnt);

   return (result);
}


/**
 * Modbus function 3 for 32-bit modulo-10000 long int data types,
 * Read Holding Registers/Read Multiple Registers as modulo-10000 long int
 * data.
 *
 * Reads the contents of pairs of consecutive output registers (holding
 * registers, 4:00000 table) representing a modulo-10000 long int value
 * into 32-bit int values and performs number format conversion.
 *
 * @remark Depending on the 32-bit Mode setting, an int will
 * be transferred as two consecutive 16-bit registers (Standard) or
 * as one 32-bit register (Daniel/Enron).
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255)
 * @param startRef Start reference (Range: 1 - 65536)
 * @param int32Arr Buffer which will be filled with the data read
 * @param refCnt Number of M10K integers to be read (Range: 1-62)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note No broadcast supported
 */
int MbusMasterFunctions::readMultipleMod10000(int slaveAddr,
                                              int startRef,
                                              int int32Arr[], int refCnt)
{
   int i;
   int result;
   short *shortArr = (short *) int32Arr;
   int wordCnt;

   TRACELOG3("Read multiple mod 10000 %d %d\n", slaveAddr, startRef);

   // Adjust register count in ENRON mode
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & DANIEL_ENRON_32_BIT_MODE) != 0)
      wordCnt = refCnt;
   else
      wordCnt = refCnt * 2;

   result = readRegisters(0x03, slaveAddr, startRef, (short *) int32Arr, refCnt * 2, wordCnt);

   for (i = 0; i < refCnt; i++)
      int32Arr[i] = shortArr[i * 2 + 1] + shortArr[i * 2] * 10000L;

   return (result);
}


/**
 * Modbus function 3 for 32-bit float data types,
 * Read Holding Registers/Read Multiple Registers as float data.
 *
 * Reads the contents of pairs of consecutive output registers (holding
 * registers, 4:00000 table) into float values.
 *
 * @remark Depending on the 32-bit Mode setting, an int will
 * be transferred as two consecutive 16-bit registers (Standard) or
 * as one 32-bit register (Daniel/Enron).
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255)
 * @param startRef Start reference (Range: 1 - 65536)
 * @param float32Arr Buffer which will be filled with the data read
 * @param refCnt Number of float values to be read (Range: 1-62)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note No broadcast supported
 */
int MbusMasterFunctions::readMultipleFloats(int slaveAddr,
                                            int startRef,
                                            float float32Arr[], int refCnt)
{
   int result;
   int wordCnt;

   TRACELOG3("Read multiple floats %d %d\n", slaveAddr, startRef);

   // Adjust register count in ENRON mode
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & DANIEL_ENRON_32_BIT_MODE) != 0)
      wordCnt = refCnt;
   else
      wordCnt = refCnt * 2;

   result = readRegisters(0x03, slaveAddr, startRef, (short *) float32Arr, refCnt * 2, wordCnt);

   if (((slaveConfigFlagsArr[slaveAddr] & FLOATS_ARE_BIG_ENDIAN) != 0) ^ bigEndianFloatMachine)
      swap32BitWords((unsigned int *) float32Arr, refCnt);

   return (result);
}


/*****************************************************************************
 * Modbus function code 4
 *****************************************************************************/

/**
 * Modbus function 4,
 * Read Input Registers.
 *
 * Read the contents of the input registers
 * (3:00000 table).
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255)
 * @param startRef Start register (Range: 1 - 65536)
 * @param regArr  Buffer which will be filled with the data read.
 * @param refCnt Number of registers to be read (Range: 1-125)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note No broadcast supported
 */
int MbusMasterFunctions::readInputRegisters(int slaveAddr, int startRef,
                                            short regArr[], int refCnt)
{
   TRACELOG3("Read input registers (short) %d %d\n", slaveAddr, startRef);

   return (readRegisters(0x04, slaveAddr, startRef, regArr, refCnt, refCnt));
}


/**
 * Modbus function 4 for 32-bit long int data types,
 * Read Input Registers as long int data
 *
 * Reads the contents of pairs of consecutive input registers (3:00000
 * table) into 32-bit long int values.
 *
 * @remark Depending on the 32-bit Mode setting, an int will
 * be transferred as two consecutive 16-bit registers (Standard) or
 * as one 32-bit register (Daniel/Enron).
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255)
 * @param startRef Start reference (Range: 1 - 65536)
 * @param int32Arr Buffer which will be filled with the data read
 * @param refCnt Number of long integers to be read (Range: 1-62)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note No broadcast supported
 */
int MbusMasterFunctions::readInputLongInts(int slaveAddr,
                                           int startRef,
                                           int int32Arr[], int refCnt)
{
   int result;
   int wordCnt;

   TRACELOG3("Read input long ints %d %d\n", slaveAddr, startRef);

   // Adjust register count in ENRON mode
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & DANIEL_ENRON_32_BIT_MODE) != 0)
      wordCnt = refCnt;
   else
      wordCnt = refCnt * 2;

   result = readRegisters(0x04, slaveAddr, startRef, (short *) int32Arr, refCnt * 2, wordCnt);

   if (((slaveConfigFlagsArr[slaveAddr] & INTS_ARE_BIG_ENDIAN) != 0) ^ bigEndianMachine)
      swap32BitWords((unsigned int *) int32Arr, refCnt);

   return (result);
}


/**
 * Modbus function 4 for 32-bit modulo-10000 long int data types,
 * Read Input Registers as modulo-10000 long int data.
 *
 * Reads the contents of pairs of consecutive input registers (3:00000
 * table) representing a modulo-10000 long int value into 32-bit long int
 * values and performs number format conversion.
 *
 * @remark Depending on the 32-bit Mode setting, an int will
 * be transferred as two consecutive 16-bit registers (Standard) or
 * as one 32-bit register (Daniel/Enron).
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255)
 * @param startRef Start reference (Range: 1 - 65536)
 * @param int32Arr Buffer which will be filled with the data read
 * @param refCnt Number of M10K integers to be read (Range: 1-62)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note No broadcast supported
 */
int MbusMasterFunctions::readInputMod10000(int slaveAddr,
                                           int startRef,
                                           int int32Arr[], int refCnt)
{
   int i;
   int result;
   short *shortArr = (short *) int32Arr;
   int wordCnt;

   TRACELOG3("Read input mod 10000 %d %d\n", slaveAddr, startRef);

   // Adjust register count in ENRON mode
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & DANIEL_ENRON_32_BIT_MODE) != 0)
      wordCnt = refCnt;
   else
      wordCnt = refCnt * 2;

   result = readRegisters(0x04, slaveAddr, startRef, (short *) int32Arr, refCnt * 2, wordCnt);

   for (i = 0; i < refCnt; i++)
      int32Arr[i] = shortArr[i * 2 + 1] + shortArr[i * 2] * 10000L;

   return (result);
}


/**
 * Modbus function 4 for 32-bit float data types,
 * Read Input Registers as float data.
 *
 * Reads the contents of pairs of consecutive input registers (3:00000
 * table) into float values.
 *
 * @remark Depending on the 32-bit Mode setting, an int will
 * be transferred as two consecutive 16-bit registers (Standard) or
 * as one 32-bit register (Daniel/Enron).
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255)
 * @param startRef Start reference (Range: 1 - 65536)
 * @param float32Arr Buffer which will be filled with the data read
 * @param refCnt Number of floats to be read (Range: 1-62)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note No broadcast supported
 */
int MbusMasterFunctions::readInputFloats(int slaveAddr,
                                         int startRef,
                                         float float32Arr[], int refCnt)
{
   int result;
   int wordCnt;

   TRACELOG3("Read input sloats %d %d\n", slaveAddr, startRef);

   // Adjust register count in ENRON mode
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & DANIEL_ENRON_32_BIT_MODE) != 0)
      wordCnt = refCnt;
   else
      wordCnt = refCnt * 2;

   result = readRegisters(0x04, slaveAddr, startRef, (short *) float32Arr, refCnt * 2, wordCnt);

   if (((slaveConfigFlagsArr[slaveAddr] & FLOATS_ARE_BIG_ENDIAN) != 0) ^ bigEndianFloatMachine)
      swap32BitWords((unsigned int *) float32Arr, refCnt);

   return (result);
}


/*****************************************************************************
 * Bit access function code 5
 *****************************************************************************/

/**
 * Modbus function 5,
 * Force Single Coil/Write Coil.
 *
 * Sets a single discrete output variable (coil, 0:00000 table)
 * to either ON or OFF.
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 0 - 255)
 * @param bitAddr Coil address (Range: 1 - 65536)
 * @param bitVal true sets, false clears discrete output variable
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note Broadcast supported for serial protocols
 */
int MbusMasterFunctions::writeCoil(int slaveAddr, int bitAddr, int bitVal)
{
   int result;

   TRACELOG3("Write coil %d %d\n", slaveAddr, bitAddr);

   //
   // Check arguments
   //
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & REFERENCES_START_AT_ZERO) == 0)
      bitAddr--;
   if ((bitAddr < 0) || (bitAddr >= 0x10000))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   //
   // Assemble Modbus request packet
   //
   bufferArr[0] = (char) (bitAddr >> 8);
   bufferArr[1] = (char) (bitAddr & 0xFF);
   bufferArr[2]  = bitVal ? (char) -1 : (char) 0;
   bufferArr[3] = 0;

   //
   // Transmit - receive
   //
   result = deliverMessage(slaveAddr, 0x05,
                           bufferArr, 4,
                           bufferArr, 4);
   // Check for error result codes
   if (result != FTALK_SUCCESS)
      return (result);

#ifndef DISABLE_REPLY_CHECK
   //
   // Verify reply. We don't check the reference number because
   // some slaves don't reply a valid reference number here.
   // We also tolerate the Schneider PLC response which is not
   // conform with their own specification.
   // No reply check for broadcasts (address 0).
   //
   if ((((bufferArr[2] != (((char) bitVal) ? (char) -1 : 0)) ||
         (bufferArr[3] != 0)) &&
        ((bufferArr[2] != 0) ||   // Schneider PLC response ...
         (bufferArr[3] != 1))) && (slaveAddr != 0))
      return (FTALK_INVALID_REPLY_ERROR);
#endif

   return (FTALK_SUCCESS);
}


/*****************************************************************************
 * Word access function code 6
 *****************************************************************************/

/**
 * Modbus function 6,
 * Preset Single Register/Write Single Register.
 *
 * Writes a value into a single output register
 * (holding register, 4:00000 reference).
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 0 - 255)
 * @param regAddr Register address (Range: 1 - 65536)
 * @param regVal  Data to be sent
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note Broadcast supported for serial protocols
 */
int MbusMasterFunctions::writeSingleRegister(int slaveAddr,
                                             int regAddr,
                                             short regVal)
{
   int result;

   TRACELOG3("Write single register %d %d\n", slaveAddr, regAddr);

   //
   // Check arguments
   //
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & REFERENCES_START_AT_ZERO) == 0)
      regAddr--;
   if ((regAddr < 0) || (regAddr >= 0x10000))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   //
   // Assemble Modbus request packet
   //
   bufferArr[0] = (char) (regAddr >> 8);
   bufferArr[1] = (char) (regAddr & 0xFF);
   bufferArr[2] = (char) (regVal >> 8);
   bufferArr[3] = (char) (regVal & 0xFF);

   //
   // Transmit - receive
   //
   result = deliverMessage(slaveAddr, 0x06,
                           bufferArr, 4,
                           bufferArr, 4);
   // Check for error result codes
   if (result != FTALK_SUCCESS)
      return (result);

#ifndef DISABLE_REPLY_CHECK
   //
   // Verify reply. We don't check the reference number because
   // some slaves don't reply a valid reference number here.
   // We also tolerate the Schneider PLC response which is not
   // conform with their own specification.
   // No reply check for broadcasts (address 0).
   //
   if ((((bufferArr[2] != (char) (regVal >> 8)) ||
         (bufferArr[3] != (char) (regVal & 0xFF))) &&
        ((bufferArr[2] != 0) ||   // Schneider PLC response ...
         (bufferArr[3] != 2))) && (slaveAddr != 0))
      return (FTALK_INVALID_REPLY_ERROR);
#endif

   return (FTALK_SUCCESS);
}


/*****************************************************************************
 * Diagnostic function codes 7 and 8
 *****************************************************************************/

/**
 * Modbus function 7,
 * Read Exception Status.
 *
 * Reads the eight exception status coils within the slave device.
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255 for serial, 0 - 255 for TCP)
 * @param statusBytePtr Slave status byte. The meaning of this status byte is
 * slave specific and varies from device to device.
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note No broadcast supported
 */
int MbusMasterFunctions::readExceptionStatus(int slaveAddr,
                                             unsigned char *statusBytePtr)
{
   int result;

   TRACELOG2("Read exception status %d\n", slaveAddr);

   //
   // Check arguments
   //
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   //
   // Transmit - receive
   //
   result = deliverMessage(slaveAddr, 0x07,
                           bufferArr, 0,
                           bufferArr, 1);
   // Check for error result codes
   if (result != FTALK_SUCCESS)
      return (result);

   *statusBytePtr = (unsigned char) bufferArr[0];

   return (FTALK_SUCCESS);
}


/**
 * Modbus function code 8, sub-function 00,
 * Return Query Data
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255 for serial, 0 - 255 for TCP)
 * @param queryArr Buffer with data to be sent
 * @param echoArr Buffer which will contain the data read
 * @param len Number of bytes send sent and read back
 * @return FTALK_SUCCESS on success, FTALK_INVALID_REPLY_ERROR
 * if reply does not match query data or error code. See @ref buserror for a
 * list of error codes.
 * @note No broadcast supported
 */
int MbusMasterFunctions::returnQueryData(int slaveAddr,
                                         const unsigned char queryArr[],
                                         unsigned char echoArr[], int len)
{
   int result;

   TRACELOG2("Diagnostic 0 0 (Return Query Data) %d\n", slaveAddr);

   // Clear result array
   memset(echoArr, 0, len);

   //
   // Check arguments
   //
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((len <= 0) || (len > 250))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   //
   // Assemble Modbus request packet
   //
   bufferArr[0] = 0; // Sub-function code
   bufferArr[1] = 0; // Sub-function code

   //
   // Copy data into buffer
   //
   memcpy(&bufferArr[2], queryArr, len);

   //
   // Transmit - receive
   //
   result = deliverMessage(slaveAddr, 0x08,
                           bufferArr, 2 + len,
                           bufferArr, 2 + len);
   // Check for error result codes
   if (result != FTALK_SUCCESS)
      return (result);

   //
   // Copy data into buffer
   //
   memcpy(echoArr, &bufferArr[2], len);

   // Check response
   if (memcmp(queryArr, echoArr, len) != 0)
      return (FTALK_INVALID_REPLY_ERROR);
   else
      return (FTALK_SUCCESS);
}


/**
 * Modbus function code 8, sub-function 01,
 * Restart Communications Option
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255 for serial, 0 - 255 for TCP)
 * @param clearEventLog Flag when set to one clears in addition the slave's
 *                      communication even log.
 * @return FTALK_SUCCESS on success. See @ref buserror for a
 *         list of error codes.
 * @note No broadcast supported
 */
int MbusMasterFunctions::restartCommunicationsOption(int slaveAddr,
                                                     int clearEventLog)
{
   int result;

   TRACELOG2("Diagnostic 0 1 (Restart Communications Option) %d\n", slaveAddr);

   //
   // Check arguments
   //
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   //
   // Assemble Modbus request packet
   //
   bufferArr[0] = 0; // Sub-function code
   bufferArr[1] = 1; // Sub-function code
   bufferArr[2]  = clearEventLog ? (char) -1 : (char) 0;
   bufferArr[3] = 0;

   //
   // Transmit - receive
   //
   result = deliverMessage(slaveAddr, 0x08,
                           bufferArr, 4,
                           bufferArr, 4);
   return (result);
}


/*****************************************************************************
 * Bit access function code 15
 *****************************************************************************/

/**
 * Modbus function 15 (0F Hex),
 * Force Multiple Coils.
 *
 * Writes binary values into a sequence of discrete outputs
 * (coils, 0:00000 table).
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 0 - 255)
 * @param startRef Start reference (Range: 1 - 65536)
 * @param bitArr  Buffer which contains the data to be sent
 * @param refCnt Number of coils to be written (Range: 1-1968)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note Broadcast supported for serial protocols
 */
int MbusMasterFunctions::forceMultipleCoils(int slaveAddr, int startRef,
                                            const int bitArr[], int refCnt)
{
   int i;
   int result;
   int numBytes = (refCnt + 7) / 8;

   TRACELOG4("Force multiple coils %d %d %d\n", slaveAddr, startRef, refCnt);

   //
   // Check arguments
   //
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & REFERENCES_START_AT_ZERO) == 0)
      startRef--;
   if ((startRef < 0) || (startRef >= 0x10000))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((refCnt <= 0) || (refCnt > MAX_FC15_COILS))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   //
   // Assemble Modbus request packet
   //
   bufferArr[0] = (char) (startRef >> 8);
   bufferArr[1] = (char) (startRef & 0xFF);
   bufferArr[2] = (char) (refCnt >> 8);
   bufferArr[3] = (char) (refCnt & 0xFF);
   bufferArr[4] = (char) (numBytes);

   //
   // Copy data into buffer
   //
   {
      int byteIdx = 4; // same index as numBytes
      int bitIdx;

      for (i = 0; i < refCnt; i++)
      {
         bitIdx = i % 8;
         if (bitIdx == 0)
         {
            byteIdx++;
            bufferArr[byteIdx] = 0;
         }
         if (bitArr[i])
            bufferArr[byteIdx] |= (char) (1 << bitIdx); // Set bit
      }
   }

   //
   // Transmit - receive
   //
   result = deliverMessage(slaveAddr, 0x0F,
                           bufferArr, 5 + numBytes,
                           bufferArr, 4);
   // Check for error result codes
   if (result != FTALK_SUCCESS)
      return (result);

   //
   // Verify reply
   // No reply check for broadcasts (address 0).
   //
   if (((bufferArr[2] != (char) (refCnt >> 8)) ||
       (bufferArr[3] != (char) (refCnt & 0xFF))) && (slaveAddr != 0))
      return (FTALK_INVALID_REPLY_ERROR);

   return (FTALK_SUCCESS);
}


/*****************************************************************************
 * Modbus function 16 (10 Hex)
 *****************************************************************************/

/**
 * Modbus function 16 (10 Hex),
 * Preset Multiple Registers/Write Multiple Registers.
 *
 * Writes values into a sequence of output registers
 * (holding registers, 4:00000 table).
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 0 - 255)
 * @param startRef Start register (Range: 1 - 65536)
 * @param regArr Buffer with the data to be sent.
 * @param refCnt Number of registers to be written (Range: 1-123)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note Broadcast supported for serial protocols
 */
int MbusMasterFunctions::writeMultipleRegisters(int slaveAddr,
                                                int startRef,
                                                const short regArr[],
                                                int refCnt)
{
   TRACELOG3("Write multiple registers %d %d\n", slaveAddr, startRef);

   return (writeRegisters(slaveAddr, startRef, regArr, refCnt, refCnt));
}


/**
 * Modbus function 16 (10 Hex) for 32-bit long int data types,
 * Preset Multiple Registers/Write Multiple Registers with
 * long int data.
 *
 * Writes long int values into pairs of output registers
 * (holding registers, 4:00000 table).
 *
 * @remark Depending on the 32-bit Mode setting, an int will
 * be transferred as two consecutive 16-bit registers (Standard) or
 * as one 32-bit register (Daniel/Enron).
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 0 - 255)
 * @param startRef Start reference (Range: 1 - 65536)
 * @param int32Arr Buffer with the data to be sent
 * @param refCnt Number of long integers to be sent (Range: 1-61)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note Broadcast supported for serial protocols
 */
int MbusMasterFunctions::writeMultipleLongInts(int slaveAddr,
                                               int startRef,
                                               const int int32Arr[],
                                               int refCnt)
{
   char tmpBufArr [MAX_DATA_SIZE];
   short *shortArr = (short *) tmpBufArr;
   int wordCnt;

   TRACELOG3("Write multiple long ints %d %d\n", slaveAddr, startRef);

   // Adjust register count in ENRON mode
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & DANIEL_ENRON_32_BIT_MODE) != 0)
      wordCnt = refCnt;
   else
      wordCnt = refCnt * 2;

   // Copy data into a local buffer because we have to modify it
   if (refCnt * 4 > (int) sizeof(tmpBufArr))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   memcpy(tmpBufArr, int32Arr, refCnt * 4);

   if (((slaveConfigFlagsArr[slaveAddr] & INTS_ARE_BIG_ENDIAN) != 0) ^ bigEndianMachine)
      swap32BitWords((unsigned int *) tmpBufArr, refCnt);

   return (writeRegisters(slaveAddr, startRef, shortArr, refCnt * 2, wordCnt));
}


/**
 * Modbus function 16 (10 Hex) for 32-bit modulo-10000 long int data types,
 * Preset Multiple Registers/Write Multiple Registers with
 * modulo-10000 long int data.
 *
 * Writes long int values into pairs of output registers (holding
 * registers, 4:00000 table) representing a modulo-10000 long int value and
 * performs number format conversion.
 *
 * @remark Depending on the 32-bit Mode setting, an int will
 * be transferred as two consecutive 16-bit registers (Standard) or
 * as one 32-bit register (Daniel/Enron).
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 0 - 255)
 * @param startRef Start reference (Range: 1 - 65536)
 * @param int32Arr Buffer with the data to be sent
 * @param refCnt Number of long integer values to be sent (Range: 1-61)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note Broadcast supported for serial protocols
 */
int MbusMasterFunctions::writeMultipleMod10000(int slaveAddr,
                                               int startRef,
                                               const int int32Arr[],
                                               int refCnt)
{
   int i, j;
   char tmpBufArr [MAX_DATA_SIZE];
   short *shortArr = (short *) tmpBufArr;
   int wordCnt;

   TRACELOG3("Write multiple mod 10000 %d %d\n", slaveAddr, startRef);

   // Adjust register count in ENRON mode
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & DANIEL_ENRON_32_BIT_MODE) != 0)
      wordCnt = refCnt;
   else
      wordCnt = refCnt * 2;

   // Copy data into a local buffer because we have to modify it
   if (refCnt * 4 > (int) sizeof(tmpBufArr))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   memcpy(tmpBufArr, int32Arr, refCnt * 4);

   for (i = 0, j = 0; i < refCnt; i++)
   {
      shortArr[j++] = (short) (int32Arr[i] / 10000L);
      shortArr[j++] = (short) (int32Arr[i] % 10000L);
   }
   return (writeRegisters(slaveAddr, startRef, shortArr, refCnt * 2, wordCnt));
}


/**
 * Modbus function 16 (10 Hex) for 32-bit float data types,
 * Preset Multiple Registers/Write Multiple Registers with float data.
 *
 * Writes float values into pairs of output registers (holding
 * registers, 4:00000 table).
 *
 * @remark Depending on the 32-bit Mode setting, an int will
 * be transferred as two consecutive 16-bit registers (Standard) or
 * as one 32-bit register (Daniel/Enron).
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 0 - 255)
 * @param startRef Start reference (Range: 1 - 65536)
 * @param float32Arr Buffer with the data to be sent
 * @param refCnt Number of float values to be sent (Range: 1-61)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note Broadcast supported for serial protocols
 */
int MbusMasterFunctions::writeMultipleFloats(int slaveAddr,
                                             int startRef,
                                             const float float32Arr[],
                                             int refCnt)
{
   char tmpBufArr [MAX_DATA_SIZE];
   short *shortArr = (short *) tmpBufArr;
   int wordCnt;

   TRACELOG3("Write multiple floats %d %d\n", slaveAddr, startRef);

   // Adjust register count in ENRON mode
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & DANIEL_ENRON_32_BIT_MODE) != 0)
      wordCnt = refCnt;
   else
      wordCnt = refCnt * 2;

   // Copy data into a local buffer because we have to modify it
   if (refCnt * sizeof(float) > sizeof(tmpBufArr))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   memcpy(tmpBufArr, float32Arr, refCnt * sizeof(float));

   if (((slaveConfigFlagsArr[slaveAddr] & FLOATS_ARE_BIG_ENDIAN) != 0) ^ bigEndianFloatMachine)
      swap32BitWords((unsigned int *) tmpBufArr, refCnt);

   return (writeRegisters(slaveAddr, startRef, shortArr, refCnt * 2, wordCnt));
}


/*****************************************************************************
 * Word access function codes 22 and 23
 *****************************************************************************/

/**
 * Modbus function 22 (16 Hex),
 * Mask Write Register.
 *
 * Masks bits according to an AND & an OR mask into a single output register
 * (holding register, 4:00000 reference). Masking is done as follows:
 * result = (currentVal AND andMask) OR (orMask AND (NOT andMask))
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255 for serial, 0 - 255 for TCP)
 * @param regAddr Register address (Range: 1 - 65536)
 * @param andMask  Mask to be applied as a logic AND to the register
 * @param orMask  Mask to be applied as a logic OR to the register
 * @note No broadcast supported
 */
int MbusMasterFunctions::maskWriteRegister(int slaveAddr,
                                           int regAddr,
                                           short andMask,
                                           short orMask)
{
   int result;

   TRACELOG5("Mask write register  %d %d %X %X\n",
             slaveAddr, regAddr, andMask, orMask);

   //
   // Check arguments
   //
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & REFERENCES_START_AT_ZERO) == 0)
      regAddr--;
   if ((regAddr < 0) || (regAddr >= 0x10000))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   //
   // Assemble Modbus request packet
   //
   bufferArr[0] = (char) (regAddr >> 8);
   bufferArr[1] = (char) (regAddr & 0xFF);
   bufferArr[2] = (char) (andMask >> 8);
   bufferArr[3] = (char) (andMask & 0xFF);
   bufferArr[4] = (char) (orMask >> 8);
   bufferArr[5] = (char) (orMask & 0xFF);

   //
   // Transmit - receive
   //
   result = deliverMessage(slaveAddr, 0x16,
                           bufferArr, 6,
                           bufferArr, 6);
   // Check for error result codes
   if (result != FTALK_SUCCESS)
      return (result);

#ifndef DISABLE_REPLY_CHECK
   //
   // Verify reply
   // No reply check for broadcasts (address 0).
   //
   if (((bufferArr[0] != (char) (regAddr >> 8)) ||
        (bufferArr[1] != (char) (regAddr & 0xFF)) ||
        (bufferArr[2] != (char) (andMask >> 8)) ||
        (bufferArr[3] != (char) (andMask & 0xFF)) ||
        (bufferArr[4] != (char) (orMask >> 8)) ||
        (bufferArr[5] != (char) (orMask & 0xFF))) && (slaveAddr != 0))
      return (FTALK_INVALID_REPLY_ERROR);
#endif

   return (FTALK_SUCCESS);
}


/**
 * Modbus function 23 (17 Hex),
 * Read/Write Registers.
 *
 * Combines reading and writing of the output registers in one transaction
 * (holding registers, 4:00000 table).
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255 for serial, 0 - 255 for TCP)
 * @param readRef Start register for reading (Range: 1 - 65536)
 * @param readArr Buffer which will contain the data read
 * @param readCnt Number of registers to be read (Range: 1-125)
 * @param writeRef Start register for writing (Range: 1 - 65536)
 * @param writeArr Buffer with data to be sent
 * @param writeCnt Number of registers to be written (Range: 1-121)
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 * list of error codes.
 * @note No broadcast supported
 */
int MbusMasterFunctions::readWriteRegisters(int slaveAddr,
                                            int readRef,
                                            short readArr[], int readCnt,
                                            int writeRef,
                                            const short writeArr[],
                                            int writeCnt)
{
   int i;
   int result;

   TRACELOG4("Read write registers %d %d %d\n",
             slaveAddr, readRef, writeRef);

   // Clear result array
   memset(readArr, 0, sizeof(short) * readCnt);

   //
   // Check arguments
   //
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((slaveConfigFlagsArr[slaveAddr & 0xFF] & REFERENCES_START_AT_ZERO) == 0)
   {
      readRef--;
      writeRef--;
   }
   if ((readRef < 0) || (readRef >= 0x10000))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((writeRef < 0) || (writeRef >= 0x10000))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((readCnt <= 0) || (readCnt > MAX_FC23_READ))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((writeCnt <= 0) || (writeCnt > MAX_FC23_WRITE))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   //
   // Assemble Modbus request packet
   //
   bufferArr[0] = (char) (readRef >> 8);
   bufferArr[1] = (char) (readRef & 0xFF);
   bufferArr[2] = (char) (readCnt >> 8);
   bufferArr[3] = (char) (readCnt & 0xFF);
   bufferArr[4] = (char) (writeRef >> 8);
   bufferArr[5] = (char) (writeRef & 0xFF);
   bufferArr[6] = (char) (writeCnt >> 8);
   bufferArr[7] = (char) (writeCnt & 0xFF);
   bufferArr[8] = (char) (writeCnt * 2);

   //
   // Copy data into buffer
   //
   for (i = 0; i < writeCnt; i++)
   {
      bufferArr[9 + i * 2] = (char) (writeArr[i] >> 8);
      bufferArr[10 + i * 2] = (char) (writeArr[i] & 0xFF);
   }

   //
   // Transmit - receive
   //
   result = deliverMessage(slaveAddr, 0x17,
                           bufferArr,
                           9 + writeCnt * 2,
                           bufferArr,
                           1 + readCnt * 2);
   // Check for error result codes
   if (result != FTALK_SUCCESS)
      return (result);

   //
   // Verify reply
   //
   if ((bufferArr[0] != (char) (readCnt * 2)))
      return (FTALK_INVALID_REPLY_ERROR);

   //
   // Copy data into buffer
   //
   for (i = 0; i < readCnt; i++)
      readArr[i] = (short) ((bufferArr[1 + i * 2] << 8) +
                            (bufferArr[2 + i * 2] & 0xFF));

   return (FTALK_SUCCESS);
}


/*****************************************************************************
 * User Defined Function Codes (65-72 and 100-110)
 *****************************************************************************/

/**
 * User Defined Function Code
 * @ingroup devicespecific
 *
 * This method can be used to implement User Defined Function Codes.
 * The caller has only to pass the user data to this function. The assembly of
 * the Modbus frame (the so called ADU) including checksums, slave address and
 * function code and subsequentially the transmission, is taken care of by this
 * method.
 *
 * The modbus specification reserves function codes 65-72 and 100-110 for user
 * defined functions.
 *
 * @note Modbus functions usually have an implied response length and therefore
 *                   the number of bytes expected to be received is known at
 *                   the time when sending the request. In case of a custom
 *                   Modbus function with an open or unknown response length,
 *                   this function can not be used.
 *
 * @param slaveAddr  Modbus address of slave device or unit
 * identifier (Range: 1 - 255 for serial, 0 - 255 for TCP)
 * @param functionCode
 *                   Custom function code to be used for Modbus transaction
 *                   (1-127)
 * @param requestData
 *                   Pointer to data sent as request (not including slave
 *                   address or function code)
 * @param requestLen Length of request data structure (0-252)
 * @param responseData
 *                   Pointer to data structure which holds response data
 * @param responseLenPtr
 *                   Length of response data (0-252). The number of bytes
 *                   expected to be sent as response
 *                   must be known when submitting the request.
 *
 * @return FTALK_SUCCESS on success or error code. See @ref buserror for a
 *         list of error codes.
 */
int MbusMasterFunctions::customFunction(int slaveAddr,
                                        int functionCode,
                                        void* requestData, size_t requestLen,
                                        void* responseData, size_t *responseLenPtr)
{
   int result;
   int bytesReceived;

   TRACELOG3("Custom function %d %d\n", slaveAddr, functionCode);

   if (!responseLenPtr)
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   // Clear result array
   memset(responseData, 0, *responseLenPtr);

   //
   // Check arguments
   //
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((functionCode <= 0) || (functionCode > 127))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((requestLen < 0) || (requestLen > MAX_DATA_SIZE))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if ((*responseLenPtr < 0) || (*responseLenPtr > MAX_DATA_SIZE))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   //
   // Transmit - receive
   //
   result = deliverMessage(slaveAddr, functionCode,
                           (char *) requestData, (int) requestLen,
                           (char *) responseData, (int) *responseLenPtr,
                           &bytesReceived);
   *responseLenPtr = bytesReceived;
   return (result);
}


/*****************************************************************************
 * Protocol Configuration
 *****************************************************************************/

/**
 * Configures time-out.  This function sets the operation or socket
 * time-out to the specified value.
 *
 * @remark The time-out value is indicative only and not guaranteed to be
 * maintained. How precise it is followed depends on the operating system
 * used, it's scheduling priority and it's system timer resolution.
 * @note A protocol must be closed in order to configure it.
 * @param msTime Timeout value in ms (Range: 1 - 100000)
 * @retval FTALK_SUCCESS Success
 * @retval FTALK_ILLEGAL_ARGUMENT_ERROR Argument out of range
 * @retval FTALK_ILLEGAL_STATE_ERROR Protocol is already open
 */
int MbusMasterFunctions::setTimeout(int msTime)
{
   if ((msTime < 1) || (msTime > 100000))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if (isOpen())
      return (FTALK_ILLEGAL_STATE_ERROR);
   this->timeOut = msTime;

   return (FTALK_SUCCESS);
}


/**
 * Configures poll delay. This function sets the delay time
 * which applies between two consecutive Modbus read/write.
 * A value of 0 disables the poll delay.
 *
 * @remark The delay value is indicative only and not guaranteed to be
 * maintained. How precise it is followed depends on the operating system
 * used, it's scheduling priority and it's system timer resolution.
 * @note A protocol must be closed in order to configure it.
 * @param msTime Delay time in ms (Range: 0 - 100000), 0 disables
 * poll delay
 * @retval FTALK_SUCCESS Success
 * @retval FTALK_ILLEGAL_ARGUMENT_ERROR Argument out of range
 * @retval FTALK_ILLEGAL_STATE_ERROR Protocol is already open
 */
int MbusMasterFunctions::setPollDelay(int msTime)
{
   TRACELOG2("Set poll delay %d\n", pollDelay);

   if ((msTime < 0) || (msTime > 100000))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if (isOpen())
      return (FTALK_ILLEGAL_STATE_ERROR);
   this->pollDelay = msTime;
   return (FTALK_SUCCESS);
}


/**
 * Configures the automatic retry setting.
 * A value of 0 disables any automatic retries.
 * @note A protocol must be closed in order to configure it.
 *
 * @param retries Retry count (Range: 0 - 10), 0 disables retries
 * @retval FTALK_SUCCESS Success
 * @retval FTALK_ILLEGAL_ARGUMENT_ERROR Argument out of range
 * @retval FTALK_ILLEGAL_STATE_ERROR Protocol is already open
 */
int MbusMasterFunctions::setRetryCnt(int retries)
{
   TRACELOG2("Set retry count %d\n", retries);

   if ((retries < 0) || (retries > 10))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);
   if (isOpen())
      return (FTALK_ILLEGAL_STATE_ERROR);
   this->retryCnt = retries;
   return (FTALK_SUCCESS);
}


/*****************************************************************************
 * Slave Configuration
 *****************************************************************************/

/**
 * Configures all slaves for Standard 32-bit Mode.
 *
 * In Standard 32-bit Register Mode a 32-bit value is transmitted
 * as two consecutive 16-bit Modbus registers.
 *
 * @remark This is the default mode
 */
void MbusMasterFunctions::configureStandard32BitMode()
{
   int i;

   for (i = 0; i <= 255; i++)
      configureStandard32BitMode(i);
}


/**
 * Configures all slaves for Daniel/ENRON 32-bit Mode.
 *
 * Some Modbus flavours like the Daniel/Enron protocol
 * represent a 32-bit value using one 32-bit Modbus register instead
 * of two 16-bit registers.
 */
void MbusMasterFunctions::configureEnron32BitMode()
{
   int i;

   for (i = 0; i <= 255; i++)
      configureEnron32BitMode(i);
}


/**
 * Configures the reference counting scheme to start with one for
 * all slaves.
 *
 * This renders the reference range to be 1 to 0x10000 and register #0
 * is an illegal register.
 *
 * @remark This is the default mode
 */
void MbusMasterFunctions::configureCountFromOne()
{
   int i;

   for (i = 0; i <= 255; i++)
      configureCountFromOne(i);
}


/**
 * Configures the reference counting scheme to start with
 * zero for all slaves.
 *
 * This renders the valid reference range to be 0 to 0xFFFF.
 *
 * This renders the first register to be #0 for all slaves.
 */
void MbusMasterFunctions::configureCountFromZero()
{
   int i;

   for (i = 0; i <= 255; i++)
      configureCountFromZero(i);
}


/**
 * Configures 32-bit int data type functions to do a word swap.
 *
 * Modbus is using little-endian word order for 32-bit values. The data
 * transfer functions operating upon 32-bit int data types can be
 * configured to do a word swap which enables them to read 32-bit data
 * correctly from a big-endian slave.
 */
void MbusMasterFunctions::configureBigEndianInts()
{
   int i;

   for (i = 0; i <= 255; i++)
      configureBigEndianInts(i);
}


/**
 * Configures float data type functions to do a word swap.
 *
 * The data functions operating upon 32-bit float data
 * types can be configured to do a word swap.
 * @note Most platforms store floats in IEEE 754 little-endian order
 * which does not need a word swap.
 */
void MbusMasterFunctions::configureSwappedFloats()
{
   int i;

   for (i = 0; i <= 255; i++)
      configureSwappedFloats(i);
}


/**
 * Configures 32-bit int data type functions not to do a word swap. This is
 * the default.
 */
void MbusMasterFunctions::configureLittleEndianInts()
{
   int i;

   for (i = 0; i <= 255; i++)
      configureLittleEndianInts(i);
}


/**
 * Configures float data type functions not to do a word swap. This
 * is the default.
 */
void MbusMasterFunctions::configureIeeeFloats()
{
   int i;

   for (i = 0; i <= 255; i++)
      configureIeeeFloats(i);
}


/**
 * Configures a slave for Standard 32-bit Register Mode.
 *
 * In Standard 32-bit Register Mode a 32-bit value is transmitted
 * as two consecutive 16-bit Modbus registers.
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255). A value of zero configures the behaviour for
 * broadcasting.
 * @retval FTALK_SUCCESS Success
 * @retval FTALK_ILLEGAL_ARGUMENT_ERROR Argument out of range
 *
 * @remark This is the default mode
 * @note This function call also re-configures the endianess to default
 *       little-endian for 32-bit values!
 *
 */
int MbusMasterFunctions::configureStandard32BitMode(int slaveAddr)
{
   // Check argument
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   slaveConfigFlagsArr[slaveAddr] &= ~DANIEL_ENRON_32_BIT_MODE;
   configureLittleEndianInts(slaveAddr);
   configureIeeeFloats(slaveAddr);
   return (FTALK_SUCCESS);
}


/**
 * Configures all slaves for Daniel/ENRON 32-bit Mode.
 *
 * Some Modbus flavours like the Daniel/Enron protocol
 * represent a 32-bit value using one 32-bit Modbus register instead
 * of two 16-bit registers.
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255). A value of zero configures the behaviour for
 * broadcasting.
 * @retval FTALK_SUCCESS Success
 * @retval FTALK_ILLEGAL_ARGUMENT_ERROR Argument out of range
 * @note This function call also re-configures the endianess to big-endian
 *       for 32-bit values as defined by the Daniel/ENRON protocol!
 */
int MbusMasterFunctions::configureEnron32BitMode(int slaveAddr)
{
   // Check argument
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   slaveConfigFlagsArr[slaveAddr] |= DANIEL_ENRON_32_BIT_MODE;
   configureBigEndianInts(slaveAddr);
   configureSwappedFloats(slaveAddr);
   return (FTALK_SUCCESS);
}


/**
 * Configures a slave's reference counting scheme to start with one.
 *
 * This renders the reference range to be 1 to 0x10000 and register #0
 * is an illegal register.
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255). A value of zero configures the behaviour for
 * broadcasting.
 *
 * @remark This is the default mode
 */
int MbusMasterFunctions::configureCountFromOne(int slaveAddr)
{
   // Check argument
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   slaveConfigFlagsArr[slaveAddr] &= ~REFERENCES_START_AT_ZERO;
   return (FTALK_SUCCESS);
}


/**
 * Configures a slave's reference counting scheme to start with zero.
 * This is also known as PDU addressing.
 *
 * This renders the valid reference range to be 0 to 0xFFFF.
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255). A value of zero configures the behaviour for
 * broadcasting.
 */
int MbusMasterFunctions::configureCountFromZero(int slaveAddr)
{
   // Check argument
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   slaveConfigFlagsArr[slaveAddr] |= REFERENCES_START_AT_ZERO;
   return (FTALK_SUCCESS);
}


/**
 * Disables word swapping for int data type functions
 * on a per slave basis.
 *
 * Modbus is using little-endian word order for 32-bit values. This setting
 * assumes that the slave also serves 32-bit data in little
 * little-endian word order.
 *
 * @remark This is the default mode
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255). A value of zero configures the behaviour for
 * broadcasting.
 */
int MbusMasterFunctions::configureLittleEndianInts(int slaveAddr)
{
   // Check argument
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   slaveConfigFlagsArr[slaveAddr] &= ~INTS_ARE_BIG_ENDIAN;
   return (FTALK_SUCCESS);
}


/**
 * Enables int data type functions to do a word swap
 * on a per slave basis.
 *
 * Modbus is using little-endian word order for 32-bit values. The data
 * transfer functions operating upon 32-bit int data types can be
 * configured to do a word swap which enables them to read 32-bit data
 * correctly from a big-endian machine.
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255). A value of zero configures the
 * behaviour for broadcasting.
 */
int MbusMasterFunctions::configureBigEndianInts(int slaveAddr)
{
   // Check argument
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   slaveConfigFlagsArr[slaveAddr] |= INTS_ARE_BIG_ENDIAN;
   return (FTALK_SUCCESS);
}


/**
 * Disables float data type functions to do a word swap
 * on a per slave basis.
 *
 * Modbus is using little-endian word order for 32-bit values. This setting
 * assumes that the slave also serves 32-bit floats in little
 * little-endian word order which is the most common case.
 *
 * @remark This is the default mode
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255). A value of zero configures the behaviour for
 * broadcasting.
 */
int MbusMasterFunctions::configureIeeeFloats(int slaveAddr)
{
   // Check argument
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   slaveConfigFlagsArr[slaveAddr] &= ~FLOATS_ARE_BIG_ENDIAN;
   return (FTALK_SUCCESS);
}


/**
 * Enables float data type functions to do a word swap
 * on a per slave basis.
 *
 * The data functions operating upon 32-bit float data
 * types can be configured to do a word swap.
 * @note Most platforms store floats in IEEE 754 little-endian order
 * which does not need a word swap.
 *
 * @param slaveAddr Modbus address of slave device or unit
 * identifier (Range: 1 - 255). A value of zero configures the behaviour for
 * broadcasting.
 */
int MbusMasterFunctions::configureSwappedFloats(int slaveAddr)
{
   // Check argument
   if ((slaveAddr < 0) || (slaveAddr > 255))
      return (FTALK_ILLEGAL_ARGUMENT_ERROR);

   // IEEE 754 uses little-endian order, which means big-endian swaps
   slaveConfigFlagsArr[slaveAddr] |= FLOATS_ARE_BIG_ENDIAN;
   return (FTALK_SUCCESS);
}


/*****************************************************************************
 * Utility routines
 *****************************************************************************/

/**
 * Returns the library version number.
 *
 * @return Library version string
 */
const TCHAR *MbusMasterFunctions::getPackageVersion()
{
   return (packageVersion);
}

