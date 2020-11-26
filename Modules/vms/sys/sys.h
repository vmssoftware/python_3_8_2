#ifndef __SYS_H__
#define __SYS_H__

#ifdef __cplusplus
        extern "C" {
#endif

extern unsigned int _add_holder(unsigned int, unsigned long long, unsigned int);
extern unsigned int _add_ident(char *, unsigned int, unsigned int, unsigned int *);
extern unsigned int _asctim(long long, char **, char);
extern unsigned int _asctoid(char *, unsigned int *, unsigned int *);
extern unsigned int _assign(char *, unsigned short int *OUTPUT, unsigned int, char *, unsigned int);
extern unsigned int _bintim(char *, long long *);
extern unsigned int _cancel(unsigned short int);
extern unsigned int _crelnm(unsigned int, char *, char *, unsigned char, void *);
extern unsigned int _crembx(char, unsigned short int *, unsigned int, unsigned int, unsigned int, unsigned int, char *, unsigned int);
extern unsigned int _dassgn(unsigned short int);
extern unsigned int _dellnm(char *tabnam, char *lognam, unsigned char acmode);
extern unsigned int _delmbx(unsigned short int);
extern unsigned int _device_scan(char **, char *, void *, unsigned long long *);
extern unsigned int _find_held(unsigned int, unsigned int *, unsigned int *, unsigned int *);
extern unsigned int _finish_rdb(unsigned int *);
extern unsigned int _forcex(int, char *, unsigned int);
extern unsigned int _getdvi(char *, void *);
extern unsigned int _getjpi(int *, char *, void *);
extern unsigned int _getlki(unsigned int *, void *);
extern unsigned int _getmsg(unsigned int, char **, unsigned int, unsigned int *);
extern unsigned int _getqui(unsigned short, int *, void *);
extern unsigned int _getrmi(void *);
extern unsigned int _getsyi(int *, char *, void *);
extern unsigned int _gettim(long long *);
extern unsigned int _getuai(char *, void *);
extern unsigned int _hiber();
extern unsigned int _idtoasc(unsigned int, char **, unsigned int *, unsigned int *, unsigned int *);
extern unsigned int _readvblk(unsigned short int chan, void *rbuffer, long long *rlen, unsigned short *iostatus, long long p3, unsigned int func_mod);
extern unsigned int _rem_holder(unsigned int, unsigned long long);
extern unsigned int _rem_ident(unsigned int);
extern unsigned int _schdwk(unsigned int *INOUT, char *, long long daytim, long long reptim);
extern unsigned int _setuai(char *, void *);
extern unsigned int _show_intrusion(char *, char **, unsigned long long *, unsigned int *, unsigned int *, unsigned int, unsigned int *);
extern unsigned int _trnlnm(unsigned int, char *, char *, unsigned char, void *);
extern unsigned int _uicstr(long int, char **, int);
extern unsigned int _writevblk(unsigned short int chan, void *wbuffer, long long *wlen, unsigned short *iostatus, long long p3, unsigned int func_mod);

#ifdef __cplusplus
        }
#endif
#endif
