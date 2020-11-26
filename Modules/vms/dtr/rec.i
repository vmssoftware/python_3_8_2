%module rec

%{
#include "rec.h"
%}

%include "typemaps.i"
%include <cstring.i>
%cstring_output_allocate(char **OUTPUT, free(*$1));

%typemap(out) char *
{
   if (result) {
      resultobj = PyUnicode_DecodeUTF8(result, strlen(result), "surrogateescape");
      free(result);
   } else {
      resultobj = Py_None;
      Py_INCREF(resultobj);
   }
}

%rename(new) _new;
%rename(delete) _delete;
%rename(getstr) _getstr;
%rename(getint) _getint;
%rename(addstr) _addstr;
%rename(addint) _addint;
%rename(load) _load;
%rename(addfloat) _addfloat;
%rename(getfloat) _getfloat;
%rename(addnum) _addnum;
%rename(gethex) _gethex;
%rename(getnum) _getnum;
%rename(setstr) _setstr;
%rename(setint) _setint;
%rename(setnum) _setnum;
%rename(setfloat) _setfloat;

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


