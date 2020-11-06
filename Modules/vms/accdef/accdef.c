#include <Python.h>
#include <accdef.h>

PyDoc_STRVAR(doc__accdef,
"ACC definitions");

static struct PyModuleDef _accdef_module = {
    PyModuleDef_HEAD_INIT,
    "accdef",
    doc__accdef,
    -1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit__accdef(void)
{
    PyObject *m = NULL;
    char buff[64];

    m = PyModule_Create(&_accdef_module);

    PyModule_AddIntConstant(m, "ACC_K_TERMLEN", 84);
    PyModule_AddIntConstant(m, "ACC_C_TERMLEN", 84);
    PyModule_AddIntConstant(m, "ACC_K_JOB_LEN", 108);
    PyModule_AddIntConstant(m, "ACC_C_JOB_LEN", 108);
    PyModule_AddIntConstant(m, "ACC_S_ACCDEF", 108);
    PyModule_AddIntConstant(m, "ACC_S_TERMTIME", 8);
    PyModule_AddIntConstant(m, "ACC_S_ACCOUNT", 8);
    PyModule_AddIntConstant(m, "ACC_S_USERNAME", 12);
    PyModule_AddIntConstant(m, "ACC_S_LOGIN", 8);
    PyModule_AddIntConstant(m, "ACC_S_JOB_NAME", 8);
    PyModule_AddIntConstant(m, "ACC_S_JOB_QUE", 16);
    PyModule_AddIntConstant(m, "ACC_K_PRT_LEN", 88);
    PyModule_AddIntConstant(m, "ACC_C_PRT_LEN", 88);
    PyModule_AddIntConstant(m, "ACC_S_ACCDEF1", 88);
    PyModule_AddIntConstant(m, "ACC_S_QUETIME", 8);
    PyModule_AddIntConstant(m, "ACC_S_PRT_NAME", 8);
    PyModule_AddIntConstant(m, "ACC_S_PRT_QUE", 12);
    PyModule_AddIntConstant(m, "ACC_K_INS_LEN", 176);
    PyModule_AddIntConstant(m, "ACC_C_INS_LEN", 176);
    PyModule_AddIntConstant(m, "ACC_K_PRCTRM", 1);
    PyModule_AddIntConstant(m, "ACC_K_BATTRM", 2);
    PyModule_AddIntConstant(m, "ACC_K_INTTRM", 3);
    PyModule_AddIntConstant(m, "ACC_K_LOGTRM", 4);
    PyModule_AddIntConstant(m, "ACC_K_IMGTRM", 5);
    PyModule_AddIntConstant(m, "ACC_K_SUBTRM", 6);
    PyModule_AddIntConstant(m, "ACC_K_DETTRM", 7);
    PyModule_AddIntConstant(m, "ACC_K_NETTRM", 8);
    PyModule_AddIntConstant(m, "ACC_K_PRTJOB", 16);
    PyModule_AddIntConstant(m, "ACC_K_INSMSG", 17);
    PyModule_AddIntConstant(m, "ACC_K_INSMESG", 1);
    PyModule_AddIntConstant(m, "ACC_K_NEWFILE", 2);
    PyModule_AddIntConstant(m, "ACC_K_ENABACC", 3);
    PyModule_AddIntConstant(m, "ACC_K_DISAACC", 4);
    PyModule_AddIntConstant(m, "ACC_K_ENABSEL", 5);
    PyModule_AddIntConstant(m, "ACC_K_DISASEL", 6);
    PyModule_AddIntConstant(m, "ACC_S_ACCDEF2", 176);
    PyModule_AddIntConstant(m, "ACC_S_USER_DATA", 132);

    return m;

}