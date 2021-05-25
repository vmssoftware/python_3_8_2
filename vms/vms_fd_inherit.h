#ifndef VMS_FCNTL_H
#define VMS_FCNTL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    _FCNTL_QUERY_SET_INHERITABLE=0,
    _FCNTL_QUERY_GET_INHERITABLE=1,
    _FCNTL_QUERY_SET_NON_INHERITABLE=2,
} _fcntl_query_type;


typedef struct {
    int fd;
    _fcntl_query_type cmd;
    int res;
} _fcntl_query;

int safe_fd_inherit(_fcntl_query *q);

#ifdef __cplusplus
}
#endif

#endif
