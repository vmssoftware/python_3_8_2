#include "vms/vms_spawn_helper.h"
#include <string.h>
#include <builtins.h>

#define MAX_SPAWN 256

unsigned int    _pid[MAX_SPAWN];
int             _status[MAX_SPAWN];
unsigned int    _finished[MAX_SPAWN];
int             _fd_stdout[MAX_SPAWN];

static unsigned long _finished_counter = 1;
static unsigned long _initialized = 0;   // 0 - uninitialized, 1 - initialization, 2 - initializes

static int _init_pos(int pos, unsigned int **pppid, int **ppstatus, unsigned int **ppfinished, int **ppfd) {
    _pid[pos] = -1;
    _status[pos] = -1;
    _finished[pos] = 0;
    _fd_stdout[pos] = -1;
    *pppid = _pid + pos;
    *ppstatus = _status + pos;
    *ppfinished = _finished + pos;
    *ppfd = _fd_stdout + pos;
    return pos;
}

int vms_spawn_alloc(unsigned int **pppid, int **ppstatus, unsigned int **ppfinished, int **ppfd) {
    // init table
    if (__CMP_SWAP_LONG(&_initialized, 0, 1)) {
        memset(_pid, 0, sizeof(_pid));
        memset(_status, -1, sizeof(_status));
        memset(_finished, 0, sizeof(_finished));
        memset(_fd_stdout, -1, sizeof(_fd_stdout));
        ++_initialized;
    } else {
        while(_initialized == 1) {
            ;
        }
    }
    // find empty pid
    while(1) {
        int pos = -1;
        unsigned int oldest = 0xffffffff;
        unsigned int oldest_pid = 0;
        for(int i = 0; i < MAX_SPAWN; ++i) {
            if (__CMP_SWAP_LONG(_pid + i, 0, -1)) {
                // found an empty position
                return _init_pos(i, pppid, ppstatus, ppfinished, ppfd);
            }
            if (_finished[i] == 0) {
                // pid is set, process is not finished
                continue;
            }
            // pid is set, process is finished, so find the oldest
            if (_finished[i] < oldest && _pid[i] != 0xffffffff) {
                pos = i;
                oldest = _finished[pos];
                oldest_pid = _pid[pos];
            }
        }
        if (pos == -1) {
            // no free positions, no finished process, do another try
            continue;
        }
        // found the oldest finished process, which retcode is not poped, so overwrite it
        if (__CMP_SWAP_LONG(_pid + pos, oldest_pid, -1)) {
            return _init_pos(pos, pppid, ppstatus, ppfinished, ppfd);
        }
        // someone beat us here, do another try
    }
    return -1;
}

int vms_spawn_finish(unsigned int *pfinished) {
    if (pfinished >= _finished && pfinished < _finished + MAX_SPAWN) {
        int idx = (pfinished - _finished) / sizeof(*pfinished);
        *pfinished = __ATOMIC_INCREMENT_LONG(&_finished_counter);
        if (*pfinished == 0) {
            // regenerate generations, from 1 to MAX
            for(int i = 0; i < MAX_SPAWN; ++i) {
                if (_finished[i]) {
                    _finished[i] = __ATOMIC_INCREMENT_LONG(&_finished_counter);
                }
            }
            // set current as latest
            *pfinished = __ATOMIC_INCREMENT_LONG(&_finished_counter);
        }
        return idx;
    }
    return -1;
}

int vms_spawn_status(unsigned int pid, int *pstatus, unsigned int *pfinished, int *pfd, int free) {
    for(int i = 0; i < MAX_SPAWN; ++i) {
        if (_pid[i] == pid) {
            if (pstatus) {
                *pstatus = _status[i];
            }
            if (pfinished) {
                *pfinished = _finished[i];
            }
            if (pfd) {
                *pfd = _fd_stdout[i];
            }
            if (free) {
                _finished[i] = 0;
                _pid[i] = 0;
            }
            return i;
        }
    }
    return -1;
}

unsigned int vms_spawn_check_fd(int fd) {
    if (fd < 0) {
        return 0;
    }
    for(int i = 0; i < MAX_SPAWN; ++i) {
        if (_fd_stdout[i] == fd) {
            return _pid[i];
        }
    }
    return 0;
}

int vms_spawn_clear_fd(int fd) {
    if (fd < 0) {
        return -1;
    }
    int pos = -1;
    int i;
    for(i = 0; i < MAX_SPAWN; ++i) {
        if (_fd_stdout[i] == fd) {
            _fd_stdout[i] = -1;
            pos = i;
            break;
        }
    }
    for(; i < MAX_SPAWN; ++i) {
        if (_fd_stdout[i] == fd) {
            _fd_stdout[i] = -1;
        }
    }
    return pos;
}
