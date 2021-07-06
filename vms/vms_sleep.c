#define __NEW_STARLET 1

#include "vms_sleep.h"

#include <gen64def.h>
#include <lib$routines.h>
#include <starlet.h>
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
