#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "stdio.h"
#include "structmember.h"
#include "iosbdef.h"
#include "descrip.h"
#include "starlet.h"
#include "stsdef.h"
#include "unixio.h"
#include "unixlib.h"
#include "lib$routines.h"
#include "iodef.h"
#include "gen64def.h"

#include "Modules/_io/_iomodule.h"

typedef struct {
    int                         fd;
    int                         efn;
    unsigned short int          channel;
    struct _iosb                iosb;
    PyObject                   *source;
    char                       *buf;
} FileWrap;

typedef struct {
    PyObject_HEAD
    FileWrap   *wrappers;
    int         size;
    int         num;
    int         bufSize;
} SelectorObject;

static PyMemberDef Selector_members[] = {
    {"num", T_INT, offsetof(SelectorObject, num), 0,
     "number of streams"},
    {NULL}  /* Sentinel */
};

static void
_clean_warpper(FileWrap *wrapper) {
    wrapper->fd = -1;
    SYS$DASSGN(wrapper->channel);
    wrapper->channel = 0;
    LIB$FREE_EF(&wrapper->efn);
    wrapper->efn = 0;
    PyMem_FREE(wrapper->buf);
    wrapper->buf = NULL;
    Py_DECREF(wrapper->source);
    wrapper->source = NULL;
}

static int
_fileno_from_source(PyObject *source) {
    int fd_int = -1;
    if (PyLong_CheckExact(source)) {
        fd_int = PyLong_AsLong(source);
    } else if (PyObject_IsInstance(source, (PyObject *)&PyIOBase_Type)) {
        PyObject* result = PyObject_CallMethod(source, "fileno", NULL);
        if (result != NULL) {
            fd_int = PyLong_AsLong(result);
            Py_DECREF(result);
        }
    }
    return fd_int;
}

static FileWrap*
_find_wrapper(SelectorObject *self, PyObject *source) {
    int num = self->num;
    while(num--) {
        if (self->wrappers[num].source == source) {
            return &self->wrappers[num];
        }
    }
    return NULL;
}

static void
Selector_dealloc(SelectorObject *self)
{
    while(self->num--) {
        _clean_warpper(&self->wrappers[self->num]);
    }
    PyMem_FREE(self->wrappers);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *
Selector_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    SelectorObject *self;
    self = (SelectorObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->num = 0;
        self->size = 0;
        self->wrappers = NULL;
        self->bufSize = decc$feature_get("DECC$PIPE_BUFFER_SIZE", __FEATURE_MODE_CURVAL);
    }
    return (PyObject *) self;
}

static PyObject *
Selector_register(SelectorObject *self, PyObject *source)
{
    int fd_int = _fileno_from_source(source);
    if (fd_int == -1) {
        PyErr_SetString(PyExc_TypeError,
            "Source has invalid fileno");
        Py_RETURN_FALSE;
    }
    // find place
    int new_pos = self->num;
    // allocate place
    if (self->num + 1 >= self->size) {
        int new_size = 4;
        if (self->size > 0) {
            if (self->size < 16) {
                new_size = self->size * 2;
            } else {
                new_size = self->size + 16;
            }
        }
        FileWrap *new_wrappers = self->wrappers;
        PyMem_RESIZE(new_wrappers, FileWrap, new_size);
        if (new_wrappers == NULL) {
            PyErr_NoMemory();
            Py_RETURN_FALSE;
        }
        // init new wrappers
        self->wrappers = new_wrappers;
        self->size = new_size;
    }

    // get name
    char *name = PyMem_MALLOC(PATH_MAX + 1);
    if (name == NULL) {
        PyErr_NoMemory();
        Py_RETURN_FALSE;
    }
    name[0] = 0;
    if (!getname(fd_int, name, 1)) {
        PyErr_Format(PyExc_OSError,
            "Cannot get name for fd: %d", fd_int);
        PyMem_FREE(name);
        Py_RETURN_FALSE;
    }
    // get channel
    struct  dsc$descriptor_s descriptor;
    descriptor.dsc$w_length = strlen(name);
    descriptor.dsc$b_dtype = DSC$K_DTYPE_T;
    descriptor.dsc$b_class = DSC$K_CLASS_S;
    descriptor.dsc$a_pointer = name;
    int channel = 0;
    int status = SYS$ASSIGN(&descriptor,   /* devnam - device name */ 
                        &channel,      /* chan - channel number */ 
                        0,             /* acmode - access mode */ 
                        0,             /* mbxnam - logical name for mailbox */ 
                        0 );           /* flags */ 
    PyMem_FREE(name);
    if (!$VMS_STATUS_SUCCESS(status)) {
        PyErr_Format(PyExc_OSError,
            "Cannot assign channel: %d", status);
        Py_RETURN_FALSE;
    }
    // get EFN
    int efn;
    status = LIB$GET_EF(&efn);
    if (!$VMS_STATUS_SUCCESS(status)) {
        SYS$DASSGN(channel);
        PyErr_Format(PyExc_OSError,
            "Cannot get EF: %d", status);
        Py_RETURN_FALSE;
    }
    // save
    self->wrappers[new_pos].fd = fd_int;
    self->wrappers[new_pos].channel = channel;
    self->wrappers[new_pos].efn = efn;
    self->wrappers[new_pos].source = source;
    self->wrappers[new_pos].buf = NULL;
    Py_INCREF(source);
    ++self->num;
    Py_RETURN_TRUE;
}

static PyObject *
Selector_unregister(SelectorObject *self, PyObject *source)
{
    int num = self->num;
    while(num--) {
        if (self->wrappers[num].source == source) {
            _clean_warpper(&self->wrappers[num]);
            --self->num;
            int tail = self->num - num;
            if (tail > 0) {
                memmove(self->wrappers + num, self->wrappers + num + 1, tail * sizeof(FileWrap));
            }
            Py_RETURN_TRUE;
        }
    }
    Py_RETURN_FALSE;
}

static PyObject *
Selector_query_write(SelectorObject *self, PyObject *args)
{
    PyObject *source;
    Py_buffer data;
    if (!PyArg_ParseTuple(args, "Oy*", &source, &data)) {
        Py_RETURN_FALSE;
    }
    if (data.ndim > 1) {
        PyErr_SetString(PyExc_BufferError,
                        "Buffer must be single dimension");
        PyBuffer_Release(&data);
        Py_RETURN_FALSE;
    }
    FileWrap *wrapper = _find_wrapper(self, source);
    if (!wrapper) {
        PyBuffer_Release(&data);
        PyErr_SetString(PyExc_KeyError,
            "Unregistered source");
        Py_RETURN_FALSE;
    }
    if (!wrapper->buf) {
        wrapper->buf = PyMem_MALLOC(self->bufSize);
    }
    int len = self->bufSize;
    if (len > data.len) {
        len = data.len;
    }
    memmove(wrapper->buf, data.buf, len);
    PyBuffer_Release(&data);
    int status = SYS$QIO(
        wrapper->efn,       /* efn - event flag */ 
        wrapper->channel,   /* chan - channel number */ 
        IO$_WRITEVBLK,      /* func - function modifier */ 
        &wrapper->iosb,     /* iosb - I/O status block */ 
        0,                  /* astadr - AST routine */ 
        0,                  /* astprm - AST parameter */ 
        wrapper->buf,       /* p1 - output buffer */ 
        len,                /* p2 - length of data */
        0,0,0,0);           /* p3-p6 are required*/
    if (!$VMS_STATUS_SUCCESS(status)) {
        PyErr_Format(PyExc_OSError,
            "SYS$QIO failed: %d", status);
        Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

static PyObject *
Selector_query_read(SelectorObject *self, PyObject *source)
{
    FileWrap *wrapper = _find_wrapper(self, source);
    if (!wrapper) {
        PyErr_SetString(PyExc_KeyError,
            "Unregistered source");
        Py_RETURN_FALSE;
    }
    if (!wrapper->buf) {
        wrapper->buf = PyMem_MALLOC(self->bufSize);
    }
    int status = SYS$QIO(
        wrapper->efn,               /* efn - event flag */ 
        wrapper->channel,           /* chan - channel number */ 
        IO$_READVBLK | IO$M_STREAM, /* func - function modifier */ 
        &wrapper->iosb,             /* iosb - I/O status block */ 
        0,                          /* astadr - AST routine */ 
        0,                          /* astprm - AST parameter */ 
        wrapper->buf,               /* p1 - input buffer */ 
        self->bufSize,              /* p2 - size of buffer */
        0,0,0,0);                   /* p3-p6 are required*/
    if (!$VMS_STATUS_SUCCESS(status)) {
        PyErr_Format(PyExc_OSError,
            "SYS$QIO failed: %d", status);
        Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

static PyObject *
Selector_wait_io(SelectorObject *self, PyObject *timeout)
{
    PyObject* result = NULL;
    if (self->num == 0) {
        goto egress;
    }
    int timerEfn = 0;
    struct _generic_64 timerOffset;
    int status;
    if (timeout != Py_None) {
        int min, sec, msec;
        if (PyFloat_Check(timeout)) {
            double dtimeout = PyFloat_AsDouble(timeout);
            msec = ((int)(dtimeout * 1000)) % 1000;
            sec = (int)dtimeout;
        } else if (PyLong_Check(timeout)) {
            msec = 0;
            sec = (int) PyLong_AS_LONG(timeout);
        } else {
            PyErr_Format(PyExc_TypeError,
                "wait_io requires float or int");
            goto egress;
        }
        min = sec / 60;
        sec = sec % 60;
        if (min >= 60) {
            min = 60;
        }
        char timeStr[32];
        snprintf(timeStr, sizeof(timeStr), "0 00:%d:%d.%d", min, sec, msec);
        struct dsc$descriptor_s descriptor;
        descriptor.dsc$w_length = strlen(timeStr);
        descriptor.dsc$b_dtype = DSC$K_DTYPE_T;
        descriptor.dsc$b_class = DSC$K_CLASS_S;
        descriptor.dsc$a_pointer = timeStr;
        status = SYS$BINTIM(&descriptor, &timerOffset);
        if (!$VMS_STATUS_SUCCESS(status)) {
            PyErr_Format(PyExc_OSError,
                "SYS$BINTIM failed: %d ", status);
            goto egress;
        }
        status = LIB$GET_EF(&timerEfn);
        if (!$VMS_STATUS_SUCCESS(status)) {
            PyErr_Format(PyExc_OSError,
                "LIB$GET_EF failed: %d ", status);
            goto egress;
        }
    }

    int claster = self->wrappers[0].efn & ~0x1f;
    int mask = 1 << (self->wrappers[0].efn & 0x1f);
    for (int i = 1; i < self->num; ++i) {
        if (claster != self->wrappers[i].efn & ~0x1f) {
            PyErr_SetString(PyExc_OSError,
                "EF in different clusters");
            goto egress_timer;
        }
        mask |= 1 << (self->wrappers[i].efn & 0x1f);
    }
    if (timerEfn) {
        if (claster != timerEfn & ~0x1f) {
            PyErr_SetString(PyExc_OSError,
                "EF in different clusters");
            goto egress_timer;
        }
        mask |= 1 << (timerEfn & 0x1f);
        status = SYS$SETIMR(timerEfn, &timerOffset, 0, 0, 0);
        if (!$VMS_STATUS_SUCCESS(status)) {
            PyErr_Format(PyExc_OSError,
                "SYS$SETIMR failed: %d ", status);
            goto egress_timer;
        }
    }

    status = SYS$WFLOR(self->wrappers[0].efn, mask);
    if (!$VMS_STATUS_SUCCESS(status)) {
        PyErr_Format(PyExc_OSError,
            "SYS$WFLOR failed: %d", status);
        goto egress_timer;
    }

    int efnCluster;
    int ready_num = 0;
    PyObject **ready_obj = PyMem_MALLOC(sizeof(PyObject*) * self->num);
    for (int i = 0; i < self->num; ++i) {
        status = SYS$READEF(self->wrappers[i].efn, &efnCluster);
        if (status == SS$_WASSET && self->wrappers[i].iosb.iosb$w_status) {
            // io done, add to ready
            ready_obj[ready_num++] = self->wrappers[i].source;
        }
    }

    if (ready_num) {
        result = PyTuple_New(ready_num);
        if (result == NULL) {
            PyErr_NoMemory();
            goto egress_ready_obj;
        }
        for (int i = 0; i < ready_num; ++i) {
            Py_INCREF(ready_obj[i]);
            PyTuple_SET_ITEM(result, i, ready_obj[i]);
        }
    }

egress_ready_obj:
    PyMem_FREE(ready_obj);

egress_timer:
    if (timerEfn) {
        LIB$FREE_EF(&timerEfn);
    }

egress:
    if (!result) {
        Py_RETURN_NONE;
    }
    return result;
}

static PyObject *
Selector_bytes_count(SelectorObject *self, PyObject *source)
{
    FileWrap *wrapper = _find_wrapper(self, source);
    if (!wrapper) {
        PyErr_SetString(PyExc_KeyError,
            "Unregistered source");
        Py_RETURN_NONE;
    }
    return PyLong_FromUnsignedLong(wrapper->iosb.iosb$w_bcnt);
}

static PyObject *
Selector_is_eof(SelectorObject *self, PyObject *source)
{
    FileWrap *wrapper = _find_wrapper(self, source);
    if (!wrapper) {
        PyErr_SetString(PyExc_KeyError,
            "Unregistered source");
        Py_RETURN_NONE;
    }
    if (SS$_ENDOFFILE == wrapper->iosb.iosb$w_status) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

static PyObject *
Selector_fetch(SelectorObject *self, PyObject *source)
{
    FileWrap *wrapper = _find_wrapper(self, source);
    if (!wrapper) {
        PyErr_SetString(PyExc_KeyError,
            "Unregistered source");
        Py_RETURN_NONE;
    }
    if (wrapper->iosb.iosb$w_bcnt && wrapper->buf) {
        return PyBytes_FromStringAndSize(wrapper->buf, wrapper->iosb.iosb$w_bcnt);
    }
    return PyBytes_FromStringAndSize(NULL, 0);
}

static PyObject *
Selector__enter__(PyObject *self, PyObject *Py_UNUSED(ignored))
{
    Py_INCREF(self);
    return self;
}

static PyObject *
Selector__exit__(PyObject *self, PyObject *Py_UNUSED(ignored))
{
    Py_RETURN_NONE;
}

static PyMethodDef Selector_methods[] = {
    {"register", (PyCFunction) Selector_register, METH_O,
     "Register file"
    },
    {"unregister", (PyCFunction) Selector_unregister, METH_O,
     "Unregister file"
    },
    {"query_read", (PyCFunction) Selector_query_read, METH_O,
     "Query read"
    },
    {"query_write", (PyCFunction) Selector_query_write, METH_VARARGS,
     "Query write"
    },
    {"wait_io", (PyCFunction) Selector_wait_io, METH_O,
     "Wait an IO operation"
    },
    {"bytes_count", (PyCFunction) Selector_bytes_count, METH_O,
     "Get count of read/written bytes"
    },
    {"is_eof", (PyCFunction) Selector_is_eof, METH_O,
     "Test if EOF is read"
    },
    {"fetch", (PyCFunction) Selector_fetch, METH_O,
     "Fetch data from stream after read operation is completed"
    },
    {"__enter__", Selector__enter__, METH_NOARGS, NULL},
    {"__exit__",  Selector__exit__, METH_VARARGS, NULL},
    {NULL, NULL}    /* sentinel */
};

static PyTypeObject SelectorType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "_pipeqio.selector",
    .tp_doc = "PipeQIO selector object",
    .tp_basicsize = sizeof(SelectorObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT, // | Py_TPFLAGS_BASETYPE,
    .tp_new = Selector_new,
    .tp_dealloc = (destructor) Selector_dealloc,
    .tp_members = Selector_members,
    .tp_methods = Selector_methods,
};

static PyModuleDef PipeQIOmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "_pipeqio",
    .m_doc = "OpenVMS pipe $QIO module.",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit__pipeqio(void)
{
    PyObject *m;
    if (PyType_Ready(&SelectorType) < 0)
        return NULL;

    m = PyModule_Create(&PipeQIOmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&SelectorType);
    if (PyModule_AddObject(m, "selector", (PyObject *) &SelectorType) < 0) {
        Py_DECREF(&SelectorType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
