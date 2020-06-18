#ifndef PLATFORM
#define PLATFORM "OpenVMS"
#endif

#ifndef ABIFLAGS
#define ABIFLAGS "m"
#endif

#ifndef SOABI
#define SOABI "cpython-38m-ia64-openvms"
#endif

#ifndef PYTHONPATH
#define PYTHONPATH ":plat-openvms"
#endif

#ifndef PREFIX
// #define PREFIX "sys$sysdevice/cameron/python"
#define PREFIX "/python$root"
#endif

#ifndef EXEC_PREFIX
// #define EXEC_PREFIX "sys$sysdevice/cameron/python"
#define EXEC_PREFIX "/python$root"
#endif

#ifndef VERSION
#define VERSION "3.8"
#endif

#ifndef VPATH
#define VPATH ""
#endif

#define _POSIX_EXIT 1

// #define __SIGNED_INT_TIME_T

// #ifndef HGVERSION
// #define HGVERSION ""
// #endif

// #ifndef HGTAG
// #define HGTAG ""
// #endif

// #ifndef HGBRANCH
// #define HGBRANCH ""
// #endif
