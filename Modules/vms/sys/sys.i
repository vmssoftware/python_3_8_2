%module sys

%{
#include "sys.h"
%}

%include "typemaps.i"
%include <cstring.i>
%cstring_output_allocate(char **OUTPUT, free(*$1));

%rename(asctim) _asctim;
%rename(asctoid) _asctoid;
%rename(assign) _assign;
%rename(bintim) _bintim;
%rename(cancel) _cancel;
%rename(crembx) _crembx;
%rename(dassgn) _dassgn;
%rename(delmbx) _delmbx;
%rename(find_held) _find_held;
%rename(finish_rdb) _finish_rdb;
%rename(getmsg) _getmsg;
%rename(hiber) _hiber;
%rename(idtoasc) _idtoasc;
%rename(getrmi) _getrmi;
%rename(getuai) _getuai;
%rename(setuai) _setuai;
%rename(getdvi) _getdvi;
%rename(device_scan) _device_scan;
%rename(trnlnm) _trnlnm;
%rename(getjpi) _getjpi;
%rename(getqui) _getqui;
%rename(getsyi) _getsyi;
%rename(forcex) _forcex;
%rename(rem_ident) _rem_ident;
%rename(add_ident) _add_ident;
%rename(rem_holder) _rem_holder;
%rename(add_holder) _add_holder;
%rename(getlki) _getlki;
%rename(uicstr) _uicstr;
%rename(gettim) _gettim;
%rename(crelnm) _crelnm;
%rename(show_intrusion) _show_intrusion;

extern unsigned int _asctim(long long, char **OUTPUT, char);
extern unsigned int _asctoid(char *, unsigned int *OUTPUT, unsigned int *OUTPUT);
extern unsigned int _assign(char *, unsigned short int *OUTPUT, unsigned int, char *, unsigned int);
extern unsigned int _bintim(char *, long long *OUTPUT);
extern unsigned int _cancel(unsigned short int);
extern unsigned int _crembx(char, unsigned short int *OUTPUT, unsigned int, unsigned int, unsigned int, unsigned int, char *, unsigned int);
extern unsigned int _dassgn(unsigned short int);
extern unsigned int _delmbx(unsigned short int);
extern unsigned int _find_held(unsigned int, unsigned int *OUTPUT, unsigned int *OUTPUT, unsigned int *INOUT);
extern unsigned int _finish_rdb(unsigned int *INPUT);
extern unsigned int _getmsg(unsigned int, char **OUTPUT, unsigned int, unsigned int *OUTPUT);
extern unsigned int _hiber();
extern unsigned int _idtoasc(unsigned int, char **OUTPUT, unsigned int *OUTPUT, unsigned int *OUTPUT, unsigned int *INOUT);
extern unsigned int _getrmi(void *);
extern unsigned int _getuai(char *, void *);
extern unsigned int _setuai(char *, void *);
extern unsigned int _getdvi(char *, void *);
extern unsigned int _device_scan(char **OUTPUT, char *, void *, unsigned long long *INOUT);
extern unsigned int _trnlnm(unsigned int, char *, char *, unsigned char, void *);
extern unsigned int _getjpi(int *INOUT, char *, void *);
extern unsigned int _getqui(unsigned short, int *INOUT, void *);
extern unsigned int _getsyi(int *INOUT, char *, void *);
extern unsigned int _forcex(int, char *, unsigned int);
extern unsigned int _rem_ident(unsigned int);
extern unsigned int _add_ident(char *, unsigned int, unsigned int, unsigned int *OUTPUT);
extern unsigned int _rem_holder(unsigned int, unsigned long long);
extern unsigned int _add_holder(unsigned int, unsigned long long, unsigned int);
extern unsigned int _getlki(int *INOUT, void *);
extern unsigned int _uicstr(long int, char **OUTPUT, int);
extern unsigned int _gettim(long long *OUTPUT);
extern unsigned int _crelnm(unsigned int, char *, char *, unsigned char, void *);
extern unsigned int _show_intrusion(char *, char **OUTPUT, unsigned long long *OUTPUT, unsigned int *OUTPUT, unsigned int *OUTPUT, unsigned int, unsigned int *INOUT);
