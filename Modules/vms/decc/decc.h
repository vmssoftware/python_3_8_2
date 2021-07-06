#ifndef __DECC_H__
#define __DECC_H__

#ifdef __cplusplus
        extern "C" {
#endif

extern unsigned int _fix_time(long long);
extern unsigned int _unixtime(long long dt);
extern long long _vmstime(unsigned int);
extern char ** _from_vms(char *, int);
extern char ** _to_vms(char *, int, int);
extern char *_getenv(char *, char *);
extern long _sysconf(int);
extern int _sleep(unsigned int);
extern int _dlopen_test(char *name);
extern int _get_symbol(char *name, char** value);
extern void *_fopen(char *, char *, ...);
extern int _fclose(void *);
extern int _fileno(void *);
extern int _write(int, const char *, int);
extern char *_fgets(void *, int);
extern int _feof(void *);
extern int _ferror(void *);

#ifdef __cplusplus
        }
#endif
#endif
