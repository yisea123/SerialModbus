// HM platform detection
#include "hmplatf.h"

#if defined(__WIN32__)
#  include "../win32/hmserio.cpp"
#elif defined(__QNX__)
#  include "../qnx/hmserio.cpp"
#elif defined(__VXWORKS__)
#  include "../vxworks/hmserio.cpp"
#elif defined(__LINUX__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#  include "../linux/hmserio.cpp"
#elif defined(__UNIX__)
#  include "../posix1/hmserio.cpp"
#else
#  error No serial port implementation for this platform!
#endif

