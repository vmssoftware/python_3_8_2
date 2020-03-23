%module ile3

%{
#include "ile3.h"
%}

%include "typemaps.i"

%rename(new) _new;
%rename(delete) _delete;
%rename(addint) _addint;
%rename(getint) _getint;
%rename(gethex) _gethex;
%rename(addstr) _addstr;
%rename(addstrd) _addstrd;
%rename(addstrn) _addstrn;
%rename(getstr) _getstr;
%rename(getbyte) _getbyte;
%rename(addbin) _addbin;

extern void *_new();
extern void _delete(void *);
extern void _addint(void *, int, int, long long);
extern long long _getint(void *, int);
extern char *_gethex(void *, int);
extern void _addstr(void *, int, char *, unsigned short);
extern void _addstrd(void *, int, char *, unsigned char);
extern void _addstrn(void *, int, char *, unsigned short);
extern char *_getstr(void *, int, int);
extern int _getbyte(void *, int, int);
extern void _addbin(void *, int, long long, unsigned short, unsigned short);
