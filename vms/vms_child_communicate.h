#ifndef VMS_CHILD_COMMUNICATE_H
#define VMS_CHILD_COMMUNICATE_H

#ifdef __cplusplus
extern "C" {
#endif

#define _MBX_NAME_TEMPLATE_ "python_fds_from_%x"
#define _MBX_COMMUNICATE_SIZE_ 64
#define _MAX_FD_TO_CLOSE_ 256

unsigned short cre_pid_mbx(int pid, int for_reading);
int del_pid_mbx(unsigned short channel, int for_reading);
int send_fds_to_keep(unsigned short channel, PyObject *py_fds_to_keep);
int close_fds_except_read_from_mbx(unsigned short channel);

#ifdef __cplusplus
}
#endif

#endif
