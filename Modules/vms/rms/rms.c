// Adapted from original code developed by Jean-Fran�ois Pieronne...
//

#define PY_SSIZE_T_CLEAN

#define __NEW_STARLET
#include <ssdef.h>
#include <stsdef.h>
#include <descrip.h>
#include <starlet.h>
#include <fdl$routines.h>
#include <ctype.h>
#include <string.h>
#include <rms.h>

#include "Python.h"
#include "structmember.h"

#define PYRMS_M_OPEN 1

typedef struct {
    PyObject_HEAD FABDEF fab;
    RABDEF rab;
    NAMLDEF naml;
    XABSUMDEF sum;
    XABALLDEF *area;
    XABKEYDEF *key;
    char long_filename[NAML$C_MAXRSS + 1];
    char esa[NAM$C_MAXRSS + 1];
    char long_esa[NAML$C_MAXRSS + 1];
    char rsa[NAM$C_MAXRSS + 1];
    char long_rsa[NAML$C_MAXRSS + 1];
    unsigned int flags;
} rms_file_t;


#ifndef adc_Assert
#define adc_Assert(x) \
    do { \
        if ((!((x)))) { \
            fprintf (stderr, "Assertion failed: %s (%s: %d)\n", #x, __FILE__, __LINE__); \
            abort(); \
        } \
    } while (0)
#endif

#ifndef OKAY
#define OKAY(STATUS) (((STATUS) & 1) != 0)
#endif


#define RMSATTR_K_ORG  0
#define RMSATTR_K_RFM  1
#define RMSATTR_K_RAT  2
#define RMSATTR_K_MRS  3
#define RMSATTR_K_LRL  4
#define RMSATTR_K_LAST RMSATTR_K_LRL


#define PARSE_K_NODE   0
#define PARSE_K_DEV    1
#define PARSE_K_DIR    2
#define PARSE_K_NAME   3
#define PARSE_K_TYPE   4
#define PARSE_K_VER    5

static PyObject *RMS_error;
PyDoc_STRVAR(RMS_error__doc__, "RMS error");
static rms_file_t *_new(char *, int, int, unsigned int);



static void addint(PyObject * d, char *name, int val)
{
    PyObject *obj = PyLong_FromLong((long) val);

    if (!obj || (PyDict_SetItemString(d, name, obj) == -1)) {
	Py_FatalError("can't initialize sane module");
    }

    Py_DECREF(obj);
}


static void getmsg(unsigned int cond, char *buf, int buflen)
{
    unsigned int status;
    struct dsc$descriptor_s bufD;
    unsigned short int msglen;

    bufD.dsc$w_length = buflen - 1;
    bufD.dsc$b_dtype = DSC$K_DTYPE_T;
    bufD.dsc$b_class = DSC$K_CLASS_S;
    bufD.dsc$a_pointer = buf;

    status = sys$getmsg(cond, &msglen, &bufD, 15, 0);

    if (!OKAY(status)) {
	strncpy(buf, "sys$getmsg failure", buflen);
	buf[buflen - 1] = '\0';
    }

    buf[msglen] = '\0';
}


static void
fill(rms_file_t * self, char *fn, int access, int share, unsigned int fop)
{
    self->fab = cc$rms_fab;
    self->rab = cc$rms_rab;
    self->sum = cc$rms_xabsum;
    self->naml = cc$rms_naml;
    self->area = 0;
    self->key = 0;
    self->flags = 0;
    self->fab.fab$b_fac = access;
    self->fab.fab$b_shr = share;
    self->fab.fab$l_fna = (char *) -1;
    self->fab.fab$b_fns = 0;
    self->fab.fab$l_fop = fop;
    self->fab.fab$l_xab = (void *) &self->sum;

    self->fab.fab$l_naml = (void *) &self->naml;

    strncpy(self->long_filename, fn, sizeof(self->long_filename) - 1);
    self->long_filename[sizeof(self->long_filename) - 1] = '\0';

    self->naml.naml$l_long_filename = (void *) self->long_filename;
    self->naml.naml$l_long_filename_size = strlen(self->long_filename);

    self->naml.naml$l_esa = (void *) self->esa;
    self->naml.naml$b_ess = sizeof(self->esa) - 1;
    self->naml.naml$l_rsa = (void *) self->rsa;
    self->naml.naml$b_rss = sizeof(self->rsa) - 1;

    self->naml.naml$l_long_expand = (void *) self->long_esa;
    self->naml.naml$l_long_expand_alloc = sizeof(self->long_esa) - 1;
    self->naml.naml$l_long_result = (void *) self->long_rsa;
    self->naml.naml$l_long_result_alloc = sizeof(self->long_rsa) - 1;

    self->naml.naml$v_synchk = 0;	        // Have $PARSE do directory existence check
    self->naml.naml$v_no_short_upcase = 1;	// Don't uppercase short expanded file specification

}


static unsigned int _open(rms_file_t * self)
{
    unsigned int status;
    int i;
    XABKEYDEF *key;
    XABALLDEF *area;
    void **xab_chain;

    status = sys$open((void *) &self->fab);

    if (!OKAY(status)) {
	return (status);
    }

    if (self->fab.fab$b_org != FAB$C_IDX) {
	return (RMS$_ORG);
    }

    xab_chain = (void *) &self->fab.fab$l_xab;

    i = self->sum.xab$b_nok;

    adc_Assert((self->key = key = calloc(i, sizeof(XABKEYDEF))));
    while (--i >= 0) {
	*key = cc$rms_xabkey;
	key->xab$b_ref = i;
	key->xab$l_nxt = *xab_chain;
	*xab_chain = key;
	key++;
    }

    i = self->sum.xab$b_noa;
    adc_Assert((self->area = area = calloc(i, sizeof(XABALLDEF))));
    while (--i >= 0) {
	*area = cc$rms_xaball;
	area->xab$b_aid = i;
	area->xab$l_nxt = *xab_chain;
	*xab_chain = area;
	area++;
    }

    status = sys$display((void *) &self->fab);

    if (!OKAY(status)) {
	return (status);
    }

    self->naml.naml$l_esa[self->naml.naml$b_esl] = '\0';
    self->naml.naml$l_rsa[self->naml.naml$b_rsl] = '\0';
    self->naml.naml$l_long_expand[self->naml.naml$l_long_expand_size] =
	'\0';
    self->naml.naml$l_long_result[self->naml.naml$l_long_result_size] =
	'\0';

    self->flags |= PYRMS_M_OPEN;

    self->rab.rab$l_fab = (void *) &self->fab;

    /* default to primary index */


    self->rab.rab$b_krf = 0;
    self->rab.rab$l_ubf = NULL;
    self->rab.rab$w_usz = self->fab.fab$w_mrs;

    return (sys$connect((void *) &self->rab));
}


static unsigned int _close(rms_file_t * self)
{
    unsigned int status;

    status = sys$disconnect((void *) &self->rab);

    if (!OKAY(status)) {
	return (status);
    }

    return (sys$close((void *) &self->fab));
}


static unsigned int _flush(rms_file_t * self)
{
    return (sys$flush((void *) &self->rab));
}


static unsigned int _free(rms_file_t * self)
{
    return (sys$free((void *) &self->rab));
}


static unsigned int _release(rms_file_t * self)
{
    return (sys$release((void *) &self->rab));
}


static unsigned int _usekey(rms_file_t * self, int keynum)
{
    if (keynum >= self->sum.xab$b_nok) {
	return (SS$_INVARG);
    }

    self->rab.rab$b_krf = keynum;
    return (RMS$_NORMAL);
}


static int _put(rms_file_t * self, char *buf, int len)
{
    self->rab.rab$b_rac = RAB$C_KEY;
    self->rab.rab$l_rbf = buf;
    self->rab.rab$w_rsz = len;
    return (sys$put((void *) &self->rab));
}


static unsigned int _update(rms_file_t * self, char *buf, int len)
{
    self->rab.rab$l_rbf = buf;
    self->rab.rab$w_rsz = len;
    return (sys$update((void *) &self->rab));
}


static unsigned int _fetch(rms_file_t * self, char *key, int len,
			   char *buf, int *retlen)
{
    unsigned int status;

    if (key) {
	self->rab.rab$l_kbf = key;
	self->rab.rab$b_ksz = len;
	self->rab.rab$b_rac = RAB$C_KEY;
    } else {
	self->rab.rab$b_rac = RAB$C_SEQ;
    }

    self->rab.rab$l_ubf = (void *) buf;

    status = sys$get((void *) &self->rab);

    if (!OKAY(status)) {
	return (status);
    }

    *retlen = self->rab.rab$w_rsz;
    return (status);
}


static unsigned int _find(rms_file_t * self, char *key, int len)
{
    if (key) {
	self->rab.rab$l_kbf = key;
	self->rab.rab$b_ksz = len;
	self->rab.rab$b_rac = RAB$C_KEY;
    } else {
	self->rab.rab$b_rac = RAB$C_SEQ;
    }

    return (sys$find((void *) &self->rab));
}


static unsigned int _delete(rms_file_t * self, char *key, int len)
{
    unsigned int status;

    if (key) {
	self->rab.rab$l_kbf = key;
	self->rab.rab$b_ksz = len;
	self->rab.rab$b_rac = RAB$C_KEY;

	status = sys$find((void *) &self->rab);

	if (!OKAY(status)) {
	    return (status);
	}
    } else {
	self->rab.rab$b_rac = RAB$C_SEQ;
    }

    return (sys$delete((void *) &self->rab));
}



#define PyFileObject PyObject

static PyObject *getiter(PyFileObject * obj)
{
    Py_INCREF(obj);
    return (PyObject *) obj;
}


static PyObject *iternext(PyFileObject * self)
{
    unsigned int status;
    int len = 0;
    PyObject *obj;
    char msg[256];
    char buf[((rms_file_t *) self)->fab.fab$w_mrs];

    status = _fetch((rms_file_t *) self, NULL, 0, buf, &len);

    if (status == RMS$_EOF) {
	return (NULL);
    }

    if (!OKAY(status)) {
	getmsg(status, msg, sizeof(msg));
	if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
	    PyErr_SetObject(RMS_error, obj);
	    Py_DECREF(obj);
	}

	return (NULL);
    }

    if (status == RMS$_EOF) {
	Py_INCREF(Py_None);
	obj = Py_None;
    } else {
	obj = PyBytes_FromStringAndSize(buf, len);
    }

    return (obj);
}



static void dealloc(rms_file_t * self)
{
    unsigned int status;
    PyObject *obj;
    char msg[256];

    if (self->flags & PYRMS_M_OPEN) {
	status = _close(self);

	if (!OKAY(status)) {
	    getmsg(status, msg, sizeof(msg));
	    if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
		PyErr_SetObject(RMS_error, obj);
		Py_DECREF(obj);
	    }
	}

	self->flags ^= PYRMS_M_OPEN;
    }

    if (self->area) {
	free(self->area);
	self->area = NULL;
    }

    if (self->key) {
	free(self->key);
	self->key = NULL;
    }

    PyObject_Del(self);
}



static PyObject *RMS_close(rms_file_t * self, PyObject * args)
{
    unsigned int status;
    char msg[256];
    PyObject *obj;

    if (!PyArg_ParseTuple(args, ":close")) {
	return (NULL);
    }

    status = _close(self);

    if (!OKAY(status)) {
	getmsg(status, msg, sizeof(msg));
	if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
	    PyErr_SetObject(RMS_error, obj);
	    Py_DECREF(obj);
	}

	return (NULL);
    }

    self->flags ^= PYRMS_M_OPEN;
    return (PyLong_FromLong(status));
}


static PyObject *RMS_flush(rms_file_t * self, PyObject * args)
{
    unsigned int status;
    char msg[256];
    PyObject *obj;

    if (!PyArg_ParseTuple(args, ":flush")) {
	return (NULL);
    }

    status = _flush(self);

    if (!OKAY(status)) {
	getmsg(status, msg, sizeof(msg));
	if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
	    PyErr_SetObject(RMS_error, obj);
	    Py_DECREF(obj);
	}

	return (NULL);
    }

    return (PyLong_FromLong(status));
}


static PyObject *RMS_free(rms_file_t * self, PyObject * args)
{
    unsigned int status;
    char msg[256];
    PyObject *obj;

    if (!PyArg_ParseTuple(args, ":free")) {
	return (NULL);
    }

    status = _free(self);

    if (!OKAY(status)) {
	getmsg(status, msg, sizeof(msg));
	if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
	    PyErr_SetObject(RMS_error, obj);
	    Py_DECREF(obj);
	}

	return (NULL);
    }

    return (PyLong_FromLong(status));
}



static PyObject *RMS_release(rms_file_t * self, PyObject * args)
{
    unsigned int status;
    char msg[256];
    PyObject *obj;

    if (!PyArg_ParseTuple(args, ":release")) {
	return (NULL);
    }

    status = _release(self);

    if (!OKAY(status)) {
	getmsg(status, msg, sizeof(msg));
	if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
	    PyErr_SetObject(RMS_error, obj);
	    Py_DECREF(obj);
	}

	return (NULL);
    }

    return (PyLong_FromLong(status));
}


static PyObject *RMS_rewind(rms_file_t * self, PyObject * args,
			    PyObject * kwargs)
{
    unsigned int status;
    int keynum = -1;
    char msg[256];
    PyObject *obj;
    char *kwnames[] = {
	"keynum", NULL
    };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, (char *) "|i:rewind",
				     kwnames, &keynum)) {
	return (NULL);
    }

    if (keynum >= 0) {
	status = _usekey((rms_file_t *) self, keynum);

	if (!OKAY(status)) {
	    getmsg(status, msg, sizeof(msg));
	    if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
		PyErr_SetObject(RMS_error, obj);
		Py_DECREF(obj);
	    }

	    return (NULL);
	}
    }

    status = sys$rewind((void *) &self->rab, 0, 0);

    if (!OKAY(status)) {
	getmsg(status, msg, sizeof(msg));
	if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
	    PyErr_SetObject(RMS_error, obj);
	    Py_DECREF(obj);
	}

	return (NULL);
    }

    return (PyLong_FromLong(status));
}


static PyObject *RMS_usekey(rms_file_t * self, PyObject * args,
			    PyObject * kwargs)
{
    unsigned int status;
    int keynum = 0;
    PyObject *res, *obj;
    char msg[256];
    char *kwnames[] = {
	"keynum", NULL
    };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, (char *) "|i:usekey",
				     kwnames, &keynum)) {
	return (NULL);
    }

    status = _usekey((rms_file_t *) self, keynum);
    if (!OKAY(status)) {
	getmsg(status, msg, sizeof(msg));
	if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
	    PyErr_SetObject(RMS_error, obj);
	    Py_DECREF(obj);
	}

	return (NULL);
    }

    return (PyLong_FromLong(status));
}


static PyObject *RMS_put(rms_file_t * self, PyObject * args,
			 PyObject * kwargs)
{
    unsigned int status;
    char *buf = NULL;
    int len = 0;
    PyObject *res, *obj;
    char msg[256];
    char *kwnames[] = {
	"record", NULL
    };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, (char *) "s#:put",
				     kwnames, &buf, &len)) {
	return (NULL);
    }

    status = _put((rms_file_t *) self, buf, len);

    if (!OKAY(status)) {
	getmsg(status, msg, sizeof(msg));
	if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
	    PyErr_SetObject(RMS_error, obj);
	    Py_DECREF(obj);
	}

	return (NULL);
    }

    return (PyLong_FromLong(status));
}


static PyObject *RMS_update(rms_file_t * self, PyObject * args,
			    PyObject * kwargs)
{
    unsigned int status;
    char *buf = NULL;
    int len = 0;
    PyObject *res, *obj;
    char msg[256];
    char *kwnames[] = {
	"record", NULL
    };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, (char *) "s#:update",
				     kwnames, &buf, &len)) {
	return (NULL);
    }

    status = _update((rms_file_t *) self, buf, len);

    if (!OKAY(status)) {
	getmsg(status, msg, sizeof(msg));
	if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
	    PyErr_SetObject(RMS_error, obj);
	    Py_DECREF(obj);
	}

	return (NULL);
    }

    return (PyLong_FromLong(status));
}


static PyObject *RMS_fetch(rms_file_t * self, PyObject * args,
			   PyObject * kwargs)
{
    unsigned int status;
    char *keyval = NULL;
    int len = 0;
    PyObject *res, *obj, *o1, *o2;
    char msg[256];
    char *kwnames[] = {
	"key", NULL
    };

    char buf[((rms_file_t *) self)->fab.fab$w_mrs];
    int retlen = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, (char *) "|s#:fetch",
				     kwnames, &keyval, &len)) {
	return (NULL);
    }

    status = _fetch((rms_file_t *) self, keyval, len, buf, &retlen);

    if (status != RMS$_EOF && status != RMS$_RNF && !OKAY(status)) {
	getmsg(status, msg, sizeof(msg));
	if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
	    PyErr_SetObject(RMS_error, obj);
	    Py_DECREF(obj);
	}

	return (NULL);
    }

    o1 = PyLong_FromLong(status);

    if (status == RMS$_EOF || status == RMS$_RNF) {
	Py_INCREF(Py_None);
	o2 = Py_None;
    } else {
	o2 = PyBytes_FromStringAndSize(buf, retlen);
    }

    res = PyTuple_New(2);
    PyTuple_SetItem(res, 0, o1);
    PyTuple_SetItem(res, 1, o2);

    return (res);
}


static PyObject *RMS_find(rms_file_t * self, PyObject * args,
			  PyObject * kwargs)
{
    unsigned int status;
    char *keyval = NULL;
    int len = 0;
    char msg[256];
    PyObject *obj;
    char *kwnames[] = {
	"key", NULL
    };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, (char *) "|s#:find",
				     kwnames, &keyval, &len)) {
	return (NULL);
    }

    status = _find((rms_file_t *) self, keyval, len);

    if (status != RMS$_EOF && !OKAY(status)) {
	getmsg(status, msg, sizeof(msg));
	if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
	    PyErr_SetObject(RMS_error, obj);
	    Py_DECREF(obj);
	}

	return (NULL);
    }

    return (PyLong_FromLong(status));
}


static PyObject *RMS_delete(rms_file_t * self, PyObject * args,
			    PyObject * kwargs)
{
    unsigned int status;
    char *keyval = NULL;
    int len = 0;
    char msg[256];
    PyObject *obj;
    char *kwnames[] = {
	"key", NULL
    };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, (char *) "|s#:delete",
				     kwnames, &keyval, &len)) {
	return (NULL);
    }

    status = _delete((rms_file_t *) self, keyval, len);

    if (status != RMS$_EOF && !OKAY(status)) {
	getmsg(status, msg, sizeof(msg));
	if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
	    PyErr_SetObject(RMS_error, obj);
	    Py_DECREF(obj);
	}

	return (NULL);
    }

    return (PyLong_FromLong(status));
}



static PyMethodDef tp_methods[] = {
    {"close", (PyCFunction) RMS_close, METH_VARARGS,
     PyDoc_STR("close() -> status")},
    {"delete", (PyCFunction) RMS_delete, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("delete([key]) -> status")},
    {"fetch", (PyCFunction) RMS_fetch, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("get([key]) -> status, record")},
    {"find", (PyCFunction) RMS_find, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("find([key]) -> status")},
    {"flush", (PyCFunction) RMS_flush, METH_VARARGS,
     PyDoc_STR("flush() -> status")},
    {"free", (PyCFunction) RMS_free, METH_VARARGS,
     PyDoc_STR("free() -> status")},
    {"put", (PyCFunction) RMS_put, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("put(record) -> status")},
    {"release", (PyCFunction) RMS_release, METH_VARARGS,
     PyDoc_STR("release() -> status")},
    {"rewind", (PyCFunction) RMS_rewind, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("rewind([keynum]) -> status")},
    {"update", (PyCFunction) RMS_update, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("update(record) -> status")},
    {"usekey", (PyCFunction) RMS_usekey, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("use_key([keynum]) -> status")},
    {NULL, NULL}
};


#define OFF(x) offsetof(rms_file_t, x)

static PyMemberDef tp_members[] = {
    {"longname", T_STRING_INPLACE, OFF(long_rsa), READONLY,
     "long filename"},
    {"nok", T_UBYTE, OFF(sum.xab$b_nok), READONLY,
     "number of keys"},
    {"org", T_UBYTE, OFF(fab.fab$b_org), READONLY,
     "file organization"},
    {NULL}
};



static PyObject *RMS_new(PyObject * self, PyObject * args,
			 PyObject * kwargs)
{
    unsigned int status;
    char *name;
    unsigned int alq = 0;
    unsigned short bls = 0;
    unsigned short deq = 0;
    unsigned int fop = cc$rms_fab.fab$l_fop;
    unsigned short gbc = 0;
    unsigned char mbc = 0;
    char mbf = 0;
    unsigned char rat = 0;
    unsigned char rfm = 0;
    unsigned int rop = 0;
    char rtv = 0;
    int fac = FAB$M_GET;
    int shr = FAB$M_SHRPUT | FAB$M_SHRGET | FAB$M_SHRDEL | FAB$M_SHRUPD;
    rms_file_t *res;
    char msg[256];
    PyObject *obj;
    char *kwnames[] = {
	"name", "fac", "shr", "fop", NULL
    };

    if (!PyArg_ParseTupleAndKeywords
	(args, kwargs, (char *) "s|iiI:open_file", kwnames, &name, &fac,
	 &shr, &fop)) {
	return (NULL);
    }

    res = _new(name, fac, shr, fop);

    if (res) {
	status = _open(res);

	if (!OKAY(status)) {
	    getmsg(status, msg, sizeof(msg));
	    if ((obj = Py_BuildValue("(is)", status, msg)) != NULL) {
		PyErr_SetObject(RMS_error, obj);
		Py_DECREF(obj);
	    }

	    Py_DECREF(res);
	    return (NULL);
	}
    }

    return ((PyObject *) res);
}


static PyObject *bcd2string(PyObject * dummy, PyObject * args)
{
    PyObject *res = NULL;
    char c;
    char *temp, *str;
    int len, prec;
    int i, j, k;

    if (!PyArg_ParseTuple(args, "s#i", &temp, &len, &prec)) {
	return NULL;
    }

    str = PyMem_Malloc(2 * len + 2);
    c = (temp[len - 1] & 0x0F);

    if ((c != 0x0C) && (c != 0x0D)) {
	PyMem_Free(str);
	PyErr_SetString(PyExc_TypeError, "Invalid sign");
	return NULL;
    }

    if ((temp[len - 1] & 0x0F) == 0x0C) {
	str[0] = '+';
    } else {
	str[0] = '-';
    }

    for (j = 0, i = 1; i < (2 * len - prec); ++i) {
	if (i & 1) {
	    str[i] = '0' + ((temp[j] & 0xF0) >> 4);
	} else {
	    str[i] = '0' + (temp[j] & 0x0F);
	    ++j;
	}
    }

    i = 2 * len - prec;
    str[i++] = '.';

    for (; i < 2 * len + 1 + 1; ++i) {
	if (i & 1) {
	    str[i] = '0' + (temp[j] & 0x0F);
	    ++j;
	} else {
	    str[i] = '0' + ((temp[j] & 0xF0) >> 4);
	}
    }

    res = PyBytes_FromStringAndSize(str, 2 * len + 1);
    PyMem_Free(str);

    return (res);
}


static PyObject *string2bcd(PyObject * dummy, PyObject * args)
{
    PyObject *res = NULL;
    char *temp;
    int len, prec;
    int n;
    char *p = NULL;
    int i;

    if (!PyArg_ParseTuple(args, "sii", &temp, &n, &prec)) {
	return (NULL);
    }

    if ((n + prec + 1) & 1) {
	++n;
    }

    len = strlen(temp);
    if ((n + prec > 31) || (len > 33)) {
	PyErr_SetString(PyExc_TypeError, "Invalid len");
	return (NULL);
    }
    if (strchr("+-0123456789", *temp) == NULL) {
	PyErr_SetString(PyExc_TypeError, "Invalid first character");
	return (NULL);
    }

    for (i = 1; i < len; ++i) {
	if ((temp[i] == '.') && (p == NULL)) {
	    p = &(temp[i]) + 1;
	} else if (!isdigit(temp[i])) {
	    PyErr_SetString(PyExc_TypeError, "Invalid numeric");
	    return (NULL);
	}
    }

    {
	long long in;
	long long id = 0;
	char astr[34];
	char rstr[16];
	char fmt[20];
	char sign = 0x0C;
	int alen;

	sscanf(temp, "%lld", &in);
	if (p) {
	    sscanf(p, "%lld", &id);
	}
	if (in < 0) {
	    in = -in;
	    sign = 0x0D;
	}
	sprintf(fmt, "%%0%dlld%%lld", n);
	sprintf(astr, fmt, in, id);
	alen = strlen(astr);
	if (alen > n + prec) {
	    astr[n + prec] = '\0';
	} else if (alen < n + prec) {
	    for (i = n + prec - 1; i >= alen; --i) {
		astr[i] = '0';
	    }
      	    astr[n + prec] = '\0';
	}
	for (i = 0; i < (n + prec - 1) / 2; ++i) {
	    rstr[i] = (astr[2 * i] - '0') * 16 + (astr[2 * i + 1] - '0');
	}
	rstr[(n + prec - 1) / 2] = (astr[n + prec - 1] - '0') * 16 + sign;
	res = PyBytes_FromStringAndSize(rstr, (n + prec + 1) / 2);
	return (res);
    }
}


static PyObject *bcd2tuple(PyObject * dummy, PyObject * args)
{
    PyObject *res = NULL, *pdigit = NULL;
    char *temp;
    int len, prec, sign;
    int i, j, k, v;
    char c;

    if (!PyArg_ParseTuple(args, "s#i", &temp, &len, &prec)) {
	return (NULL);
    }

    c = (temp[len - 1] & 0x0F);

    if ((c != 0x0C) && (c != 0x0D)) {
	PyErr_SetString(PyExc_TypeError, "Invalid sign");
	return (NULL);
    }

    res = PyTuple_New(3);
    pdigit = PyTuple_New(2 * len - 1);

    if ((temp[len - 1] & 0x0F) == 0x0C) {
	sign = 0;
    } else {
	sign = 1;
    }

    PyTuple_SetItem(res, 0, (PyObject *) PyLong_FromLong(sign));
    PyTuple_SetItem(res, 1, pdigit);
    PyTuple_SetItem(res, 2, (PyObject *) PyLong_FromLong(-prec));

    for (j = 0, i = 0; i < (2 * len - 1); ++i) {
	if (i & 1) {
	    v = (temp[j] & 0x0F);
	    ++j;
	} else {
	    v = ((temp[j] & 0xF0) >> 4);
	}
	PyTuple_SetItem(pdigit, i, (PyObject *) PyLong_FromLong(v));
    }

    return (res);
}


static PyObject *tuple2bcd(PyObject * dummy, PyObject * args)
{
    PyObject *res = NULL;
    PyObject *lst;
    char *temp;
    int len, dprec, prec, sign;
    int n;

    if (!PyArg_ParseTuple(args, "(iO!i)ii", &sign, &PyTuple_Type, &lst,
			  &dprec, &n, &prec)) {
	return (NULL);
    }

    len = PyTuple_Size(lst);

    if (len > 31) {
	PyErr_SetString(PyExc_TypeError, "Invalid digits tuple length");
	return (NULL);
    }

    if (((n + prec) & 1) == 0) {
	++n;
    }
    if (n + prec > 31) {
	PyErr_SetString(PyExc_TypeError, "Invalid BCD length");
	return (NULL);
    }

    dprec = -dprec;

    if (len - dprec > n) {
	PyErr_SetString(PyExc_TypeError, "BCD integer part too short");
	return (NULL);
    }

    {
	char rstr[16];
	char digit[n + prec + 1];
	int i, j, k;

	for (i = 0; i < n - len + dprec; ++i) {
	    digit[i] = 0;
	}

	for (j = 0; j < len - dprec; ++j, ++i) {
	    digit[i] = PyLong_AsLong(PyTuple_GetItem(lst, j));
	}

	if (dprec > prec) {
	    dprec = prec;
	}

	for (; i < n + dprec; ++i, ++j) {
	    digit[i] = PyLong_AsLong(PyTuple_GetItem(lst, j));
	}

	for (; i < n + prec; ++i) {
	    digit[i] = 0;
	}

	digit[n + prec] = 0;

	for (i = 0; i < (n + prec - 1) / 2; ++i) {
	    rstr[i] = (digit[2 * i]) * 16 + (digit[2 * i + 1]);
	}

	if (sign == 0) {
	    sign = 0x0C;
	} else {
	    sign = 0x0D;
	}

	rstr[(n + prec - 1) / 2] = (digit[n + prec - 1]) * 16 + sign;
	res = PyBytes_FromStringAndSize(rstr, (n + prec + 1) / 2);

	return (res);
    }

    Py_INCREF(Py_None);
    return (Py_None);
}


static unsigned int _getrmsattr(char *name, int attr, int *res)
{
    int status;
    unsigned int fop = 0;
    int fac = 0;
    int shr = FAB$M_SHRGET;
    rms_file_t *fo;
    XABFHCDEF *fhc;

    adc_Assert((fo = malloc(sizeof(rms_file_t))));
    fill(fo, name, fac, shr, fop);

    if (attr == RMSATTR_K_LRL) {
	adc_Assert((fhc = malloc(sizeof(XABFHCDEF))));
	*fhc = cc$rms_xabfhc;
	fo->fab.fab$l_xab = fhc;
    }

    status = sys$open((void *) &fo->fab);

    if (!OKAY(status)) {
	if (attr == RMSATTR_K_LRL) {
	    free(fhc);
	}
	free(fo);
	return (status);
    }

    status = sys$display((void *) &fo->fab);

    if (!OKAY(status)) {
	free(fo);
	return (status);
    }

    *res = 0;

    switch (attr) {
    case RMSATTR_K_ORG:
	*res = fo->fab.fab$b_org;
	break;
    case RMSATTR_K_RFM:
	*res = fo->fab.fab$b_rfm;
	break;
    case RMSATTR_K_MRS:
	*res = fo->fab.fab$w_mrs;
	break;
    case RMSATTR_K_RAT:
	*res = fo->fab.fab$b_rat;
	break;
    case RMSATTR_K_LRL:
	*res = fhc->xab$w_lrl;
	break;
    default:
	abort();
	break;
    }

    status = sys$close((void *) &fo->fab);

    if (attr == RMSATTR_K_LRL) {
	free(fhc);
    }

    free(fo);
    return (status);
}


static PyObject *RMS_getrmsattr(PyObject * dummy, PyObject * args)
{
    unsigned int status;
    char *name;
    int attr, res;
    char msg[256];
    PyObject *obj;

    if (!PyArg_ParseTuple(args, "si:getrmsattr", &name, &attr)) {
	return (NULL);
    }

    if (attr < 0 || attr > RMSATTR_K_LAST) {
	PyErr_Format(RMS_error, "invalid attribute value: %d", attr);
	return (NULL);
    }

    status = _getrmsattr(name, attr, &res);

    if (!OKAY(status)) {
	getmsg(status, msg, sizeof(msg));
	if ((obj = Py_BuildValue("(is)", stat, msg)) != NULL) {
	    PyErr_SetObject(RMS_error, obj);
	    Py_DECREF(obj);
	}
	return (NULL);
    }

    return PyLong_FromLong((long) res);
}


static unsigned int
_parse(char *path, char *node, char *dev, char *dir, char *name,
       char *type, char *ver)
{
    unsigned int status;
    unsigned int fop = NAM$M_SYNCHK;
    int fac = 0;
    int shr = 0;
    rms_file_t *fo;
    int p1, p2, p3, p4, p5, p6;

    fo = PyMem_NEW(rms_file_t, 1);
    fill(fo, path, fac, shr, fop);

    status = sys$parse((void *) &fo->fab);

    if (!OKAY(status)) {
	PyMem_FREE(fo);
	return (status);
    }

    fo->naml.naml$l_esa[fo->naml.naml$b_esl] = '\0';
    fo->naml.naml$l_long_expand[fo->naml.naml$l_long_expand_size] = '\0';

    p1 = fo->naml.naml$b_node;
    p2 = fo->naml.naml$b_dev;
    p3 = fo->naml.naml$b_dir;
    p4 = fo->naml.naml$b_name;
    p5 = fo->naml.naml$b_type;
    p6 = fo->naml.naml$b_ver;

    *node = *dev = *dir = *name = *type = *ver = '\0';

    if (p1) {
	strncpy(node, fo->naml.naml$l_long_expand, p1);
	node[p1] = '\0';
    }

    if (p2) {
	strncpy(dev, fo->naml.naml$l_long_expand + p1, p2);
	dev[p2] = '\0';
    }

    if (p3) {
	strncpy(dir, fo->naml.naml$l_long_expand + p1 + p2, p3);
	dir[p3] = '\0';
    }

    if (p4) {
	strncpy(name, fo->naml.naml$l_long_expand + p1 + p2 + p3, p4);
	name[p4] = '\0';
    }

    if (p5) {
	strncpy(type, fo->naml.naml$l_long_expand + p1 + p2 + p3 + p4, p5);
	type[p5] = '\0';
    }

    if (p6) {
	strncpy(ver, fo->naml.naml$l_long_expand + p1 + p2 + p3 + p4 + p5,
		p6);
	ver[p6] = '\0';
    }

    PyMem_FREE(fo);
    return (status);
}


static PyObject *RMS_parse(PyObject * dummy, PyObject * args)
{
    unsigned int status;
    char *path;
    char node[NAML$C_MAXRSS + 1];
    char dev[NAML$C_MAXRSS + 1];
    char dir[NAML$C_MAXRSS + 1];
    char name[NAML$C_MAXRSS + 1];
    char type[NAML$C_MAXRSS + 1];
    char ver[NAML$C_MAXRSS + 1];
    PyObject *obj;
    char msg[256];

    if (!PyArg_ParseTuple(args, "s:parse", &path)) {
	return (NULL);
    }

    status = _parse(path, node, dev, dir, name, type, ver);

    if (!OKAY(status)) {
	getmsg(status, msg, sizeof(msg));
	if ((obj = Py_BuildValue("(is)", stat, msg)) != NULL) {
	    PyErr_SetObject(RMS_error, obj);
	    Py_DECREF(obj);
	}

	return (NULL);
    }

    obj = PyTuple_New(6);
    PyTuple_SetItem(obj, 0, PyBytes_FromString(node));
    PyTuple_SetItem(obj, 1, PyBytes_FromString(dev));
    PyTuple_SetItem(obj, 2, PyBytes_FromString(dir));
    PyTuple_SetItem(obj, 3, PyBytes_FromString(name));
    PyTuple_SetItem(obj, 4, PyBytes_FromString(type));
    PyTuple_SetItem(obj, 5, PyBytes_FromString(ver));

    return (obj);
}


PyTypeObject RmsFile_Type = {
    PyObject_HEAD_INIT(NULL)
	"rmsFile",		// tp_name
    sizeof(rms_file_t),		// tp_basicsize
    0,				// tp_itemsize
    (destructor) dealloc,	// tp_dealloc
    0,				// tp_print
    0,				// tp_getattr
    0,				// tp_setattr
    NULL,			// formerly known as tp_compare or tp_reserved
    0,				// tp_repr
    0,				// tp_as_number
    0,				// tp_as_sequence
    0,				// tp_as_mapping
    0,				// tp_hash
    0,				// tp_call
    0,				// tp_str
    PyObject_GenericGetAttr,	// tp_getattro
    0,				// tp_setattro
    0,				// tp_as_buffer
    Py_TPFLAGS_DEFAULT,		// tp_flags
    0,				// tp_doc
    0,				// tp_traverse
    0,				// tp_clear
    0,				// tp_richcompare
    0,				// tp_weaklistoffset
    (getiterfunc) getiter,	// tp_iter
    (iternextfunc) iternext,	// tp_iternext
    tp_methods,			// tp_methods
    tp_members,			// tp_members
    0,				// tp_getset
    0,				// tp_base
    0,				// tp_dict
    0,				// tp_descr_get
    0,				// tp_descr_set
    0,				// tp_dictoffset
    0,				// tp_init
    0,				// tp_alloc
    0,				// tp_new
    0,				// tp_free
    0,				// tp_is_gc
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    NULL
};


static rms_file_t *_new(char *fn, int access, int share, unsigned int fop)
{
    rms_file_t *self = PyObject_New(rms_file_t, &RmsFile_Type);

    if (self) {
	fill(self, fn, access, share, fop);
    }

    return (self);
}



static PyMethodDef m_methods[] = {
    {"file", (PyCFunction) RMS_new, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("file(name [,fac] [,shr], [fop]) -> new RMS file object")},
    {"BCD2String", (PyCFunction) bcd2string, METH_VARARGS,
     PyDoc_STR("BCD2String(s) -> string")},
    {"String2BCD", (PyCFunction) string2bcd, METH_VARARGS,
     PyDoc_STR("String2BCD(obj) -> BCD string")},
    {"BCD2Tuple", (PyCFunction) bcd2tuple, METH_VARARGS,
     PyDoc_STR("BCD2Tuple(s) -> tuple")},
    {"Tuple2BCD", (PyCFunction) tuple2bcd, METH_VARARGS,
     PyDoc_STR("Tuple2BCD(obj) -> BCD string")},
    {"getrmsattr", (PyCFunction) RMS_getrmsattr, METH_VARARGS,
     PyDoc_STR("getrmsattr(path, attr) -> value")},
    {"parse", (PyCFunction) RMS_parse, METH_VARARGS,
     PyDoc_STR("parse(path) -> (node, dev, dir, name, ext, ver)")},
    {NULL, NULL}
};


static struct PyModuleDef moddef = {
    PyModuleDef_HEAD_INIT,
    "_rms",			/* m_name */
    "OpenVMS RMS module",	/* m_doc */
    -1,				/* m_size */
    m_methods,			/* m_methods */
    NULL,			/* m_reload */
    NULL,			/* m_traverse */
    NULL,			/* m_clear */
    NULL,			/* m_free */
};


PyMODINIT_FUNC PyInit__rms(void)
{
    PyObject *m, *d;
    PyObject *dict = PyDict_New();
    PyObject *str = NULL;

    m = PyModule_Create(&moddef);
    d = PyModule_GetDict(m);

    addint(d, "RMSATTR_K_ORG", RMSATTR_K_ORG);
    addint(d, "RMSATTR_K_RFM", RMSATTR_K_RFM);
    addint(d, "RMSATTR_K_RAT", RMSATTR_K_RAT);
    addint(d, "RMSATTR_K_MRS", RMSATTR_K_MRS);
    addint(d, "RMSATTR_K_LRL", RMSATTR_K_LRL);
    addint(d, "RMSATTR_K_LAST", RMSATTR_K_LAST);
    addint(d, "PARSE_K_NODE", PARSE_K_NODE);
    addint(d, "PARSE_K_DEV", PARSE_K_DEV);
    addint(d, "PARSE_K_DIR", PARSE_K_DIR);
    addint(d, "PARSE_K_NAME", PARSE_K_NAME);
    addint(d, "PARSE_K_TYPE", PARSE_K_TYPE);
    addint(d, "PARSE_K_VER", PARSE_K_VER);

    str = PyBytes_FromString(RMS_error__doc__);
    PyDict_SetItemString(dict, "__doc__", str);

    if (RMS_error == NULL) {
	if ((RMS_error =
	     PyErr_NewException("vms.rms.error", PyExc_IOError,
				dict)) == NULL) {
	    return (NULL);
	}
    }

    Py_XDECREF(dict);
    Py_XDECREF(str);
    Py_INCREF(RMS_error);
    PyModule_AddObject(m, "error", RMS_error);

    return (m);
}
