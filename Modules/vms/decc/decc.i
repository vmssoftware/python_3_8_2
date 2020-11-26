%module decc

%{
#include "decc.h"
%}

%include "typemaps.i"
%include <cstring.i>
%cstring_output_allocate(char **OUTPUT, free(*$1));

%exception {
    Py_BEGIN_ALLOW_THREADS
    $action
    Py_END_ALLOW_THREADS
}

%typemap(out) char *
{
   if ($1 == NULL) {
      $result = Py_None;
      Py_INCREF($result);
   } else {
      $result = PyUnicode_DecodeUTF8($1, strlen($1), "surrogateescape");
      free($1);
   }
}

%typemap(out) char** {
  int len,i;
  if ($1 == NULL) {
      $result = Py_None;
      Py_INCREF($result);
  } else {
      len = 0;
      while ($1[len]) len++;
      $result = PyList_New(len);
      for (i = 0; i < len; i++) {
        PyList_SetItem($result, i, PyString_FromString($1[i]));
      }
  }
}

%typemap(newfree) char ** {
  if ($1 != NULL) {
      int i;
      i = 0;
      while ($1[i]) {
          free($1[i]);
          ++i;
      }
      free($1);
  }
}

%constant int _SC_ARG_MAX = 100;
%constant int _SC_CHILD_MAX = 101;
%constant int _SC_CLK_TCK = 102;
%constant int _SC_NGROUPS_MAX = 103;
%constant int _SC_OPEN_MAX = 104;
%constant int _SC_STREAM_MAX = 105;
%constant int _SC_TZNAME_MAX = 106;
%constant int _SC_JOB_CONTROL = 107;
%constant int _SC_SAVED_IDS = 108;
%constant int _SC_VERSION = 109;
%constant int _SC_BC_BASE_MAX = 121;
%constant int _SC_BC_DIM_MAX = 122;
%constant int _SC_BC_SCALE_MAX = 123;
%constant int _SC_BC_STRING_MAX = 124;
%constant int _SC_COLL_WEIGHTS_MAX = 125;
%constant int _SC_EXPR_NEST_MAX = 126;
%constant int _SC_LINE_MAX = 127;
%constant int _SC_RE_DUP_MAX = 128;
%constant int _SC_2_VERSION = 129;
%constant int _SC_2_C_BIND = 130;
%constant int _SC_2_C_DEV = 131;
%constant int _SC_2_FORT_DEV = 132;
%constant int _SC_2_SW_DEV = 133;
%constant int _SC_2_FORT_RUN = 134;
%constant int _SC_2_LOCALEDEF = 135;
%constant int _SC_2_UPE = 136;
%constant int _SC_GETGR_R_SIZE_MAX = 150;
%constant int _SC_GETPW_R_SIZE_MAX = 151;
%constant int _SC_THREAD_DESTRUCTOR_ITERATIONS = 140;
%constant int _SC_THREAD_KEYS_MAX = 141;
%constant int _SC_THREAD_STACK_MIN = 142;
%constant int _SC_THREAD_THREADS_MAX = 143;
%constant int _SC_THREAD_SAFE_FUNCTIONS = 144;
%constant int _SC_THREADS = 145;
%constant int _SC_THREAD_ATTR_STACKSIZE = 146;
%constant int _SC_THREAD_PRIORITY_SCHEDULING = 147;
%constant int _SC_XOPEN_VERSION = 110;
%constant int _SC_PASS_MAX = 111;
%constant int _SC_XOPEN_CRYPT = 118;
%constant int _SC_XOPEN_ENH_I18N = 119;
%constant int _SC_XOPEN_SHM = 120;
%constant int _SC_PAGESIZE = 117;
%constant int _SC_PAGE_SIZE = 117; 		/* _SC_PAGESIZE */
%constant int _SC_ATEXIT_MAX = 138; 		/* not in POSIX, but in XPG4 */
%constant int _SC_IOV_MAX = 139; 		/* not in POSIX, but in XPG4 */
%constant int _SC_XOPEN_UNIX = 148;
%constant int _SC_NPROC_CONF = 200;
%constant int _SC_NPROCESSORS_CONF = 200;	/* _SC_NPROC_CONF */
%constant int _SC_NPROC_ONLN = 201;
%constant int _SC_NPROCESSORS_ONLN = 201;	/* _SC_NPROC_ONLN */
%constant int _SC_2_C_VERSION = 129;		/* _SC_2_VERSION  */
%constant int _SC_2_CHAR_TERM = 137;
%constant int _SC_DELAYTIMER_MAX = 112;
%constant int _SC_TIMER_MAX = 113;
%constant int _SC_MAPPED_FILES = 114;
%constant int _SC_FSYNC = 115;
%constant int _SC_TIMERS = 116;
%constant int _SC_CPU_CHIP_TYPE = 160;

%rename(fix_time) _fix_time;
%rename(unixtime) _unixtime;
%rename(vmstime) _vmstime;
%rename(from_vms) _from_vms;
%rename(to_vms) _to_vms;
%rename(getenv) _getenv;
%rename(sysconf) _sysconf;
%rename(sleep) _sleep;
%rename(dlopen_test) _dlopen_test;
%rename(get_symbol) _get_symbol;

%newobject _from_vms;
%newobject _to_vms;

extern unsigned int _fix_time(long long);
extern unsigned int _unixtime(long long dt);
extern long long _vmstime(unsigned int);
extern char ** _from_vms(char *, int);
extern char ** _to_vms(char *, int, int);
extern char *_getenv(char *, char *);
extern long _sysconf(int);
extern int _sleep(unsigned int);
extern int _dlopen_test(char *name);
extern int _get_symbol(char *name, char** OUTPUT);


