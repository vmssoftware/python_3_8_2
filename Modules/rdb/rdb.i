%module rdb
%{
  #include "db.h"
%}

%typemap(out) char** {
  int len,i;
  if ($1 == NULL) {
      $result = Py_None;
  } else {
      len = 0;
      while ($1[len]) len++;
      $result = PyList_New(len);
      for (i = 0; i < len; i++) {
        PyList_SetItem($result, i, PyString_FromString($1[i]));
      }
  }
}
%typemap(freearg) char **
{
    extern void eib_DB_FreeRow(char **);
    eib_DB_FreeRow( $1 );
}

%include "db.h"

