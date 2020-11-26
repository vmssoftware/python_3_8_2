#ifndef __RTL_H__
#define __RTL_H__

# ifdef __cplusplus
        extern "C" {
#endif

extern void *_new();
extern void _delete(void *);
extern void _addint(void *, int, int, long long);
extern long long _getint(void *, int);
extern char* _gethex(void *, int);
extern void _addstr(void *, int, char *, unsigned short);
extern void _addstrd(void *, int, char *, unsigned char);
extern void _addstrn(void *, int, char *, unsigned short);
extern char *_getstr(void *, int, int);
extern int _getbyte(void *, int, int);
extern void _addbin(void *, int, long long, unsigned short, unsigned short);
extern int _size(void *);

# ifdef __cplusplus
        }
#endif
#endif
