! MMS/EXT/DESCR=Python3.mms/MACRO=("OUTDIR=OUT","CONFIG=DEBUG")

! define output folder
.IF OUTDIR
! defined - ok
.ELSE
! not defined - define as OUT
OUTDIR = OUT
.ENDIF

! check DEBUG or RELEASE
.IF CONFIG
! defined - ok
.ELSE
! not defined - define as DEBUG
CONFIG = DEBUG
.ENDIF

.IF $(CONFIG) .EQ DEBUG
! debug
OPT_Q = /DEBUG/NOOPTIMIZE/LIST=$(MMS$TARGET_NAME)
OPT_DEF = _DEBUG
OUT_DIR = $(OUTDIR).$(CONFIG)
OBJ_DIR = $(OUT_DIR).OBJ
LINKFLAGS = /DEBUG/MAP=[.$(OUT_DIR)]$(NOTDIR $(MMS$TARGET_NAME))
PYTHON$SHR_OPT = PYTHON$SHR_DBG
.ELSE
! release
OPT_Q = /NODEBUG/OPTIMIZE/NOLIST
OPT_DEF = _NDEBUG
OUT_DIR = $(OUTDIR).$(CONFIG)
OBJ_DIR = $(OUT_DIR).OBJ
LINKFLAGS = /NODEBUG/NOMAP
PYTHON$SHR_OPT = PYTHON$SHR
.ENDIF

DYNLOAD_DIR = lib-dynload

DYNLOADFILE = dynload_shlib
ABIFLAGS = m
PLATFORM = OpenVMS
SOABI = cpython-38m-ia64-openvms

PY_CFLAGS_Q = $(OPT_Q)/NAMES=(AS_IS,SHORTENED)/WARNINGS=DISABLE=(NONSTANDCAST,NOTINCRTL,MIXFUNCVOID,QUESTCOMPARE,QUESTCOMPARE1)
PY_CFLAGS_DEF = $(OPT_DEF),_USE_STD_STAT
PY_CFLAGS_INC = [],[.Include],[.Include.internal],oss$root:[include]
PY_CFLAGS = $(PY_CFLAGS_Q)/DEFINE=($(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

PY_BUILTIN_MODULE_CFLAGS = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE_BUILTIN",$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))
PY_CORE_CFLAGS = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE",$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

GETPATH_DEF = PYTHONPATH="""""",PREFIX="""/usr/local""",EXEC_PREFIX="""/usr/local""",VERSION="""3.8""",VPATH=""""""
PY_CORE_CFLAGS_GETPATH = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE",$(GETPATH_DEF),$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

SOABI_DEF = SOABI="""$(SOABI)"""
PY_CORE_CFLAGS_SHLIB = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE",$(SOABI_DEF),$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

HPUX_DEF = SHLIB_EXT=""".EXE"""
PY_CORE_CFLAGS_HPUX = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE",$(HPUX_DEF),$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

SYSMODULE_DEF = ABIFLAGS="""$(ABIFLAGS)""",MULTIARCH="""$(SOABI)"""
PY_CORE_CFLAGS_SYSMODULE = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE",$(SYSMODULE_DEF),$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

GETPLATFORM_DEF = PLATFORM="""$(PLATFORM)"""
PY_CORE_CFLAGS_GETPLATFORM = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE",$(GETPLATFORM_DEF),$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

IO_INC = [.Modules._io]
PY_BUILTIN_MODULE_CFLAGS_IO = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE_BUILTIN",$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC),$(IO_INC))

PY_OSF_CFLAGS = $(PY_CFLAGS_Q)/DEFINE=(_OSF_SOURCE,$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

.FIRST
    ! defines for nested includes, like: 
    ! #include "clinic/transmogrify.h.h"
    define clinic [.Objects.clinic],[.Python.clinic],[.Modules.clinic],[.Modules._io.clinic],[.Modules.cjkcodecs.clinic],[.Objects.stringlib.clinic]
    define stringlib [.Objects.stringlib]
    define modules [.Modules]
    define readline oss$root:[include.readline]
    define lzma oss$root:[include.lzma]
    define cpython [.Include.cpython]
    define internal [.Include.internal]
    BUILD_OUT_DIR = F$ENVIRONMENT("DEFAULT")-"]"+".$(OUT_DIR).]"
    BUILD_OBJ_DIR = F$ENVIRONMENT("DEFAULT")-"]"+".$(OBJ_DIR).]"
    define /trans=concealed python$build_out 'BUILD_OUT_DIR'
    define /trans=concealed python$build_obj 'BUILD_OBJ_DIR'

.SUFFIXES
.SUFFIXES .EXE .OLB .OBJ .OBM .C

.C.OBJ
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

.OBJ.OLB
    @ IF F$SEARCH("$(MMS$TARGET)") .EQS. "" -
        THEN $(LIBR)/CREATE $(MMS$TARGET)
    $(LIBR) $(MMS$TARGET) $(MMS$SOURCE)

! modules hack
.C.OBM
    ! .C to .OBM
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

.OBM.EXE
    ! .OBM to .EXE
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT


LIBDYNLOAD = -
[.$(OUT_DIR).$(DYNLOAD_DIR)]array.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_struct.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_contextvars.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_decc.exe

TARGET : [.$(OUT_DIR)]python3.exe $(LIBDYNLOAD)
    ! done

CLEAN :
    del/tree [.$(OUT_DIR)...]*.*;*

pyconfig.h : [.vms]pyconfig.h
    copy [.vms]pyconfig.h []

IO_HEADERS = -
[.Modules._io]_iomodule.h

BYTESTR_HEADERS = -
[.Objects.stringlib]count.h -
[.Objects.stringlib]ctype.h -
[.Objects.stringlib]fastsearch.h -
[.Objects.stringlib]find.h -
[.Objects.stringlib]join.h -
[.Objects.stringlib]partition.h -
[.Objects.stringlib]split.h -
[.Objects.stringlib]stringdefs.h -
[.Objects.stringlib]transmogrify.h

UNICODE_HEADERS = -
[.Objects.stringlib]asciilib.h -
[.Objects.stringlib]codecs.h -
[.Objects.stringlib]count.h -
[.Objects.stringlib]fastsearch.h -
[.Objects.stringlib]find.h -
[.Objects.stringlib]find_max_char.h -
[.Objects.stringlib]localeutil.h -
[.Objects.stringlib]partition.h -
[.Objects.stringlib]replace.h -
[.Objects.stringlib]split.h -
[.Objects.stringlib]ucs1lib.h -
[.Objects.stringlib]ucs2lib.h -
[.Objects.stringlib]ucs4lib.h -
[.Objects.stringlib]undef.h -
[.Objects.stringlib]unicode_format.h -
[.Objects.stringlib]unicodedefs.h

PARSER_HEADERS = -
[.Include]grammar.h -
[.Include]parsetok.h -
[.Parser]parser.h -
[.Parser]tokenizer.h

PYTHON_HEADERS = -
[.Include]Python.h -
[.Include]abstract.h -
[.Include]asdl.h -
[.Include]ast.h -
[.Include]bitset.h -
[.Include]bltinmodule.h -
[.Include]boolobject.h -
[.Include]bytearrayobject.h -
[.Include]bytes_methods.h -
[.Include]bytesobject.h -
[.Include]cellobject.h -
[.Include]ceval.h -
[.Include]classobject.h -
[.Include]code.h -
[.Include]codecs.h -
[.Include]compile.h -
[.Include]complexobject.h -
[.Include]context.h -
[.Include]descrobject.h -
[.Include]dictobject.h -
[.Include]dtoa.h -
[.Include]dynamic_annotations.h -
[.Include]enumobject.h -
[.Include]errcode.h -
[.Include]eval.h -
[.Include]fileobject.h -
[.Include]fileutils.h -
[.Include]floatobject.h -
[.Include]frameobject.h -
[.Include]funcobject.h -
[.Include]genobject.h -
[.Include]import.h -
[.Include]interpreteridobject.h -
[.Include]intrcheck.h -
[.Include]iterobject.h -
[.Include]listobject.h -
[.Include]longintrepr.h -
[.Include]longobject.h -
[.Include]marshal.h -
[.Include]memoryobject.h -
[.Include]methodobject.h -
[.Include]modsupport.h -
[.Include]moduleobject.h -
[.Include]namespaceobject.h -
[.Include]node.h -
[.Include]object.h -
[.Include]objimpl.h -
[.Include]odictobject.h -
[.Include]opcode.h -
[.Include]osdefs.h -
[.Include]osmodule.h -
[.Include]patchlevel.h -
[.Include]picklebufobject.h -
[.Include]pyarena.h -
[.Include]pycapsule.h -
[.Include]pyctype.h -
[.Include]pydebug.h -
[.Include]pydtrace.h -
[.Include]pyerrors.h -
[.Include]pyfpe.h -
[.Include]pyhash.h -
[.Include]pylifecycle.h -
[.Include]pymacconfig.h -
[.Include]pymacro.h -
[.Include]pymath.h -
[.Include]pymem.h -
[.Include]pyport.h -
[.Include]pystate.h -
[.Include]pystrcmp.h -
[.Include]pystrhex.h -
[.Include]pystrtod.h -
[.Include]pythonrun.h -
[.Include]pythread.h -
[.Include]pytime.h -
[.Include]rangeobject.h -
[.Include]setobject.h -
[.Include]sliceobject.h -
[.Include]structmember.h -
[.Include]structseq.h -
[.Include]symtable.h -
[.Include]sysmodule.h -
[.Include]token.h -
[.Include]traceback.h -
[.Include]tracemalloc.h -
[.Include]tupleobject.h -
[.Include]ucnhash.h -
[.Include]unicodeobject.h -
[.Include]warnings.h -
[.Include]weakrefobject.h -
pyconfig.h -
[.vms]py_vms.h -
$(PARSER_HEADERS) -
[.Include]Python-ast.h -
[.Include.cpython]abstract.h -
[.Include.cpython]dictobject.h -
[.Include.cpython]fileobject.h -
[.Include.cpython]initconfig.h -
[.Include.cpython]interpreteridobject.h -
[.Include.cpython]object.h -
[.Include.cpython]objimpl.h -
[.Include.cpython]pyerrors.h -
[.Include.cpython]pylifecycle.h -
[.Include.cpython]pymem.h -
[.Include.cpython]pystate.h -
[.Include.cpython]sysmodule.h -
[.Include.cpython]traceback.h -
[.Include.cpython]tupleobject.h -
[.Include.cpython]unicodeobject.h -
[.Include.internal]pycore_accu.h -
[.Include.internal]pycore_atomic.h -
[.Include.internal]pycore_ceval.h -
[.Include.internal]pycore_code.h -
[.Include.internal]pycore_condvar.h -
[.Include.internal]pycore_context.h -
[.Include.internal]pycore_fileutils.h -
[.Include.internal]pycore_getopt.h -
[.Include.internal]pycore_gil.h -
[.Include.internal]pycore_hamt.h -
[.Include.internal]pycore_initconfig.h -
[.Include.internal]pycore_object.h -
[.Include.internal]pycore_pathconfig.h -
[.Include.internal]pycore_pyerrors.h -
[.Include.internal]pycore_pyhash.h -
[.Include.internal]pycore_pylifecycle.h -
[.Include.internal]pycore_pymem.h -
[.Include.internal]pycore_pystate.h -
[.Include.internal]pycore_traceback.h -
[.Include.internal]pycore_tupleobject.h -
[.Include.internal]pycore_warnings.h

MODULE_OBJS = -
[.$(OBJ_DIR).Modules]config.obj -
[.$(OBJ_DIR).Modules]getpath.obj -
[.$(OBJ_DIR).Modules]main.obj -
[.$(OBJ_DIR).Modules]gcmodule.obj

IO_OBJS = -
[.$(OBJ_DIR).Modules._io]_iomodule.obj -
[.$(OBJ_DIR).Modules._io]iobase.obj -
[.$(OBJ_DIR).Modules._io]fileio.obj -
[.$(OBJ_DIR).Modules._io]bufferedio.obj -
[.$(OBJ_DIR).Modules._io]textio.obj -
[.$(OBJ_DIR).Modules._io]bytesio.obj -
[.$(OBJ_DIR).Modules._io]stringio.obj

POBJS = -
[.$(OBJ_DIR).Parser]acceler.obj -
[.$(OBJ_DIR).Parser]grammar1.obj -
[.$(OBJ_DIR).Parser]listnode.obj -
[.$(OBJ_DIR).Parser]node.obj -
[.$(OBJ_DIR).Parser]parser.obj -
[.$(OBJ_DIR).Parser]token.obj

PARSER_OBJS = $(POBJS) -
[.$(OBJ_DIR).Parser]myreadline.obj -
[.$(OBJ_DIR).vms]stdioreadline.obj -
[.$(OBJ_DIR).Parser]parsetok.obj -
[.$(OBJ_DIR).Parser]tokenizer.obj

PYTHON_OBJS = -
[.$(OBJ_DIR).Python]_warnings.obj -
[.$(OBJ_DIR).Python]Python-ast.obj -
[.$(OBJ_DIR).Python]asdl.obj -
[.$(OBJ_DIR).Python]ast.obj -
[.$(OBJ_DIR).Python]ast_opt.obj -
[.$(OBJ_DIR).Python]ast_unparse.obj -
[.$(OBJ_DIR).Python]bltinmodule.obj -
[.$(OBJ_DIR).Python]ceval.obj -
[.$(OBJ_DIR).Python]codecs.obj -
[.$(OBJ_DIR).Python]compile.obj -
[.$(OBJ_DIR).Python]context.obj -
[.$(OBJ_DIR).Python]dynamic_annotations.obj -
[.$(OBJ_DIR).Python]errors.obj -
[.$(OBJ_DIR).Python]frozenmain.obj -
[.$(OBJ_DIR).Python]future.obj -
[.$(OBJ_DIR).Python]getargs.obj -
[.$(OBJ_DIR).Python]getcompiler.obj -
[.$(OBJ_DIR).Python]getcopyright.obj -
[.$(OBJ_DIR).Python]getplatform.obj -
[.$(OBJ_DIR).Python]getversion.obj -
[.$(OBJ_DIR).Python]graminit.obj -
[.$(OBJ_DIR).Python]hamt.obj -
[.$(OBJ_DIR).Python]import.obj -
[.$(OBJ_DIR).Python]importdl.obj -
[.$(OBJ_DIR).Python]initconfig.obj -
[.$(OBJ_DIR).Python]marshal.obj -
[.$(OBJ_DIR).Python]modsupport.obj -
[.$(OBJ_DIR).Python]mysnprintf.obj -
[.$(OBJ_DIR).Python]mystrtoul.obj -
[.$(OBJ_DIR).Python]pathconfig.obj -
[.$(OBJ_DIR).Python]peephole.obj -
[.$(OBJ_DIR).Python]preconfig.obj -
[.$(OBJ_DIR).Python]pyarena.obj -
[.$(OBJ_DIR).Python]pyctype.obj -
[.$(OBJ_DIR).Python]pyfpe.obj -
[.$(OBJ_DIR).Python]pyhash.obj -
[.$(OBJ_DIR).Python]pylifecycle.obj -
[.$(OBJ_DIR).Python]pymath.obj -
[.$(OBJ_DIR).Python]pystate.obj -
[.$(OBJ_DIR).Python]pythonrun.obj -
[.$(OBJ_DIR).Python]pytime.obj -
[.$(OBJ_DIR).Python]bootstrap_hash.obj -
[.$(OBJ_DIR).Python]structmember.obj -
[.$(OBJ_DIR).Python]symtable.obj -
[.$(OBJ_DIR).Python]sysmodule.obj -
[.$(OBJ_DIR).Python]thread.obj -
[.$(OBJ_DIR).Python]traceback.obj -
[.$(OBJ_DIR).Python]getopt.obj -
[.$(OBJ_DIR).Python]pystrcmp.obj -
[.$(OBJ_DIR).Python]pystrtod.obj -
[.$(OBJ_DIR).Python]pystrhex.obj -
[.$(OBJ_DIR).Python]dtoa.obj -
[.$(OBJ_DIR).Python]formatter_unicode.obj -
[.$(OBJ_DIR).Python]fileutils.obj -
[.$(OBJ_DIR).Python]$(DYNLOADFILE).obj

OBJECT_OBJS = -
[.$(OBJ_DIR).Objects]abstract.obj -
[.$(OBJ_DIR).Objects]accu.obj -
[.$(OBJ_DIR).Objects]boolobject.obj -
[.$(OBJ_DIR).Objects]bytes_methods.obj -
[.$(OBJ_DIR).Objects]bytearrayobject.obj -
[.$(OBJ_DIR).Objects]bytesobject.obj -
[.$(OBJ_DIR).Objects]call.obj -
[.$(OBJ_DIR).Objects]capsule.obj -
[.$(OBJ_DIR).Objects]cellobject.obj -
[.$(OBJ_DIR).Objects]classobject.obj -
[.$(OBJ_DIR).Objects]codeobject.obj -
[.$(OBJ_DIR).Objects]complexobject.obj -
[.$(OBJ_DIR).Objects]descrobject.obj -
[.$(OBJ_DIR).Objects]enumobject.obj -
[.$(OBJ_DIR).Objects]exceptions.obj -
[.$(OBJ_DIR).Objects]genobject.obj -
[.$(OBJ_DIR).Objects]fileobject.obj -
[.$(OBJ_DIR).Objects]floatobject.obj -
[.$(OBJ_DIR).Objects]frameobject.obj -
[.$(OBJ_DIR).Objects]funcobject.obj -
[.$(OBJ_DIR).Objects]interpreteridobject.obj -
[.$(OBJ_DIR).Objects]iterobject.obj -
[.$(OBJ_DIR).Objects]listobject.obj -
[.$(OBJ_DIR).Objects]longobject.obj -
[.$(OBJ_DIR).Objects]dictobject.obj -
[.$(OBJ_DIR).Objects]odictobject.obj -
[.$(OBJ_DIR).Objects]memoryobject.obj -
[.$(OBJ_DIR).Objects]methodobject.obj -
[.$(OBJ_DIR).Objects]moduleobject.obj -
[.$(OBJ_DIR).Objects]namespaceobject.obj -
[.$(OBJ_DIR).Objects]object.obj -
[.$(OBJ_DIR).Objects]obmalloc.obj -
[.$(OBJ_DIR).Objects]picklebufobject.obj -
[.$(OBJ_DIR).Objects]rangeobject.obj -
[.$(OBJ_DIR).Objects]setobject.obj -
[.$(OBJ_DIR).Objects]sliceobject.obj -
[.$(OBJ_DIR).Objects]structseq.obj -
[.$(OBJ_DIR).Objects]tupleobject.obj -
[.$(OBJ_DIR).Objects]typeobject.obj -
[.$(OBJ_DIR).Objects]unicodeobject.obj -
[.$(OBJ_DIR).Objects]unicodectype.obj -
[.$(OBJ_DIR).Objects]weakrefobject.obj

MODOBJS = -
[.$(OBJ_DIR).Modules]posixmodule.obj -
[.$(OBJ_DIR).Modules]errnomodule.obj -
[.$(OBJ_DIR).Modules]pwdmodule.obj -
[.$(OBJ_DIR).Modules]_sre.obj -
[.$(OBJ_DIR).Modules]_codecsmodule.obj -
[.$(OBJ_DIR).Modules]_weakref.obj -
[.$(OBJ_DIR).Modules]_functoolsmodule.obj -
[.$(OBJ_DIR).Modules]_operator.obj -
[.$(OBJ_DIR).Modules]_collectionsmodule.obj -
[.$(OBJ_DIR).Modules]_abc.obj -
[.$(OBJ_DIR).Modules]itertoolsmodule.obj -
[.$(OBJ_DIR).Modules]atexitmodule.obj -
[.$(OBJ_DIR).Modules]signalmodule.obj -
[.$(OBJ_DIR).Modules]_stat.obj -
[.$(OBJ_DIR).Modules]timemodule.obj -
[.$(OBJ_DIR).Modules]_threadmodule.obj -
[.$(OBJ_DIR).Modules]_localemodule.obj -
[.$(OBJ_DIR).Modules._io]_iomodule.obj -
[.$(OBJ_DIR).Modules._io]iobase.obj -
[.$(OBJ_DIR).Modules._io]fileio.obj -
[.$(OBJ_DIR).Modules._io]bytesio.obj -
[.$(OBJ_DIR).Modules._io]bufferedio.obj -
[.$(OBJ_DIR).Modules._io]textio.obj -
[.$(OBJ_DIR).Modules._io]stringio.obj -
[.$(OBJ_DIR).Modules]faulthandler.obj -
[.$(OBJ_DIR).Modules]_tracemalloc.obj -
[.$(OBJ_DIR).Modules]hashtable.obj -
[.$(OBJ_DIR).Modules]symtablemodule.obj -
- ![.$(OBJ_DIR).Modules.vms.decc]decc.obj -
- ![.$(OBJ_DIR).Modules.vms.decc]decc_wrap.obj -
[.$(OBJ_DIR).Modules]xxsubtype.obj

LIBRARY_OBJS_OMIT_FROZEN = -
[.$(OBJ_DIR).Modules]getbuildinfo.obj -
$(PARSER_OBJS) -
$(OBJECT_OBJS) -
$(PYTHON_OBJS) -
$(MODULE_OBJS) -
$(MODOBJS)

LIBRARY_OBJS = -
$(LIBRARY_OBJS_OMIT_FROZEN) -
[.$(OBJ_DIR).Python]frozen.obj

DTRACE_DEPS = -
[.$(OBJ_DIR).Python]ceval.obj -
[.$(OBJ_DIR).Python]import.obj -
[.$(OBJ_DIR).Python]sysmodule.obj -
[.$(OBJ_DIR).Modules]gcmodule.obj

[.$(OBJ_DIR).Modules]_math.obj : [.Modules]_math.c [.Modules]_math.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules]config.obj : [.Modules]config.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules]gcmodule.obj : [.Modules]gcmodule.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules]grpmodule.obj : [.Modules]grpmodule.c [.Modules]posixmodule.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules]main.obj : [.Modules]main.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]abstract.obj : [.Objects]abstract.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]accu.obj : [.Objects]accu.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]boolobject.obj : [.Objects]boolobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]bytearrayobject.obj : [.Objects]bytearrayobject.c $(BYTESTR_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]bytes_methods.obj : [.Objects]bytes_methods.c $(BYTESTR_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]bytesobject.obj : [.Objects]bytesobject.c $(BYTESTR_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]call.obj : [.Objects]call.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]capsule.obj : [.Objects]capsule.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]cellobject.obj : [.Objects]cellobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]classobject.obj : [.Objects]classobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]codeobject.obj : [.Objects]codeobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]complexobject.obj : [.Objects]complexobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]descrobject.obj : [.Objects]descrobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]dictobject.obj : [.Objects]dictobject.c [.Objects.stringlib]eq.h [.Objects]dict-common.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]enumobject.obj : [.Objects]enumobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]exceptions.obj : [.Objects]exceptions.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]fileobject.obj : [.Objects]fileobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]floatobject.obj : [.Objects]floatobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]frameobject.obj : [.Objects]frameobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]funcobject.obj : [.Objects]funcobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]genobject.obj : [.Objects]genobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]iterobject.obj : [.Objects]iterobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]listobject.obj : [.Objects]listobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]longobject.obj : [.Objects]longobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]memoryobject.obj : [.Objects]memoryobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]methodobject.obj : [.Objects]methodobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]moduleobject.obj : [.Objects]moduleobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]namespaceobject.obj : [.Objects]namespaceobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]object.obj : [.Objects]object.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]obmalloc.obj : [.Objects]obmalloc.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]odictobject.obj : [.Objects]odictobject.c [.Objects]dict-common.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]picklebufobject.obj : [.Objects]picklebufobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]rangeobject.obj : [.Objects]rangeobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]setobject.obj : [.Objects]setobject.c [.Objects.stringlib]eq.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]sliceobject.obj : [.Objects]sliceobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]structseq.obj : [.Objects]structseq.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]tupleobject.obj : [.Objects]tupleobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]typeobject.obj : [.Objects]typeobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]unicodectype.obj : [.Objects]unicodectype.c [.Objects]unicodetype_db.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]unicodeobject.obj : [.Objects]unicodeobject.c $(UNICODE_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Objects]weakrefobject.obj : [.Objects]weakrefobject.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Parser]acceler.obj : [.Parser]acceler.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Parser]grammar1.obj : [.Parser]grammar1.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Parser]listnode.obj : [.Parser]listnode.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Parser]myreadline.obj : [.Parser]myreadline.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Parser]node.obj : [.Parser]node.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Parser]parser.obj : [.Parser]parser.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Parser]parsetok.obj : [.Parser]parsetok.c [.Include]graminit.h [.Include]Python-ast.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Parser]token.obj : [.Parser]token.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Parser]tokenizer.obj : [.Parser]tokenizer.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Programs]_freeze_importlib.obj : [.Programs]_freeze_importlib.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Programs]python.obj : [.Programs]python.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]Python-ast.obj : [.Python]Python-ast.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]_warnings.obj : [.Python]_warnings.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]asdl.obj : [.Python]asdl.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]ast.obj : [.Python]ast.c [.Include]graminit.h [.Include]Python-ast.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]ast_opt.obj : [.Python]ast_opt.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]ast_unparse.obj : [.Python]ast_unparse.c [.Include]graminit.h [.Include]Python-ast.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]bltinmodule.obj : [.Python]bltinmodule.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]bootstrap_hash.obj : [.Python]bootstrap_hash.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]ceval.obj : [.Python]ceval.c [.Python]opcode_targets.h [.Python]ceval_gil.h [.Python]condvar.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]codecs.obj : [.Python]codecs.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]compile.obj : [.Python]compile.c [.Include]graminit.h [.Include]Python-ast.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]context.obj : [.Python]context.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]dtoa.obj : [.Python]dtoa.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]dynamic_annotations.obj : [.Python]dynamic_annotations.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]errors.obj : [.Python]errors.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]fileutils.obj : [.Python]fileutils.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]formatter_unicode.obj : [.Python]formatter_unicode.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]frozen.obj : [.Python]frozen.c [.Python]importlib.h [.Python]importlib_external.h [.Python]importlib_zipimport.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]frozenmain.obj : [.Python]frozenmain.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]future.obj : [.Python]future.c [.Include]graminit.h [.Include]Python-ast.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]getargs.obj : [.Python]getargs.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]getcompiler.obj : [.Python]getcompiler.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]getcopyright.obj : [.Python]getcopyright.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]getopt.obj : [.Python]getopt.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]getversion.obj : [.Python]getversion.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]graminit.obj : [.Python]graminit.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]hamt.obj : [.Python]hamt.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]import.obj : [.Python]import.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]importdl.obj : [.Python]importdl.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]initconfig.obj : [.Python]initconfig.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]marshal.obj : [.Python]marshal.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]modsupport.obj : [.Python]modsupport.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]mysnprintf.obj : [.Python]mysnprintf.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]mystrtoul.obj : [.Python]mystrtoul.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]pathconfig.obj : [.Python]pathconfig.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]peephole.obj : [.Python]peephole.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]preconfig.obj : [.Python]preconfig.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]pyarena.obj : [.Python]pyarena.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]pyctype.obj : [.Python]pyctype.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]pyfpe.obj : [.Python]pyfpe.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]pyhash.obj : [.Python]pyhash.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]pylifecycle.obj : [.Python]pylifecycle.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]pymath.obj : [.Python]pymath.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]pystate.obj : [.Python]pystate.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]pystrcmp.obj : [.Python]pystrcmp.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]pystrhex.obj : [.Python]pystrhex.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]pystrtod.obj : [.Python]pystrtod.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]pythonrun.obj : [.Python]pythonrun.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]pytime.obj : [.Python]pytime.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]structmember.obj : [.Python]structmember.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]symtable.obj : [.Python]symtable.c [.Include]graminit.h [.Include]Python-ast.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]thread.obj : [.Python]thread.c [.Python]thread_nt.h [.Python]thread_pthread.h [.Python]condvar.h $(PYTHON_HEADERS)
[.$(OBJ_DIR).Python]traceback.obj : [.Python]traceback.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).vms]vms_crtl_init.obj : [.vms]vms_crtl_init.c
[.$(OBJ_DIR).vms]stdioreadline.obj : [.vms]stdioreadline.c

[.$(OBJ_DIR).Objects]interpreteridobject.obj : [.Objects]interpreteridobject.c $(PYTHON_HEADERS) [.vms]format_macros.h
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_CFLAGS) /OBJECT=$(MMS$TARGET) /FIRST=[.vms]format_macros.h $(MMS$SOURCE)

[.$(OBJ_DIR).Programs]_testembed.obj : [.Programs]_testembed.c $(PYTHON_HEADERS) [.vms]format_macros.h
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_CFLAGS) /OBJECT=$(MMS$TARGET) /FIRST=[.vms]format_macros.h $(MMS$SOURCE)

! no git info, just dependency
[.$(OBJ_DIR).Modules]getbuildinfo.obj : [.Modules]getbuildinfo.c $(PYTHON_HEADERS) -
    $(PARSER_OBJS) -
    $(OBJECT_OBJS) -
    $(PYTHON_OBJS) -
    $(MODULE_OBJS) -
    $(MODOBJS)

[.$(OBJ_DIR).Modules]getpath.obj : [.Modules]getpath.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
	$(CC) $(PY_CORE_CFLAGS_GETPATH) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Python]dynload_shlib.obj : [.Python]dynload_shlib.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_CFLAGS_SHLIB) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Python]dynload_hpux.obj : [.Python]dynload_hpux.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_CFLAGS_HPUX) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Python]sysmodule.obj : [.Python]sysmodule.c [.Include]pydtrace.h $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_CFLAGS_SYSMODULE) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Python]getplatform.obj : [.Python]getplatform.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_CFLAGS_GETPLATFORM) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

! PY_BUILTIN_MODULE_CFLAGS

[.$(OBJ_DIR).Modules]posixmodule.obj : [.Modules]posixmodule.c [.Modules]posixmodule.h $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]errnomodule.obj : [.Modules]errnomodule.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]pwdmodule.obj : [.Modules]pwdmodule.c [.Modules]posixmodule.h $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_sre.obj : [.Modules]_sre.c [.Modules]sre.h [.Modules]sre_constants.h [.Modules]sre_lib.h $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_codecsmodule.obj : [.Modules]_codecsmodule.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_weakref.obj : [.Modules]_weakref.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_functoolsmodule.obj : [.Modules]_functoolsmodule.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_operator.obj : [.Modules]_operator.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_collectionsmodule.obj : [.Modules]_collectionsmodule.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_abc.obj : [.Modules]_abc.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]itertoolsmodule.obj : [.Modules]itertoolsmodule.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]atexitmodule.obj : [.Modules]atexitmodule.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]signalmodule.obj : [.Modules]signalmodule.c [.Modules]posixmodule.h $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_stat.obj : [.Modules]_stat.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]timemodule.obj : [.Modules]timemodule.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_threadmodule.obj : [.Modules]_threadmodule.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_localemodule.obj : [.Modules]_localemodule.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

! _io

[.$(OBJ_DIR).Modules._io]_iomodule.obj : [.Modules._io]_iomodule.c $(IO_HEADERS) $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS_IO) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._io]iobase.obj : [.Modules._io]iobase.c $(IO_HEADERS) $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS_IO) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._io]fileio.obj : [.Modules._io]fileio.c $(IO_HEADERS) $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS_IO) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._io]bytesio.obj : [.Modules._io]bytesio.c $(IO_HEADERS) $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS_IO) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._io]bufferedio.obj : [.Modules._io]bufferedio.c $(IO_HEADERS) $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS_IO) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._io]textio.obj : [.Modules._io]textio.c $(IO_HEADERS) $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS_IO) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._io]stringio.obj : [.Modules._io]stringio.c $(IO_HEADERS) $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS_IO) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]faulthandler.obj : [.Modules]faulthandler.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_tracemalloc.obj : [.Modules]_tracemalloc.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]hashtable.obj : [.Modules]hashtable.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]symtablemodule.obj : [.Modules]symtablemodule.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]xxsubtype.obj : [.Modules]xxsubtype.c $(PYTHON_HEADERS)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_BUILTIN_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OUT_DIR)]python$shr.exe : [.$(OUT_DIR)]libpython3.olb
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[000000]$(NOTDIR $(MMS$TARGET_NAME)).EXE [.opt]$(PYTHON$SHR_OPT).opt/OPT

[.$(OUT_DIR)]python3.exe : [.$(OBJ_DIR).Programs]python.obj,[.$(OBJ_DIR).vms]vms_crtl_init.obj,[.$(OUT_DIR)]python$shr.exe
   @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/EXECUTABLE=python$build_out:[000000]$(NOTDIR $(MMS$TARGET_NAME)).EXE [.$(OBJ_DIR).vms]vms_crtl_init.obj,[.$(OBJ_DIR).Programs]python.obj,[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT
    ! [.$(OUT_DIR)]python3.exe is built => python$build_out:[000000]$(NOTDIR $(MMS$TARGET_NAME)).EXE

[.$(OUT_DIR)]libpython3.olb : [.$(OUT_DIR)]libpython3.olb($(LIBRARY_OBJS))
    continue

! _decc
[.$(OBJ_DIR).Modules.vms.decc]decc_wrap.obm : [.Modules.vms.decc]decc_wrap.c
[.$(OBJ_DIR).Modules.vms.decc]decc.obm : [.Modules.vms.decc]decc.c
[.$(OUT_DIR).$(DYNLOAD_DIR)]_decc.exe : [.$(OBJ_DIR).Modules.vms.decc]decc_wrap.obm,[.$(OBJ_DIR).Modules.vms.decc]decc.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _struct
[.$(OBJ_DIR).Modules]_struct.obm : [.Modules]_struct.c
[.$(OUT_DIR).$(DYNLOAD_DIR)]_struct.exe : [.$(OBJ_DIR).Modules]_struct.obm

! array
[.$(OBJ_DIR).Modules]arraymodule.obm : [.Modules]arraymodule.c
[.$(OUT_DIR).$(DYNLOAD_DIR)]array.exe : [.$(OBJ_DIR).Modules]arraymodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _contextvars
[.$(OBJ_DIR).Modules]_contextvarsmodule.obm : [.Modules]_contextvarsmodule.c
[.$(OUT_DIR).$(DYNLOAD_DIR)]_contextvars.exe : [.$(OBJ_DIR).Modules]_contextvarsmodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

!! modulename modulesource
! [.$(OBJ_DIR).Modules]modulesource.obm : [.Modules]modulesource.c
! [.$(OUT_DIR).$(DYNLOAD_DIR)]modulename.exe : [.$(OBJ_DIR).Modules]modulesource.obm
!     @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
!     $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

!! modulename modulesource
! [.$(OBJ_DIR).Modules]modulesource.obm : [.Modules]modulesource.c
! [.$(OUT_DIR).$(DYNLOAD_DIR)]modulename.exe : [.$(OBJ_DIR).Modules]modulesource.obm
!     @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
!     $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _freeze_importlib
[.$(OUT_DIR).Programs]_freeze_importlib.exe : [.$(OBJ_DIR).Programs]_freeze_importlib.obj $(LIBRARY_OBJS_OMIT_FROZEN)
    pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)/NODEBUG/NOMAP/EXEC=python$build_out:[Programs]$(NOTDIR $(MMS$TARGET_NAME)).EXE [.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.Python]importlib_external.h : [.Lib.importlib]_bootstrap_external.py [.$(OUT_DIR).Programs]_freeze_importlib.exe
    mcr [.$(OUT_DIR).Programs]_freeze_importlib.exe importlib._bootstrap_external Lib/importlib/_bootstrap_external.py Python/importlib_external.h

[.Python]importlib.h : [.Lib.importlib]_bootstrap.py [.$(OUT_DIR).Programs]_freeze_importlib.exe
    mcr [.$(OUT_DIR).Programs]_freeze_importlib.exe importlib._bootstrap Lib/importlib/_bootstrap.py Python/importlib.h

[.Python]importlib_zipimport.h : [.Lib]zipimport.py [.$(OUT_DIR).Programs]_freeze_importlib.exe
    mcr [.$(OUT_DIR).Programs]_freeze_importlib.exe zipimport Lib/zipimport.py Python/importlib_zipimport.h


! [.$(OUT_DIR).$(DYNLOAD_DIR)]posix$(EXT_SUFFIX) : [.$(OBJ_DIR).Modules]posixmodule.o
!     $(BLDSHARED)  Modules/posixmodule.o   -o Modules/posix$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]errno$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]errnomodule.o; $(BLDSHARED)  Modules/errnomodule.o   -o Modules/errno$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]pwd$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]pwdmodule.o; $(BLDSHARED)  Modules/pwdmodule.o   -o Modules/pwd$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]_sre$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]_sre.o; $(BLDSHARED)  Modules/_sre.o   -o Modules/_sre$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]_codecs$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]_codecsmodule.o; $(BLDSHARED)  Modules/_codecsmodule.o   -o Modules/_codecs$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]_weakref$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]_weakref.o; $(BLDSHARED)  Modules/_weakref.o   -o Modules/_weakref$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]_functools$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]_functoolsmodule.o; $(BLDSHARED)  Modules/_functoolsmodule.o   -o Modules/_functools$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]_operator$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]_operator.o; $(BLDSHARED)  Modules/_operator.o   -o Modules/_operator$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]_collections$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]_collectionsmodule.o; $(BLDSHARED)  Modules/_collectionsmodule.o   -o Modules/_collections$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]_abc$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]_abc.o; $(BLDSHARED)  Modules/_abc.o   -o Modules/_abc$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]itertools$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]itertoolsmodule.o; $(BLDSHARED)  Modules/itertoolsmodule.o   -o Modules/itertools$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]atexit$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]atexitmodule.o; $(BLDSHARED)  Modules/atexitmodule.o   -o Modules/atexit$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]_signal$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]signalmodule.o; $(BLDSHARED)  Modules/signalmodule.o   -o Modules/_signal$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]_stat$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]_stat.o; $(BLDSHARED)  Modules/_stat.o   -o Modules/_stat$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]time$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]timemodule.o; $(BLDSHARED)  Modules/timemodule.o   -o Modules/time$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]_thread$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]_threadmodule.o; $(BLDSHARED)  Modules/_threadmodule.o   -o Modules/_thread$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]_locale$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]_localemodule.o; $(BLDSHARED)  Modules/_localemodule.o   -o Modules/_locale$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]_io$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]_iomodule.o Modules/iobase.o Modules/fileio.o Modules/bytesio.o Modules/bufferedio.o Modules/textio.o Modules/stringio.o; $(BLDSHARED)  Modules/_iomodule.o Modules/iobase.o Modules/fileio.o Modules/bytesio.o Modules/bufferedio.o Modules/textio.o Modules/stringio.o   -o Modules/_io$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]faulthandler$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]faulthandler.o; $(BLDSHARED)  Modules/faulthandler.o   -o Modules/faulthandler$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]_tracemalloc$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]_tracemalloc.o Modules/hashtable.o; $(BLDSHARED)  Modules/_tracemalloc.o Modules/hashtable.o   -o Modules/_tracemalloc$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]_symtable$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]symtablemodule.o; $(BLDSHARED)  Modules/symtablemodule.o   -o Modules/_symtable$(EXT_SUFFIX)
! [.$(OUT_DIR).Modules]xxsubtype$(EXT_SUFFIX) :  [.$(OBJ_DIR).Modules]xxsubtype.o; $(BLDSHARED)  Modules/xxsubtype.o   -o Modules/xxsubtype$(EXT_SUFFIX)

