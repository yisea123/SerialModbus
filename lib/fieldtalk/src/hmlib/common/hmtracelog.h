/**
 * @file hmtracelog.h
 *
 * Simple trace logging for debugging
 *
 * @if NOTICE
 *
 * $Id: hmtracelog.h,v 1.2 2008/11/13 06:44:41 henrik Exp $
 *
 * Copyright (c) 2003-2008 proconX Pty Ltd. All rights reserved. 
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


#ifndef _HMTRACELOG_H_INCLUDED
#define _HMTRACELOG_H_INCLUDED


#ifdef TRACELOG
#  include <stdio.h>
#endif

#ifdef TRACELOG
#  define TRACELOG1(a)                printf("TRACELOG: " a)
#  define TRACEADD1(a)                printf(a)
#  define TRACELOG2(a,b)              printf("TRACELOG: " a, b)
#  define TRACEADD2(a,b)              printf(a, b)
#  define TRACELOG3(a,b,c)            printf("TRACELOG: " a, b, c)
#  define TRACELOG4(a,b,c,d)          printf("TRACELOG: " a, b, c, d)
#  define TRACELOG5(a,b,c,d,e)        printf("TRACELOG: " a, b, c, d, e)
#else
#  define TRACELOG1(a)                ((void) 0)
#  define TRACEADD1(a)                ((void) 0)
#  define TRACELOG2(a,b)              ((void) 0)
#  define TRACEADD2(a,b)              ((void) 0)
#  define TRACELOG3(a,b,c)            ((void) 0)
#  define TRACELOG4(a,b,c,d)          ((void) 0)
#  define TRACELOG5(a,b,c,d,e)        ((void) 0)
#endif


#endif /* ifdef ..._H_INCLUDED */

