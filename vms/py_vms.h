#ifndef PLATFORM
#define PLATFORM "OpenVMS"
#endif

#ifndef ABIFLAGS
#define ABIFLAGS ""
#endif

#ifndef SOABI
#define SOABI "cpython-38-ia64-openvms"
#endif

#ifndef PYTHONPATH
#define PYTHONPATH ":plat-openvms"
#endif

#ifndef PREFIX
#define PREFIX "/python$root"
#endif

#ifndef EXEC_PREFIX
#define EXEC_PREFIX "/python$root"
#endif

#ifndef VERSION
#define VERSION "3.8"
#endif

#ifndef VPATH
#define VPATH ""
#endif

#define _POSIX_EXIT 1

#define SIGBUS     10

#define O_BINARY 0100
