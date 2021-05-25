#define __NEW_STARLET 1

#include <builtins.h>
#include <descrip.h>
#include <efndef.h>
#include <gen64def.h>
#include <iodef.h>
#include <iosbdef.h>
#include <lib$routines.h>
#include <psldef.h>
#include <starlet.h>
#include <string.h>
#include <stsdef.h>

#include "vms_mbx_util.h"

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

