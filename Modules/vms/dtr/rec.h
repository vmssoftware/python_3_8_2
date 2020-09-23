#ifndef __REC_H__
#define __REC_H__

#ifdef __cplusplus
        extern "C" {
#endif

extern void *_new();
extern void _delete(void *);
extern char *_getstr(void *, int);
extern long long _getint(void *, int);
extern void _addstr(void *, char *, unsigned short, unsigned short);
extern void _addint(void *, int, long long, unsigned short);
extern void _load(void *, void *, unsigned short);
extern void _addfloat(void *, int, double, unsigned short);
extern double _getfloat(void *, int);
extern void _addnum(void *, int, double, short, unsigned short, unsigned short, unsigned short);
extern char *_gethex(void *, int);
extern double _getnum(void *, int);
extern void _setstr(void *, char *, int);
extern void _setint(void *, long long, int);
extern void _setnum(void *, double, int);
extern void _setfloat(void *, double, int);

#ifdef __cplusplus
        }
#endif
#endif
