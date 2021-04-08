#ifndef VMS_SELECT_H
#define VMS_SELECT_H

#ifdef __cplusplus
extern "C" {
#endif

int vms_channel_lookup(int fd, unsigned short *channel);
int vms_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
unsigned long read_mbx(int fd, char *buf, int size, int *pid_ptr);

#ifdef __cplusplus
}
#endif

#endif
