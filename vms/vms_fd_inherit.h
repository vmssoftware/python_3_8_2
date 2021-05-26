#ifndef VMS_INHERIT_H
#define VMS_INHERIT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    _INHERIT_QUERY_SET_INHERITABLE=0,
    _INHERIT_QUERY_GET_INHERITABLE=1,
    _INHERIT_QUERY_SET_NON_INHERITABLE=2,
} _inherit_query_type;


typedef struct {
    int fd;
    _inherit_query_type cmd;
    int res;
} _inherit_query;

int safe_fd_inherit(_inherit_query *q);

#ifdef __cplusplus
}
#endif

#endif
