#ifndef VMS_SELECT_H
#define VMS_SELECT_H

#ifdef __cplusplus
extern "C" {
#endif

int vms_channel_lookup(int fd, unsigned short *channel);
int vms_channel_lookup_by_name(char* name, unsigned short *channel);
int vms_channel_free(unsigned short channel);
int vms_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

#ifdef __cplusplus
}
#endif

#endif
