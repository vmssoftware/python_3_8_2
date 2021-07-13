#define __NEW_STARLET 1

#include "vms_sleep.h"

#include <builtins.h>
#include <gen64def.h>
#include <lib$routines.h>
#include <psldef.h>
#include <ssdef.h>
#include <starlet.h>
#include <stdlib.h>
#include <string.h>
#include <stsdef.h>

int vms_sleep(int microsecs) {
    int status;
    struct _generic_64 timerOffset;
    timerOffset.gen64$q_quadword = -(microsecs * 10L);

    unsigned int timerEfn;
    status = LIB$GET_EF(&timerEfn);
    if (!$VMS_STATUS_SUCCESS(status)) {
        return -2;
    }

    status = SYS$SETIMR(timerEfn, &timerOffset, 0, 0, 0);
    if (!$VMS_STATUS_SUCCESS(status)) {
        return -3;
    }

    status = SYS$WAITFR(timerEfn);
    if (!$VMS_STATUS_SUCCESS(status)) {
        return -4;
    }

    status = LIB$FREE_EF(&timerEfn);
    if (!$VMS_STATUS_SUCCESS(status)) {
        return -5;
    }

    return 0;
}

extern unsigned __int64 global_timer_reqidt;

typedef struct {
    unsigned int signalEfn;
    unsigned int timerEfn;
    unsigned int efnMask;
    unsigned int efnStart;
} _waiter_struct;

void vms_waiter_free(void* waiter) {
    if (((_waiter_struct*)waiter)->timerEfn) {
        lib$free_ef(&((_waiter_struct*)waiter)->timerEfn);
    }
    if (((_waiter_struct*)waiter)->signalEfn) {
        lib$free_ef(&((_waiter_struct*)waiter)->signalEfn);
    }
    free(waiter);
}

void* vms_waiter_init() {
    _waiter_struct *waiter = (_waiter_struct *)malloc(sizeof(_waiter_struct));
    if (!waiter) {
        return (void*)-1;
    }
    memset(waiter, 0, sizeof(_waiter_struct));
    int status = lib$get_ef(&waiter->signalEfn);
    if (!$VMS_STATUS_SUCCESS(status)) {
        goto error;
    }
    status = lib$get_ef(&waiter->timerEfn);
    if (!$VMS_STATUS_SUCCESS(status)) {
        goto error;
    }
    if (waiter->timerEfn < 32 && waiter->signalEfn < 32) {
        waiter->efnMask = (1 << waiter->timerEfn) | (1 << waiter->signalEfn);
    } else if (waiter->timerEfn < 64 && waiter->signalEfn < 64) {
        waiter->efnStart = 32;
        waiter->efnMask = (1 << (waiter->timerEfn - waiter->efnStart)) | (1 << (waiter->signalEfn - waiter->efnStart));
    } else if (waiter->timerEfn < 96 && waiter->signalEfn < 96) {
        waiter->efnStart = 64;
        waiter->efnMask = (1 << (waiter->timerEfn - waiter->efnStart)) | (1 << (waiter->signalEfn - waiter->efnStart));
    } else if (waiter->timerEfn < 128 && waiter->signalEfn < 128) {
        waiter->efnStart = 96;
        waiter->efnMask = (1 << (waiter->timerEfn - waiter->efnStart)) | (1 << (waiter->signalEfn - waiter->efnStart));
    } else {
        goto error;
    }
    return (void*)waiter;
error:
    vms_waiter_free(waiter);
    return (void*)-1;
}

void vms_waiter_set(void* waiter) {
    sys$setef(((_waiter_struct*)waiter)->signalEfn);
}

void vms_waiter_clr(void* waiter) {
    sys$clref(((_waiter_struct*)waiter)->signalEfn);
}

int vms_waiter_wait(void* pwaiter, int timeout_microseconds) {
    int status;
    struct _generic_64 timerOffset;
    unsigned __int64 timer_reqidt = 0;
    int retcode = 0;
    unsigned int state;
    _waiter_struct *waiter = (_waiter_struct *)pwaiter;

    timerOffset.gen64$q_quadword = -(timeout_microseconds * 10L);
    while (!timer_reqidt) {
        timer_reqidt = __ATOMIC_INCREMENT_QUAD(&global_timer_reqidt);
    }
    sys$clref(((_waiter_struct*)waiter)->timerEfn);
    status = sys$setimr(waiter->timerEfn, &timerOffset, 0, timer_reqidt, 0);
    if (!$VMS_STATUS_SUCCESS(status)) {
        retcode = -1;
        goto error;
    }

    while(1) {
        status = sys$wflor(waiter->timerEfn, waiter->efnMask);
        if (!$VMS_STATUS_SUCCESS(status)) {
            retcode = -2;
            goto error;
        }
        status = sys$readef(waiter->signalEfn, &state);
        if (!$VMS_STATUS_SUCCESS(status)) {
            retcode = -4;
            goto error;
        }
        if (status == SS$_WASSET) {
            // OK
            retcode = 0;
            break;
        }
        status = sys$readef(waiter->timerEfn, &state);
        if (!$VMS_STATUS_SUCCESS(status)) {
            retcode = -3;
            goto error;
        }
        if (status == SS$_WASSET) {
            // TIMEOUT
            timer_reqidt = 0;
            retcode = 1;
            break;
        }
    }

error:
    if (timer_reqidt) {
        sys$cantim(timer_reqidt, PSL$C_USER);
    }
    return retcode;
}

