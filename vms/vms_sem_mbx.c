/**************************************************************
*
*
*
**************************************************************/

#include "vms_sem_mbx.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unixio.h>

#include <descrip.h>
#include <efndef.h>
#include <iodef.h>
#include <iosbdef.h>
#include <lib$routines.h>
#include <psldef.h>
#include <ssdef.h>
#include <starlet.h>

#undef _USE_EFN_

/* Open a named semaphore */
sem_t_mbx *sem_open_mbx (const char *name, int oflag, ...) {
    int status = -1;
    unsigned short chan = 0;
    struct dsc$descriptor_s dsc_name;
    sem_t_mbx *sem = SEM_FAILED_MBX;
    va_list vargs;
    unsigned long mode;
    unsigned int value;
    struct _iosb iosb;
    char buffer[8] = {0};
    int fd = -1;
    int efn = EFN$C_ENF;

    if (oflag != 0) {
        va_start(vargs, oflag);
        mode = va_arg(vargs, unsigned long);
        value = va_arg(vargs, unsigned int);
        va_end(vargs);

        fd = open(name, oflag, mode, "fop=dlt");
        if (-1 == fd) {
            return sem;
        }
    }

    dsc_name.dsc$a_pointer = (char*)name;
    dsc_name.dsc$w_length = strlen(name);
    dsc_name.dsc$b_dtype = DSC$K_DTYPE_T;
    dsc_name.dsc$b_class = DSC$K_CLASS_S;

    status = SYS$CREMBX(
        0,              // temporary
        &chan,          // channel
        sizeof(buffer), // maxmsg
        0,              // bufquo
        0,              // promsk
        PSL$C_USER,     // acmode
        &dsc_name,      // lognam
        0,              // flags
        0);             // nullarg

    if (SS$_NORMAL == status) {
        if (oflag == 0) {
            // just open
            sem = malloc(sizeof(sem_t_mbx));
            sem->channel = chan;
            sem->fd = fd;
        } else {
            if (value && value <= SEM_VALUE_MAX_MBX) {
                // write value
                *((unsigned long*)buffer) = value;
            #ifdef _USE_EFN_
                status = LIB$GET_EF(&efn);      /* if it fails just use 0 */
            #endif
                status = SYS$QIOW(
                    efn,                        /* efn - event flag */
                    chan,                       /* chan - channel number */
                    IO$_WRITEVBLK | IO$M_NOW,   /* func - function modifier */
                    &iosb,                      /* iosb - I/O status block */
                    0,                          /* astadr - AST routine */
                    0,                          /* astprm - AST parameter */
                    buffer,                     /* p1 - output buffer */
                    sizeof(buffer),             /* p2 - size of buffer */
                    0,                          /* starting vblock */
                    0,0,0);                     /* p4-p6*/
                if (efn != EFN$C_ENF) {
                    LIB$FREE_EF(&efn);
                    efn = EFN$C_ENF;
                }
                if (SS$_NORMAL == status) {
                    sem = malloc(sizeof(sem_t_mbx));
                    sem->channel = chan;
                    sem->fd = fd;
                }
            } else {
                // create already locked semaphore, do not write value
                sem = malloc(sizeof(sem_t_mbx));
                sem->channel = chan;
                sem->fd = fd;
            }
        }
    }
    if (sem == SEM_FAILED_MBX) {
        if (chan != 0) {
            SYS$DASSGN(chan);
        }
        if (fd != -1) {
            close(fd);
        }
        errno = EVMSERR;
    }
    return sem;
}

/* close the semaphore opened through sem_open */
int sem_close_mbx (sem_t_mbx *sem) {
    SYS$DASSGN(sem->channel);
    if (sem->fd != -1) {
        char name[256] = {0};
        char *filename = getname(sem->fd, name, 0);
        close(sem->fd);
        if (filename) {
            remove(filename);
        }
    }
    free(sem);
    return 0;
}

/* Lock the semaphore (blocking) */
int sem_wait_mbx (sem_t_mbx *sem) {
    int status = -1;
    struct _iosb iosb;
    char buffer[8] = {0};
    unsigned long *pvalue = (unsigned long *)buffer;
    int efn = EFN$C_ENF;

    // read current value with waiting
#ifdef _USE_EFN_
    status = LIB$GET_EF(&efn);      /* if it fails just use 0 */
#endif
    status = SYS$QIOW(
        efn,                        /* efn - event flag */
        sem->channel,               /* chan - channel number */
        IO$_READVBLK,               /* func - function modifier */
        &iosb,                      /* iosb - I/O status block */
        0,                          /* astadr - AST routine */
        0,                          /* astprm - AST parameter */
        buffer,                     /* p1 - input buffer */
        sizeof(buffer),             /* p2 - size of buffer */
        0,                          /* starting vblock */
        0,0,0);                     /* p4-p6*/
    if (efn != EFN$C_ENF) {
        LIB$FREE_EF(&efn);
        efn = EFN$C_ENF;
    }

    if (SS$_NORMAL != status) {
        // something goes wrong
        errno = EVMSERR;
        return -1;
    }

    if (!*pvalue) {
        // something goes wrong, no zero allowed
        errno = EVMSERR;
        return -1;
    }

    // decrement value
    --*pvalue;

    if (*pvalue && *pvalue <= SEM_VALUE_MAX_MBX) {
        // write it back if it is positive
    #ifdef _USE_EFN_
        status = LIB$GET_EF(&efn);      /* if it fails just use 0 */
    #endif
        status = SYS$QIOW(
            efn,                        /* efn - event flag */
            sem->channel,               /* chan - channel number */
            IO$_WRITEVBLK | IO$M_NOW,   /* func - function modifier */
            &iosb,                      /* iosb - I/O status block */
            0,                          /* astadr - AST routine */
            0,                          /* astprm - AST parameter */
            buffer,                     /* p1 - output buffer */
            sizeof(buffer),             /* p2 - size of buffer */
            0,                          /* starting vblock */
            0,0,0);                     /* p4-p6*/
        if (efn != EFN$C_ENF) {
            LIB$FREE_EF(&efn);
            efn = EFN$C_ENF;
        }

        if (SS$_NORMAL != status) {
            // something goes wrong
            errno = EVMSERR;
            return -1;
        }
    }
    // ok
    return 0;
}

/* Lock the specified semaphore (non-blocking) */
int sem_trywait_mbx (sem_t_mbx *sem) {
    int status = -1;
    struct _iosb iosb;
    char buffer[8] = {0};
    unsigned long *pvalue = (unsigned long *)buffer;
    int efn = EFN$C_ENF;

    // try to read current value without waiting
#ifdef _USE_EFN_
    status = LIB$GET_EF(&efn);      /* if it fails just use 0 */
#endif
    status = SYS$QIOW(
        efn,                        /* efn - event flag */
        sem->channel,               /* chan - channel number */
        IO$_READVBLK | IO$M_NOW,    /* func - function modifier */
        &iosb,                      /* iosb - I/O status block */
        0,                          /* astadr - AST routine */
        0,                          /* astprm - AST parameter */
        buffer,                     /* p1 - input buffer */
        sizeof(buffer),             /* p2 - size of buffer */
        0,                          /* starting vblock */
        0,0,0);                     /* p4-p6*/
    if (efn != EFN$C_ENF) {
        LIB$FREE_EF(&efn);
        efn = EFN$C_ENF;
    }

    if (SS$_NORMAL != status) {
        // something goes wrong
        errno = EVMSERR;
        return -1;
    }

    if (SS$_NORMAL == iosb.iosb$w_status && 
        sizeof(buffer) == iosb.iosb$w_bcnt)
    {
        if (!*pvalue) {
            // something goes wrong, no zero allowed
            errno = EVMSERR;
            return -1;
        }
        // we have got the value, so decrement it
        --*pvalue;
        if (*pvalue && *pvalue <= SEM_VALUE_MAX_MBX) {
            // write it back if it is positive
        #ifdef _USE_EFN_
            status = LIB$GET_EF(&efn);      /* if it fails just use 0 */
        #endif
            status = SYS$QIOW(
                efn,                        /* efn - event flag */
                sem->channel,               /* chan - channel number */
                IO$_WRITEVBLK | IO$M_NOW,   /* func - function modifier */
                &iosb,                      /* iosb - I/O status block */
                0,                          /* astadr - AST routine */
                0,                          /* astprm - AST parameter */
                buffer,                     /* p1 - output buffer */
                sizeof(buffer),             /* p2 - size of buffer */
                0,                          /* starting vblock */
                0,0,0);                     /* p4-p6*/
            if (efn != EFN$C_ENF) {
                LIB$FREE_EF(&efn);
                efn = EFN$C_ENF;
            }

            if (SS$_NORMAL != status) {
                // something goes wrong
                errno = EVMSERR;
                return -1;
            }
        }
    } else {
        // we have no value, wait failed
        errno = EAGAIN;
        return -1;
    }

    return 0;
}

/* Unlock the specified semaphore */
int sem_post_mbx (sem_t_mbx *sem) {
    int status = -1;
    struct _iosb iosb;
    char buffer[8] = {0};
    unsigned long *pvalue = (unsigned long *)buffer;
    int efn = EFN$C_ENF;

    // try to read current value without waiting
#ifdef _USE_EFN_
    status = LIB$GET_EF(&efn);      /* if it fails just use 0 */
#endif
    status = SYS$QIOW(
        efn,                        /* efn - event flag */
        sem->channel,               /* chan - channel number */
        IO$_READVBLK | IO$M_NOW,    /* func - function modifier */
        &iosb,                      /* iosb - I/O status block */
        0,                          /* astadr - AST routine */
        0,                          /* astprm - AST parameter */
        buffer,                     /* p1 - input buffer */
        sizeof(buffer),             /* p2 - size of buffer */
        0,                          /* starting vblock */
        0,0,0);                     /* p4-p6*/
    if (efn != EFN$C_ENF) {
        LIB$FREE_EF(&efn);
        efn = EFN$C_ENF;
    }

    if (SS$_NORMAL != status) {
        // something goes wrong
        errno = EVMSERR;
        return -1;
    }

    if (SS$_NORMAL == iosb.iosb$w_status && 
        sizeof(buffer) == iosb.iosb$w_bcnt)
    {
        // we have got the value, so increment it
        if (*pvalue < SEM_VALUE_MAX_MBX) {
            ++*pvalue;
        }
    } else {
        // we have no value, so initialize it
        *pvalue = 1;
    }

    // write value
#ifdef _USE_EFN_
    status = LIB$GET_EF(&efn);      /* if it fails just use 0 */
#endif
    status = SYS$QIOW(
        efn,                        /* efn - event flag */
        sem->channel,               /* chan - channel number */
        IO$_WRITEVBLK | IO$M_NOW,   /* func - function modifier */
        &iosb,                      /* iosb - I/O status block */
        0,                          /* astadr - AST routine */
        0,                          /* astprm - AST parameter */
        buffer,                     /* p1 - output buffer */
        sizeof(buffer),             /* p2 - size of buffer */
        0,                          /* starting vblock */
        0,0,0);                     /* p4-p6*/
    if (efn != EFN$C_ENF) {
        LIB$FREE_EF(&efn);
        efn = EFN$C_ENF;
    }

    if (SS$_NORMAL != status) {
        // something goes wrong
        errno = EVMSERR;
        return -1;
    }

    // ok
    return 0;
}

/* Get the semaphore value */
int sem_getvalue_mbx (sem_t_mbx *sem, int *sval) {
    int status = -1;
    struct _iosb iosb;
    char buffer[8] = {0};
    unsigned long *pvalue = (unsigned long *)buffer;
    int efn = EFN$C_ENF;

    if (!sval) {
        errno = EVMSERR;
        return -1;
    }

    // try to read current value without waiting
#ifdef _USE_EFN_
    status = LIB$GET_EF(&efn);      /* if it fails just use 0 */
#endif
    status = SYS$QIOW(
        efn,                        /* efn - event flag */
        sem->channel,               /* chan - channel number */
        IO$_READVBLK | IO$M_NOW,    /* func - function modifier */
        &iosb,                      /* iosb - I/O status block */
        0,                          /* astadr - AST routine */
        0,                          /* astprm - AST parameter */
        buffer,                     /* p1 - input buffer */
        sizeof(buffer),             /* p2 - size of buffer */
        0,                          /* starting vblock */
        0,0,0);                     /* p4-p6*/

    if (efn != EFN$C_ENF) {
        LIB$FREE_EF(&efn);
        efn = EFN$C_ENF;
    }

    if (SS$_NORMAL != status) {
        // something goes wrong
        errno = EVMSERR;
        return -1;
    }

    if (SS$_NORMAL == iosb.iosb$w_status && 
        sizeof(buffer) == iosb.iosb$w_bcnt)
    {
        *sval = *pvalue;
        // we have got the value, write it back unchanged
    #ifdef _USE_EFN_
        status = LIB$GET_EF(&efn);      /* if it fails just use 0 */
    #endif
        status = SYS$QIOW(
            efn,                        /* efn - event flag */
            sem->channel,               /* chan - channel number */
            IO$_WRITEVBLK | IO$M_NOW,   /* func - function modifier */
            &iosb,                      /* iosb - I/O status block */
            0,                          /* astadr - AST routine */
            0,                          /* astprm - AST parameter */
            buffer,                     /* p1 - output buffer */
            sizeof(buffer),             /* p2 - size of buffer */
            0,                          /* starting vblock */
            0,0,0);                     /* p4-p6*/

        if (efn != EFN$C_ENF) {
            LIB$FREE_EF(&efn);
            efn = EFN$C_ENF;
        }

        if (SS$_NORMAL != status) {
            // something goes wrong
            errno = EVMSERR;
            return -1;
        }
    } else {
        // we have no value
        *sval = 0;
    }

    return 0;
}
