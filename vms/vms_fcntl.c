
#define _POSIX_EXIT 1

#define __NEW_STARLET 1

#include <assert.h>
#include <builtins.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stsdef.h>
#include <time.h>
#include <unistd.h>
#include <unixio.h>
#include <unixlib.h>

#include "vms_fcntl.h"
#include "vms_sleep.h"

#ifdef _DEBUG
#define _TIMEOUT_MICROSEC 1000000*3
#else
#define _TIMEOUT_MICROSEC 100000
#endif

typedef struct {
    int fd;
    int fd2;
    int cmd;
    int flg;
    int res;
    int err;
    void* waiter;   // also means we are in the thread
} fcntl_buf;

static void* proceed_buf_thread(void *param) {
    fcntl_buf buf = *(fcntl_buf*)param;
    int state;
    int fds[2];
    if (buf.waiter) {
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &state);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &state);
    }
    switch(buf.cmd) {
        case F_PIPE_NOINHERIT_VMS:
            buf.res = pipe(fds);
            if (buf.res == -1) {
                buf.err = errno;
                break;
            }
            if ((buf.res = fcntl(fds[0], F_SETFD, FD_CLOEXEC)) == -1) {
                buf.err = errno;
                close(fds[0]);
                close(fds[1]);
                break;
            }
            if ((buf.res = fcntl(fds[1], F_SETFD, FD_CLOEXEC)) == -1) {
                buf.err = errno;
                close(fds[0]);
                close(fds[1]);
                break;
            }
            buf.fd = fds[0];
            buf.fd2 = fds[1];
            buf.err = 0;
            buf.res = 0;
            break;
        case F_DUPFD_CLOEXEC_VMS:
            if (buf.fd < 0) {
                buf.res = -1;
                buf.err = EINVAL;
                break;
            }
            buf.err = 0;
            buf.res = dup(buf.fd);
            if (buf.res < 0) {
                buf.err = errno;
                break;
            }
            if (fcntl(buf.res, F_SETFD, FD_CLOEXEC) == -1) {
                buf.err = errno;
                close(buf.res);
                buf.res = -1;
            }
            break;
        case F_GETFD:
            if (buf.fd < 0) {
                buf.res = -1;
                buf.err = EINVAL;
                break;
            }
            buf.res = fcntl(buf.fd, F_GETFD, 0);
            buf.err = errno;
            break;
        case F_SETFD:
            if (buf.fd < 0) {
                buf.res = -1;
                buf.err = EINVAL;
                break;
            }
            if (buf.flg & FD_CLOEXEC) {
                buf.res = fcntl(buf.fd, F_SETFD, buf.flg);
                buf.err = errno;
            } else {
                // fcntl() definitely does not set inheritable, so we should use dup()
                buf.res = 0;
                buf.err = 0;
                buf.fd2 = dup(buf.fd);
                if (buf.fd2 < 0) {
                    buf.res = -1;
                    buf.err = errno;
                } else {
                    buf.fd = dup2(buf.fd2, buf.fd);
                    buf.err = errno;
                    close(buf.fd2);
                    if (buf.fd < 0) {
                        buf.res = -1;
                    }
                }
            }
            break;
        default:
            buf.res = -1;
            buf.err = EINVAL;
    }
// #ifdef _DEBUG
//     if (buf.res < 0) {
//         char str[256];
//         sprintf(str, "\nfd %i, cmd %i, flg %i, err %i ", buf.fd, buf.cmd, buf.flg, buf.err);
//         write(2, str, strlen(str));
//     }
// #endif
    // test thread cancellation before setting param, it might be destroyed
    if (buf.waiter) {
        pthread_testcancel();
    }
    *(fcntl_buf*)param = buf;
    // signal that all ok
    if (buf.waiter) {
        vms_waiter_set(buf.waiter);
    }
    return param;
}

/* buffer must be initialized
*/
static int proceed_buf(fcntl_buf *pbuf) {
    static pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
    // enter critical section
    pthread_mutex_lock(&_mutex);

#if 1
    // prepare waiter
    pbuf->waiter = vms_waiter_init();
    vms_waiter_clr(pbuf->waiter);
    // start thread
    pthread_t _thread = 0;
    int retcode;
    if (pthread_create(&_thread, NULL, proceed_buf_thread, (void*)pbuf)) {
        pbuf->err = EVMSERR;
        pbuf->res = -1;
    } else {
        // wait a result
        switch(vms_waiter_wait(pbuf->waiter, _TIMEOUT_MICROSEC)) {
            case 0:
                // OK
                pthread_join(_thread, (void**)&retcode);
                break;
            case 1:
                // BUSY
                pthread_cancel(_thread);
                pthread_detach(_thread);
                pbuf->err = EBUSY;
                pbuf->res = -1;
                break;
            default:
                // ERROR
                pthread_cancel(_thread);
                pthread_detach(_thread);
                pbuf->err = EVMSERR;
                pbuf->res = -1;
                break;
        }
    }
    vms_waiter_free(pbuf->waiter);
#else
    pbuf->waiter = 0;
    proceed_buf_thread((void*)pbuf);
#endif
    // leave critical section
    pthread_mutex_unlock(&_mutex);
    errno = pbuf->err;
    return pbuf->res;
}

int vms_pipe_noinherit(int fds[2]) {
    fcntl_buf buf;
    buf.cmd = F_PIPE_NOINHERIT_VMS;
    if (proceed_buf(&buf) != -1) {
        fds[0] = buf.fd;
        fds[1] = buf.fd2;
        return 0;
    }
    return -1;
}

int vms_fcntl(int fd, int cmd, int flg) {
    if (fd < 0) {
        errno = EINVAL;
        return -1;
    }
    fcntl_buf buf;
    buf.fd = fd;
    buf.cmd = cmd;
    buf.flg = flg;
    return proceed_buf(&buf);
}
