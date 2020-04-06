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
#include "fcntl.h"

#include "Modules/_io/_iomodule.h"

#define TO_READ 0
#define TO_WRITE 1

typedef struct {
    int                         fd;
    int                         efn;
    unsigned short int          channel;
    struct _iosb                iosb;
    PyObject                   *source;
    char                       *buf;
    int                         bufSize;
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
    if (wrapper->source) {
        Py_DECREF(wrapper->source);
        wrapper->source = NULL;
    }
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

static int
_init_wrapper(FileWrap *wrapper, int fd, int bufSize) {
    
    memset(wrapper, 0, sizeof(FileWrap));

    // get name
    char *name = PyMem_MALLOC(PATH_MAX + 1);
    if (name == NULL) {
        PyErr_NoMemory();
        return 0;
    }
    name[0] = 0;
    if (!getname(fd, name, 1)) {
        PyErr_Format(PyExc_OSError,
            "Cannot get name for fd: %d", fd);
        PyMem_FREE(name);
        return 0;
    }

    // get channel
    struct  dsc$descriptor_s descriptor;
    descriptor.dsc$w_length = strlen(name);
    descriptor.dsc$b_dtype = DSC$K_DTYPE_T;
    descriptor.dsc$b_class = DSC$K_CLASS_S;
    descriptor.dsc$a_pointer = name;
    int channel = 0;
    int status = SYS$ASSIGN(
        &descriptor,        /* devnam - device name */ 
        &channel,  /* chan - channel number */ 
        0,                  /* acmode - access mode */ 
        0,                  /* mbxnam - logical name for mailbox */ 
        0 );                /* flags */ 
    PyMem_FREE(name);
    if (!$VMS_STATUS_SUCCESS(status)) {
        PyErr_Format(PyExc_OSError,
            "Cannot assign channel: %d", status);
        return 0;
    }

    // get EFN
    int efn;
    status = LIB$GET_EF(&efn);
    if (!$VMS_STATUS_SUCCESS(status)) {
        SYS$DASSGN(wrapper->channel);
        PyErr_Format(PyExc_OSError,
            "Cannot get EF: %d", status);
        return 0;
    }

    wrapper->fd = fd;
    wrapper->channel = channel;
    wrapper->efn = efn;
    wrapper->buf = PyMem_MALLOC(bufSize);
    wrapper->bufSize = bufSize;

    return 1;
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

    if (!_init_wrapper(&self->wrappers[new_pos], fd_int, self->bufSize)) {
        // clean
        Py_RETURN_FALSE;
    }
    // save source
    Py_INCREF(source);
    self->wrappers[new_pos].source = source;
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

static int
_query_write(FileWrap *wrapper, Py_buffer *pdata, int start) {
    int len = wrapper->bufSize;
    if (len + start > pdata->len) {
        len = pdata->len - start;
    }
    memmove(wrapper->buf, pdata->buf, len);
    int status = SYS$QIO(
        wrapper->efn,           /* efn - event flag */ 
        wrapper->channel,       /* chan - channel number */ 
        IO$_WRITEVBLK,          /* func - function modifier */ 
        &wrapper->iosb,         /* iosb - I/O status block */ 
        0,                      /* astadr - AST routine */ 
        0,                      /* astprm - AST parameter */ 
        wrapper->buf + start,   /* p1 - output buffer */ 
        len,                    /* p2 - length of data */
        0,0,0,0);               /* p3-p6 are required*/
    if (!$VMS_STATUS_SUCCESS(status)) {
        PyErr_Format(PyExc_OSError,
            "SYS$QIO failed: %d", status);
        return 0;
    }
    return 1;
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
    if (_query_write(wrapper, &data, 0)) {
        PyBuffer_Release(&data);
        Py_RETURN_TRUE;
    }
    PyBuffer_Release(&data);
    Py_RETURN_FALSE;
}

static int
_query_read(FileWrap *wrapper) {
    int status = SYS$QIO(
        wrapper->efn,               /* efn - event flag */ 
        wrapper->channel,           /* chan - channel number */ 
        IO$_READVBLK | IO$M_STREAM, /* func - function modifier */ 
        &wrapper->iosb,             /* iosb - I/O status block */ 
        0,                          /* astadr - AST routine */ 
        0,                          /* astprm - AST parameter */ 
        wrapper->buf,               /* p1 - input buffer */ 
        wrapper->bufSize,           /* p2 - size of buffer */
        0,0,0,0);                   /* p3-p6 are required*/
    if (!$VMS_STATUS_SUCCESS(status)) {
        PyErr_Format(PyExc_OSError,
            "SYS$QIO failed: %d", status);
        return 0;
    }
    return 1;
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
    if (_query_read(wrapper)) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

static int 
_timer_efn(PyObject *timeout, struct _generic_64 *timerOffset) {
    int timerEfn = -1;
    int status;
    if (timeout && timeout != Py_None) {
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
            return -1;
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
        status = SYS$BINTIM(&descriptor, timerOffset);
        if (!$VMS_STATUS_SUCCESS(status)) {
            PyErr_Format(PyExc_OSError,
                "SYS$BINTIM failed: %d ", status);
            return -1;
        }
        status = LIB$GET_EF(&timerEfn);
        if (!$VMS_STATUS_SUCCESS(status)) {
            PyErr_Format(PyExc_OSError,
                "LIB$GET_EF failed: %d ", status);
            return -1;
        }
    }
    return timerEfn;
}

static PyObject *
Selector_wait_io(SelectorObject *self, PyObject *timeout)
{
    PyObject* result = NULL;
    if (self->num == 0) {
        goto egress;
    }
    int status;
    struct _generic_64 timerOffset;
    int timerEfn = _timer_efn(timeout, &timerOffset);

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
    if (timerEfn != -1) {
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
    if (timerEfn != -1) {
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

static void
child_exec(char *const exec_array[],
           char *const argv[],
           char *const envp[],
           const char *cwd,
           int p2cread,
           int c2pwrite,
           int errwrite)
{
    if (cwd) {
        decc$set_child_default_dir(cwd);
    }
    
    decc$set_child_standard_streams(p2cread, c2pwrite, errwrite);

    /* This loop matches the Lib/os.py _execvpe()'s PATH search when */
    /* given the executable_list generated by Lib/subprocess.py.     */
    for (int i = 0; exec_array[i] != NULL; ++i) {
        const char *executable = exec_array[i];
        if (envp) {
            execve(executable, argv, envp);
        } else {
            execv(executable, argv);
        }
        if (errno != ENOENT && errno != ENOTDIR) {
            break;
        }
    }

error:
    // No report, we are at parent process
}

static PyObject *
PipeQIOmodule_vfork_exec(SelectorObject *self, PyObject *args)
{
    PyObject* result = NULL;
    int pid = -1;
    PyObject *executable_list;
    PyObject *env_list;
    PyObject *process_args, *converted_args = NULL, *fast_args = NULL;
    int stdin_override, stdout_override, stderr_override;
    PyObject *cwd_obj, *cwd_obj2;
    const char *cwd;
    char *const *exec_array, *const *argv = NULL, *const *envp = NULL;
    Py_ssize_t arg_num;
    int saved_errno = 0;
    int fdin[2], fdout[2], fderr[2];

    if (!PyArg_ParseTuple(
            args, "OOOOppp:vfork_exec",
            &executable_list, &process_args, 
            &cwd_obj, &env_list,
            &stdin_override, &stdout_override, &stderr_override)) {
        Py_RETURN_NONE;
    }
    exec_array = _PySequence_BytesToCharpArray(executable_list);
    if (!exec_array) {
        goto cleanup;
    }

    /* Convert args and env into appropriate arguments for exec() */
    /* These conversions are done in the parent process to avoid allocating
       or freeing memory in the child process. */
    if (process_args != Py_None) {
        Py_ssize_t num_args;
        /* Equivalent to:  */
        /*  tuple(PyUnicode_FSConverter(arg) for arg in process_args)  */
        fast_args = PySequence_Fast(process_args, "argv must be a tuple");
        if (fast_args == NULL) {
            goto cleanup;
        }
        num_args = PySequence_Fast_GET_SIZE(fast_args);
        converted_args = PyTuple_New(num_args);
        if (converted_args == NULL) {
            goto cleanup;
        }
        for (arg_num = 0; arg_num < num_args; ++arg_num) {
            PyObject *borrowed_arg, *converted_arg;
            if (PySequence_Fast_GET_SIZE(fast_args) != num_args) {
                PyErr_SetString(PyExc_RuntimeError, "args changed during iteration");
                goto cleanup;
            }
            borrowed_arg = PySequence_Fast_GET_ITEM(fast_args, arg_num);
            if (PyUnicode_FSConverter(borrowed_arg, &converted_arg) == 0) {
                goto cleanup;
            }
            PyTuple_SET_ITEM(converted_args, arg_num, converted_arg);
        }

        argv = _PySequence_BytesToCharpArray(converted_args);
        Py_CLEAR(converted_args);
        Py_CLEAR(fast_args);
        if (!argv) {
            goto cleanup;
        }
    }

    if (env_list != Py_None) {
        envp = _PySequence_BytesToCharpArray(env_list);
        if (!envp) {
            goto cleanup;
        }
    }

    if (cwd_obj != Py_None) {
        if (PyUnicode_FSConverter(cwd_obj, &cwd_obj2) == 0) {
            goto cleanup;
        }
        cwd = PyBytes_AsString(cwd_obj2);
    } else {
        cwd = NULL;
        cwd_obj2 = NULL;
    }

    if (stdin_override) {
        pipe(fdin);
        fcntl(fdin[TO_WRITE], F_SETFD, FD_CLOEXEC);
    } else {
        fdin[TO_READ] = -1;
        fdin[TO_WRITE] = -1;
    }

    if (stdout_override) {
        pipe(fdout);
        fcntl(fdout[TO_READ], F_SETFD, FD_CLOEXEC);
    } else {
        fdout[TO_READ] = -1;
        fdout[TO_WRITE] = -1;
    }

    if (stderr_override) {
        pipe(fderr);
        fcntl(fderr[TO_READ], F_SETFD, FD_CLOEXEC);
    } else {
        fderr[TO_READ] = -1;
        fderr[TO_WRITE] = -1;
    }

    pid = vfork();

    /* after vfork() */
    if (pid == 0) {
        /* Parent process */
        child_exec(exec_array, argv, envp, cwd,
                   fdin[TO_READ], 
                   fdout[TO_WRITE],
                   fderr[TO_WRITE]);
        /* we are here - error occured */
        pid = -1;
    }

    /* Parent (original) process */
    if (pid == -1) {
        /* Capture errno for the exception. */
        saved_errno = errno;
    }

    Py_XDECREF(cwd_obj2);

    if (envp) {
        _Py_FreeCharPArray(envp);
    }
    if (argv) {
        _Py_FreeCharPArray(argv);
    }
    _Py_FreeCharPArray(exec_array);

    if (pid == -1) {
        errno = saved_errno;
        /* We can't call this above as PyOS_AfterFork_Parent() calls back
         * into Python code which would see the unreturned error. */
        PyErr_SetFromErrno(PyExc_OSError);
        Py_RETURN_NONE;  /* vfork_exec() failed. */
    }

    result = PyTuple_New(4);
    if (result == NULL) {
        PyErr_NoMemory();
        Py_RETURN_NONE;
    }

    PyTuple_SET_ITEM(result, 0, PyLong_FromLong(pid));
    PyTuple_SET_ITEM(result, 1, PyLong_FromLong(fdin[TO_WRITE]));
    PyTuple_SET_ITEM(result, 2, PyLong_FromLong(fdout[TO_READ]));
    PyTuple_SET_ITEM(result, 3, PyLong_FromLong(fderr[TO_READ]));

    return result;

cleanup:
    if (envp) {
        _Py_FreeCharPArray(envp);
    }
    if (argv) {
        _Py_FreeCharPArray(argv);
    }
    if (exec_array) {
        _Py_FreeCharPArray(exec_array);
    }
    Py_XDECREF(converted_args);
    Py_XDECREF(fast_args);
    Py_RETURN_NONE;
}

static PyObject *
PipeQIOmodule_vfork_exec_communicate(SelectorObject *self, PyObject *args, PyObject *kw)
{
    PyObject* result = NULL;
    int pid = -1;
    PyObject *executable_list = NULL;
    PyObject *env_list = NULL;
    PyObject *process_args = NULL;
    PyObject *converted_args = NULL;
    PyObject *fast_args = NULL;
    PyObject *cwd_obj = NULL;
    PyObject *cwd_obj2 = NULL;
    const char *cwd = NULL;
    char *const *exec_array = NULL, 
         *const *argv = NULL, 
         *const *envp = NULL;
    Py_ssize_t arg_num = 0;
    int saved_errno = 0;
    int fd[2];
    Py_buffer data = {0};
    PyObject *timeout = NULL;
    int p2cread = -1, 
        p2cwrite = -1,
        c2pread = -1,
        c2pwrite = -1,
        e2pread = -1,
        e2pwrite = -1;
    static char *keywords[] = {
        "executable",
        "args",
        "cwd",
        "env",
        "data",
        "timeout",
        "p2cread",
        "p2cwrite",
        "c2pread",
        "c2pwrite",
        NULL
    };

    if (!PyArg_ParseTupleAndKeywords(
            args, kw, "OO|OOs*Oiiii:vfork_exec_communicate", keywords,
            &executable_list, &process_args, 
            &cwd_obj, &env_list, &data, &timeout, &p2cread, &p2cwrite, &c2pread, &c2pwrite)) {
        Py_RETURN_NONE;
    }
    
    if (data.len && data.ndim > 1) {
        PyErr_SetString(PyExc_BufferError,
                        "Buffer must be single dimension");
        PyBuffer_Release(&data);
        goto cleanup;
    }

    exec_array = _PySequence_BytesToCharpArray(executable_list);
    if (!exec_array) {
        goto cleanup;
    }

    /* Convert args and env into appropriate arguments for exec() */
    /* These conversions are done in the parent process to avoid allocating
       or freeing memory in the child process. */
    if (process_args != Py_None) {
        Py_ssize_t num_args;
        /* Equivalent to:  */
        /*  tuple(PyUnicode_FSConverter(arg) for arg in process_args)  */
        fast_args = PySequence_Fast(process_args, "argv must be a tuple");
        if (fast_args == NULL) {
            goto cleanup;
        }
        num_args = PySequence_Fast_GET_SIZE(fast_args);
        converted_args = PyTuple_New(num_args);
        if (converted_args == NULL) {
            goto cleanup;
        }
        for (arg_num = 0; arg_num < num_args; ++arg_num) {
            PyObject *borrowed_arg, *converted_arg;
            if (PySequence_Fast_GET_SIZE(fast_args) != num_args) {
                PyErr_SetString(PyExc_RuntimeError, "args changed during iteration");
                goto cleanup;
            }
            borrowed_arg = PySequence_Fast_GET_ITEM(fast_args, arg_num);
            if (PyUnicode_FSConverter(borrowed_arg, &converted_arg) == 0) {
                goto cleanup;
            }
            PyTuple_SET_ITEM(converted_args, arg_num, converted_arg);
        }

        argv = _PySequence_BytesToCharpArray(converted_args);
        Py_CLEAR(converted_args);
        Py_CLEAR(fast_args);
        if (!argv) {
            goto cleanup;
        }
    }

    if (env_list != NULL && env_list != Py_None) {
        envp = _PySequence_BytesToCharpArray(env_list);
        if (!envp) {
            goto cleanup;
        }
    }

    if (cwd_obj != NULL && cwd_obj != Py_None) {
        if (PyUnicode_FSConverter(cwd_obj, &cwd_obj2) == 0) {
            goto cleanup;
        }
        cwd = PyBytes_AsString(cwd_obj2);
    } else {
        cwd = NULL;
        cwd_obj2 = NULL;
    }

    if (data.len) {
        if (p2cwrite == -1 || p2cread == -1) {
            pipe(fd);
            p2cread = fd[TO_READ];
            p2cwrite = fd[TO_WRITE];
        }
        fcntl(p2cwrite, F_SETFD, FD_CLOEXEC);
        // fcntl(p2cread, F_SETFD, FD_CLOEXEC);
        // fcntl(p2cread, F_SETFD, 0);
    }

    if (c2pwrite == -1 || c2pread == -1) {
        pipe(fd);
        c2pread = fd[TO_READ];
        c2pwrite = fd[TO_WRITE];
    }
    fcntl(c2pread, F_SETFD, FD_CLOEXEC);
    // fcntl(c2pwrite, F_SETFD, FD_CLOEXEC);
    // fcntl(c2pwrite, F_SETFD, 0);

    if (e2pwrite == -1 || e2pread == -1) {
        pipe(fd);
        e2pread = fd[TO_READ];
        e2pwrite = fd[TO_WRITE];
    }
    fcntl(e2pread, F_SETFD, FD_CLOEXEC);
    // fcntl(e2pwrite, F_SETFD, FD_CLOEXEC);
    // fcntl(e2pwrite, F_SETFD, 0);

    pid = vfork();

    if (pid == 0) {
        /* Parent process for child */
        if (cwd) {
            decc$set_child_default_dir(cwd);
        }

        decc$set_child_standard_streams(p2cread, c2pwrite, e2pwrite);

        for (int i = 0; exec_array[i] != NULL; ++i) {
            const char *executable = exec_array[i];
            if (envp) {
                execve(executable, argv, envp);
            } else {
                execv(executable, argv);
            }
            if (errno != ENOENT && errno != ENOTDIR) {
                break;
            }
        }
        /* we are here - error occured */
        pid = -1;
    }

    /* Parent (original) process */
    if (pid == -1) {
        /* Capture errno for the exception. */
        saved_errno = errno;
    }

    if (cwd_obj2) {
        Py_XDECREF(cwd_obj2);
    }
    if (envp) {
        _Py_FreeCharPArray(envp);
    }
    if (argv) {
        _Py_FreeCharPArray(argv);
    }
    _Py_FreeCharPArray(exec_array);

    if (pid == -1) {
        errno = saved_errno;
        /* We can't call this above as PyOS_AfterFork_Parent() calls back
         * into Python code which would see the unreturned error. */
        PyErr_SetFromErrno(PyExc_OSError);
        PyBuffer_Release(&data);
        Py_RETURN_NONE;  /* vfork_exec() failed. */
    } else {
        // communicate
        FileWrap wrapIn = {0};
        FileWrap wrapOut = {0};
        FileWrap wrapErr = {0};
        FileWrap *wrappers[3] = {0};
        int num_wrappers = 0;
        int timerEfn = -1;
        struct _generic_64 timerOffset;
        int status;
        int write_pos = 0;
        char *stdout_buff = NULL;
        int stdout_buff_size = 0;
        char *stderr_buff = NULL;
        int stderr_buff_size = 0;

        int bufSize = decc$feature_get("DECC$PIPE_BUFFER_SIZE", __FEATURE_MODE_CURVAL);

        if (p2cwrite != -1) {
            if (!_init_wrapper(&wrapIn, p2cwrite, bufSize)) {
                goto communicate_end;
            }
            wrappers[num_wrappers++] = &wrapIn;
            _query_write(&wrapIn, &data, write_pos);
        }

        if (!_init_wrapper(&wrapOut, c2pread, bufSize)) {
            goto communicate_end;
        }
        wrappers[num_wrappers++] = &wrapOut;
        _query_read(&wrapOut);

        if (!_init_wrapper(&wrapErr, e2pread, bufSize)) {
            goto communicate_end;
        }
        wrappers[num_wrappers++] = &wrapErr;
        _query_read(&wrapErr);

        timerEfn = _timer_efn(timeout, &timerOffset);

        // init from out channel
        int claster = wrappers[0]->efn & ~0x1f;
        int mask = 1 << (wrappers[0]->efn & 0x1f);
        for(int i = 1; i < num_wrappers; ++i) {
            // test err channel
            if (claster != (wrappers[i]->efn & ~0x1f)) {
                PyErr_SetString(PyExc_OSError,
                    "EF in different clusters");
                goto communicate_end;
            }
            mask |= 1 << (wrappers[i]->efn & 0x1f);
        }
        // test and create timer 
        if (timerEfn != -1) {
            if (claster != timerEfn & ~0x1f) {
                PyErr_SetString(PyExc_OSError,
                    "EF in different clusters");
                goto communicate_end;
            }
            mask |= 1 << (timerEfn & 0x1f);
            status = SYS$SETIMR(timerEfn, &timerOffset, 0, 0, 0);
            if (!$VMS_STATUS_SUCCESS(status)) {
                PyErr_Format(PyExc_OSError,
                    "SYS$SETIMR failed: %d ", status);
                goto communicate_end;
            }
        }

        int timerElapsed = 0;
        while (num_wrappers) {
            status = SYS$WFLOR(wrappers[0]->efn, mask);
            if (!$VMS_STATUS_SUCCESS(status)) {
                PyErr_Format(PyExc_OSError,
                    "SYS$WFLOR failed: %d", status);
                goto communicate_end;
            }

            int efnCluster;
            for (int i = 0; i < num_wrappers; ++i) {
                status = SYS$READEF(wrappers[i]->efn, &efnCluster);
                if (status == SS$_WASSET && wrappers[i]->iosb.iosb$w_status) {
                    // io done
                    int clean_it = 0;
                    if (wrappers[i] == &wrapIn) {
                        // test remainning bytes and request write again
                        write_pos += wrapIn.iosb.iosb$w_bcnt;
                        if (write_pos >= data.len) {
                            clean_it = 1;
                        } else {
                            _query_write(&wrapIn, &data, write_pos);
                        }
                    } else if (wrappers[i] == &wrapOut) {
                        if (SS$_ENDOFFILE == wrapOut.iosb.iosb$w_status) {
                            clean_it = 1;
                        } else {
                            // fetch and save data for stdout
                            if (wrapOut.iosb.iosb$w_bcnt) {
                                stdout_buff = PyMem_REALLOC(stdout_buff, stdout_buff_size + wrapOut.iosb.iosb$w_bcnt);
                                memmove(stdout_buff + stdout_buff_size, wrapOut.buf, wrapOut.iosb.iosb$w_bcnt);
                                stdout_buff_size += wrapOut.iosb.iosb$w_bcnt;
                            }
                            _query_read(&wrapOut);
                        }
                    } else if (wrappers[i] == &wrapErr) {
                        if (SS$_ENDOFFILE == wrapErr.iosb.iosb$w_status) {
                            clean_it = 1;
                        } else {
                            // fetch and save data for stderr
                            if (wrapErr.iosb.iosb$w_bcnt) {
                                stderr_buff = PyMem_REALLOC(stderr_buff, stderr_buff_size + wrapErr.iosb.iosb$w_bcnt);
                                memmove(stderr_buff + stderr_buff_size, wrapErr.buf, wrapErr.iosb.iosb$w_bcnt);
                                stderr_buff_size += wrapErr.iosb.iosb$w_bcnt;
                            }
                            _query_read(&wrapErr);
                        }
                    }
                    if (clean_it) {
                        close(wrappers[i]->fd);
                        _clean_warpper(wrappers[i]);
                        --num_wrappers;
                        int tail = num_wrappers - i;
                        if (tail > 0) {
                            memmove(wrappers + i, wrappers + i + 1, tail * sizeof(FileWrap*));
                        }
                        --i;
                    }
                }
            }
            // test timer
            if (timerEfn != -1) {
                status = SYS$READEF(timerEfn, &efnCluster);
                if (status == SS$_WASSET) {
                    // exit loop by timer
                    timerElapsed = 1;
                    break;
                }
            }
        }

        int cstatus;
        if (timerElapsed) {
            status = waitpid(pid, &cstatus, WNOHANG);
            if (status != pid) {
                kill(pid, SIGKILL);
            }
        } else {
            // TODO: wait remained time?
            status = waitpid(pid, &cstatus, 0);
        }
        

communicate_end:
        
        if (p2cread != -1) {
            close(p2cread);
        }
        if (c2pwrite != -1) {
            close(c2pwrite);
        }
        if (e2pwrite) {
            close(e2pwrite);
        }

        PyBuffer_Release(&data);

        if (timerEfn != -1) {
            LIB$FREE_EF(&timerEfn);
        }

        for (int i = 0; i < num_wrappers; ++i) {
            if (wrappers[i]->channel) {
                _clean_warpper(wrappers[i]);
            }
        }

        result = PyTuple_New(2);
        if (result == NULL) {
            PyErr_NoMemory();
            Py_RETURN_NONE;
        }

        PyTuple_SET_ITEM(result, 0, PyBytes_FromStringAndSize(stdout_buff, stdout_buff_size));    // stdout
        PyTuple_SET_ITEM(result, 1, PyBytes_FromStringAndSize(stderr_buff, stderr_buff_size));    // stderr

        PyMem_FREE(stdout_buff);
        PyMem_FREE(stderr_buff);

        return result;
    }

cleanup:
    if (envp) {
        _Py_FreeCharPArray(envp);
    }
    if (argv) {
        _Py_FreeCharPArray(argv);
    }
    if (exec_array) {
        _Py_FreeCharPArray(exec_array);
    }
    Py_XDECREF(converted_args);
    Py_XDECREF(fast_args);

    PyBuffer_Release(&data);

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

static struct PyMethodDef PipeQIOmodule_methods[] = {
    {"vfork_exec", (PyCFunction) PipeQIOmodule_vfork_exec,
     METH_VARARGS,
     "Do vfork() and exec()"
    },
    {"vfork_exec_communicate", (PyCFunction) PipeQIOmodule_vfork_exec_communicate,
     METH_VARARGS|METH_KEYWORDS,
     "Do vfork() and exec() and communicate"
    },
    {NULL,       NULL}          /* sentinel */
};
static PyModuleDef PipeQIOmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "_pipeqio",
    .m_doc = "OpenVMS pipe $QIO module.",
    .m_size = -1,
    .m_methods = PipeQIOmodule_methods,
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
