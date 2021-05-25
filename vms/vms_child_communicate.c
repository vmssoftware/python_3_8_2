#define _POSIX_EXIT 1

#define __NEW_STARLET 1

#include "Python.h"

#include "vms_child_communicate.h"
#include "vms_mbx_util.h"
#include "vms_select.h"

#include <fcntl.h>

#ifndef _DO_TRACE_FDS_MBX_
#define _TRACE_LINE_(line)
#define _TRACE_LINE_V_(line, ...)
#else
#define _TRACE_LINE_(line) \
    do {    \
        char name[64];  \
        sprintf(name, "tmp_cmm_%x.txt", getpid());   \
        int fd = open(name, O_CREAT | O_APPEND | O_RDWR, 0600); \
        if (fd) {   \
            write(fd, line, strlen(line));  \
            close(fd);  \
        }   \
    } while(0)

#define _TRACE_LINE_V_(line, ...) \
    do {    \
        char buf[256];  \
        sprintf(buf, line, __VA_ARGS__); \
        _TRACE_LINE_(buf);  \
    } while(0)
#endif

unsigned short cre_pid_mbx(int pid, int for_reading) {
    unsigned short channel = 0;
    char name[sizeof(_MBX_NAME_TEMPLATE_) + 32];
    
    sprintf(name, _MBX_NAME_TEMPLATE_, pid);
    channel = simple_create_mbx(name, _MBX_COMMUNICATE_SIZE_);
    
    _TRACE_LINE_V_("created mbx: %s%s\n", name, for_reading?" for_reading":"");

    return channel;
}

int del_pid_mbx(unsigned short channel, int for_reading) {
    _TRACE_LINE_V_("deleted mbx%s\n", for_reading?" for_reading":"");
    return vms_channel_free(channel);
}

int send_fds_to_keep(unsigned short chan, PyObject *py_fds_to_keep) {
    _TRACE_LINE_("send_fds_to_keep\n");
    if (!PyTuple_CheckExact(py_fds_to_keep)) {
        _TRACE_LINE_("not a tuple?\n");
        return -1;
    }
    unsigned char buf[_MBX_COMMUNICATE_SIZE_] = {0};
    unsigned int len = PyTuple_GET_SIZE(py_fds_to_keep);
    for (unsigned int i = 0; i < len; ++i) {
        unsigned int keep_fd = PyLong_AsUnsignedLong(PyTuple_GET_ITEM(py_fds_to_keep, i));
        if ((keep_fd>>3) < _MBX_COMMUNICATE_SIZE_) {
            buf[keep_fd>>3] |= 1 << (keep_fd & 7);
            _TRACE_LINE_V_("keep_fd %i\n", keep_fd);
        }
    }
    return simple_write_mbx(chan, buf, _MBX_COMMUNICATE_SIZE_);
}

int close_fds_except_read_from_mbx(unsigned short channel) {
    unsigned char buf[_MBX_COMMUNICATE_SIZE_] = {0};
    unsigned int amount = 0;
    if (simple_read_mbx(channel, buf, _MBX_COMMUNICATE_SIZE_) > 0) {
        unsigned char *ptr = buf;
        unsigned int start_fd = 3;
        unsigned int end_fd = _MAX_FD_TO_CLOSE_;
        for (unsigned int i = 0; i < _MBX_COMMUNICATE_SIZE_; ++i, ++ptr) {
            unsigned int keep_fd = i << 3;
            while (*ptr) {
                if (*ptr & 1) {
                    for (unsigned int fd_num = start_fd; fd_num < keep_fd; ++fd_num) {
                        if (close(fd_num) == 0) {
                            _TRACE_LINE_V_("read = closed %i\n", fd_num);
                            ++amount;
                        }
                    }
                    _TRACE_LINE_V_("read = keep_fd %i\n", keep_fd);
                    start_fd = keep_fd + 1;
                }
                *ptr >>= 1;
                ++keep_fd;
            }
        }
        while (start_fd < end_fd) {
            if (close(start_fd) == 0) {
                _TRACE_LINE_V_("read = closed %i\n", start_fd);
                ++amount;
            }
            ++start_fd;
        }
    } else {
        _TRACE_LINE_("read = no data\n");
    }
    return amount;
}
