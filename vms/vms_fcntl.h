#ifndef VMS_INHERIT_H
#define VMS_INHERIT_H

#ifdef __cplusplus
extern "C" {
#endif

#define F_DUPFD_CLOEXEC_VMS (1024+6)
#define F_PIPE_NOINHERIT_VMS (1024+7)

int vms_fcntl(int fd, int cmd, int flags);
int vms_pipe_noinherit(int fds[2]);

#ifdef __cplusplus
}
#endif

#endif
