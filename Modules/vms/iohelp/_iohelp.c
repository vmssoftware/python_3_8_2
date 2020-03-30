#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"
#include "iosbdef.h"
#include "descrip.h"
#include "starlet.h"
#include "stsdef.h"
#include "unixio.h"
#include "lib$routines.h"

typedef struct {
    int                         fd;
    int                         efn;
    unsigned short int          channel;
    struct _iosb                iosb;
} FileWrap;

typedef struct {
    PyObject_HEAD
    FileWrap   *wrappers;
    int         size;
    int         num;
} IOHelpObject;

static void
_clean_warpper(FileWrap *wrapper) {
    SYS$DASSGN(wrapper->channel);
    LIB$FREE_EF(&wrapper->efn);
}

static void
IOHelp_dealloc(IOHelpObject *self)
{
    while(self->num--) {
        _clean_warpper(&self->wrappers[self->num]);
    }
    PyMem_FREE(self->wrappers);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *
IOHelp_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    IOHelpObject *self;
    self = (IOHelpObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->num = 0;
        self->size = 0;
        self->wrappers = NULL;
    }
    return (PyObject *) self;
}

static PyMemberDef IOHelp_members[] = {
    {"num", T_INT, offsetof(IOHelpObject, num), 0,
     "number of streams"},
    {NULL}  /* Sentinel */
};

static PyObject *
IOHelp_register(IOHelpObject *self, PyObject *fd)
{
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
    // ----------------------------------------------------------------
    int fd_int = PyLong_AsLong(fd);

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
        Py_RETURN_FALSE;
    }
    // get EFN
    int efn;
    status = LIB$GET_EF(&efn);
    if (!$VMS_STATUS_SUCCESS(status)) {
        SYS$DASSGN(channel);
        Py_RETURN_FALSE;
    }
    // save
    self->wrappers[new_pos].fd = fd_int;
    self->wrappers[new_pos].channel = channel;
    self->wrappers[new_pos].efn = efn;
    ++self->num;
    Py_RETURN_TRUE;
}

static PyObject *
IOHelp_unregister(IOHelpObject *self, PyObject *fd)
{
    int fd_int = PyLong_AsLong(fd);
    int num = self->num;
    while(num--) {
        if (self->wrappers[num].fd == fd_int) {
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

static PyMethodDef IOHelp_methods[] = {
    {"register", (PyCFunction) IOHelp_register, METH_O,
     "Register file descriptor"
    },
    {"unregister", (PyCFunction) IOHelp_unregister, METH_O,
     "Unregister file descriptor"
    },
    {NULL}  /* Sentinel */
};

static PyTypeObject IOHelpType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "_iohelp.IOHelp",
    .tp_doc = "IOHelp objects",
    .tp_basicsize = sizeof(IOHelpObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT, // | Py_TPFLAGS_BASETYPE,
    .tp_new = IOHelp_new,
    .tp_dealloc = (destructor) IOHelp_dealloc,
    .tp_members = IOHelp_members,
    .tp_methods = IOHelp_methods,
};

static PyModuleDef iohelpmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "_iohelp",
    .m_doc = "IOHelp doc.",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit__iohelp(void)
{
    PyObject *m;
    if (PyType_Ready(&IOHelpType) < 0)
        return NULL;

    m = PyModule_Create(&iohelpmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&IOHelpType);
    if (PyModule_AddObject(m, "IOHelp", (PyObject *) &IOHelpType) < 0) {
        Py_DECREF(&IOHelpType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
