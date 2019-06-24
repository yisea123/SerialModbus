#include "hmtimer.hpp"

#if defined(__WIN32__)
#  include "../win32/hmtimer.cpp"
#elif (defined(_POSIX_TIMERS) && defined(_POSIX_MONOTONIC_CLOCK) && (_POSIX_MONOTONIC_CLOCK >= 0))
#  include "../posix4/hmtimer.cpp"
#elif defined(_POSIX_TIMERS) || defined(__VXWORKS__)
#  warning Your system may not support monotonic clocks. It is recommended to upgrade your OS version.
#  include "../posix4/hmtimer.cpp"
#elif defined(__UNIX__)
#  warning Your system does not support monotonic clocks. It is recommended to upgrade your runtime library and kernel.
#  include "../posix1/hmtimer.cpp"
#else
#  error No timer implementation for this platform!
#endif

