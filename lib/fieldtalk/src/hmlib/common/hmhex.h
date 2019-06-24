/**
 * @file hmhex.h
 *
 * Hex conversion utilities header file
 *
 * @if NOTICE
 *
 * $Id: hmhex.h,v 1.4 2008/11/13 06:44:41 henrik Exp $
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


#ifndef _HMHEX_H_INCLUDED
#define _HMHEX_H_INCLUDED


/*****************************************************************************
 * Function prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void strHexValPrint (char *buffer, unsigned long val, int type);

char loNibbletoHex(char val);

char hiNibbletoHex(char val);

int hexToNibble(char nibble);

int hexToByte(char hiNibble, char loNibble);


#ifdef __cplusplus
}
#endif


#endif /* ifdef ..._H_INCLUDED */
