#ifndef VMS_MBX_UTIL_H
#define VMS_MBX_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned short simple_create_mbx(const char *name, int mbx_size);
int simple_write_mbx(unsigned short channel, unsigned char *buf, int size);
int simple_read_mbx(unsigned short channel, unsigned char *buf, int size);
int simple_read_mbx_w(unsigned short channel, unsigned char *buf, int size);
int simple_read_mbx_timeout(unsigned short channel, unsigned char *buf, int size, int timeout_microseconds);
int simple_check_mbx(unsigned short channel);
int read_mbx(int fd, char *buf, int size, int pid);
unsigned short get_mbx_size(unsigned short channel);

#ifdef __cplusplus
}
#endif

#endif
