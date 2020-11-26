%module sys

%{
#include "sys.h"
%}

%include "typemaps.i"
%include <cstring.i>
%cstring_output_allocate(char **OUTPUT, free(*$1));

%begin %{
#define PY_SSIZE_T_CLEAN
%}

%exception {
    Py_BEGIN_ALLOW_THREADS
    $action
    Py_END_ALLOW_THREADS
}

%typemap(default) unsigned int func_mod {
   $1 = 0;
}

%typemap(default) long long reptim {
   $1 = 0;
}

// typemap for an outgoing buffer
%typemap(in) (void *wbuffer, long long *wlen) (long long w_len){
   if (!PyBytes_Check($input)) {
       PyErr_SetString(PyExc_ValueError, "Expecting a bytes");
       return NULL;
   }
   w_len = PyBytes_Size($input);
   $1 = (void *) PyBytes_AsString($input);
   $2 = &w_len;
}

%typemap(argout) (void *wbuffer, long long *wlen) {
    $result = SWIG_Python_AppendOutput($result, Py_BuildValue("l", *$2));
}

// typemap for an incoming buffer
%typemap(in) (void *rbuffer, long long *rlen) (long long r_len){
   if (!PyInt_Check($input)) {
       PyErr_SetString(PyExc_ValueError, "Expecting an integer");
       return NULL;
   }
   r_len = PyInt_AsLong($input);
   if (r_len < 0) {
       PyErr_SetString(PyExc_ValueError, "Positive integer expected");
       return NULL;
   }
   $1 = (void *) malloc(r_len);
   $2 = &r_len;
}

%typemap(freearg) (void *rbuffer, long long *rlen) {
    free($1);
}

%typemap(argout) (void *rbuffer, long long *rlen) {
    $result = SWIG_Python_AppendOutput($result, Py_BuildValue("y#", $1, (Py_ssize_t)*$2));
}

%typemap(in, numinputs=0) unsigned short *iostatus (unsigned short io_status){
    io_status = 0;
    $1 = &io_status;
}

%typemap(argout) (unsigned short *iostatus) {
    $result = SWIG_Python_AppendOutput($result, Py_BuildValue("H", *$1));
}

%rename(add_holder) _add_holder;
%rename(add_ident) _add_ident;
%rename(asctim) _asctim;
%rename(asctoid) _asctoid;
%rename(assign) _assign;
%rename(bintim) _bintim;
%rename(cancel) _cancel;
%rename(crelnm) _crelnm;
%rename(crembx) _crembx;
%rename(dassgn) _dassgn;
%rename(dellnm) _dellnm;
%rename(delmbx) _delmbx;
%rename(device_scan) _device_scan;
%rename(find_held) _find_held;
%rename(finish_rdb) _finish_rdb;
%rename(forcex) _forcex;
%rename(getdvi) _getdvi;
%rename(getjpi) _getjpi;
%rename(getlki) _getlki;
%rename(getmsg) _getmsg;
%rename(getqui) _getqui;
%rename(getrmi) _getrmi;
%rename(getsyi) _getsyi;
%rename(gettim) _gettim;
%rename(getuai) _getuai;
%rename(hiber) _hiber;
%rename(idtoasc) _idtoasc;
%rename(readvblk) _readvblk;
%rename(rem_holder) _rem_holder;
%rename(rem_ident) _rem_ident;
%rename(schdwk) _schdwk;
%rename(setuai) _setuai;
%rename(show_intrusion) _show_intrusion;
%rename(trnlnm) _trnlnm;
%rename(uicstr) _uicstr;
%rename(writevblk) _writevblk;

extern unsigned int _add_holder(unsigned int, unsigned long long, unsigned int);
extern unsigned int _add_ident(char *, unsigned int, unsigned int, unsigned int *OUTPUT);
extern unsigned int _asctim(long long, char **OUTPUT, char);
extern unsigned int _asctoid(char *, unsigned int *OUTPUT, unsigned int *OUTPUT);
extern unsigned int _assign(char *, unsigned short int *OUTPUT, unsigned int, char *, unsigned int);
extern unsigned int _bintim(char *, long long *OUTPUT);
extern unsigned int _cancel(unsigned short int);
extern unsigned int _crelnm(unsigned int, char *, char *, unsigned char, void *);
extern unsigned int _crembx(char, unsigned short int *OUTPUT, unsigned int, unsigned int, unsigned int, unsigned int, char *, unsigned int);
extern unsigned int _dassgn(unsigned short int);
extern unsigned int _dellnm(char *tabnam, char *lognam, unsigned char acmode);
extern unsigned int _delmbx(unsigned short int);
extern unsigned int _device_scan(char **OUTPUT, char *, void *, unsigned long long *INOUT);
extern unsigned int _find_held(unsigned int, unsigned int *OUTPUT, unsigned int *OUTPUT, unsigned int *INOUT);
extern unsigned int _finish_rdb(unsigned int *INPUT);
extern unsigned int _forcex(int, char *, unsigned int);
extern unsigned int _getdvi(char *, void *);
extern unsigned int _getjpi(int *INOUT, char *, void *);
extern unsigned int _getlki(unsigned int *INOUT, void *);
extern unsigned int _getmsg(unsigned int, char **OUTPUT, unsigned int, unsigned int *OUTPUT);
extern unsigned int _getqui(unsigned short, int *INOUT, void *);
extern unsigned int _getrmi(void *);
extern unsigned int _getsyi(int *INOUT, char *, void *);
extern unsigned int _gettim(long long *OUTPUT);
extern unsigned int _getuai(char *, void *);
extern unsigned int _hiber();
extern unsigned int _idtoasc(unsigned int, char **OUTPUT, unsigned int *OUTPUT, unsigned int *OUTPUT, unsigned int *INOUT);
extern unsigned int _readvblk(unsigned short int chan, void *rbuffer, long long *rlen, unsigned short *iostatus, long long p3, unsigned int func_mod);
extern unsigned int _rem_holder(unsigned int, unsigned long long);
extern unsigned int _rem_ident(unsigned int);
extern unsigned int _schdwk(unsigned int *INOUT, char *, long long daytim, long long reptim);
extern unsigned int _setuai(char *, void *);
extern unsigned int _show_intrusion(char *, char **OUTPUT, unsigned long long *OUTPUT, unsigned int *OUTPUT, unsigned int *OUTPUT, unsigned int, unsigned int *INOUT);
extern unsigned int _trnlnm(unsigned int, char *, char *, unsigned char, void *);
extern unsigned int _uicstr(long int, char **OUTPUT, int);
extern unsigned int _writevblk(unsigned short int chan, void *wbuffer, long long *wlen, unsigned short *iostatus, long long p3, unsigned int func_mod);

