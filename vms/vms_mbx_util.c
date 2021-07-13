#define __NEW_STARLET 1

#include <builtins.h>
#include <dcdef.h>
#include <descrip.h>
#include <dvidef.h>
#include <efndef.h>
#include <errno.h>
#include <gen64def.h>
#include <iledef.h>
#include <iodef.h>
#include <iosbdef.h>
#include <lib$routines.h>
#include <psldef.h>
#include <ssdef.h>
#include <starlet.h>
#include <string.h>
#include <stsdef.h>
#include <unixio.h>

#include "vms_mbx_util.h"
int vms_channel_lookup(int fd, unsigned short *channel);
int vms_channel_lookup_by_name(char* name, unsigned short *channel);

unsigned short simple_create_mbx(const char *name, int mbx_size) {
    unsigned short channel = 0;
    struct dsc$descriptor_s dsc_name, *pdsc_name = 0;
    if (name) {
        dsc_name.dsc$a_pointer = (char*)name;
        dsc_name.dsc$w_length = strlen(name);
        dsc_name.dsc$b_dtype = DSC$K_DTYPE_T;
        dsc_name.dsc$b_class = DSC$K_CLASS_S;
        pdsc_name = &dsc_name;
    }
    sys$crembx(0, &channel, mbx_size, 0, 0, PSL$C_USER, pdsc_name, 0, 0);
    return channel;
}

int simple_free_mbx(unsigned short channel) {
    return sys$dassgn(channel);
}

int simple_check_mbx(unsigned short channel) {
    IOSB iosb;
    int status = sys$qiow(EFN$C_ENF, channel, IO$_SENSEMODE, &iosb, NULL, 0, 0, 0, 0, 0, 0, 0);
    if ($VMS_STATUS_SUCCESS(status) && $VMS_STATUS_SUCCESS(iosb.iosb$w_status)) {
        return iosb.iosb$w_bcnt;
    }
    return -1;
}

int simple_write_mbx(unsigned short channel, unsigned char *buf, int size) {
    IOSB iosb = {0};
    return sys$qiow(EFN$C_ENF, channel, IO$_WRITEVBLK | IO$M_NOW, &iosb, 0, 0, buf, size, 0, 0, 0, 0);
}

int simple_write_mbx_w(unsigned short channel, unsigned char *buf, int size) {
    IOSB iosb = {0};
    return sys$qiow(EFN$C_ENF, channel, IO$_WRITEVBLK, &iosb, 0, 0, buf, size, 0, 0, 0, 0);
}

int simple_write_mbx_eof(unsigned short channel) {
    IOSB iosb = {0};
    return sys$qiow(EFN$C_ENF, channel, IO$_WRITEOF | IO$M_NOW, &iosb, 0, 0, 0, 0, 0, 0, 0, 0);
}

int write_mbx_eof(int fd) {
    if (fd >= 0 && isapipe(fd) == 1) {
        unsigned short channel;
        if (vms_channel_lookup(fd, &channel) == 0) {
            simple_write_mbx_eof(channel);
            simple_free_mbx(channel);
            return 0;
        }
    }
    return -1;
}

int simple_read_mbx(unsigned short channel, unsigned char *buf, int size) {
    IOSB iosb = {0};
    int status = sys$qiow(EFN$C_ENF, channel, IO$_READVBLK | IO$M_NOW, &iosb, 0, 0, buf, size, 0, 0, 0, 0);
    if ($VMS_STATUS_SUCCESS(status) && $VMS_STATUS_SUCCESS(iosb.iosb$w_status)) {
        return iosb.iosb$w_bcnt;
    }
    return -1;
}

int simple_read_mbx_w(unsigned short channel, unsigned char *buf, int size) {
    IOSB iosb = {0};
    int status = SYS$QIOW(EFN$C_ENF, channel, IO$_READVBLK, &iosb, 0, 0, buf, size, 0, 0, 0, 0);
    if ($VMS_STATUS_SUCCESS(status) && $VMS_STATUS_SUCCESS(iosb.iosb$w_status)) {
        return iosb.iosb$w_bcnt;
    }
    return -1;
}

int simple_read_mbx_w_stream(unsigned short channel, unsigned char *buf, int size) {
    IOSB iosb = {0};
    int status = SYS$QIOW(EFN$C_ENF, channel, IO$_READVBLK | IO$M_STREAM, &iosb, 0, 0, buf, size, 0, 0, 0, 0);
    if ($VMS_STATUS_SUCCESS(status) && $VMS_STATUS_SUCCESS(iosb.iosb$w_status)) {
        return iosb.iosb$w_bcnt;
    }
    return -1;
}

unsigned __int64 global_timer_reqidt = 1;

int simple_read_mbx_timeout(unsigned short channel, unsigned char *buf, int size, int timeout_microseconds) {
    int status;
    struct _generic_64 timerOffset;
    unsigned __int64 timer_reqidt = 0;
    unsigned int timerEfn = 0;
    unsigned int qiowEfn = 0;
    unsigned int efnMask = 0;
    int qioStarted = 0;
    int retCode = 0;
    IOSB iosb = {0};

    timerOffset.gen64$q_quadword = -(timeout_microseconds * 10L);

    status = lib$get_ef(&timerEfn);
    if (!$VMS_STATUS_SUCCESS(status)) {
        retCode = -2;
        goto egress;
    }
    status = lib$get_ef(&qiowEfn);
    if (!$VMS_STATUS_SUCCESS(status)) {
        retCode = -3;
        goto egress;
    }
    if (timerEfn < 32 && qiowEfn < 32) {
        efnMask = (1 << timerEfn) | (1 << qiowEfn);
    } else if (timerEfn < 64 && qiowEfn < 64) {
        efnMask = (1 << (timerEfn - 32)) | (1 << (qiowEfn - 32));
    } else if (timerEfn < 96 && qiowEfn < 96) {
        efnMask = (1 << (timerEfn - 64)) | (1 << (qiowEfn - 64));
    } else if (timerEfn < 128 && qiowEfn < 128) {
        efnMask = (1 << (timerEfn - 96)) | (1 << (qiowEfn - 96));
    } else {
        retCode = -4;
        goto egress;
    }

    while (!timer_reqidt) {
        timer_reqidt = __ATOMIC_INCREMENT_QUAD(&global_timer_reqidt);
    }
    status = sys$setimr(timerEfn, &timerOffset, 0, timer_reqidt, 0);
    if (!$VMS_STATUS_SUCCESS(status)) {
        retCode = -5;
        goto egress;
    }

    status = sys$qio(qiowEfn, channel, IO$_READVBLK, &iosb, 0, 0, buf, size, 0, 0, 0, 0);
    if (!$VMS_STATUS_SUCCESS(status)) {
        retCode = -6;
        goto egress;
    }
    qioStarted = 1;

    status = sys$wflor(timerEfn, efnMask);
    if (!$VMS_STATUS_SUCCESS(status)) {
        retCode = -7;
        goto egress;
    }

    if ($VMS_STATUS_SUCCESS(iosb.iosb$w_status)) {
        retCode = iosb.iosb$w_bcnt;
    }

egress:
    if (qiowEfn) {
        lib$free_ef(&qiowEfn);
    }
    if (timerEfn) {
        lib$free_ef(&timerEfn);
    }
    if (timer_reqidt) {
        sys$cantim(timer_reqidt, PSL$C_USER);
    }
    if (qioStarted) {
        sys$cancel(channel);
    }

    return retCode;
}

unsigned int get_mbx_size(unsigned short channel) {
    unsigned int   mbx_buffer_size = 0;
    unsigned short mbx_buffer_size_len = 0;
    unsigned int   mbx_char = 0;
    unsigned short mbx_char_len = 0;
    ILE3 item_list[3];
    item_list[0].ile3$w_length = sizeof(mbx_buffer_size);
    item_list[0].ile3$w_code = DVI$_DEVBUFSIZ;
    item_list[0].ile3$ps_bufaddr = &mbx_buffer_size;
    item_list[0].ile3$ps_retlen_addr = &mbx_buffer_size_len;
    item_list[1].ile3$w_length = sizeof(mbx_char);
    item_list[1].ile3$w_code = DVI$_DEVCLASS;
    item_list[1].ile3$ps_bufaddr = &mbx_char;
    item_list[1].ile3$ps_retlen_addr = &mbx_char_len;
    memset(item_list + 2, 0, sizeof(ILE3));
    int status = SYS$GETDVIW(EFN$C_ENF, channel, 0, &item_list, 0, 0, 0, 0);
    if ($VMS_STATUS_SUCCESS(status) && (mbx_char & DC$_MAILBOX)) {
        return mbx_buffer_size;
    }
    return 0;
}

#undef _DO_TRACE_MBX_EOF_
#ifndef _DO_TRACE_MBX_EOF_
#define _TRACE_LINE_(line)
#define _TRACE_LINE_V_(line, ...)
#else
#include <fcntl.h>
#include <unixlib.h>
#define _TRACE_LINE_(line) \
    do {    \
        char name[64];  \
        sprintf(name, "mbx_eof_%x.txt", getpid());   \
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

#define _MAX_FD_TO_REGISTER_ 256

static int fd_map[_MAX_FD_TO_REGISTER_] = {0};

/*  pid == 0, no child (unmap)
    pid >  0, child is created by exec()
    pid <  0, child is created by lib$spawn()
*/
int map_fd_to_child(int fd, int pid) {
    if ((unsigned int)fd < (unsigned int)_MAX_FD_TO_REGISTER_) {
        fd_map[fd] = pid;
        return 0;
    }
    return -1;
}

int read_mbx(int fd, char *buf, int size) {
    if (fd < 0 || isapipe(fd) != 1) {
        return -1;
    }
    int fd_pid = 0;
    if ((unsigned int)fd < (unsigned int)_MAX_FD_TO_REGISTER_) {
        fd_pid = fd_map[fd];
    }
    unsigned short channel;
    IOSB iosb = {0};
    unsigned int op = IO$_READVBLK;
    // set result is not ok
    int nbytes = -1;
    errno = EVMSERR;
    char devicename[256];
    if (vms_channel_lookup_by_name(getname(fd, devicename, 1), &channel) == 0) {
        unsigned short mbx_size = get_mbx_size(channel);
        if (mbx_size < size) {
            size = mbx_size;
        }
        if (size > 0) {
            if (fd_pid >= -1) {
                // no lib$spawn(), use channel as regular stream
                op |= IO$M_STREAM;
            }
            int status = sys$qiow(EFN$C_ENF, channel, op, &iosb, NULL, 0, buf, size, 0, 0, 0, 0);
            if ($VMS_STATUS_SUCCESS(status)) {
                if (iosb.iosb$w_status == SS$_ENDOFFILE) {
                    errno = 0;
                    nbytes = 0;
                    _TRACE_LINE_V_("%i: \"%s\" eof from 0x%x", fd, devicename, iosb.iosb$l_pid);
                    if ((fd_pid < -1 && iosb.iosb$l_pid != -fd_pid) ||
                        (fd_pid > 0 && iosb.iosb$l_pid != fd_pid)) {
                        // accept EOF only given pid
                        nbytes = -1;
                        errno = EAGAIN;
                        _TRACE_LINE_(" again\n");
                    }
                    if (!nbytes) {
                        //we should prevent future reading/waiting already closed pipe
                        _TRACE_LINE_(" sentinel\n");
                        simple_write_mbx_eof(channel);
                        //free fd
                        map_fd_to_child(fd, 0);
                    }
                } else if (iosb.iosb$w_status == SS$_NORMAL) {
                    errno = 0;
                    nbytes = iosb.iosb$w_bcnt;
                    _TRACE_LINE_V_("%i: \"%s\" data[%i] from 0x%x", fd, devicename, nbytes, iosb.iosb$l_pid);
                    if (fd_pid < -1) {
                        // add LF to the end of each RECORD
                        buf[nbytes] = '\n';
                        ++nbytes;
                        _TRACE_LINE_(" EOL added\n");
                    } else if (!nbytes) {
                        // somebody writes zero length buffer into the stream...
                        // assume it is just emply line
                        buf[nbytes] = '\n';
                        ++nbytes;
                        _TRACE_LINE_(" EOL added to stream\n");
                    } else {
                        _TRACE_LINE_("\n");
                    }
                }
            }
        }
        sys$dassgn(channel);
    }
    return nbytes;
}

/*  test if name is like _MBA9999(9):
    returns:
        -1 error
        0  not a pipe
        1  is read end
        2  is write end
*/
int vms_isapipe_by_name(char *name) {
    if (!name || !*name) {
        // error
        return -1;
    }
    if (strncmp(name, "_MBA", 4) != 0) {
        // does not start with _MBA - not a pipe
        return 0;
    }
    char *ptr = name + 4;
    while(*(ptr++)) {
        if (*ptr == ':') {
            // found a semicolon
            break;
        }
        if (*ptr < '0' || *ptr > '9') {
            // not a number or semicolon - not a pipe
            return 0;
        }
    }
    if (ptr - name < 8 || ptr - name > 9) {
        // amount of number must be 4 or 5
        return 0;
    }
    if (strcmp(ptr, ":[].") == 0) {
        return 1;
    }
    return 2;
}

int vms_isapipe(int fd) {
    char name[256];
    return vms_isapipe_by_name(getname(fd, name, 1));
}
