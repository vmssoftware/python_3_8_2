
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

static unsigned short _fcntl_mbx_channel_in = 0;
static unsigned short _fcntl_mbx_channel_out = 0;

static void* _fcntl_thread(void* unused) {
    _fcntl_query    q;
    int             fd_dup;
    while(_fcntl_mbx_channel_in && _fcntl_mbx_channel_out) {
        if (simple_read_mbx_w(_fcntl_mbx_channel_in, (unsigned char*)&q, sizeof(q)) == sizeof(q)) {
            if (q.fd >= 0) {
                switch(q.cmd) {
                    case _FCNTL_QUERY_SET_INHERITABLE:
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
                        break;
                    case _FCNTL_QUERY_SET_NON_INHERITABLE:
                        q.res = fcntl(q.fd, F_SETFL, FD_CLOEXEC);
                        break;
                    case _FCNTL_QUERY_GET_INHERITABLE:
                        q.res = fcntl(q.fd, F_GETFL);
                        break;
                    default:
                        q.res = -1;
                        break;
                }
            }
            simple_write_mbx(_fcntl_mbx_channel_out, (unsigned char*)&q, sizeof(q));
        }
    }
    return 0;
}

int safe_fd_inherit(_fcntl_query *pQuery) {
    static pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
    static pthread_t _thread;
    static int _initialized = 0;

    int retCode = 0;

    if (!pQuery) {
        return -1;
    }

    pthread_mutex_lock(&_mutex);
    if (!_initialized) {
        _fcntl_mbx_channel_in = simple_create_mbx(NULL, sizeof(_fcntl_query));
        if (!_fcntl_mbx_channel_in) {
            retCode = -2;
            goto egress;
        }
        _fcntl_mbx_channel_out = simple_create_mbx(NULL, sizeof(_fcntl_query));
        if (!_fcntl_mbx_channel_out) {
            retCode = -3;
            goto egress;
        }
        if (pthread_create(&_thread, NULL, _fcntl_thread, 0)) {
            retCode = -4;
            goto egress;
        }
        _fcntl_query t;
        t.fd = -1;
        simple_write_mbx(_fcntl_mbx_channel_in, (unsigned char*)&t, sizeof(t));
        simple_read_mbx(_fcntl_mbx_channel_out, (unsigned char*)&t, sizeof(t));
        _initialized = 1;
    }

    simple_write_mbx(_fcntl_mbx_channel_in, (unsigned char*)pQuery, sizeof(*pQuery));
    if (simple_read_mbx_timeout(_fcntl_mbx_channel_out, (unsigned char*)pQuery, sizeof(*pQuery), 1000) != sizeof(*pQuery)) {
        retCode = -5;
        pthread_cancel(_thread);
        vms_channel_free(_fcntl_mbx_channel_in);
        _fcntl_mbx_channel_in = 0;
        vms_channel_free(_fcntl_mbx_channel_out);
        _fcntl_mbx_channel_out = 0;
        _initialized = 0;
    }

egress:
    pthread_mutex_unlock(&_mutex);
    return retCode;
}