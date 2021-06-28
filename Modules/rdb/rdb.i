%module rdb
%{
  #include "db.h"
%}

%typemap(in) (...) {
   PyObject *rep, *str, *obj;
   char **argv;
   int argc;
   int i;

   argc = PyTuple_Size(varargs);
   argv = (char **) malloc(sizeof(char *) * (argc + 1));
   for (i = 0; i < argc; i++) {
      obj = PyTuple_GetItem(varargs, i);
      if (!PyUnicode_Check(obj)) {
          rep = PyObject_Repr(obj);
          str = PyUnicode_AsEncodedString(rep, "utf-8", "~E~");
          argv[i] = strdup(PyBytes_AS_STRING(str));
          Py_XDECREF(rep);
          Py_XDECREF(str);
      } else {
    	  str = PyUnicode_AsUTF8String(obj);
          argv[i] = strdup(PyString_AsString(str));
	  Py_XDECREF(str);
      }
   }
   argv[i] = NULL;
   $1 = (void *) argv;
}
%typemap(freearg) (...)
{
    char **arr = $1;
    int i = 0;
    if (arr != NULL) {
        while (arr[i]) {
            free(arr[i]);
            i++;
        }
        free(arr);
    }
}


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

