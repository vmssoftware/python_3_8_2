#ifndef _PYRMSFILE_H
#define _PYRMSFILE_H

#include <rms.h>

#define PYRMS_M_OPEN 1

typedef struct rmsfilestruct {
  PyObject_HEAD
  FABDEF fab;
  RABDEF rab;
  NAMLDEF naml;
  XABSUMDEF sum;
  XABALLDEF* area;
  XABKEYDEF* key;
  char primarySpec[NAML$C_MAXRSS + 1];
 /*
  *  Create the string buffers for the resultant and expanded strings
  */
  char Naml_Shrt_Esa[NAM$C_MAXRSS + 1],
       Naml_Long_Esa[NAML$C_MAXRSS + 1],
       Naml_Shrt_Rsa[NAM$C_MAXRSS + 1],
       Naml_Long_Rsa[NAML$C_MAXRSS + 1];
  unsigned int flags;
} RmsFileObject;

PyAPI_DATA(PyTypeObject) RmsFile_Type;

#define RmsFileObject_Check(v) ((v)->ob_type == &RmsFile_Type)

#endif
