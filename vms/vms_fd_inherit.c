
#define _POSIX_EXIT 1

#define __NEW_STARLET 1

#include <builtins.h>
#include <descrip.h>
#include <efndef.h>
#include <errno.h>
#include <fcntl.h>
#include <gen64def.h>
#include <iodef.h>
#include <iosbdef.h>
#include <lib$routines.h>
#include <psldef.h>
#include <pthread.h>
#include <starlet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stsdef.h>
#include <unistd.h>
#include <unixio.h>
#include <unixlib.h>

#include "vms_fd_inherit.h"
#include "vms_mbx_util.h"
#include "vms_select.h"

#ifndef _USE_THREAD_INHERIT_

int safe_fd_inherit(_inherit_query *pQuery) {
    int retCode = 0;
    int fd_dup;
    if (!pQuery) {
        return -1;
    }
    if (pQuery->fd >= 0) {
        switch(pQuery->cmd) {
            case _INHERIT_QUERY_SET_INHERITABLE:
        #ifndef _USE_DUP_INHERIT_
                pQuery->res = fcntl(pQuery->fd, F_SETFD, 0);
        #else
                pQuery->res = 0;
                fd_dup = dup(pQuery->fd);
                if (fd_dup < 0) {
                    pQuery->res = -1;
                } else {
                    pQuery->fd = dup2(fd_dup, pQuery->fd);
                    if (pQuery->fd < 0) {
                        pQuery->res = -1;
                    }
                    close(fd_dup);
                    fd_dup = -1;
                }
        #endif
                break;
            case _INHERIT_QUERY_SET_NON_INHERITABLE:
                pQuery->res = fcntl(pQuery->fd, F_SETFD, FD_CLOEXEC);
                break;
            case _INHERIT_QUERY_GET_INHERITABLE:
                pQuery->res = fcntl(pQuery->fd, F_GETFD, 0);
                break;
            default:
                pQuery->res = -1;
                errno = EINVAL;
                break;
        }
    }
    return 0;
}

#else

static unsigned short _inherit_mbx_channel_in = 0;
static unsigned short _inherit_mbx_channel_out = 0;

static void* _inherit_thread(void* unused) {
    _inherit_query    q;
    int             fd_dup;
    while(_inherit_mbx_channel_in && _inherit_mbx_channel_out) {
        if (simple_read_mbx_w(_inherit_mbx_channel_in, (unsigned char*)&q, sizeof(q)) == sizeof(q)) {
            if (q.fd >= 0) {
                switch(q.cmd) {
                    case _INHERIT_QUERY_SET_INHERITABLE:
                #ifndef _USE_DUP_INHERIT_
                        q.res = fcntl(q.fd, F_SETFD, 0);
                #else
                        q.res = 0;
                        fd_dup = dup(q.fd);
                        if (fd_dup < 0) {
                            q.res = -1;
                        } else {
                            q.fd = dup2(fd_dup, q.fd);
                            if (q.fd < 0) {
                                q.res = -1;
                            }
                            close(fd_dup);
                            fd_dup = -1;
                        }
                #endif
                        break;
                    case _INHERIT_QUERY_SET_NON_INHERITABLE:
                        q.res = fcntl(q.fd, F_SETFD, FD_CLOEXEC);
                        break;
                    case _INHERIT_QUERY_GET_INHERITABLE:
                        q.res = fcntl(q.fd, F_GETFD, 0);
                        break;
                    default:
                        q.res = -1;
                        break;
                }
            }
            simple_write_mbx(_inherit_mbx_channel_out, (unsigned char*)&q, sizeof(q));
        }
    }
    return 0;
}

int safe_fd_inherit(_inherit_query *pQuery) {
    static pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
    static pthread_t _thread;
    static int _initialized = 0;

    int retCode = 0;

    if (!pQuery) {
        return -1;
    }

    pthread_mutex_lock(&_mutex);
    if (!_initialized) {
        _inherit_mbx_channel_in = simple_create_mbx(NULL, sizeof(_inherit_query));
        if (!_inherit_mbx_channel_in) {
            retCode = -2;
            goto egress;
        }
        _inherit_mbx_channel_out = simple_create_mbx(NULL, sizeof(_inherit_query));
        if (!_inherit_mbx_channel_out) {
            retCode = -3;
            goto egress;
        }
        if (pthread_create(&_thread, NULL, _inherit_thread, 0)) {
            retCode = -4;
            goto egress;
        }
        _inherit_query t;
        t.fd = -1;
        simple_write_mbx(_inherit_mbx_channel_in, (unsigned char*)&t, sizeof(t));
        simple_read_mbx(_inherit_mbx_channel_out, (unsigned char*)&t, sizeof(t));
        _initialized = 1;
    }

    simple_write_mbx(_inherit_mbx_channel_in, (unsigned char*)pQuery, sizeof(*pQuery));
    if (simple_read_mbx_timeout(_inherit_mbx_channel_out, (unsigned char*)pQuery, sizeof(*pQuery), 1000) != sizeof(*pQuery)) {
        retCode = -5;
        pthread_cancel(_thread);
        vms_channel_free(_inherit_mbx_channel_in);
        _inherit_mbx_channel_in = 0;
        vms_channel_free(_inherit_mbx_channel_out);
        _inherit_mbx_channel_out = 0;
        _initialized = 0;
    }

egress:
    pthread_mutex_unlock(&_mutex);
    return retCode;
}

#endif
