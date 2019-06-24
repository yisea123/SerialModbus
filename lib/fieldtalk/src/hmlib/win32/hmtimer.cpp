/**
 * @file win32/hmtimer.cpp
 *
 * Timer utility routines and classes for the Windows 32-bit platform
 *
 * @if NOTICE
 *
 * $Id: hmtimer.cpp,v 1.14 2009/03/31 01:13:20 henrik Exp $
 *
 * Copyright (c) 2002-2009 proconX Pty Ltd. All rights reserved. 
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


// Platform header
#include <limits.h>

// HM C/C++ library header
#include "hmtimer.hpp"

// Version identifier
static char ver[] = "$Id: hmtimer.cpp,v 1.14 2009/03/31 01:13:20 henrik Exp $";



/*****************************************************************************
 * Helper functions
 *****************************************************************************/

#define TICK_GET_DIFF(a, b) (a - b)


/*****************************************************************************
 * Timer implementation
 *****************************************************************************/

/**
 * @defgroup hm_timer_win32 Timer Routines for Windows 32-bit platforms
 *
 * These functions form a Windows specific implementation of a platform
 * independant interface layer for access to timers.
 */
//@{

/**
 * Sleeps for a given amount of milliseconds.
 *
 * @param msTime Time in milliseconds
 * @remark The sleep time is only an approximate value and usually rounded
 * to the smallest base time resolution of the OS. It also depends on the
 * OS' task scheduling mechanism.
 */
void HmTimer::sleepMillis(unsigned long msTime)
{
   if (msTime == 0)
      return;

   // Handle this special case
   if (msTime == INFINITE)
   {
      msTime--;
      Sleep(1);
   }
   Sleep(msTime);
}


/**
 * Creates a Timer object and initialises internal data.
 */
HmTimer::HmTimer()
{
   (void) ver; // Avoid compiler warning
   startMillis(0);
}


/**
 * Creates a Timer object and starts the timer
 *
 * @param msTime Expiry time interval in milliseconds
 */
HmTimer::HmTimer(unsigned long msTime)
{
   startMillis(msTime);
}


/**
 * Tests if timer has expired.
 *
 * @return true if it is expired else false
 */
int HmTimer::isExpired()
{
   return ((TICK_GET_DIFF(GetTickCount(), startTicks) >= timerTicks));
}


/**
 * Starts a timer. The timer will expire after a number of milliseconds has
 * elapsed.
 *
 * @param msTime Expiry time interval in milliseconds
 */
void HmTimer::startMillis(unsigned long msTime)
{
   timerTicks = msTime;
   startTicks = GetTickCount();
}


/**
 * Queries how much longer the timer has to run for before expiry.
 *
 * @return Remaining time until expiry in ms
 */
unsigned long HmTimer::left()
{
   DWORD diffTicks;

   diffTicks = TICK_GET_DIFF(GetTickCount(), startTicks);
   if (diffTicks < timerTicks)
   {
      return (timerTicks - diffTicks);
   }
   else
      return 0;
}


/**
 * Queries how much time has passed since the timer expired
 *
 * @return Time elapsed since expiry in ms
 */
unsigned long HmTimer::over()
{
   DWORD diffTicks;

   diffTicks = TICK_GET_DIFF(GetTickCount(), startTicks);
   if (diffTicks > timerTicks)
   {
      return (diffTicks - timerTicks);
   }
   else
      return 0;
}


/**
 * Sleeps until the timer expires.
 */
void HmTimer::sleepUntilExpiry()
{
   DWORD diffTicks;

   diffTicks = TICK_GET_DIFF(GetTickCount(), startTicks);
   if (diffTicks < timerTicks)
   {
      sleepMillis(timerTicks - diffTicks);
   }
}

//@}


/*****************************************************************************
 * Test case
 *****************************************************************************/

#ifdef TEST_RUN

#include <stdio.h>

int main()
{
   HmTimer timer1, timer2;

   printf("Test Run for hmtimer.cpp\n\n");

   printf("Sleeping for 0 ms... ");
   HmTimer::sleepMillis(0);
   printf("OK\n"); // If we arrive here we succeeded

   printf("Left test ... ");
   timer1.startMillis(0);
   if (timer1.left() != 0)
   {
      printf("FAILED!\n");
      return (1);
   }
   timer1.startMillis(1);
   if (timer1.left() != 1)
   {
      printf("FAILED!\n");
      return (1);
   }
   timer1.startMillis(100);
   if (timer1.left() != 100)
   {
      printf("FAILED!\n");
      return (1);
   }
   timer1.startMillis(10000000);
   if (timer1.left() != 10000000)
   {
      printf("FAILED!\n");
      return (1);
   }
   timer1.startMillis(0xFFFFFFFE);
   if (timer1.left() != 0xFFFFFFFE)
   {
      printf("FAILED!\n");
      return (1);
   }
   timer1.startMillis(0xFFFFFFFF);
   if (timer1.left() != 0xFFFFFFFF)
   {
      printf("FAILED!\n");
      return (1);
   }
   printf("OK\n");

   printf("Sleeping for 100 ms... ");
   timer1.startMillis(150);
   timer2.startMillis(50);
   HmTimer::sleepMillis(100);
   if (timer1.isExpired() || !timer2.isExpired())
   {
      printf("FAILED!\n");
      return (1);
   }
   else
   {
      printf("OK\n");
   }

   printf("Antitest 1 ... ");
   timer1.startMillis(190);
   HmTimer::sleepMillis(200);
   if (!timer1.isExpired())
   {
      printf("FAILED!\n");
      return (1);
   }
   else
   {
      printf("OK\n");
   }

   printf("Antitest 2 ... ");
   timer1.startMillis(210);
   HmTimer::sleepMillis(200);
   if (timer1.isExpired())
   {
      printf("FAILED!\n");
      return (1);
   }
   else
   {
      printf("OK\n");
   }

   printf("Sleeping for 1000 ms... ");
   timer1.startMillis(1100);
   timer2.startMillis(900);
   HmTimer::sleepMillis(1000);
   if (timer1.isExpired() || !timer2.isExpired())
   {
      printf("FAILED!\n");
      return (1);
   }
   else
   {
      printf("OK\n");
   }

   printf("Sleeping for 500 ms... \n");
   timer1.startMillis(1000);
   timer2.startMillis(1100);
   HmTimer::sleepMillis(500);
   printf("Sleeping until expiry (another 500 ms) ...");
   timer1.sleepUntilExpiry();
   if (!timer1.isExpired() || timer2.isExpired())
   {
      printf("FAILED!\n");
      return (1);
   }
   else
   {
      printf("OK\n");
   }

   printf("Sleeping for 10000 ms... ");
   timer1.startMillis(11000);
   timer2.startMillis(9000);
   HmTimer::sleepMillis(10000);
   if (timer1.isExpired() || !timer2.isExpired())
   {
      printf("FAILED!\n");
      return (1);
   }
   else
   {
      printf("OK\n");
   }

   printf("Sleeping for 10 ms... ");
   timer1.startMillis(18);
   timer2.startMillis(5);
   HmTimer::sleepMillis(10);
   if (timer1.isExpired() || !timer2.isExpired())
   {
      printf("FAILED!");
      return (1);
   }
   else
   {
      printf("OK\n");
   }

   return (0);
}


#endif
