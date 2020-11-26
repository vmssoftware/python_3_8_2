#define __NEW_STARLET
#include <rms.h>
#include <ssdef.h>
#include <stsdef.h>
#include <descrip.h>
#include <starlet.h>
#include <fdl$routines.h>
#include <ctype.h>
#include <string.h>

#include "Python.h"
#include "structmember.h"

#include "pyrms.h"


#define RMSATTR_K_ORG  0
#define RMSATTR_K_RFM  1
#define RMSATTR_K_RAT  2
#define RMSATTR_K_MRS  3
#define RMSATTR_K_LRL  4

/* update next if items are added */
#define RMSATTR_K_LAST RMSATTR_K_LRL

#define PARSE_K_NODE   0
#define PARSE_K_DEV    1
#define PARSE_K_DIR    2
#define PARSE_K_NAME   3
#define PARSE_K_TYPE   4
#define PARSE_K_VER    5

static PyObject *RMSError;
PyDoc_STRVAR(RMSError__doc__, "OpenVMS RMS call failed");

PyTypeObject RmsFile_Type;

static void insint(PyObject * d, char *name, int value)
{
    PyObject *v = PyLong_FromLong((long) value);
    if (!v || PyDict_SetItemString(d, name, v))
	Py_FatalError("can't initialize sane module");

    Py_DECREF(v);
}

static void getmsg(int cond, char *buf, int buflen)
{
    int s;
    struct dsc$descriptor_s bufD;
    char *tmp;
    unsigned short int msglen;

    tmp = malloc(buflen);
    assert(tmp);

    bufD.dsc$w_length = buflen - 1;
    bufD.dsc$b_dtype = DSC$K_DTYPE_T;
    bufD.dsc$b_class = DSC$K_CLASS_S;
    bufD.dsc$a_pointer = tmp;

    s = sys$getmsg(cond, &msglen, &bufD, 15, 0);

    if (!$VMS_STATUS_SUCCESS(s)) {
	strncpy(buf, "sys$getmsg failure", buflen);
	buf[buflen - 1] = '\0';
    } else {
	strcpy(buf, tmp);
    }

    free(tmp);
    buf[msglen] = '\0';
}

static long long_converter(PyObject * o)
{
    long res;

    if (PyLong_Check(o))
	res = PyLong_AsLong(o);
    else if (PyLong_Check(o))
	res = PyLong_AsUnsignedLong(o);
    else {
	res = 0;
	PyErr_SetString(PyExc_TypeError, "an integer is required");
    }

    return res;
}

static unsigned long ulong_converter(PyObject * o)
{
    unsigned long res;

    if (PyLong_Check(o))
	res = PyLong_AsLong(o);
    else if (PyLong_Check(o))
	res = PyLong_AsUnsignedLong(o);
    else {
	res = 0;
	PyErr_SetString(PyExc_TypeError, "an integer is required");
    }

    return res;
}

static void
fill_fields(RmsFileObject * self, char *fn, int access, int share,
	    unsigned int fop)
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
    self->fab.fab$l_xab = (void *) & self->sum;

    self->fab.fab$l_naml = (void *) & self->naml;

    strncpy(self->primarySpec, fn, sizeof(self->primarySpec) - 1);
    self->primarySpec[sizeof(self->primarySpec) - 1] = '\0';

    self->naml.naml$l_long_filename = (void *) self->primarySpec;
    self->naml.naml$l_long_filename_size = strlen(self->primarySpec);

    self->naml.naml$l_esa = (void *) self->Naml_Shrt_Esa;
    self->naml.naml$b_ess = sizeof(self->Naml_Shrt_Esa) - 1;
    self->naml.naml$l_rsa = (void *) self->Naml_Shrt_Rsa;
    self->naml.naml$b_rss = sizeof(self->Naml_Shrt_Rsa) - 1;

    self->naml.naml$l_long_expand = (void *) self->Naml_Long_Esa;
    self->naml.naml$l_long_expand_alloc = sizeof(self->Naml_Long_Esa) - 1;
    self->naml.naml$l_long_result = (void *) self->Naml_Long_Rsa;
    self->naml.naml$l_long_result_alloc = sizeof(self->Naml_Long_Rsa) - 1;

    self->naml.naml$v_synchk = 0;	// Have $PARSE do directory existence check

    self->naml.naml$v_no_short_upcase = 1;	// Don't upcase short expanded spec.
}

static int open_file(RmsFileObject * self)
{
    int stat;
    int i;
    XABKEYDEF *key;
    XABALLDEF *area;
    void **xab_chain;

    stat = sys$open((void *) & self->fab);

    if (!$VMS_STATUS_SUCCESS(stat)) {
	return stat;
    }

    if (self->fab.fab$b_org != FAB$C_IDX) {
	return RMS$_ORG;
    }

    xab_chain = (void *) & self->fab.fab$l_xab;

    i = self->sum.xab$b_nok;

    self->key = key = calloc(i, sizeof(XABKEYDEF));
    assert(self->key);
    while (--i >= 0) {
	*key = cc$rms_xabkey;
	key->xab$b_ref = i;
	key->xab$l_nxt = *xab_chain;
	*xab_chain = key;
	key++;
    }

    i = self->sum.xab$b_noa;
    self->area = area = calloc(i, sizeof(XABALLDEF));
    assert(self->area);
    while (--i >= 0) {
	*area = cc$rms_xaball;
	area->xab$b_aid = i;
	area->xab$l_nxt = *xab_chain;
	*xab_chain = area;
	area++;
    }

    stat = sys$display((void *) & self->fab);

    if (!$VMS_STATUS_SUCCESS(stat)) {
	return stat;
    }

    self->naml.naml$l_esa[self->naml.naml$b_esl] = '\0';
    self->naml.naml$l_rsa[self->naml.naml$b_rsl] = '\0';
    self->naml.naml$l_long_expand[self->naml.naml$l_long_expand_size] =
	'\0';
    self->naml.naml$l_long_result[self->naml.naml$l_long_result_size] =
	'\0';

    self->flags |= PYRMS_M_OPEN;

    self->rab.rab$l_fab = (void *) & self->fab;

    /* default to primary index */
    self->rab.rab$b_krf = 0;
    self->rab.rab$l_ubf = NULL;
    self->rab.rab$w_usz = self->fab.fab$w_mrs;

    stat = sys$connect((void *) & self->rab);

    if (!$VMS_STATUS_SUCCESS(stat)) {
	return stat;
    }

    return stat;
}

static int close_file(RmsFileObject * self)
{
    int stat;

    stat = sys$disconnect((void *) & self->rab);

    if (!$VMS_STATUS_SUCCESS(stat)) {
	return stat;
    }

    stat = sys$close((void *) & self->fab);

    if (!$VMS_STATUS_SUCCESS(stat)) {
	return stat;
    }

    return stat;
}


static int flush_file(RmsFileObject * self)
{
    int stat;

    stat = sys$flush((void *) & self->rab);

    if (!$VMS_STATUS_SUCCESS(stat)) {
	return stat;
    }

    return stat;
}


static int free_file(RmsFileObject * self)
{
    int stat;

    stat = sys$free((void *) & self->rab);
    if (!$VMS_STATUS_SUCCESS(stat)) {
	return stat;
    }

    return stat;
}


static int release_file(RmsFileObject * self)
{
    int stat;

    stat = sys$release((void *) & self->rab);

    if (!$VMS_STATUS_SUCCESS(stat)) {
	return stat;
    }

    return stat;
}


static int usekey(RmsFileObject * self, int keynum)
{
    if (keynum >= self->sum.xab$b_nok) {
	return SS$_INVARG;
    }

    self->rab.rab$b_krf = keynum;
    return RMS$_NORMAL;
}


static int put(RmsFileObject * self, char *buf, int len)
{
    char *tmp;
    int stat;

    tmp = malloc(len);
    assert(tmp);
    memcpy(tmp, buf, len);
    self->rab.rab$b_rac = RAB$C_KEY;
    self->rab.rab$l_rbf = tmp;
    self->rab.rab$w_rsz = len;

    stat = sys$put((void *) & self->rab);
    free(tmp);

    if (!$VMS_STATUS_SUCCESS(stat)) {
	return stat;
    }

    return stat;
}


static int update(RmsFileObject * self, char *buf, int len)
{
    char *tmp;
    int stat;

    tmp = malloc(len);
    assert(tmp);
    memcpy(tmp, buf, len);
    self->rab.rab$l_rbf = tmp;
    self->rab.rab$w_rsz = len;

    stat = sys$update((void *) & self->rab);
    free(tmp);

    if (!$VMS_STATUS_SUCCESS(stat)) {
	return stat;
    }

    return stat;
}

static int
fetch(RmsFileObject * self, char *keyval, int len, char *buf, int *retlen)
{
    char *key = NULL;
    int stat;

    if (keyval) {
        key = malloc(len);
	assert(key);
	memcpy(key, keyval, len);
	self->rab.rab$l_kbf = key;
	self->rab.rab$b_ksz = len;
	self->rab.rab$b_rac = RAB$C_KEY;
    } else {
	self->rab.rab$b_rac = RAB$C_SEQ;
    }

    self->rab.rab$l_ubf = (void *) buf;

    stat = sys$get((void *) & self->rab);

    if (key) {
	free(key);
    }

    if (!$VMS_STATUS_SUCCESS(stat)) {
	return stat;
    }

    *retlen = self->rab.rab$w_rsz;
    return stat;
}


static int find(RmsFileObject * self, char *keyval, int len)
{
    char *tmp = NULL;
    int stat;

    if (keyval) {
        tmp = malloc(len);
	assert(tmp);
	memcpy(tmp, keyval, len);
	self->rab.rab$l_kbf = tmp;
	self->rab.rab$b_ksz = len;
	self->rab.rab$b_rac = RAB$C_KEY;
    } else {
	self->rab.rab$b_rac = RAB$C_SEQ;
    }

    stat = sys$find((void *) & self->rab);

    if (tmp) {
	free(tmp);
    }

    if (!$VMS_STATUS_SUCCESS(stat)) {
	return stat;
    }

    return stat;
}


static int delrec(RmsFileObject * self, char *keyval, int len)
{
    char *tmp = NULL;
    int stat;

    if (keyval) {
        tmp = malloc(len);
	assert(tmp);
	memcpy(tmp, keyval, len);
	self->rab.rab$l_kbf = tmp;
	self->rab.rab$b_ksz = len;
	self->rab.rab$b_rac = RAB$C_KEY;

	stat = sys$find((void *) & self->rab);

	if (!$VMS_STATUS_SUCCESS(stat)) {
	    free(tmp);
	    return stat;
	}
    } else {
	self->rab.rab$b_rac = RAB$C_SEQ;
    }

    stat = sys$delete((void *) & self->rab);

    if (tmp) {
	free(tmp);
    }

    if (!$VMS_STATUS_SUCCESS(stat)) {
	return stat;
    }

    return stat;
}

// BRC
#define PyFileObject PyObject

static PyObject *file_getiter(PyFileObject * f)
{
    Py_INCREF(f);
    return (PyObject *) f;
}

static PyObject *file_iternext(PyFileObject * self)
{
    int stat;
    int retlen = 0;
    PyObject *res;
    char buf[((RmsFileObject *) self)->fab.fab$w_mrs];

    stat = fetch((RmsFileObject *) self, NULL, 0, buf, &retlen);

    if (stat == RMS$_EOF) {
	return NULL;
    }

    if (!$VMS_STATUS_SUCCESS(stat)) {
	char msg[256];
	PyObject *v;
	getmsg(stat, msg, sizeof(msg));
	v = Py_BuildValue("(is)", stat, msg);
	if (v != NULL) {
	    PyErr_SetObject(RMSError, v);
	    Py_DECREF(v);
	}
	return NULL;
    }

    if (stat == RMS$_EOF) {
	Py_INCREF(Py_None);
	res = Py_None;
    } else {
	res = PyBytes_FromStringAndSize(buf, retlen);
    }

    return res;
}


static RmsFileObject *newRmsFileObject(char *fn, int access, int share,
				       unsigned int fop)
{
    RmsFileObject *self;
    self = PyObject_New(RmsFileObject, &RmsFile_Type);
    if (self == NULL)
	return NULL;

    fill_fields(self, fn, access, share, fop);
    return (self);
}

static void RmsFile_dealloc(RmsFileObject * self)
{
    if (self->flags & PYRMS_M_OPEN) {
	int stat;
	stat = close_file(self);
	if (!$VMS_STATUS_SUCCESS(stat)) {
	    char msg[256];
	    PyObject *v;
	    getmsg(stat, msg, sizeof(msg));
	    v = Py_BuildValue("(is)", stat, msg);
	    if (v != NULL) {
		PyErr_SetObject(RMSError, v);
		Py_DECREF(v);
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


static PyObject *rmsFile_close(RmsFileObject * self, PyObject * args)
{
    int stat;

    if (!PyArg_ParseTuple(args, ":close")) {
	return NULL;
    }

    stat = close_file(self);

    if (!$VMS_STATUS_SUCCESS(stat)) {
	char msg[256];
	PyObject *v;
	getmsg(stat, msg, sizeof(msg));
	v = Py_BuildValue("(is)", stat, msg);
	if (v != NULL) {
	    PyErr_SetObject(RMSError, v);
	    Py_DECREF(v);
	}
	return NULL;
    }

    self->flags ^= PYRMS_M_OPEN;
    return (PyLong_FromLong(stat));
}


static PyObject *rmsFile_flush(RmsFileObject * self, PyObject * args)
{
    int stat;

    if (!PyArg_ParseTuple(args, ":flush")) {
	return NULL;
    }

    stat = flush_file(self);

    if (!$VMS_STATUS_SUCCESS(stat)) {
	char msg[256];
	PyObject *v;
	getmsg(stat, msg, sizeof(msg));
	v = Py_BuildValue("(is)", stat, msg);
	if (v != NULL) {
	    PyErr_SetObject(RMSError, v);
	    Py_DECREF(v);
	}
	return NULL;
    }

    return (PyLong_FromLong(stat));
}


static PyObject *rmsFile_free(RmsFileObject * self, PyObject * args)
{
    int stat;

    if (!PyArg_ParseTuple(args, ":free")) {
	return NULL;
    }

    stat = free_file(self);

    if (!$VMS_STATUS_SUCCESS(stat)) {
	char msg[256];
	PyObject *v;
	getmsg(stat, msg, sizeof(msg));
	v = Py_BuildValue("(is)", stat, msg);
	if (v != NULL) {
	    PyErr_SetObject(RMSError, v);
	    Py_DECREF(v);
	}
	return NULL;
    }

    return (PyLong_FromLong(stat));
}



static PyObject *rmsFile_release(RmsFileObject * self, PyObject * args)
{
    int stat;

    if (!PyArg_ParseTuple(args, ":release")) {
	return NULL;
    }

    stat = release_file(self);

    if (!$VMS_STATUS_SUCCESS(stat)) {
	char msg[256];
	PyObject *v;
	getmsg(stat, msg, sizeof(msg));
	v = Py_BuildValue("(is)", stat, msg);
	if (v != NULL) {
	    PyErr_SetObject(RMSError, v);
	    Py_DECREF(v);
	}
	return NULL;
    }

    return (PyLong_FromLong(stat));
}


static PyObject *rmsFile_rewind(RmsFileObject * self, PyObject * args,
				PyObject * kwargs)
{
    int stat;
    int keynum = -1;
    char *kwnames[] = {
	"keynum", NULL
    };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, (char *) "|i:rewind",
				     kwnames, &keynum))
	return NULL;


    if (keynum >= 0) {
	stat = usekey((RmsFileObject *) self, keynum);
	if (!$VMS_STATUS_SUCCESS(stat)) {
	    char msg[256];
	    PyObject *v;
	    getmsg(stat, msg, sizeof(msg));
	    v = Py_BuildValue("(is)", stat, msg);
	    if (v != NULL) {
		PyErr_SetObject(RMSError, v);
		Py_DECREF(v);
	    }
	    return NULL;
	}
    }
    stat = sys$rewind((void *) & self->rab, 0, 0);
    if (!$VMS_STATUS_SUCCESS(stat)) {
	char msg[256];
	PyObject *v;
	getmsg(stat, msg, sizeof(msg));
	v = Py_BuildValue("(is)", stat, msg);
	if (v != NULL) {
	    PyErr_SetObject(RMSError, v);
	    Py_DECREF(v);
	}
	return NULL;
    }

    return PyLong_FromLong(stat);
}

static PyObject *rmsFile_usekey(RmsFileObject * self, PyObject * args,
				PyObject * kwargs)
{
    int stat;
    int keynum = 0;
    PyObject *res;
    char *kwnames[] = {
	"keynum", NULL
    };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, (char *) "|i:usekey",
				     kwnames, &keynum))
	return NULL;

    stat = usekey((RmsFileObject *) self, keynum);
    if (!$VMS_STATUS_SUCCESS(stat)) {
	char msg[256];
	PyObject *v;
	getmsg(stat, msg, sizeof(msg));
	v = Py_BuildValue("(is)", stat, msg);
	if (v != NULL) {
	    PyErr_SetObject(RMSError, v);
	    Py_DECREF(v);
	}
	return NULL;
    }

    return PyLong_FromLong(stat);
}

static PyObject *rmsFile_put(RmsFileObject * self, PyObject * args,
			     PyObject * kwargs)
{
    int stat;
    char *buf = NULL;
    int len = 0;
    PyObject *res;
    char *kwnames[] = {
	"record", NULL
    };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, (char *) "s#:put",
				     kwnames, &buf, &len))
	return NULL;

    stat = put((RmsFileObject *) self, buf, len);
    if (!$VMS_STATUS_SUCCESS(stat)) {
	char msg[256];
	PyObject *v;
	getmsg(stat, msg, sizeof(msg));
	v = Py_BuildValue("(is)", stat, msg);
	if (v != NULL) {
	    PyErr_SetObject(RMSError, v);
	    Py_DECREF(v);
	}
	return NULL;
    }

    return PyLong_FromLong(stat);
}

static PyObject *rmsFile_update(RmsFileObject * self, PyObject * args,
				PyObject * kwargs)
{
    int stat;
    char *buf = NULL;
    int len = 0;
    PyObject *res;
    char *kwnames[] = {
	"record", NULL
    };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, (char *) "s#:update",
				     kwnames, &buf, &len))
	return NULL;

    stat = update((RmsFileObject *) self, buf, len);
    if (!$VMS_STATUS_SUCCESS(stat)) {
	char msg[256];
	PyObject *v;
	getmsg(stat, msg, sizeof(msg));
	v = Py_BuildValue("(is)", stat, msg);
	if (v != NULL) {
	    PyErr_SetObject(RMSError, v);
	    Py_DECREF(v);
	}
	return NULL;
    }

    return PyLong_FromLong(stat);
}

static PyObject *rmsFile_fetch(RmsFileObject * self, PyObject * args,
			       PyObject * kwargs)
{
    int stat;
    char *keyval = NULL;
    int len = 0;
    PyObject *res;
    char *kwnames[] = {
	"key", NULL
    };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, (char *) "|s#:fetch",
				     kwnames, &keyval, &len))
	return NULL;

    char buf[((RmsFileObject *) self)->fab.fab$w_mrs];
    int retlen = 0;

    stat = fetch((RmsFileObject *) self, keyval, len, buf, &retlen);
    if (stat != RMS$_EOF && stat != RMS$_RNF && !$VMS_STATUS_SUCCESS(stat)) {
	char msg[256];
	PyObject *v;
	getmsg(stat, msg, sizeof(msg));
	v = Py_BuildValue("(is)", stat, msg);
	if (v != NULL) {
	    PyErr_SetObject(RMSError, v);
	    Py_DECREF(v);
	}
	return NULL;
    }

    {
	PyObject *o1 = PyLong_FromLong(stat);
	PyObject *o2;
	if (stat == RMS$_EOF || stat == RMS$_RNF) {
	    Py_INCREF(Py_None);
	    o2 = Py_None;
	} else {
	    o2 = PyBytes_FromStringAndSize(buf, retlen);
	}
	res = PyTuple_New(2);
	PyTuple_SetItem(res, 0, o1);
	PyTuple_SetItem(res, 1, o2);
    }
    return res;
}

static PyObject *rmsFile_find(RmsFileObject * self, PyObject * args,
			      PyObject * kwargs)
{
    int stat;
    char *keyval = NULL;
    int len = 0;
    char *kwnames[] = {
	"key", NULL
    };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, (char *) "|s#:find",
				     kwnames, &keyval, &len))
	return NULL;

    stat = find((RmsFileObject *) self, keyval, len);
    if (stat != RMS$_EOF && !$VMS_STATUS_SUCCESS(stat)) {
	char msg[256];
	PyObject *v;
	getmsg(stat, msg, sizeof(msg));
	v = Py_BuildValue("(is)", stat, msg);
	if (v != NULL) {
	    PyErr_SetObject(RMSError, v);
	    Py_DECREF(v);
	}
	return NULL;
    }

    return PyLong_FromLong(stat);
}

static PyObject *rmsFile_delete(RmsFileObject * self, PyObject * args,
				PyObject * kwargs)
{
    int stat;
    char *keyval = NULL;
    int len = 0;
    char *kwnames[] = {
	"key", NULL
    };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, (char *) "|s#:delete",
				     kwnames, &keyval, &len))
	return NULL;

    stat = delrec((RmsFileObject *) self, keyval, len);
    if (stat != RMS$_EOF && !$VMS_STATUS_SUCCESS(stat)) {
	char msg[256];
	PyObject *v;
	getmsg(stat, msg, sizeof(msg));
	v = Py_BuildValue("(is)", stat, msg);
	if (v != NULL) {
	    PyErr_SetObject(RMSError, v);
	    Py_DECREF(v);
	}
	return NULL;
    }

    return PyLong_FromLong(stat);
}

static PyMethodDef RmsFile_methods[] = {
    {"close", (PyCFunction) rmsFile_close, METH_VARARGS,
     PyDoc_STR("close() -> status")},
    {"delete", (PyCFunction) rmsFile_delete, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("delete([key]) -> status")},
    {"fetch", (PyCFunction) rmsFile_fetch, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("get([key]) -> status, record")},
    {"find", (PyCFunction) rmsFile_find, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("find([key]) -> status")},
    {"flush", (PyCFunction) rmsFile_flush, METH_VARARGS,
     PyDoc_STR("flush() -> status")},
    {"free", (PyCFunction) rmsFile_free, METH_VARARGS,
     PyDoc_STR("free() -> status")},
    {"put", (PyCFunction) rmsFile_put, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("put(record) -> status")},
    {"release", (PyCFunction) rmsFile_release, METH_VARARGS,
     PyDoc_STR("release() -> status")},
    {"rewind", (PyCFunction) rmsFile_rewind, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("rewind([keynum]) -> status")},
    {"update", (PyCFunction) rmsFile_update, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("update(record) -> status")},
    {"usekey", (PyCFunction) rmsFile_usekey, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("use_key([keynum]) -> status")},
    {NULL, NULL}		/* sentinel */

};

#define OFF(x) offsetof(RmsFileObject, x)

static PyMemberDef RmsFile_memberlist[] = {
    {"longname", T_STRING_INPLACE, OFF(Naml_Long_Rsa), READONLY,
     "long filename"},
    {"nok", T_UBYTE, OFF(sum.xab$b_nok), READONLY,
     "number of keys"},
    {"org", T_UBYTE, OFF(fab.fab$b_org), READONLY,
     "file organization"},
    {NULL}			/* Sentinel */

};

PyTypeObject RmsFile_Type = {
    PyObject_HEAD_INIT(NULL)
	"rmsFile",		/*tp_name */
    sizeof(RmsFileObject),	/*tp_basicsize */
    0,				/*tp_itemsize */
    /* methods */
    (destructor) RmsFile_dealloc,	/*tp_dealloc */
    0,				/*tp_print */
    0,				/*tp_getattr */
    0,				/*tp_setattr */
    NULL,			/*formerly known as tp_compare or tp_reserved */
    0,				/*tp_repr */
    0,				/*tp_as_number */
    0,				/*tp_as_sequence */
    0,				/*tp_as_mapping */
    0,				/*tp_hash */
    0,				/*tp_call */
    0,				/*tp_str */
    PyObject_GenericGetAttr,	/*tp_getattro */
    0,				/*tp_setattro */
    0,				/*tp_as_buffer */
    Py_TPFLAGS_DEFAULT,		/*tp_flags */
    0,				/*tp_doc */
    0,				/*tp_traverse */
    0,				/*tp_clear */
    0,				/*tp_richcompare */
    0,				/*tp_weaklistoffset */
    (getiterfunc) file_getiter,	/*tp_iter */
    (iternextfunc) file_iternext,	/*tp_iternext */
    RmsFile_methods,		/*tp_methods */
    RmsFile_memberlist,		/* tp_members */
    0,				/*tp_getset */
    0,				/*tp_base */
    0,				/*tp_dict */
    0,				/*tp_descr_get */
    0,				/*tp_descr_set */
    0,				/*tp_dictoffset */
    0,				/*tp_init */
    0,				/*tp_alloc */
    0,				/*tp_new */
    0,				/*tp_free */
    0,				/*tp_is_gc */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    NULL
};

static PyObject *rmsFile_new(PyObject * self, PyObject * args,
			     PyObject * kwargs)
{
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
    PyObject *res;
#if 0
    char *kwnames[] = {
	"name", "alq", "bls", "deq", "fac", "fop", "gbc",
	"mbc", "mbf", "rat", "rfm", "rop", "rtv", "shr", NULL
    };
#else
    char *kwnames[] = {
	"name", "fac", "shr", "fop", NULL
    };
#endif

    if (!PyArg_ParseTupleAndKeywords
	(args, kwargs, (char *) "s|iiI:open_file", kwnames, &name, &fac,
	 &shr, &fop))
	return NULL;

    res = (PyObject *) newRmsFileObject(name, fac, shr, fop);

    if (res) {
	int stat;
	stat = open_file((RmsFileObject *) res);
	if (!$VMS_STATUS_SUCCESS(stat)) {
	    char msg[256];
	    PyObject *v;
	    getmsg(stat, msg, sizeof(msg));
	    v = Py_BuildValue("(is)", stat, msg);
	    if (v != NULL) {
		PyErr_SetObject(RMSError, v);
		Py_DECREF(v);
	    }
	    Py_DECREF(res);
	    return NULL;
	}
    }

    return res;
}

static PyObject *rms_bcd2string(PyObject * dummy, PyObject * args)
{
    PyObject *result = NULL;
    char *temp;
    char *str;
    int len;
    int prec;
    int i, j, k;
    char c;

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
    if ((temp[len - 1] & 0x0F) == 0x0C)
	str[0] = '+';
    else
	str[0] = '-';
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

    result = PyBytes_FromStringAndSize(str, 2 * len + 1);
    PyMem_Free(str);
    return result;
}

static PyObject *rms_string2bcd(PyObject * dummy, PyObject * args)
{
    PyObject *result = NULL;
    char *temp;
    int len;
    int prec;
    int n;
    char *p = NULL;
    int i;

    if (!PyArg_ParseTuple(args, "sii", &temp, &n, &prec)) {
	return NULL;
    }

    if ((n + prec + 1) & 1)
	++n;
    len = strlen(temp);
    if ((n + prec > 31) || (len > 33)) {
	PyErr_SetString(PyExc_TypeError, "Invalid len");
	return NULL;
    }
    if (strchr("+-0123456789", *temp) == NULL) {
	PyErr_SetString(PyExc_TypeError, "Invalid first character");
	return NULL;
    }

    for (i = 1; i < len; ++i) {
	if ((temp[i] == '.') && (p == NULL))
	    p = &(temp[i]) + 1;
	else if (!isdigit(temp[i])) {
	    PyErr_SetString(PyExc_TypeError, "Invalid numeric");
	    return NULL;
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
	if (p)
	    sscanf(p, "%lld", &id);
	if (in < 0) {
	    in = -in;
	    sign = 0x0D;
	}
	sprintf(fmt, "%%0%dlld%%lld", n);
	sprintf(astr, fmt, in, id);
	alen = strlen(astr);
	if (alen > n + prec)
	    astr[n + prec] = '\0';
	else if (alen < n + prec) {
	    for (i = n + prec - 1; i >= alen; --i)
		astr[i] = '0';
	    astr[n + prec] = '\0';
	}
	for (i = 0; i < (n + prec - 1) / 2; ++i) {
	    rstr[i] = (astr[2 * i] - '0') * 16 + (astr[2 * i + 1] - '0');
	}
	rstr[(n + prec - 1) / 2] = (astr[n + prec - 1] - '0') * 16 + sign;
	result = PyBytes_FromStringAndSize(rstr, (n + prec + 1) / 2);
	return result;
    }
}

static PyObject *rms_bcd2tuple(PyObject * dummy, PyObject * args)
{
    PyObject *result = NULL;
    char *temp;
    int len;
    int prec;
    int i, j, k;
    char c;
    int sign;
    PyObject *pdigit = NULL;

    if (!PyArg_ParseTuple(args, "s#i", &temp, &len, &prec)) {
	return NULL;
    }
    c = (temp[len - 1] & 0x0F);
    if ((c != 0x0C) && (c != 0x0D)) {
	PyErr_SetString(PyExc_TypeError, "Invalid sign");
	return NULL;
    }
    result = PyTuple_New(3);
    pdigit = PyTuple_New(2 * len - 1);
    if ((temp[len - 1] & 0x0F) == 0x0C)
	sign = 0;
    else
	sign = 1;
    PyTuple_SetItem(result, 0, (PyObject *) PyLong_FromLong(sign));
    PyTuple_SetItem(result, 1, pdigit);
    PyTuple_SetItem(result, 2, (PyObject *) PyLong_FromLong(-prec));
    for (j = 0, i = 0; i < (2 * len - 1); ++i) {
	int v;
	if (i & 1) {
	    v = (temp[j] & 0x0F);
	    ++j;
	} else {
	    v = ((temp[j] & 0xF0) >> 4);
	}
	PyTuple_SetItem(pdigit, i, (PyObject *) PyLong_FromLong(v));
    }
    return result;
}

static PyObject *rms_tuple2bcd(PyObject * dummy, PyObject * args)
{
    PyObject *result = NULL;
    PyObject *lst;
    char *temp;
    int sign;
    int len;
    int dprec;
    int prec;
    int n;

    if (!PyArg_ParseTuple(args, "(iO!i)ii", &sign, &PyTuple_Type, &lst,
			  &dprec, &n, &prec)) {
	return NULL;
    }
    len = PyTuple_Size(lst);
    if (len > 31) {
	PyErr_SetString(PyExc_TypeError, "Invalid digits tuple length");
	return NULL;
    }

    if (((n + prec) & 1) == 0)
	++n;
    if (n + prec > 31) {
	PyErr_SetString(PyExc_TypeError, "Invalid BCD length");
	return NULL;
    }

    dprec = -dprec;
    if (len - dprec > n) {
	PyErr_SetString(PyExc_TypeError, "BCD integer part too short");
	return NULL;
    }

    {
	char rstr[16];
	char digit[n + prec + 1];
	int i, j, k;
	for (i = 0; i < n - len + dprec; ++i)
	    digit[i] = 0;
	for (j = 0; j < len - dprec; ++j, ++i)
	    digit[i] = PyLong_AsLong(PyTuple_GetItem(lst, j));
	if (dprec > prec)
	    dprec = prec;
	for (; i < n + dprec; ++i, ++j)
	    digit[i] = PyLong_AsLong(PyTuple_GetItem(lst, j));
	for (; i < n + prec; ++i)
	    digit[i] = 0;
	digit[n + prec] = 0;

	for (i = 0; i < (n + prec - 1) / 2; ++i) {
	    rstr[i] = (digit[2 * i]) * 16 + (digit[2 * i + 1]);
	}
	if (sign == 0)
	    sign = 0x0C;
	else
	    sign = 0x0D;
	rstr[(n + prec - 1) / 2] = (digit[n + prec - 1]) * 16 + sign;
	result = PyBytes_FromStringAndSize(rstr, (n + prec + 1) / 2);
	return result;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static int getrmsattr(char *name, int attr, int *result)
{
    int stat;
    unsigned int fop = 0;
    int fac = 0;
    int shr = FAB$M_SHRGET;
    RmsFileObject *fo;
    XABFHCDEF *fhc;

    fo = malloc(sizeof(RmsFileObject));
    assert(fo);
    fill_fields(fo, name, fac, shr, fop);

    if (attr == RMSATTR_K_LRL) {
        fhc = malloc(sizeof(XABFHCDEF));
	assert(fhc);
	*fhc = cc$rms_xabfhc;
	fo->fab.fab$l_xab = fhc;
    }

    stat = sys$open((void *) & fo->fab);
    if (!$VMS_STATUS_SUCCESS(stat)) {
	if (attr == RMSATTR_K_LRL) {
	    free(fhc);
	}
	free(fo);
	return stat;
    }

    stat = sys$display((void *) & fo->fab);
    if (!$VMS_STATUS_SUCCESS(stat)) {
	free(fo);
	return stat;
    }

    *result = 0;
    switch (attr) {
    case RMSATTR_K_ORG:
	*result = fo->fab.fab$b_org;
	break;
    case RMSATTR_K_RFM:
	*result = fo->fab.fab$b_rfm;
	break;
    case RMSATTR_K_MRS:
	*result = fo->fab.fab$w_mrs;
	break;
    case RMSATTR_K_RAT:
	*result = fo->fab.fab$b_rat;
	break;
    case RMSATTR_K_LRL:
	*result = fhc->xab$w_lrl;
	break;
    }

    stat = sys$close((void *) & fo->fab);

    if (attr == RMSATTR_K_LRL) {
	free(fhc);
    }
    free(fo);
    return stat;
}

static PyObject *rms_getrmsattr(PyObject * dummy, PyObject * args)
{
    char *name;
    int attr;
    int result;

    if (!PyArg_ParseTuple(args, "si:getrmsattr", &name, &attr))
	return NULL;

    if (attr < 0 || attr > RMSATTR_K_LAST) {
	PyErr_Format(RMSError, "invalid attr value: %d", attr);
	return NULL;
    }

    int stat;
    stat = getrmsattr(name, attr, &result);
    if (!$VMS_STATUS_SUCCESS(stat)) {
	char msg[256];
	PyObject *v;
	getmsg(stat, msg, sizeof(msg));
	v = Py_BuildValue("(is)", stat, msg);
	if (v != NULL) {
	    PyErr_SetObject(RMSError, v);
	    Py_DECREF(v);
	}
	return NULL;
    }

    return PyLong_FromLong((long) result);
}

static int
parse(char *path, char *node, char *dev, char *dir, char *name, char *type,
      char *ver)
{
    int stat;
    unsigned int fop = NAM$M_SYNCHK;
    int fac = 0;
    int shr = 0;
    RmsFileObject *fo;

    fo = PyMem_NEW(RmsFileObject, 1);
    fill_fields(fo, path, fac, shr, fop);

    stat = sys$parse((void *) & fo->fab);
    if (!$VMS_STATUS_SUCCESS(stat)) {
	PyMem_FREE(fo);
	return stat;
    }

    fo->naml.naml$l_esa[fo->naml.naml$b_esl] = '\0';
    fo->naml.naml$l_long_expand[fo->naml.naml$l_long_expand_size] = '\0';

    int p1, p2, p3, p4, p5, p6;
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
    return stat;
}

static PyObject *rms_parse(PyObject * dummy, PyObject * args)
{
    char *path;
    char node[NAML$C_MAXRSS + 1];
    char dev[NAML$C_MAXRSS + 1];
    char dir[NAML$C_MAXRSS + 1];
    char name[NAML$C_MAXRSS + 1];
    char type[NAML$C_MAXRSS + 1];
    char ver[NAML$C_MAXRSS + 1];
    PyObject *res;

    if (!PyArg_ParseTuple(args, "s:parse", &path))
	return NULL;

    int stat;
    stat = parse(path, node, dev, dir, name, type, ver);
    if (!$VMS_STATUS_SUCCESS(stat)) {
	char msg[256];
	PyObject *v;
	getmsg(stat, msg, sizeof(msg));
	v = Py_BuildValue("(is)", stat, msg);
	if (v != NULL) {
	    PyErr_SetObject(RMSError, v);
	    Py_DECREF(v);
	}
	return NULL;
    }
    res = PyTuple_New(6);
    PyTuple_SetItem(res, 0, PyBytes_FromString(node));
    PyTuple_SetItem(res, 1, PyBytes_FromString(dev));
    PyTuple_SetItem(res, 2, PyBytes_FromString(dir));
    PyTuple_SetItem(res, 3, PyBytes_FromString(name));
    PyTuple_SetItem(res, 4, PyBytes_FromString(type));
    PyTuple_SetItem(res, 5, PyBytes_FromString(ver));
    return res;
}


/* List of functions defined in the module */

static PyMethodDef _rmsFile_methods[] = {
    {"file", (PyCFunction) rmsFile_new, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("file(name [,fac] [,shr], [fop]) -> new RMS file object")},
    {"BCD2String", (PyCFunction) rms_bcd2string, METH_VARARGS,
     PyDoc_STR("BCD2String(s) -> string")},
    {"String2BCD", (PyCFunction) rms_string2bcd, METH_VARARGS,
     PyDoc_STR("String2BCD(v) -> BCD string")},
    {"BCD2Tuple", (PyCFunction) rms_bcd2tuple, METH_VARARGS,
     PyDoc_STR("BCD2Tuple(s) -> tuple")},
    {"Tuple2BCD", (PyCFunction) rms_tuple2bcd, METH_VARARGS,
     PyDoc_STR("Tuple2BCD(v) -> BCD string")},
    {"getrmsattr", (PyCFunction) rms_getrmsattr, METH_VARARGS,
     PyDoc_STR("getrmsattr(path, attr) -> value")},
    {"parse", (PyCFunction) rms_parse, METH_VARARGS,
     PyDoc_STR("parse(path) -> (node, dev, dir, name, ext, ver)")},
    {NULL, NULL}		/* sentinel */

};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_rms",			/* m_name */

    "The OpenVMS RMS module",	/* m_doc */

    -1,				/* m_size */

    _rmsFile_methods,		/* m_methods */

    NULL,			/* m_reload */

    NULL,			/* m_traverse */

    NULL,			/* m_clear */

    NULL,			/* m_free */

};


PyMODINIT_FUNC PyInit__rms(void)
{
    PyObject *m;
    PyObject *d;
    PyObject *dict = PyDict_New();
    PyObject *str = NULL;

// BRC
//    RmsFile_Type.ob_type = &PyType_Type;

    m = PyModule_Create(&moduledef);
    d = PyModule_GetDict(m);

    insint(d, "RMSATTR_K_ORG", RMSATTR_K_ORG);
    insint(d, "RMSATTR_K_RFM", RMSATTR_K_RFM);
    insint(d, "RMSATTR_K_RAT", RMSATTR_K_RAT);
    insint(d, "RMSATTR_K_MRS", RMSATTR_K_MRS);
    insint(d, "RMSATTR_K_LRL", RMSATTR_K_LRL);
    insint(d, "RMSATTR_K_LAST", RMSATTR_K_LAST);
    insint(d, "PARSE_K_NODE", PARSE_K_NODE);
    insint(d, "PARSE_K_DEV", PARSE_K_DEV);
    insint(d, "PARSE_K_DIR", PARSE_K_DIR);
    insint(d, "PARSE_K_NAME", PARSE_K_NAME);
    insint(d, "PARSE_K_TYPE", PARSE_K_TYPE);
    insint(d, "PARSE_K_VER", PARSE_K_VER);
    str = PyBytes_FromString(RMSError__doc__);
    PyDict_SetItemString(dict, "__doc__", str);
    if (RMSError == NULL) {
	if ((RMSError =
	     PyErr_NewException("vms.rms.error", PyExc_IOError,
				dict)) == NULL) {
	    return (NULL);
	}
    }
    Py_XDECREF(dict);
    Py_XDECREF(str);
    Py_INCREF(RMSError);
    PyModule_AddObject(m, "error", RMSError);

    return (m);
}
