/* Minimal main program -- everything is loaded from the library */

#include "Python.h"
#include "pycore_pylifecycle.h"

#ifdef MS_WINDOWS
int
wmain(int argc, wchar_t **argv)
{
    return Py_Main(argc, argv);
}
#else
int
main(int argc, char **argv)
{
#ifdef __VMS
#ifdef _SEND_FDS_MBX_
    #include "vms/vms_child_communicate.h"
    int ppid = getppid();
    if (ppid) {
        unsigned short ch_pid_mbx = cre_pid_mbx(ppid, 1);
        if (ch_pid_mbx) {
            close_fds_except_read_from_mbx(ch_pid_mbx);
            del_pid_mbx(ch_pid_mbx, 1);
        }
    }
#endif
    exit(Py_BytesMain(argc, (char**)argv));
#else
    return Py_BytesMain(argc, argv);
#endif
}
#endif
