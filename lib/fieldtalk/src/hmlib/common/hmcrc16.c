/**
 * @file hmcrc16.h
 *
 * 16-bit CRC calculation routine
 *
 * @if NOTICE
 *
 * $Id: hmcrc16.c,v 1.14 2010/05/17 10:51:22 henrik Exp $
 *
 * Copyright (c) 2002-2010 proconX Pty Ltd. All rights reserved.
 * CRC checksum algorithm from Schneider Automation Inc.
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


/* Platform header */
#include <stdlib.h>
#if defined(__AVR__)
#  include <avr/pgmspace.h>
#endif

/* HM C/C++ library header */
#include "hmcrc16.h"


/*
 * Table of CRC Values for high-order byte
 */
#if defined(__AVR__)
PROGMEM
#endif
static const char crcHiArr[] =  //lint --e{743} Negative character constant
{
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x00', '\xC1', '\x81', '\x40', '\x01', '\xC0', '\x80', '\x41',
   '\x01', '\xC0', '\x80', '\x41', '\x00', '\xC1', '\x81', '\x40'
};


/*
 * Table of CRC values for low-order byte
 */
#if defined(__AVR__)
PROGMEM
#endif
static const char crcLoArr[] =
{
   '\x00', '\xC0', '\xC1', '\x01', '\xC3', '\x03', '\x02', '\xC2',
   '\xC6', '\x06', '\x07', '\xC7', '\x05', '\xC5', '\xC4', '\x04',
   '\xCC', '\x0C', '\x0D', '\xCD', '\x0F', '\xCF', '\xCE', '\x0E',
   '\x0A', '\xCA', '\xCB', '\x0B', '\xC9', '\x09', '\x08', '\xC8',
   '\xD8', '\x18', '\x19', '\xD9', '\x1B', '\xDB', '\xDA', '\x1A',
   '\x1E', '\xDE', '\xDF', '\x1F', '\xDD', '\x1D', '\x1C', '\xDC',
   '\x14', '\xD4', '\xD5', '\x15', '\xD7', '\x17', '\x16', '\xD6',
   '\xD2', '\x12', '\x13', '\xD3', '\x11', '\xD1', '\xD0', '\x10',
   '\xF0', '\x30', '\x31', '\xF1', '\x33', '\xF3', '\xF2', '\x32',
   '\x36', '\xF6', '\xF7', '\x37', '\xF5', '\x35', '\x34', '\xF4',
   '\x3C', '\xFC', '\xFD', '\x3D', '\xFF', '\x3F', '\x3E', '\xFE',
   '\xFA', '\x3A', '\x3B', '\xFB', '\x39', '\xF9', '\xF8', '\x38',
   '\x28', '\xE8', '\xE9', '\x29', '\xEB', '\x2B', '\x2A', '\xEA',
   '\xEE', '\x2E', '\x2F', '\xEF', '\x2D', '\xED', '\xEC', '\x2C',
   '\xE4', '\x24', '\x25', '\xE5', '\x27', '\xE7', '\xE6', '\x26',
   '\x22', '\xE2', '\xE3', '\x23', '\xE1', '\x21', '\x20', '\xE0',
   '\xA0', '\x60', '\x61', '\xA1', '\x63', '\xA3', '\xA2', '\x62',
   '\x66', '\xA6', '\xA7', '\x67', '\xA5', '\x65', '\x64', '\xA4',
   '\x6C', '\xAC', '\xAD', '\x6D', '\xAF', '\x6F', '\x6E', '\xAE',
   '\xAA', '\x6A', '\x6B', '\xAB', '\x69', '\xA9', '\xA8', '\x68',
   '\x78', '\xB8', '\xB9', '\x79', '\xBB', '\x7B', '\x7A', '\xBA',
   '\xBE', '\x7E', '\x7F', '\xBF', '\x7D', '\xBD', '\xBC', '\x7C',
   '\xB4', '\x74', '\x75', '\xB5', '\x77', '\xB7', '\xB6', '\x76',
   '\x72', '\xB2', '\xB3', '\x73', '\xB1', '\x71', '\x70', '\xB0',
   '\x50', '\x90', '\x91', '\x51', '\x93', '\x53', '\x52', '\x92',
   '\x96', '\x56', '\x57', '\x97', '\x55', '\x95', '\x94', '\x54',
   '\x9C', '\x5C', '\x5D', '\x9D', '\x5F', '\x9F', '\x9E', '\x5E',
   '\x5A', '\x9A', '\x9B', '\x5B', '\x99', '\x59', '\x58', '\x98',
   '\x88', '\x48', '\x49', '\x89', '\x4B', '\x8B', '\x8A', '\x4A',
   '\x4E', '\x8E', '\x8F', '\x4F', '\x8D', '\x4D', '\x4C', '\x8C',
   '\x44', '\x84', '\x85', '\x45', '\x87', '\x47', '\x46', '\x86',
   '\x82', '\x42', '\x43', '\x83', '\x41', '\x81', '\x80', '\x40'
};


/**
 * Generates a 16-bit Cyclic Redundancy Check value
 *
 * The polynomial used is X**16 + X**15 + X**2 + 1. The calculation is
 * based on tables for efficiency.
 *
 * @param byteArr Buffer with bytes to be used for generation
 * @param byteCnt Number of bytes
 * @param startVal Start value (0 or 0xFF)
 * @return 16-bit CRC
 */
unsigned short calcCrc16(const char byteArr[],
                         size_t byteCnt, unsigned char startVal)
{
   char crcHi = (char) startVal; /* High byte of CRC initialized */
   char crcLo = (char) startVal; /* Low byte of CRC initialized */
   int index; /* Will index into CRC lookup */
   size_t i;

   for (i = 0; i < byteCnt; i++)
   {
      index = (int) ((crcHi & 0xFF) ^ (byteArr[i] & 0xFF));
#if defined(__AVR__)
      crcHi = (char) (crcLo ^ pgm_read_byte(&crcHiArr[index]));
      crcLo = (char) pgm_read_byte(&crcLoArr[index]);
#else
      crcHi = (char) (crcLo ^ crcHiArr[index]);
      crcLo = (char) crcLoArr[index];
#endif
   }
   return ((unsigned short) ((crcLo << 8) | (crcHi & 0xFF)));
}


/**
 * Adds a byte to a 16-bit Cyclic Redundancy Check value
 *
 * The polynomial used is X**16 + X**15 + X**2 + 1.
 *
 * @param byteVal Byte to add to CRC
 * @param oldCrc Previous 16-bit CRC
 * @return New 16-bit CRC
 */
unsigned short addCrc16(char byteVal, unsigned short oldCrc)
{
   int i;
   unsigned short current = (unsigned char) byteVal;

   for (i = 0 ; i < 8 ; i++, current >>= 1)
      oldCrc = (signed short) ((unsigned short) oldCrc >> 1) ^
               (((current ^ oldCrc) & 1) != 0 ? 0xA001 : 0);
   return (oldCrc);
}
