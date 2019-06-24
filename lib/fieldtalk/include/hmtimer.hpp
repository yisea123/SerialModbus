/**
 * @file hmtimer.hpp
 *
 * Timer utility routines and classes header file
 *
 * @if NOTICE
 *
 * $Id: hmtimer.hpp,v 1.26 2012/02/16 23:57:15 henrik Exp $
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


#ifndef _HMTIMER_H_INCLUDED
#define _HMTIMER_H_INCLUDED

#ifndef __cplusplus
#  error Must use C++ to compile this module!
#endif

// HM platform detection
#include "hmplatf.h"

#if defined(__WIN32__)
#  include <windows.h>
#elif defined(__VXWORKS__)
#  include <timers.h> // timerLib and clockLib
#  include <selectLib.h> // selectLib
#elif defined(__UNIX__)
#  include <unistd.h>
#  include <time.h>
// Include all headers where a timeval could hide:
#  include <sys/time.h>
#  include <sys/select.h>
#else
#  error "Platform not yet supported. Please add a new condition to this #if block or update hmplatf.h"
#endif


/*****************************************************************************
 * Timer interface
 *****************************************************************************/

/**
 * Timer class. This class implements a platform independent access
 * to timers.
 */
class
#if defined (_WINDLL) || defined(__DLL__)
   __declspec(dllexport)
#endif
HmTimer
{
  public:

   static void sleepMillis(unsigned long msTime);

   HmTimer();

   HmTimer(unsigned long msTime);

   int isExpired();

   void startMillis(unsigned long msTime);

   void sleepUntilExpiry();

   unsigned long left();

   unsigned long over();

#if defined(__UNIX__)
   void remain(timeval *remainTime); // Used for select on some platforms
#endif

  protected:

#if defined(__WIN32__)
     DWORD startTicks;
     DWORD timerTicks;
#elif defined(_POSIX_TIMERS) || defined(__VXWORKS__)
   struct timespec expireTime;
   static clockid_t clockMode;
#elif defined(__UNIX__)
   struct timeval expireTime;
#else
#  error No timer implementation for this platform!
#endif

  private:

   // Disable default operator and copy constructor
   HmTimer &operator=(HmTimer &);
   HmTimer(const HmTimer &);

};


#endif // ifdef ..._H_INCLUDED
