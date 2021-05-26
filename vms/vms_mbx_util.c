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

#include "vms_mbx_util.h"
int vms_channel_lookup(int fd, unsigned short *channel);

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

int simple_write_mbx(unsigned short channel, unsigned char *buf, int size) {
    IOSB iosb = {0};
    return sys$qiow(EFN$C_ENF, channel, IO$_WRITEVBLK | IO$M_NOW, &iosb, 0, 0, buf, size, 0, 0, 0, 0);
}

int simple_read_mbx(unsigned short channel, unsigned char *buf, int size) {
    IOSB iosb = {0};
    int status = sys$qiow(EFN$C_ENF, channel, IO$_READVBLK | IO$M_NOW, &iosb, 0, 0, buf, size, 0, 0, 0, 0);
    if ($VMS_STATUS_SUCCESS(status) && $VMS_STATUS_SUCCESS(iosb.iosb$w_status)) {
        return iosb.iosb$w_bcnt;
    }
    return -1;
}

int simple_check_mbx(unsigned short channel) {
    IOSB iosb;
    int status = sys$qiow(EFN$C_ENF, channel, IO$_SENSEMODE, &iosb, NULL, 0, 0, 0, 0, 0, 0, 0);
    if ($VMS_STATUS_SUCCESS(status) && $VMS_STATUS_SUCCESS(iosb.iosb$w_status)) {
        return iosb.iosb$w_bcnt;
    }
    return -1;
}

static unsigned __int64 global_timer_reqidt = 1;

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

    timer_reqidt = __ATOMIC_INCREMENT_QUAD(&global_timer_reqidt);
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

int simple_read_mbx_w(unsigned short channel, unsigned char *buf, int size) {
    IOSB iosb = {0};
    int status = SYS$QIOW(EFN$C_ENF, channel, IO$_READVBLK, &iosb, 0, 0, buf, size, 0, 0, 0, 0);
    if ($VMS_STATUS_SUCCESS(status) && $VMS_STATUS_SUCCESS(iosb.iosb$w_status)) {
        return iosb.iosb$w_bcnt;
    }
    return -1;
}

unsigned short get_mbx_size(unsigned short channel) {
    unsigned short mbx_buffer_size = 0;
    unsigned short mbx_buffer_size_len = 0;
    unsigned int   mbx_char = 0;
    unsigned short mbx_char_len = 0;
    ILE3 item_list[3];
    item_list[0].ile3$w_length = 4;
    item_list[0].ile3$w_code = DVI$_DEVBUFSIZ;
    item_list[0].ile3$ps_bufaddr = &mbx_buffer_size;
    item_list[0].ile3$ps_retlen_addr = &mbx_buffer_size_len;
    item_list[1].ile3$w_length = 4;
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

#undef _PID_IS_PROVIDED_
#define _PID_IS_PROVIDED_(x) ((x) != 0 && (x) != -1)

int read_mbx(int fd, char *buf, int size, int pid) {
    unsigned short channel;
    IOSB iosb = {0};
    unsigned int op = IO$_READVBLK;
    int nbytes = 0;
    // set result is ok
    errno = 0;
    if (vms_channel_lookup(fd, &channel) == 0) {
        unsigned short mbx_size = get_mbx_size(channel);
        if (mbx_size < size) {
            size = mbx_size;
        }
        if (size <= 0) {
            errno = EVMSERR;
            sys$dassgn(channel);
            return -1;
        }
        if (!_PID_IS_PROVIDED_(pid)) {
            // no PID -> regular stream
            op |= IO$M_STREAM;
        }
        int status = sys$qiow(EFN$C_ENF, channel, op, &iosb, NULL, 0, buf, size, 0, 0, 0, 0);
        if ($VMS_STATUS_SUCCESS(status)) {
            if (iosb.iosb$w_status == SS$_ENDOFFILE) {
                if (_PID_IS_PROVIDED_(pid)) {
                    // PID is provided -> accept EOF only from provided PID
                    if (iosb.iosb$l_pid == pid) {
                        nbytes = 0;
                    } else {
                        nbytes = -1;
                        errno = EAGAIN;
                    }
                } else {
                    // no PID -> accept EOF from any process
                    nbytes = 0;
                }
            } else {
                nbytes = iosb.iosb$w_bcnt;
                if (_PID_IS_PROVIDED_(pid)) {
                    // PID is provided -> add LF to the end of RECORD
                    buf[nbytes] = '\n';
                    ++nbytes;
                }
                if (!nbytes) {
                    // somebody writes zero length buffer into the stream...
                    // assume it is just emply line
                    buf[nbytes] = '\n';
                    ++nbytes;
                }
            }
        }
        sys$dassgn(channel);
    }
    return nbytes;
}
