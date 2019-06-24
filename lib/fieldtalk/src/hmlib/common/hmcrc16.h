/**
 * @file hmcrc16.h
 *
 * 16-bit CRC calculation routine header file
 *
 * @if NOTICE
 *
 * $Id: hmcrc16.h,v 1.7 2008/11/13 06:44:41 henrik Exp $
 *
 * Copyright (c) 2002-2008 proconX Pty Ltd. All rights reserved. 
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


#ifndef _HMCRC16_H_INCLUDED
#define _HMCRC16_H_INCLUDED


/* Platform header */
#include <stdlib.h>


/*****************************************************************************
 * Function prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


unsigned short calcCrc16(const char byteArr[], size_t byteCnt,
                         unsigned char startVal);

unsigned short addCrc16(char byteVal, unsigned short oldCrc);


#ifdef __cplusplus
}
#endif


#endif /* ifdef ..._H_INCLUDED */
