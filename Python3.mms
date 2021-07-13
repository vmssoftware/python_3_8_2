! MMS/EXT/DESCR=Python3.mms/MACRO=("OUTDIR=OUT","CONFIG=DEBUG")
PY_CFLAGS_Q = /NAMES=(AS_IS,SHORTENED)/WARNINGS=WARNINGS=ALL/ACCEPT=NOVAXC_KEYWORDS
PY_CFLAGS_DEF = _USE_STD_STAT,__STDC_FORMAT_MACROS,_LARGEFILE

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
OPT_Q = /DEBUG/NOOPTIMIZE/LIST=$(MMS$TARGET_NAME)/SHOW=ALL
OPT_DEF = _DEBUG
OUT_DIR = $(OUTDIR).$(CONFIG)
OBJ_DIR = $(OUT_DIR).OBJ
LINKFLAGS = /NODEBUG/MAP=[.$(OUT_DIR)]$(NOTDIR $(MMS$TARGET_NAME))/TRACE/DSF=[.$(OUT_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).DSF
PYTHON$SHR_OPT = PYTHON$SHR_DBG
.ELSE
! release
OPT_Q = /NODEBUG/OPTIMIZE/NOLIST
OPT_DEF = _NDEBUG
OUT_DIR = $(OUTDIR).$(CONFIG)
OBJ_DIR = $(OUT_DIR).OBJ
LINKFLAGS = /NODEBUG/NOMAP/NOTRACEBACK
PYTHON$SHR_OPT = PYTHON$SHR
.ENDIF

DYNLOAD_DIR = lib-dynload

DYNLOADFILE = dynload_shlib
PLATFORM = OpenVMS
SOABI = cpython-38-ia64-openvms

PY_CFLAGS_Q = $(OPT_Q)$(PY_CFLAGS_Q)
PY_CFLAGS_DEF = $(OPT_DEF),$(PY_CFLAGS_DEF)

PY_CFLAGS_INC = [],[.Include],[.Include.internal],oss$root:[include],[.vms]
PY_CFLAGS = $(PY_CFLAGS_Q)/DEFINE=($(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

PY_CFLAGS_OSF = $(PY_CFLAGS_Q)/DEFINE=(_OSF_SOURCE,$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))
PY_CFLAGS_OSF_ILE = $(PY_CFLAGS_Q)/DEFINE=(_OSF_SOURCE,$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=([.modules.vms.ile3],$(PY_CFLAGS_INC))

PY_CFLAGS_SQL = $(PY_CFLAGS)/FIRST=[.vms]vms_sqlite3_first.h

PY_CFLAGS_USE_ZLIB = $(PY_CFLAGS_Q)/DEFINE=(USE_ZLIB_CRC32,$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

PY_CFLAGS_EXPAT = $(PY_CFLAGS_Q)/DEFINE=(HAVE_EXPAT_CONFIG_H,XML_POOR_ENTROPY,$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=([.Modules.expat],$(PY_CFLAGS_INC))
PY_CFLAGS_EXPAT_ELEM = $(PY_CFLAGS_Q)/DEFINE=(HAVE_EXPAT_CONFIG_H,USE_PYEXPAT_CAPI,XML_POOR_ENTROPY,$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=([.Modules.expat],$(PY_CFLAGS_INC))

PY_CFLAGS_DECIMAL = $(PY_CFLAGS_Q)/DEFINE=(CONFIG_32,ANSI,$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=([.Modules._decimal.libmpdec],$(PY_CFLAGS_INC))

PY_CFLAGS_MULT = $(PY_CFLAGS_Q)/DEFINE=($(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=([.Modules._multiprocessing],$(PY_CFLAGS_INC))

PY_CORE_MODULE_CFLAGS = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE_MODULE",$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

PY_CORE_BUILTIN_CFLAGS = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE_BUILTIN",$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))
PY_CORE_CFLAGS = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE",$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

GETPATH_DEF = PYTHONPATH="""""",PREFIX="""/usr/local""",EXEC_PREFIX="""/usr/local""",VERSION="""3.8""",VPATH=""""""
PY_CORE_CFLAGS_GETPATH = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE",$(GETPATH_DEF),$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

SOABI_DEF = SOABI="""$(SOABI)"""
PY_CORE_CFLAGS_SHLIB = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE",$(SOABI_DEF),$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

HPUX_DEF = SHLIB_EXT=""".EXE"""
PY_CORE_CFLAGS_HPUX = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE",$(HPUX_DEF),$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

SYSMODULE_DEF = ABIFLAGS="""""",MULTIARCH="""$(SOABI)"""
PY_CORE_CFLAGS_SYSMODULE = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE",$(SYSMODULE_DEF),$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

GETPLATFORM_DEF = PLATFORM="""$(PLATFORM)"""
PY_CORE_CFLAGS_GETPLATFORM = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE",$(GETPLATFORM_DEF),$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

IO_INC = [.Modules._io]
PY_CORE_BUILTIN_CFLAGS_IO = $(PY_CFLAGS_Q)/DEFINE=("Py_BUILD_CORE_BUILTIN",$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC),$(IO_INC))

PY_OSF_CFLAGS = $(PY_CFLAGS_Q)/DEFINE=(_OSF_SOURCE,$(PY_CFLAGS_DEF))/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC))

PY_CFLAGS_DTR = $(PY_CFLAGS_Q)/DEFINE=($(PY_CFLAGS_DEF),__PYTHON)/INCLUDE_DIRECTORY=($(PY_CFLAGS_INC),dtr$library)

.FIRST
    ! defines for nested includes, like:
    ! #include "clinic/transmogrify.h.h"
    define clinic [.Objects.clinic],[.Python.clinic],[.Modules.clinic],[.Modules._io.clinic],[.Modules.cjkcodecs.clinic],[.Objects.stringlib.clinic],[.Modules._blake2.clinic],[.Modules._sha3.clinic],[.Modules._multiprocessing.clinic]
    define stringlib [.Objects.stringlib]
    define modules [.Modules]
    define readline oss$root:[include.readline]
    define lzma oss$root:[include.lzma]
    define cpython [.Include.cpython]
    define internal [.Include.internal]
    define _ssl [.Modules._ssl]
    define impl [.Modules._blake2.impl]
    define kcp  [.Modules._sha3.kcp]
    define ctypes [.Modules._ctypes]
    ! SWIG
    swig :== $swig$root:[bin]swig.exe
    ! OPENSSL 111
    define openssl ssl111$include:
    ! names
    BUILD_OUT_DIR = F$ENVIRONMENT("DEFAULT")-"]"+".$(OUT_DIR).]"
    BUILD_OBJ_DIR = F$ENVIRONMENT("DEFAULT")-"]"+".$(OBJ_DIR).]"
    define /trans=concealed python$build_out 'BUILD_OUT_DIR'
    define /trans=concealed python$build_obj 'BUILD_OBJ_DIR'
    ! SQL
    sqlmod :==  mcr sql$mod

.SUFFIXES
.SUFFIXES .EXE .OLB .OBJ .OBM .OBD .OBS .C

.C.OBJ
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

.OBJ.OLB
    @ IF F$SEARCH("$(MMS$TARGET)") .EQS. "" -
        THEN $(LIBR)/CREATE $(MMS$TARGET)
    $(LIBR) $(MMS$TARGET) $(MMS$SOURCE)

! modules hack
.C.OBM
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

.OBM.EXE
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

.C.OBD
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_DECIMAL) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

.OBD.EXE
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

.C.OBS
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_OSF) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

.OBS.EXE
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

.IFDEF MMSIA64
LIBDYNLOAD_VMS_IA = -
- ! [.$(OUT_DIR).$(DYNLOAD_DIR)]_dtr.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_rdb.exe
.ENDIF

LIBDYNLOAD_VMS = -
$(LIBDYNLOAD_VMS_IA) -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_accdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_acldef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_acrdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_armdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_brkdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_capdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_chpdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_ciadef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_clidef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_cmbdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_cvtfnmdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_dcdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_decc.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_dmtdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_dpsdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_dscdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_dvidef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_dvsdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_efndef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_eradef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_fabdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_fdldef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_fpdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_fscndef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_iccdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_ile3.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_iledef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_impdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_initdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_iodef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_issdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_jbcmsgdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_jpidef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_kgbdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_lckdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_lib.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_libclidef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_libdtdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_libfisdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_lkidef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_lnmdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_maildef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_mntdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_nsadef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_ossdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_pcbdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_ppropdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_pqldef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_prcdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_prdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_prvdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_prxdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_pscandef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_psldef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_pxbdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_quidef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_rabdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_regdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_rmidef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_rms.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_rmsdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_rsdmdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_sdvdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_sjcdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_ssdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_statedef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_stenvdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_stsdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_syidef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_sys.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_uafdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_uaidef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_rec.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_secsrvdef.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_secsrvmsg.exe

.IFDEF MMSIA64
LIBDYNLOAD_IA = -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_bz2.exe
.ENDIF

LIBDYNLOAD = -
$(LIBDYNLOAD_VMS) -
$(LIBDYNLOAD_IA) -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_asyncio.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_bisect.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_blake2.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_codecs_cn.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_codecs_hk.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_codecs_iso2022.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_codecs_jp.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_codecs_kr.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_codecs_tw.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_contextvars.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_crypt.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_csv.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_ctypes.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_ctypes_test.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_datetime.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_decimal.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_elementtree.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_gdbm.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_hashlib.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_heapq.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_json.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_lsprof.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_lzma.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_md5.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_multibytecodec.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_multiprocessing.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_opcode.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_pickle.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_posixshmem.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_posixsubprocess.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_queue.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_random.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_sha1.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_sha256.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_sha3.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_sha512.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_socket.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_sqlite3.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_ssl.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_statistics.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_struct.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_testbuffer.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_testcapi.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_testimportmultiple.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_testinternalcapi.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_testmultiphase.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_xxsubinterpreters.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]_xxtestfuzz.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]array.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]audioop.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]binascii.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]cmath.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]fcntl.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]grp.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]math.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]mmap.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]parser.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]pyexpat.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]readline.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]select.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]syslog.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]unicodedata.exe -
[.$(OUT_DIR).$(DYNLOAD_DIR)]zlib.exe
! [.$(OUT_DIR).$(DYNLOAD_DIR)]_uuid.exe

TARGET : [.$(OUT_DIR)]python3.exe [.$(OUT_DIR)]_testembed.exe $(LIBDYNLOAD)
    ! TARGET BUILT

CLEAN :
    del/tree [.$(OUT_DIR)...]*.*;*

pyconfig.h : [.vms]pyconfig.h
    copy [.vms]pyconfig.h []

py_vms.h : [.vms]py_vms.h
    copy [.vms]py_vms.h []

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
py_vms.h -
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
[.$(OBJ_DIR).Parser]parsetok.obj -
[.$(OBJ_DIR).Parser]tokenizer.obj -
[.$(OBJ_DIR).vms]vms_smg_readline.obj

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
[.$(OBJ_DIR).Modules]xxsubtype.obj

LIBRARY_OBJS_OMIT_FROZEN = -
[.$(OBJ_DIR).Modules]getbuildinfo.obj -
[.$(OBJ_DIR).vms]vms_crtl_values.obj -
[.$(OBJ_DIR).vms]vms_select.obj -
[.$(OBJ_DIR).vms]vms_spawn_helper.obj -
[.$(OBJ_DIR).vms]vms_sleep.obj -
[.$(OBJ_DIR).vms]vms_mbx_util.obj -
[.$(OBJ_DIR).vms]vms_fcntl.obj -
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

[.$(OBJ_DIR).vms]vms_smg_readline.obj : [.vms]vms_smg_readline.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).vms]vms_crtl_values.obj : [.vms]vms_crtl_values.c  $(PYTHON_HEADERS)
[.$(OBJ_DIR).vms]vms_select.obj : [.vms]vms_select.c [.vms]vms_select.h [.vms]vms_spawn_helper.h [.vms]vms_sleep.h  $(PYTHON_HEADERS)
[.$(OBJ_DIR).vms]vms_spawn_helper.obj : [.vms]vms_spawn_helper.c [.vms]vms_spawn_helper.h  $(PYTHON_HEADERS)
[.$(OBJ_DIR).vms]vms_sleep.obj : [.vms]vms_sleep.c [.vms]vms_sleep.h  $(PYTHON_HEADERS)
[.$(OBJ_DIR).vms]vms_mbx_util.obj : [.vms]vms_mbx_util.c [.vms]vms_mbx_util.h  $(PYTHON_HEADERS)
[.$(OBJ_DIR).vms]vms_fcntl.obj : [.vms]vms_fcntl.c [.vms]vms_fcntl.h [.vms]vms_mbx_util.h  $(PYTHON_HEADERS)
[.$(OBJ_DIR).vms]vms_crtl_init.obj : [.vms]vms_crtl_init.c

[.$(OBJ_DIR).Objects]interpreteridobject.obj : [.Objects]interpreteridobject.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Programs]_testembed.obj : [.Programs]_testembed.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

! no git info, just dependency
[.$(OBJ_DIR).Modules]getbuildinfo.obj : [.Modules]getbuildinfo.c $(PYTHON_HEADERS) -
    $(PARSER_OBJS) -
    $(OBJECT_OBJS) -
    $(PYTHON_OBJS) -
    $(MODULE_OBJS) -
    $(MODOBJS)

[.$(OBJ_DIR).Modules]getpath.obj : [.Modules]getpath.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
	$(CC) $(PY_CORE_CFLAGS_GETPATH) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Python]dynload_shlib.obj : [.Python]dynload_shlib.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_CFLAGS_SHLIB) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Python]dynload_hpux.obj : [.Python]dynload_hpux.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_CFLAGS_HPUX) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Python]sysmodule.obj : [.Python]sysmodule.c [.Include]pydtrace.h $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_CFLAGS_SYSMODULE) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Python]getplatform.obj : [.Python]getplatform.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_CFLAGS_GETPLATFORM) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

! PY_CORE_BUILTIN_CFLAGS

[.$(OBJ_DIR).Modules]posixmodule.obj : [.Modules]posixmodule.c [.Modules]posixmodule.h $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]errnomodule.obj : [.Modules]errnomodule.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]pwdmodule.obj : [.Modules]pwdmodule.c [.Modules]posixmodule.h $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_sre.obj : [.Modules]_sre.c [.Modules]sre.h [.Modules]sre_constants.h [.Modules]sre_lib.h $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_codecsmodule.obj : [.Modules]_codecsmodule.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_weakref.obj : [.Modules]_weakref.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_functoolsmodule.obj : [.Modules]_functoolsmodule.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_operator.obj : [.Modules]_operator.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_collectionsmodule.obj : [.Modules]_collectionsmodule.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_abc.obj : [.Modules]_abc.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]itertoolsmodule.obj : [.Modules]itertoolsmodule.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]atexitmodule.obj : [.Modules]atexitmodule.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]signalmodule.obj : [.Modules]signalmodule.c [.Modules]posixmodule.h $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_stat.obj : [.Modules]_stat.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]timemodule.obj : [.Modules]timemodule.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_threadmodule.obj : [.Modules]_threadmodule.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_localemodule.obj : [.Modules]_localemodule.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

! _io

[.$(OBJ_DIR).Modules._io]_iomodule.obj : [.Modules._io]_iomodule.c $(IO_HEADERS) $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS_IO) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._io]iobase.obj : [.Modules._io]iobase.c $(IO_HEADERS) $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS_IO) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._io]fileio.obj : [.Modules._io]fileio.c $(IO_HEADERS) $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS_IO) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._io]bytesio.obj : [.Modules._io]bytesio.c $(IO_HEADERS) $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS_IO) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._io]bufferedio.obj : [.Modules._io]bufferedio.c $(IO_HEADERS) $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS_IO) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._io]textio.obj : [.Modules._io]textio.c $(IO_HEADERS) $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS_IO) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._io]stringio.obj : [.Modules._io]stringio.c $(IO_HEADERS) $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS_IO) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]faulthandler.obj : [.Modules]faulthandler.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]_tracemalloc.obj : [.Modules]_tracemalloc.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]hashtable.obj : [.Modules]hashtable.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]symtablemodule.obj : [.Modules]symtablemodule.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules]xxsubtype.obj : [.Modules]xxsubtype.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_BUILTIN_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

! python$shr.exe
[.$(OUT_DIR)]python$shr.exe : [.$(OUT_DIR)]libpython3^.8.olb
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[000000]$(NOTDIR $(MMS$TARGET_NAME)).EXE [.opt]$(PYTHON$SHR_OPT).opt/OPT

! python3.exe
[.$(OUT_DIR)]python3.exe : [.$(OBJ_DIR).Programs]python.obj,[.$(OBJ_DIR).vms]vms_crtl_init.obj,[.$(OUT_DIR)]python$shr.exe
   @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/THREADS/EXECUTABLE=python$build_out:[000000]$(NOTDIR $(MMS$TARGET_NAME)).EXE [.$(OBJ_DIR).vms]vms_crtl_init.obj,[.$(OBJ_DIR).Programs]python.obj,[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT
    ! [.$(OUT_DIR)]python3.exe is built => python$build_out:[000000]$(NOTDIR $(MMS$TARGET_NAME)).EXE

! _testembed.exe
[.$(OUT_DIR)]_testembed.exe : [.$(OBJ_DIR).Programs]_testembed.obj,[.$(OBJ_DIR).vms]vms_crtl_init.obj,[.$(OUT_DIR)]python$shr.exe
   @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/EXECUTABLE=python$build_out:[000000]$(NOTDIR $(MMS$TARGET_NAME)).EXE [.$(OBJ_DIR).vms]vms_crtl_init.obj,$(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! libpython3^.8.olb
[.$(OUT_DIR)]libpython3^.8.olb : [.$(OUT_DIR)]libpython3^.8.olb($(LIBRARY_OBJS))
    continue

! _freeze_importlib
[.$(OUT_DIR).Programs]_freeze_importlib.exe : [.$(OBJ_DIR).Programs]_freeze_importlib.obj [.$(OBJ_DIR).vms]vms_crtl_init.obj $(LIBRARY_OBJS_OMIT_FROZEN)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)/NODEBUG/NOMAP/EXECUTABLE=python$build_out:[Programs]$(NOTDIR $(MMS$TARGET_NAME)).EXE [.$(OBJ_DIR).vms]vms_crtl_init.obj,$(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.Python]importlib_external.h : [.Lib.importlib]_bootstrap_external.py [.$(OUT_DIR).Programs]_freeze_importlib.exe
    mcr [.$(OUT_DIR).Programs]_freeze_importlib.exe importlib._bootstrap_external Lib/importlib/_bootstrap_external.py Python/importlib_external.h

[.Python]importlib.h : [.Lib.importlib]_bootstrap.py [.$(OUT_DIR).Programs]_freeze_importlib.exe
    mcr [.$(OUT_DIR).Programs]_freeze_importlib.exe importlib._bootstrap Lib/importlib/_bootstrap.py Python/importlib.h

[.Python]importlib_zipimport.h : [.Lib]zipimport.py [.$(OUT_DIR).Programs]_freeze_importlib.exe
    mcr [.$(OUT_DIR).Programs]_freeze_importlib.exe zipimport Lib/zipimport.py Python/importlib_zipimport.h

! _struct
[.$(OBJ_DIR).Modules]_struct.obm : [.Modules]_struct.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_struct.exe : [.$(OBJ_DIR).Modules]_struct.obm

! array
[.$(OBJ_DIR).Modules]arraymodule.obm : [.Modules]arraymodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]array.exe : [.$(OBJ_DIR).Modules]arraymodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _contextvars
[.$(OBJ_DIR).Modules]_contextvarsmodule.obm : [.Modules]_contextvarsmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_contextvars.exe : [.$(OBJ_DIR).Modules]_contextvarsmodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! math mathmodule
[.$(OBJ_DIR).Modules]mathmodule.obm : [.Modules]mathmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]math.exe : [.$(OBJ_DIR).Modules]mathmodule.obm, [.$(OBJ_DIR).Modules]_math.obj
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! cmath cmathmodule
[.$(OBJ_DIR).Modules]cmathmodule.obm : [.Modules]cmathmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]cmath.exe : [.$(OBJ_DIR).Modules]cmathmodule.obm, [.$(OBJ_DIR).Modules]_math.obj
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _datetime _datetimemodule
[.$(OBJ_DIR).Modules]_datetimemodule.obm : [.Modules]_datetimemodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_datetime.exe : [.$(OBJ_DIR).Modules]_datetimemodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _random _randommodule
[.$(OBJ_DIR).Modules]_randommodule.obm : [.Modules]_randommodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_random.exe : [.$(OBJ_DIR).Modules]_randommodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _bisect _bisectmodule
[.$(OBJ_DIR).Modules]_bisectmodule.obm : [.Modules]_bisectmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_bisect.exe : [.$(OBJ_DIR).Modules]_bisectmodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _heapq _heapqmodule
[.$(OBJ_DIR).Modules]_heapqmodule.obm : [.Modules]_heapqmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_heapq.exe : [.$(OBJ_DIR).Modules]_heapqmodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _pickle _pickle
[.$(OBJ_DIR).Modules]_pickle.obm : [.Modules]_pickle.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OUT_DIR).$(DYNLOAD_DIR)]_pickle.exe : [.$(OBJ_DIR).Modules]_pickle.obm

! _json _json
[.$(OBJ_DIR).Modules]_json.obm : [.Modules]_json.c $(PYTHON_HEADERS)
  @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OUT_DIR).$(DYNLOAD_DIR)]_json.exe : [.$(OBJ_DIR).Modules]_json.obm

! _lsprof _lsprof rotatingtree
[.$(OBJ_DIR).Modules]_lsprof.obm : [.Modules]_lsprof.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules]rotatingtree.obm : [.Modules]rotatingtree.c
[.$(OUT_DIR).$(DYNLOAD_DIR)]_lsprof.exe : [.$(OBJ_DIR).Modules]_lsprof.obm,[.$(OBJ_DIR).Modules]rotatingtree.obm

! unicodedata unicodedata
[.$(OBJ_DIR).Modules]unicodedata.obm : [.Modules]unicodedata.c,[.Modules]unicodedata_db.h,[.Modules]unicodename_db.h,$(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]unicodedata.exe : [.$(OBJ_DIR).Modules]unicodedata.obm

! _opcode _opcode
[.$(OBJ_DIR).Modules]_opcode.obm : [.Modules]_opcode.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_opcode.exe : [.$(OBJ_DIR).Modules]_opcode.obm

! _asyncio _asynciomodule
[.$(OBJ_DIR).Modules]_asynciomodule.obm : [.Modules]_asynciomodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_asyncio.exe : [.$(OBJ_DIR).Modules]_asynciomodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _queue _queuemodule
[.$(OBJ_DIR).Modules]_queuemodule.obm : [.Modules]_queuemodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_queue.exe : [.$(OBJ_DIR).Modules]_queuemodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _statistics _statisticsmodule
[.$(OBJ_DIR).Modules]_statisticsmodule.obm : [.Modules]_statisticsmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_statistics.exe : [.$(OBJ_DIR).Modules]_statisticsmodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! fcntl fcntlmodule
[.$(OBJ_DIR).Modules]fcntlmodule.obm : [.Modules]fcntlmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]fcntl.exe : [.$(OBJ_DIR).Modules]fcntlmodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! grp grpmodule
[.$(OBJ_DIR).Modules]grpmodule.obm : [.Modules]grpmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]grp.exe : [.$(OBJ_DIR).Modules]grpmodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! select selectmodule
[.$(OBJ_DIR).Modules]selectmodule.obm : [.Modules]selectmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]select.exe : [.$(OBJ_DIR).Modules]selectmodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! parser parsermodule
[.$(OBJ_DIR).Modules]parsermodule.obm : [.Modules]parsermodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]parser.exe : [.$(OBJ_DIR).Modules]parsermodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! mmap mmapmodule
[.$(OBJ_DIR).Modules]mmapmodule.obm : [.Modules]mmapmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]mmap.exe : [.$(OBJ_DIR).Modules]mmapmodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! syslog syslogmodule [vms]syslog
[.$(OBJ_DIR).vms]syslog.obm : [.vms]syslog.c [.vms]syslog.h
[.$(OBJ_DIR).Modules]syslogmodule.obm : [.Modules]syslogmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]syslog.exe : [.$(OBJ_DIR).vms]syslog.obm,[.$(OBJ_DIR).Modules]syslogmodule.obm

! _xxsubinterpreters _xxsubinterpretersmodule
[.$(OBJ_DIR).Modules]_xxsubinterpretersmodule.obm : [.Modules]_xxsubinterpretersmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_xxsubinterpreters.exe : [.$(OBJ_DIR).Modules]_xxsubinterpretersmodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! audioop audioop
[.$(OBJ_DIR).Modules]audioop.obm : [.Modules]audioop.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]audioop.exe : [.$(OBJ_DIR).Modules]audioop.obm

! _csv _csv
[.$(OBJ_DIR).Modules]_csv.obm : [.Modules]_csv.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_csv.exe : [.$(OBJ_DIR).Modules]_csv.obm

! _posixsubprocess _posixsubprocess
[.$(OBJ_DIR).Modules]_posixsubprocess.obm : [.Modules]_posixsubprocess.c [.vms]vms_spawn_helper.h $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_posixsubprocess.exe : [.$(OBJ_DIR).Modules]_posixsubprocess.obm

! _testcapi _testcapimodule
[.$(OBJ_DIR).Modules]_testcapimodule.obm : [.Modules]_testcapimodule.c [.Modules]testcapi_long.h $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_testcapi.exe : [.$(OBJ_DIR).Modules]_testcapimodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _testinternalcapi _testinternalcapi
[.$(OBJ_DIR).Modules]_testinternalcapi.obm : [.Modules]_testinternalcapi.c $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CORE_MODULE_CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OUT_DIR).$(DYNLOAD_DIR)]_testinternalcapi.exe : [.$(OBJ_DIR).Modules]_testinternalcapi.obm

! _testbuffer _testbuffer
[.$(OBJ_DIR).Modules]_testbuffer.obm : [.Modules]_testbuffer.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_testbuffer.exe : [.$(OBJ_DIR).Modules]_testbuffer.obm

! _testimportmultiple _testimportmultiple
[.$(OBJ_DIR).Modules]_testimportmultiple.obm : [.Modules]_testimportmultiple.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_testimportmultiple.exe : [.$(OBJ_DIR).Modules]_testimportmultiple.obm

! _testmultiphase _testmultiphase
[.$(OBJ_DIR).Modules]_testmultiphase.obm : [.Modules]_testmultiphase.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_testmultiphase.exe : [.$(OBJ_DIR).Modules]_testmultiphase.obm

! _xxtestfuzz [_xxtestfuzz]_xxtestfuzz [_xxtestfuzz]fuzzer
[.$(OBJ_DIR).Modules._xxtestfuzz]_xxtestfuzz.obm : [.Modules._xxtestfuzz]_xxtestfuzz.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._xxtestfuzz]fuzzer.obm : [.Modules._xxtestfuzz]fuzzer.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_xxtestfuzz.exe : [.$(OBJ_DIR).Modules._xxtestfuzz]_xxtestfuzz.obm,[.$(OBJ_DIR).Modules._xxtestfuzz]fuzzer.obm

! readline readline
[.$(OBJ_DIR).Modules]readline.obm : [.Modules]readline.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]readline.exe : [.$(OBJ_DIR).Modules]readline.obm

! _crypt _cryptmodule
[.$(OBJ_DIR).Modules]_cryptmodule.obm : [.Modules]_cryptmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_crypt.exe : [.$(OBJ_DIR).Modules]_cryptmodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _socket socketmodule
[.$(OBJ_DIR).Modules]socketmodule.obm : [.Modules]socketmodule.c [.Modules]socketmodule.h $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_socket.exe : [.$(OBJ_DIR).Modules]socketmodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _gdbm _gdbmmodule
[.$(OBJ_DIR).Modules]_gdbmmodule.obm : [.Modules]_gdbmmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_gdbm.exe : [.$(OBJ_DIR).Modules]_gdbmmodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _sqlite3
SQL_OBJ_LIST = -
[.$(OBJ_DIR).Modules._sqlite]cache.obm -
[.$(OBJ_DIR).Modules._sqlite]connection.obm -
[.$(OBJ_DIR).Modules._sqlite]cursor.obm -
[.$(OBJ_DIR).Modules._sqlite]microprotocols.obm -
[.$(OBJ_DIR).Modules._sqlite]module.obm -
[.$(OBJ_DIR).Modules._sqlite]prepare_protocol.obm -
[.$(OBJ_DIR).Modules._sqlite]row.obm -
[.$(OBJ_DIR).Modules._sqlite]statement.obm -
[.$(OBJ_DIR).Modules._sqlite]util.obm

[.$(OBJ_DIR).Modules._sqlite]cache.obm : [.Modules._sqlite]cache.c $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_SQL) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._sqlite]connection.obm : [.Modules._sqlite]connection.c $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_SQL) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._sqlite]cursor.obm : [.Modules._sqlite]cursor.c $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_SQL) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._sqlite]microprotocols.obm : [.Modules._sqlite]microprotocols.c $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_SQL) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._sqlite]module.obm : [.Modules._sqlite]module.c $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_SQL) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._sqlite]prepare_protocol.obm : [.Modules._sqlite]prepare_protocol.c $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_SQL) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._sqlite]row.obm : [.Modules._sqlite]row.c $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_SQL) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._sqlite]statement.obm : [.Modules._sqlite]statement.c $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_SQL) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._sqlite]util.obm : [.Modules._sqlite]util.c $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_SQL) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OUT_DIR).$(DYNLOAD_DIR)]_sqlite3.exe : $(SQL_OBJ_LIST)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! zlib zlibmodule
[.$(OBJ_DIR).Modules]zlibmodule.obm : [.Modules]zlibmodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]zlib.exe : [.$(OBJ_DIR).Modules]zlibmodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! binascii binascii
[.$(OBJ_DIR).Modules]binascii.obm : [.Modules]binascii.c $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_USE_ZLIB) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OUT_DIR).$(DYNLOAD_DIR)]binascii.exe : [.$(OBJ_DIR).Modules]binascii.obm

! _bz2 _bz2module
[.$(OBJ_DIR).Modules]_bz2module.obm : [.Modules]_bz2module.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_bz2.exe : [.$(OBJ_DIR).Modules]_bz2module.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _lzma _lzmamodule
[.$(OBJ_DIR).Modules]_lzmamodule.obm : [.Modules]_lzmamodule.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_lzma.exe : [.$(OBJ_DIR).Modules]_lzmamodule.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! pyexpat
EXPAT_OBJ_LIST = -
[.$(OBJ_DIR).Modules.expat]xmlparse.obm -
[.$(OBJ_DIR).Modules.expat]xmlrole.obm -
[.$(OBJ_DIR).Modules.expat]xmltok.obm

EXPAT_HEADERS = -
[.Modules.expat]ascii.h -
[.Modules.expat]asciitab.h -
[.Modules.expat]expat.h -
[.Modules.expat]expat_config.h -
[.Modules.expat]expat_external.h -
[.Modules.expat]internal.h -
[.Modules.expat]latin1tab.h -
[.Modules.expat]utf8tab.h -
[.Modules.expat]xmlrole.h -
[.Modules.expat]xmltok.h -
[.Modules.expat]xmltok_impl.h

[.$(OBJ_DIR).Modules]pyexpat.obm : [.Modules]pyexpat.c $(EXPAT_HEADERS) $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_EXPAT) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules.expat]xmlparse.obm : [.Modules.expat]xmlparse.c $(EXPAT_HEADERS) $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_EXPAT) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules.expat]xmlrole.obm : [.Modules.expat]xmlrole.c $(EXPAT_HEADERS) $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_EXPAT) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules.expat]xmltok.obm : [.Modules.expat]xmltok.c $(EXPAT_HEADERS) $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_EXPAT) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OUT_DIR).$(DYNLOAD_DIR)]pyexpat.exe : [.$(OBJ_DIR).Modules]pyexpat.obm,$(EXPAT_OBJ_LIST)

! _elementtree _elementtree
[.$(OBJ_DIR).Modules]_elementtree.obm : [.Modules]_elementtree.c $(EXPAT_HEADERS) $(PYTHON_HEADERS) [.$(OUT_DIR).$(DYNLOAD_DIR)]pyexpat.exe
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_EXPAT_ELEM) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OUT_DIR).$(DYNLOAD_DIR)]_elementtree.exe : [.$(OBJ_DIR).Modules]_elementtree.obm

! _multibytecodec cjkcodecs/multibytecodec.c
[.$(OBJ_DIR).Modules.cjkcodecs]multibytecodec.obm : [.Modules.cjkcodecs]multibytecodec.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_multibytecodec.exe : [.$(OBJ_DIR).Modules.cjkcodecs]multibytecodec.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _codecs_kr cjkcodecs/_codecs_kr.c
[.$(OBJ_DIR).Modules.cjkcodecs]_codecs_kr.obm : [.Modules.cjkcodecs]_codecs_kr.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_codecs_kr.exe : [.$(OBJ_DIR).Modules.cjkcodecs]_codecs_kr.obm

! _codecs_jp cjkcodecs/_codecs_jp.c
[.$(OBJ_DIR).Modules.cjkcodecs]_codecs_jp.obm : [.Modules.cjkcodecs]_codecs_jp.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_codecs_jp.exe : [.$(OBJ_DIR).Modules.cjkcodecs]_codecs_jp.obm

! _codecs_cn cjkcodecs/_codecs_cn.c
[.$(OBJ_DIR).Modules.cjkcodecs]_codecs_cn.obm : [.Modules.cjkcodecs]_codecs_cn.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_codecs_cn.exe : [.$(OBJ_DIR).Modules.cjkcodecs]_codecs_cn.obm

! _codecs_tw cjkcodecs/_codecs_tw.c
[.$(OBJ_DIR).Modules.cjkcodecs]_codecs_tw.obm : [.Modules.cjkcodecs]_codecs_tw.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_codecs_tw.exe : [.$(OBJ_DIR).Modules.cjkcodecs]_codecs_tw.obm

! _codecs_hk cjkcodecs/_codecs_hk.c
[.$(OBJ_DIR).Modules.cjkcodecs]_codecs_hk.obm : [.Modules.cjkcodecs]_codecs_hk.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_codecs_hk.exe : [.$(OBJ_DIR).Modules.cjkcodecs]_codecs_hk.obm

! _codecs_iso2022 cjkcodecs/_codecs_iso2022.c
[.$(OBJ_DIR).Modules.cjkcodecs]_codecs_iso2022.obm : [.Modules.cjkcodecs]_codecs_iso2022.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_codecs_iso2022.exe : [.$(OBJ_DIR).Modules.cjkcodecs]_codecs_iso2022.obm

! _multiprocessing
[.$(OBJ_DIR).Modules._multiprocessing]multiprocessing.obm : [.Modules._multiprocessing]multiprocessing.c [.Modules._multiprocessing]multiprocessing.h $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_MULT) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).Modules._multiprocessing]semaphore.obm : [.Modules._multiprocessing]semaphore.c [.Modules._multiprocessing]multiprocessing.h [.vms]sem_mbx.h $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_MULT) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).vms]vms_sem_mbx.obm : [.vms]vms_sem_mbx.c [.vms]vms_sem_mbx.h
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_MULT) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OUT_DIR).$(DYNLOAD_DIR)]_multiprocessing.exe : [.$(OBJ_DIR).Modules._multiprocessing]multiprocessing.obm,[.$(OBJ_DIR).vms]vms_sem_mbx.obm,[.$(OBJ_DIR).Modules._multiprocessing]semaphore.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _posixshmem
[.$(OBJ_DIR).Modules._multiprocessing]posixshmem.obm : [.Modules._multiprocessing]posixshmem.c [.Modules._multiprocessing]multiprocessing.h $(PYTHON_HEADERS)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC) $(PY_CFLAGS_MULT) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OUT_DIR).$(DYNLOAD_DIR)]_posixshmem.exe : [.$(OBJ_DIR).Modules._multiprocessing]posixshmem.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! ! _uuid _uuidmodule
! [.$(OBJ_DIR).Modules]_uuidmodule.obm : [.Modules]_uuidmodule.c $(PYTHON_HEADERS)
! [.$(OUT_DIR).$(DYNLOAD_DIR)]_uuid.exe : [.$(OBJ_DIR).Modules]_uuidmodule.obm
!     @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
!     $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _ctypes
CTYPES_OBJ_LIST = -
[.$(OBJ_DIR).Modules._ctypes]callbacks.obm -
[.$(OBJ_DIR).Modules._ctypes]callproc.obm -
[.$(OBJ_DIR).Modules._ctypes]stgdict.obm -
[.$(OBJ_DIR).Modules._ctypes]cfield.obm

CTYPES_HEADERS = -
[.Modules._ctypes]ctypes.h

[.$(OBJ_DIR).Modules._ctypes]_ctypes.obm : [.Modules._ctypes]_ctypes.c $(CTYPES_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._ctypes]callbacks.obm : [.Modules._ctypes]callbacks.c $(CTYPES_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._ctypes]stgdict.obm : [.Modules._ctypes]stgdict.c $(CTYPES_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._ctypes]callproc.obm : [.Modules._ctypes]callproc.c $(CTYPES_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._ctypes]cfield.obm : [.Modules._ctypes]cfield.c $(CTYPES_HEADERS) $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_ctypes.exe : [.$(OBJ_DIR).Modules._ctypes]_ctypes.obm,$(CTYPES_OBJ_LIST)

! _ctypes_test _ctypes_test
[.$(OBJ_DIR).Modules._ctypes]_ctypes_test.obm : [.Modules._ctypes]_ctypes_test.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_ctypes_test.exe : [.$(OBJ_DIR).Modules._ctypes]_ctypes_test.obm

! _decimal
DECIMAL_OBJ_LIST = -
[.$(OBJ_DIR).Modules._decimal.libmpdec]basearith.obd -
[.$(OBJ_DIR).Modules._decimal.libmpdec]constants.obd -
[.$(OBJ_DIR).Modules._decimal.libmpdec]context.obd -
[.$(OBJ_DIR).Modules._decimal.libmpdec]convolute.obd -
[.$(OBJ_DIR).Modules._decimal.libmpdec]crt.obd -
[.$(OBJ_DIR).Modules._decimal.libmpdec]difradix2.obd -
[.$(OBJ_DIR).Modules._decimal.libmpdec]fnt.obd -
[.$(OBJ_DIR).Modules._decimal.libmpdec]fourstep.obd -
[.$(OBJ_DIR).Modules._decimal.libmpdec]io.obd -
[.$(OBJ_DIR).Modules._decimal.libmpdec]memory.obd -
[.$(OBJ_DIR).Modules._decimal.libmpdec]mpdecimal.obd -
[.$(OBJ_DIR).Modules._decimal.libmpdec]numbertheory.obd -
[.$(OBJ_DIR).Modules._decimal.libmpdec]sixstep.obd -
[.$(OBJ_DIR).Modules._decimal.libmpdec]transpose.obd

DECIMAL_HEADERS = -
[.Modules._decimal]docstrings.h -
[.Modules._decimal.libmpdec]basearith.h -
[.Modules._decimal.libmpdec]bits.h -
[.Modules._decimal.libmpdec]constants.h -
[.Modules._decimal.libmpdec]convolute.h -
[.Modules._decimal.libmpdec]crt.h -
[.Modules._decimal.libmpdec]difradix2.h -
[.Modules._decimal.libmpdec]fnt.h -
[.Modules._decimal.libmpdec]fourstep.h -
[.Modules._decimal.libmpdec]io.h -
[.Modules._decimal.libmpdec]mpalloc.h -
[.Modules._decimal.libmpdec]mpdecimal.h -
[.Modules._decimal.libmpdec]numbertheory.h -
[.Modules._decimal.libmpdec]sixstep.h -
[.Modules._decimal.libmpdec]transpose.h -
[.Modules._decimal.libmpdec]typearith.h -
[.Modules._decimal.libmpdec]umodarith.h

[.$(OBJ_DIR).Modules._decimal]_decimal.obd : [.Modules._decimal]_decimal.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._decimal.libmpdec]basearith.obd : [.Modules._decimal.libmpdec]basearith.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._decimal.libmpdec]constants.obd : [.Modules._decimal.libmpdec]constants.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._decimal.libmpdec]context.obd : [.Modules._decimal.libmpdec]context.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._decimal.libmpdec]convolute.obd : [.Modules._decimal.libmpdec]convolute.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._decimal.libmpdec]crt.obd : [.Modules._decimal.libmpdec]crt.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._decimal.libmpdec]difradix2.obd : [.Modules._decimal.libmpdec]difradix2.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._decimal.libmpdec]fnt.obd : [.Modules._decimal.libmpdec]fnt.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._decimal.libmpdec]fourstep.obd : [.Modules._decimal.libmpdec]fourstep.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._decimal.libmpdec]io.obd : [.Modules._decimal.libmpdec]io.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._decimal.libmpdec]memory.obd : [.Modules._decimal.libmpdec]memory.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._decimal.libmpdec]mpdecimal.obd : [.Modules._decimal.libmpdec]mpdecimal.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._decimal.libmpdec]numbertheory.obd : [.Modules._decimal.libmpdec]numbertheory.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._decimal.libmpdec]sixstep.obd : [.Modules._decimal.libmpdec]sixstep.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._decimal.libmpdec]transpose.obd : [.Modules._decimal.libmpdec]transpose.c $(DECIMAL_HEADERS) $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_decimal.exe : [.$(OBJ_DIR).Modules._decimal]_decimal.obd,$(DECIMAL_OBJ_LIST)

! _ssl
[.$(OBJ_DIR).Modules]_ssl.obm : [.Modules]_ssl.c [.Modules]socketmodule.h $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_ssl.exe : [.$(OBJ_DIR).Modules]_ssl.obm

! _hashlib _hashopenssl
[.$(OBJ_DIR).Modules]_hashopenssl.obm : [.Modules]_hashopenssl.c [.Modules]hashlib.h $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_hashlib.exe : [.$(OBJ_DIR).Modules]_hashopenssl.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _sha256 sha256module
[.$(OBJ_DIR).Modules]sha256module.obm : [.Modules]sha256module.c [.Modules]hashlib.h $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_sha256.exe : [.$(OBJ_DIR).Modules]sha256module.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _sha512 sha512module
[.$(OBJ_DIR).Modules]sha512module.obm : [.Modules]sha512module.c [.Modules]hashlib.h $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_sha512.exe : [.$(OBJ_DIR).Modules]sha512module.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _md5 md5module
[.$(OBJ_DIR).Modules]md5module.obm : [.Modules]md5module.c [.Modules]hashlib.h $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_md5.exe : [.$(OBJ_DIR).Modules]md5module.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _sha1 sha1module
[.$(OBJ_DIR).Modules]sha1module.obm : [.Modules]sha1module.c [.Modules]hashlib.h $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_sha1.exe : [.$(OBJ_DIR).Modules]sha1module.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _blake2 modulesource
BLAKE2_OBJ_LIST = -
[.$(OBJ_DIR).Modules._blake2]blake2module.obm -
[.$(OBJ_DIR).Modules._blake2]blake2b_impl.obm -
[.$(OBJ_DIR).Modules._blake2]blake2s_impl.obm

BLAKE2_HEADERS = -
[.Modules._blake2.impl]blake2-config.h -
[.Modules._blake2.impl]blake2-dispatch.c -
[.Modules._blake2.impl]blake2-impl.h -
[.Modules._blake2.impl]blake2-kat.h -
[.Modules._blake2.impl]blake2.h -
[.Modules._blake2.impl]blake2b-load-sse2.h -
[.Modules._blake2.impl]blake2b-load-sse41.h -
[.Modules._blake2.impl]blake2b-ref.c -
[.Modules._blake2.impl]blake2b-round.h -
[.Modules._blake2.impl]blake2b-test.c -
[.Modules._blake2.impl]blake2b.c -
[.Modules._blake2.impl]blake2bp-test.c -
[.Modules._blake2.impl]blake2bp.c -
[.Modules._blake2.impl]blake2s-load-sse2.h -
[.Modules._blake2.impl]blake2s-load-sse41.h -
[.Modules._blake2.impl]blake2s-load-xop.h -
[.Modules._blake2.impl]blake2s-ref.c -
[.Modules._blake2.impl]blake2s-round.h -
[.Modules._blake2.impl]blake2s-test.c -
[.Modules._blake2.impl]blake2s.c -
[.Modules._blake2.impl]blake2sp-test.c -
[.Modules._blake2.impl]blake2sp.c -
[.Modules]hashlib.h

[.$(OBJ_DIR).Modules._blake2]blake2module.obm : [.Modules._blake2]blake2module.c $(BLAKE2_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._blake2]blake2b_impl.obm : [.Modules._blake2]blake2b_impl.c $(BLAKE2_HEADERS) $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules._blake2]blake2s_impl.obm : [.Modules._blake2]blake2s_impl.c $(BLAKE2_HEADERS) $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_blake2.exe : $(BLAKE2_OBJ_LIST)
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! _sha3
SHA3_HEADERS = -
[.Modules._sha3.kcp]align.h -
[.Modules._sha3.kcp]KeccakHash.c -
[.Modules._sha3.kcp]KeccakHash.h -
[.Modules._sha3.kcp]KeccakP-1600-64.macros -
[.Modules._sha3.kcp]KeccakP-1600-inplace32BI.c -
[.Modules._sha3.kcp]KeccakP-1600-opt64-config.h -
[.Modules._sha3.kcp]KeccakP-1600-opt64.c -
[.Modules._sha3.kcp]KeccakP-1600-SnP-opt32.h -
[.Modules._sha3.kcp]KeccakP-1600-SnP-opt64.h -
[.Modules._sha3.kcp]KeccakP-1600-SnP.h -
[.Modules._sha3.kcp]KeccakP-1600-unrolling.macros -
[.Modules._sha3.kcp]KeccakSponge.c -
[.Modules._sha3.kcp]KeccakSponge.h -
[.Modules._sha3.kcp]KeccakSponge.inc -
[.Modules._sha3.kcp]PlSnP-Fallback.inc -
[.Modules._sha3.kcp]SnP-Relaned.h -
[.Modules]hashlib.h

[.$(OBJ_DIR).Modules._sha3]sha3module.obm : [.Modules._sha3]sha3module.c $(SHA3_HEADERS) $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_sha3.exe : [.$(OBJ_DIR).Modules._sha3]sha3module.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

!! modulename modulesource
! [.$(OBJ_DIR).Modules]modulesource.obm : [.Modules]modulesource.c $(PYTHON_HEADERS)
! [.$(OUT_DIR).$(DYNLOAD_DIR)]modulename.exe : [.$(OBJ_DIR).Modules]modulesource.obm
!     @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
!     $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

!SWIG ============================================================

[.Modules.vms.decc]decc_wrap.c : [.Modules.vms.decc]decc.i
    SWIG -python modules/vms/decc/decc.i
    purge [.modules.vms.decc]decc_wrap.c

[.$(OBJ_DIR).Modules.vms.decc]decc_wrap.obm : [.Modules.vms.decc]decc_wrap.c $(PYTHON_HEADERS)
[.$(OBJ_DIR).Modules.vms.decc]decc.obm : [.Modules.vms.decc]decc.c $(PYTHON_HEADERS)
[.$(OUT_DIR).$(DYNLOAD_DIR)]_decc.exe : [.$(OBJ_DIR).Modules.vms.decc]decc_wrap.obm,[.$(OBJ_DIR).Modules.vms.decc]decc.obm
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.jpidef]jpidef_wrap.c : [.modules.vms.jpidef]jpidef.i
    SWIG -python modules/vms/jpidef/jpidef.i
    purge [.modules.vms.jpidef]jpidef_wrap.c

[.$(OBJ_DIR).modules.vms.jpidef]jpidef_wrap.obs : [.modules.vms.jpidef]jpidef_wrap.c
[.$(OUT_DIR).$(DYNLOAD_DIR)]_jpidef.exe : [.$(OBJ_DIR).modules.vms.jpidef]jpidef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.syidef]syidef_wrap.c : [.modules.vms.syidef]syidef.i
    SWIG -python modules/vms/syidef/syidef.i
    purge [.modules.vms.syidef]syidef_wrap.c

[.$(OBJ_DIR).modules.vms.syidef]syidef_wrap.obs : [.modules.vms.syidef]syidef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_syidef.exe : [.$(OBJ_DIR).modules.vms.syidef]syidef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.clidef]clidef_wrap.c : [.modules.vms.clidef]clidef.i
    SWIG -python modules/vms/clidef/clidef.i
    purge [.modules.vms.clidef]clidef_wrap.c

[.$(OBJ_DIR).modules.vms.clidef]clidef_wrap.obs : [.modules.vms.clidef]clidef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_clidef.exe : [.$(OBJ_DIR).modules.vms.clidef]clidef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.dvidef]dvidef_wrap.c : [.modules.vms.dvidef]dvidef.i
    SWIG -python modules/vms/dvidef/dvidef.i
    purge [.modules.vms.dvidef]dvidef_wrap.c

[.$(OBJ_DIR).modules.vms.dvidef]dvidef_wrap.obs : [.modules.vms.dvidef]dvidef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_dvidef.exe : [.$(OBJ_DIR).modules.vms.dvidef]dvidef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.quidef]quidef_wrap.c : [.modules.vms.quidef]quidef.i
    SWIG -python modules/vms/quidef/quidef.i
    purge [.modules.vms.quidef]quidef_wrap.c

[.$(OBJ_DIR).modules.vms.quidef]quidef_wrap.obs : [.modules.vms.quidef]quidef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_quidef.exe : [.$(OBJ_DIR).modules.vms.quidef]quidef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.psldef]psldef_wrap.c : [.modules.vms.psldef]psldef.i
    SWIG -python modules/vms/psldef/psldef.i
    purge [.modules.vms.psldef]psldef_wrap.c

[.$(OBJ_DIR).modules.vms.psldef]psldef_wrap.obs : [.modules.vms.psldef]psldef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_psldef.exe : [.$(OBJ_DIR).modules.vms.psldef]psldef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.fabdef]fabdef_wrap.c : [.modules.vms.fabdef]fabdef.i
    SWIG -python modules/vms/fabdef/fabdef.i
    purge [.modules.vms.fabdef]fabdef_wrap.c

[.$(OBJ_DIR).modules.vms.fabdef]fabdef_wrap.obs : [.modules.vms.fabdef]fabdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_fabdef.exe : [.$(OBJ_DIR).modules.vms.fabdef]fabdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.rmsdef]rmsdef_wrap.c : [.modules.vms.rmsdef]rmsdef.i
    SWIG -python modules/vms/rmsdef/rmsdef.i
    purge [.modules.vms.rmsdef]rmsdef_wrap.c

[.$(OBJ_DIR).modules.vms.rmsdef]rmsdef_wrap.obs : [.modules.vms.rmsdef]rmsdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_rmsdef.exe : [.$(OBJ_DIR).modules.vms.rmsdef]rmsdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.pscandef]pscandef_wrap.c : [.modules.vms.pscandef]pscandef.i
    SWIG -python modules/vms/pscandef/pscandef.i
    purge [.modules.vms.pscandef]pscandef_wrap.c

[.$(OBJ_DIR).modules.vms.pscandef]pscandef_wrap.obs : [.modules.vms.pscandef]pscandef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_pscandef.exe : [.$(OBJ_DIR).modules.vms.pscandef]pscandef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.prdef]prdef_wrap.c : [.modules.vms.prdef]prdef.i
    SWIG -python modules/vms/prdef/prdef.i
    purge [.modules.vms.prdef]prdef_wrap.c

[.$(OBJ_DIR).modules.vms.prdef]prdef_wrap.obs : [.modules.vms.prdef]prdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_prdef.exe : [.$(OBJ_DIR).modules.vms.prdef]prdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.rsdmdef]rsdmdef_wrap.c : [.modules.vms.rsdmdef]rsdmdef.i
    SWIG -python modules/vms/rsdmdef/rsdmdef.i
    purge [.modules.vms.rsdmdef]rsdmdef_wrap.c

[.$(OBJ_DIR).modules.vms.rsdmdef]rsdmdef_wrap.obs : [.modules.vms.rsdmdef]rsdmdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_rsdmdef.exe : [.$(OBJ_DIR).modules.vms.rsdmdef]rsdmdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.ppropdef]ppropdef_wrap.c : [.modules.vms.ppropdef]ppropdef.i
    SWIG -python modules/vms/ppropdef/ppropdef.i
    purge [.modules.vms.ppropdef]ppropdef_wrap.c

[.$(OBJ_DIR).modules.vms.ppropdef]ppropdef_wrap.obs : [.modules.vms.ppropdef]ppropdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_ppropdef.exe : [.$(OBJ_DIR).modules.vms.ppropdef]ppropdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.maildef]maildef_wrap.c : [.modules.vms.maildef]maildef.i
    SWIG -python modules/vms/maildef/maildef.i
    purge [.modules.vms.maildef]maildef_wrap.c

[.$(OBJ_DIR).modules.vms.maildef]maildef_wrap.obs : [.modules.vms.maildef]maildef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_maildef.exe : [.$(OBJ_DIR).modules.vms.maildef]maildef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.jbcmsgdef]jbcmsgdef_wrap.c : [.modules.vms.jbcmsgdef]jbcmsgdef.i
    SWIG -python modules/vms/jbcmsgdef/jbcmsgdef.i
    purge [.modules.vms.jbcmsgdef]jbcmsgdef_wrap.c

[.$(OBJ_DIR).modules.vms.jbcmsgdef]jbcmsgdef_wrap.obs : [.modules.vms.jbcmsgdef]jbcmsgdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_jbcmsgdef.exe : [.$(OBJ_DIR).modules.vms.jbcmsgdef]jbcmsgdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.libclidef]libclidef_wrap.c : [.modules.vms.libclidef]libclidef.i
    SWIG -python modules/vms/libclidef/libclidef.i
    purge [.modules.vms.libclidef]libclidef_wrap.c

[.$(OBJ_DIR).modules.vms.libclidef]libclidef_wrap.obs : [.modules.vms.libclidef]libclidef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_libclidef.exe : [.$(OBJ_DIR).modules.vms.libclidef]libclidef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.libdtdef]libdtdef_wrap.c : [.modules.vms.libdtdef]libdtdef.i
    SWIG -python modules/vms/libdtdef/libdtdef.i
    purge [.modules.vms.libdtdef]libdtdef_wrap.c

[.$(OBJ_DIR).modules.vms.libdtdef]libdtdef_wrap.obs : [.modules.vms.libdtdef]libdtdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_libdtdef.exe : [.$(OBJ_DIR).modules.vms.libdtdef]libdtdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.libfisdef]libfisdef_wrap.c : [.modules.vms.libfisdef]libfisdef.i
    SWIG -python modules/vms/libfisdef/libfisdef.i
    purge [.modules.vms.libfisdef]libfisdef_wrap.c

[.$(OBJ_DIR).modules.vms.libfisdef]libfisdef_wrap.obs : [.modules.vms.libfisdef]libfisdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_libfisdef.exe : [.$(OBJ_DIR).modules.vms.libfisdef]libfisdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.dcdef]dcdef_wrap.c : [.modules.vms.dcdef]dcdef.i
    SWIG -python modules/vms/dcdef/dcdef.i
    purge [.modules.vms.dcdef]dcdef_wrap.c

[.$(OBJ_DIR).modules.vms.dcdef]dcdef_wrap.obs : [.modules.vms.dcdef]dcdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_dcdef.exe : [.$(OBJ_DIR).modules.vms.dcdef]dcdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.cvtfnmdef]cvtfnmdef_wrap.c : [.modules.vms.cvtfnmdef]cvtfnmdef.i
    SWIG -python modules/vms/cvtfnmdef/cvtfnmdef.i
    purge [.modules.vms.cvtfnmdef]cvtfnmdef_wrap.c

[.$(OBJ_DIR).modules.vms.cvtfnmdef]cvtfnmdef_wrap.obs : [.modules.vms.cvtfnmdef]cvtfnmdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_cvtfnmdef.exe : [.$(OBJ_DIR).modules.vms.cvtfnmdef]cvtfnmdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.fscndef]fscndef_wrap.c : [.modules.vms.fscndef]fscndef.i
    SWIG -python modules/vms/fscndef/fscndef.i
    purge [.modules.vms.fscndef]fscndef_wrap.c

[.$(OBJ_DIR).modules.vms.fscndef]fscndef_wrap.obs : [.modules.vms.fscndef]fscndef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_fscndef.exe : [.$(OBJ_DIR).modules.vms.fscndef]fscndef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.fpdef]fpdef_wrap.c : [.modules.vms.fpdef]fpdef.i
    SWIG -python modules/vms/fpdef/fpdef.i
    purge [.modules.vms.fpdef]fpdef_wrap.c

[.$(OBJ_DIR).modules.vms.fpdef]fpdef_wrap.obs : [.modules.vms.fpdef]fpdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_fpdef.exe : [.$(OBJ_DIR).modules.vms.fpdef]fpdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.fdldef]fdldef_wrap.c : [.modules.vms.fdldef]fdldef.i
    SWIG -python modules/vms/fdldef/fdldef.i
    purge [.modules.vms.fdldef]fdldef_wrap.c

[.$(OBJ_DIR).modules.vms.fdldef]fdldef_wrap.obs : [.modules.vms.fdldef]fdldef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_fdldef.exe : [.$(OBJ_DIR).modules.vms.fdldef]fdldef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.dpsdef]dpsdef_wrap.c : [.modules.vms.dpsdef]dpsdef.i
    SWIG -python modules/vms/dpsdef/dpsdef.i
    purge [.modules.vms.dpsdef]dpsdef_wrap.c

[.$(OBJ_DIR).modules.vms.dpsdef]dpsdef_wrap.obs : [.modules.vms.dpsdef]dpsdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_dpsdef.exe : [.$(OBJ_DIR).modules.vms.dpsdef]dpsdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.efndef]efndef_wrap.c : [.modules.vms.efndef]efndef.i
    SWIG -python modules/vms/efndef/efndef.i
    purge [.modules.vms.efndef]efndef_wrap.c

[.$(OBJ_DIR).modules.vms.efndef]efndef_wrap.obs : [.modules.vms.efndef]efndef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_efndef.exe : [.$(OBJ_DIR).modules.vms.efndef]efndef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.dvsdef]dvsdef_wrap.c : [.modules.vms.dvsdef]dvsdef.i
    SWIG -python modules/vms/dvsdef/dvsdef.i
    purge [.modules.vms.dvsdef]dvsdef_wrap.c

[.$(OBJ_DIR).modules.vms.dvsdef]dvsdef_wrap.obs : [.modules.vms.dvsdef]dvsdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_dvsdef.exe : [.$(OBJ_DIR).modules.vms.dvsdef]dvsdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.eradef]eradef_wrap.c : [.modules.vms.eradef]eradef.i
    SWIG -python modules/vms/eradef/eradef.i
    purge [.modules.vms.eradef]eradef_wrap.c

[.$(OBJ_DIR).modules.vms.eradef]eradef_wrap.obs : [.modules.vms.eradef]eradef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_eradef.exe : [.$(OBJ_DIR).modules.vms.eradef]eradef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.iodef]iodef_wrap.c : [.modules.vms.iodef]iodef.i
    SWIG -python modules/vms/iodef/iodef.i
    purge [.modules.vms.iodef]iodef_wrap.c

[.$(OBJ_DIR).modules.vms.iodef]iodef_wrap.obs : [.modules.vms.iodef]iodef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_iodef.exe : [.$(OBJ_DIR).modules.vms.iodef]iodef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.initdef]initdef_wrap.c : [.modules.vms.initdef]initdef.i
    SWIG -python modules/vms/initdef/initdef.i
    purge [.modules.vms.initdef]initdef_wrap.c

[.$(OBJ_DIR).modules.vms.initdef]initdef_wrap.obs : [.modules.vms.initdef]initdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_initdef.exe : [.$(OBJ_DIR).modules.vms.initdef]initdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.issdef]issdef_wrap.c : [.modules.vms.issdef]issdef.i
    SWIG -python modules/vms/issdef/issdef.i
    purge [.modules.vms.issdef]issdef_wrap.c

[.$(OBJ_DIR).modules.vms.issdef]issdef_wrap.obs : [.modules.vms.issdef]issdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_issdef.exe : [.$(OBJ_DIR).modules.vms.issdef]issdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.impdef]impdef_wrap.c : [.modules.vms.impdef]impdef.i
    SWIG -python modules/vms/impdef/impdef.i
    purge [.modules.vms.impdef]impdef_wrap.c

[.$(OBJ_DIR).modules.vms.impdef]impdef_wrap.obs : [.modules.vms.impdef]impdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_impdef.exe : [.$(OBJ_DIR).modules.vms.impdef]impdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.mntdef]mntdef_wrap.c : [.modules.vms.mntdef]mntdef.i
    SWIG -python modules/vms/mntdef/mntdef.i
    purge [.modules.vms.mntdef]mntdef_wrap.c

[.$(OBJ_DIR).modules.vms.mntdef]mntdef_wrap.obs : [.modules.vms.mntdef]mntdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_mntdef.exe : [.$(OBJ_DIR).modules.vms.mntdef]mntdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.nsadef]nsadef_wrap.c : [.modules.vms.nsadef]nsadef.i
    SWIG -python modules/vms/nsadef/nsadef.i
    purge [.modules.vms.nsadef]nsadef_wrap.c

[.$(OBJ_DIR).modules.vms.nsadef]nsadef_wrap.obs : [.modules.vms.nsadef]nsadef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_nsadef.exe : [.$(OBJ_DIR).modules.vms.nsadef]nsadef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.ossdef]ossdef_wrap.c : [.modules.vms.ossdef]ossdef.i
    SWIG -python modules/vms/ossdef/ossdef.i
    purge [.modules.vms.ossdef]ossdef_wrap.c

[.$(OBJ_DIR).modules.vms.ossdef]ossdef_wrap.obs : [.modules.vms.ossdef]ossdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_ossdef.exe : [.$(OBJ_DIR).modules.vms.ossdef]ossdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.pcbdef]pcbdef_wrap.c : [.modules.vms.pcbdef]pcbdef.i
    SWIG -python modules/vms/pcbdef/pcbdef.i
    purge [.modules.vms.pcbdef]pcbdef_wrap.c

[.$(OBJ_DIR).modules.vms.pcbdef]pcbdef_wrap.obs : [.modules.vms.pcbdef]pcbdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_pcbdef.exe : [.$(OBJ_DIR).modules.vms.pcbdef]pcbdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.prxdef]prxdef_wrap.c : [.modules.vms.prxdef]prxdef.i
    SWIG -python modules/vms/prxdef/prxdef.i
    purge [.modules.vms.prxdef]prxdef_wrap.c

[.$(OBJ_DIR).modules.vms.prxdef]prxdef_wrap.obs : [.modules.vms.prxdef]prxdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_prxdef.exe : [.$(OBJ_DIR).modules.vms.prxdef]prxdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.prvdef]prvdef_wrap.c : [.modules.vms.prvdef]prvdef.i
    SWIG -python modules/vms/prvdef/prvdef.i
    purge [.modules.vms.prvdef]prvdef_wrap.c

[.$(OBJ_DIR).modules.vms.prvdef]prvdef_wrap.obs : [.modules.vms.prvdef]prvdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_prvdef.exe : [.$(OBJ_DIR).modules.vms.prvdef]prvdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.pqldef]pqldef_wrap.c : [.modules.vms.pqldef]pqldef.i
    SWIG -python modules/vms/pqldef/pqldef.i
    purge [.modules.vms.pqldef]pqldef_wrap.c

[.$(OBJ_DIR).modules.vms.pqldef]pqldef_wrap.obs : [.modules.vms.pqldef]pqldef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_pqldef.exe : [.$(OBJ_DIR).modules.vms.pqldef]pqldef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.prcdef]prcdef_wrap.c : [.modules.vms.prcdef]prcdef.i
    SWIG -python modules/vms/prcdef/prcdef.i
    purge [.modules.vms.prcdef]prcdef_wrap.c

[.$(OBJ_DIR).modules.vms.prcdef]prcdef_wrap.obs : [.modules.vms.prcdef]prcdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_prcdef.exe : [.$(OBJ_DIR).modules.vms.prcdef]prcdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.lckdef]lckdef_wrap.c : [.modules.vms.lckdef]lckdef.i
    SWIG -python modules/vms/lckdef/lckdef.i
    purge [.modules.vms.lckdef]lckdef_wrap.c

[.$(OBJ_DIR).modules.vms.lckdef]lckdef_wrap.obs : [.modules.vms.lckdef]lckdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_lckdef.exe : [.$(OBJ_DIR).modules.vms.lckdef]lckdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.kgbdef]kgbdef_wrap.c : [.modules.vms.kgbdef]kgbdef.i
    SWIG -python modules/vms/kgbdef/kgbdef.i
    purge [.modules.vms.kgbdef]kgbdef_wrap.c

[.$(OBJ_DIR).modules.vms.kgbdef]kgbdef_wrap.obs : [.modules.vms.kgbdef]kgbdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_kgbdef.exe : [.$(OBJ_DIR).modules.vms.kgbdef]kgbdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.pxbdef]pxbdef_wrap.c : [.modules.vms.pxbdef]pxbdef.i
    SWIG -python modules/vms/pxbdef/pxbdef.i
    purge [.modules.vms.pxbdef]pxbdef_wrap.c

[.$(OBJ_DIR).modules.vms.pxbdef]pxbdef_wrap.obs : [.modules.vms.pxbdef]pxbdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_pxbdef.exe : [.$(OBJ_DIR).modules.vms.pxbdef]pxbdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.sdvdef]sdvdef_wrap.c : [.modules.vms.sdvdef]sdvdef.i
    SWIG -python modules/vms/sdvdef/sdvdef.i
    purge [.modules.vms.sdvdef]sdvdef_wrap.c

[.$(OBJ_DIR).modules.vms.sdvdef]sdvdef_wrap.obs : [.modules.vms.sdvdef]sdvdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_sdvdef.exe : [.$(OBJ_DIR).modules.vms.sdvdef]sdvdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.stsdef]stsdef_wrap.c : [.modules.vms.stsdef]stsdef.i
    SWIG -python modules/vms/stsdef/stsdef.i
    purge [.modules.vms.stsdef]stsdef_wrap.c

[.$(OBJ_DIR).modules.vms.stsdef]stsdef_wrap.obs : [.modules.vms.stsdef]stsdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_stsdef.exe : [.$(OBJ_DIR).modules.vms.stsdef]stsdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.lnmdef]lnmdef_wrap.c : [.modules.vms.lnmdef]lnmdef.i
    SWIG -python modules/vms/lnmdef/lnmdef.i
    purge [.modules.vms.lnmdef]lnmdef_wrap.c

[.$(OBJ_DIR).modules.vms.lnmdef]lnmdef_wrap.obs : [.modules.vms.lnmdef]lnmdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_lnmdef.exe : [.$(OBJ_DIR).modules.vms.lnmdef]lnmdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.iccdef]iccdef_wrap.c : [.modules.vms.iccdef]iccdef.i
    SWIG -python modules/vms/iccdef/iccdef.i
    purge [.modules.vms.iccdef]iccdef_wrap.c

[.$(OBJ_DIR).modules.vms.iccdef]iccdef_wrap.obs : [.modules.vms.iccdef]iccdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_iccdef.exe : [.$(OBJ_DIR).modules.vms.iccdef]iccdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.dscdef]dscdef_wrap.c : [.modules.vms.dscdef]dscdef.i
    SWIG -python modules/vms/dscdef/dscdef.i
    purge [.modules.vms.dscdef]dscdef_wrap.c

[.$(OBJ_DIR).modules.vms.dscdef]dscdef_wrap.obs : [.modules.vms.dscdef]dscdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_dscdef.exe : [.$(OBJ_DIR).modules.vms.dscdef]dscdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.dmtdef]dmtdef_wrap.c : [.modules.vms.dmtdef]dmtdef.i
    SWIG -python modules/vms/dmtdef/dmtdef.i
    purge [.modules.vms.dmtdef]dmtdef_wrap.c

[.$(OBJ_DIR).modules.vms.dmtdef]dmtdef_wrap.obs : [.modules.vms.dmtdef]dmtdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_dmtdef.exe : [.$(OBJ_DIR).modules.vms.dmtdef]dmtdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.cmbdef]cmbdef_wrap.c : [.modules.vms.cmbdef]cmbdef.i
    SWIG -python modules/vms/cmbdef/cmbdef.i
    purge [.modules.vms.cmbdef]cmbdef_wrap.c

[.$(OBJ_DIR).modules.vms.cmbdef]cmbdef_wrap.obs : [.modules.vms.cmbdef]cmbdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_cmbdef.exe : [.$(OBJ_DIR).modules.vms.cmbdef]cmbdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.iledef]iledef_wrap.c : [.modules.vms.iledef]iledef.i
    SWIG -python modules/vms/iledef/iledef.i
    purge [.modules.vms.iledef]iledef_wrap.c

[.$(OBJ_DIR).modules.vms.iledef]iledef_wrap.obs : [.modules.vms.iledef]iledef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_iledef.exe : [.$(OBJ_DIR).modules.vms.iledef]iledef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.lkidef]lkidef_wrap.c : [.modules.vms.lkidef]lkidef.i
    SWIG -python modules/vms/lkidef/lkidef.i
    purge [.modules.vms.lkidef]lkidef_wrap.c

[.$(OBJ_DIR).modules.vms.lkidef]lkidef_wrap.obs : [.modules.vms.lkidef]lkidef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_lkidef.exe : [.$(OBJ_DIR).modules.vms.lkidef]lkidef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.sjcdef]sjcdef_wrap.c : [.modules.vms.sjcdef]sjcdef.i
    SWIG -python modules/vms/sjcdef/sjcdef.i
    purge [.modules.vms.sjcdef]sjcdef_wrap.c

[.$(OBJ_DIR).modules.vms.sjcdef]sjcdef_wrap.obs : [.modules.vms.sjcdef]sjcdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_sjcdef.exe : [.$(OBJ_DIR).modules.vms.sjcdef]sjcdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.ssdef]ssdef_wrap.c : [.modules.vms.ssdef]ssdef.i
    SWIG -python modules/vms/ssdef/ssdef.i
    purge [.modules.vms.ssdef]ssdef_wrap.c

[.$(OBJ_DIR).modules.vms.ssdef]ssdef_wrap.obs : [.modules.vms.ssdef]ssdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_ssdef.exe : [.$(OBJ_DIR).modules.vms.ssdef]ssdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.ciadef]ciadef_wrap.c : [.modules.vms.ciadef]ciadef.i
    SWIG -python modules/vms/ciadef/ciadef.i
    purge [.modules.vms.ciadef]ciadef_wrap.c

[.$(OBJ_DIR).modules.vms.ciadef]ciadef_wrap.obs : [.modules.vms.ciadef]ciadef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_ciadef.exe : [.$(OBJ_DIR).modules.vms.ciadef]ciadef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.capdef]capdef_wrap.c : [.modules.vms.capdef]capdef.i
    SWIG -python modules/vms/capdef/capdef.i
    purge [.modules.vms.capdef]capdef_wrap.c

[.$(OBJ_DIR).modules.vms.capdef]capdef_wrap.obs : [.modules.vms.capdef]capdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_capdef.exe : [.$(OBJ_DIR).modules.vms.capdef]capdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.chpdef]chpdef_wrap.c : [.modules.vms.chpdef]chpdef.i
    SWIG -python modules/vms/chpdef/chpdef.i
    purge [.modules.vms.chpdef]chpdef_wrap.c

[.$(OBJ_DIR).modules.vms.chpdef]chpdef_wrap.obs : [.modules.vms.chpdef]chpdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_chpdef.exe : [.$(OBJ_DIR).modules.vms.chpdef]chpdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.uaidef]uaidef_wrap.c : [.modules.vms.uaidef]uaidef.i
    SWIG -python modules/vms/uaidef/uaidef.i
    purge [.modules.vms.uaidef]uaidef_wrap.c

[.$(OBJ_DIR).modules.vms.uaidef]uaidef_wrap.obs : [.modules.vms.uaidef]uaidef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_uaidef.exe : [.$(OBJ_DIR).modules.vms.uaidef]uaidef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.uafdef]uafdef_wrap.c : [.modules.vms.uafdef]uafdef.i
    SWIG -python modules/vms/uafdef/uafdef.i
    purge [.modules.vms.uafdef]uafdef_wrap.c

[.$(OBJ_DIR).modules.vms.uafdef]uafdef_wrap.obs : [.modules.vms.uafdef]uafdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_uafdef.exe : [.$(OBJ_DIR).modules.vms.uafdef]uafdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.rmidef]rmidef_wrap.c : [.modules.vms.rmidef]rmidef.i
    SWIG -python modules/vms/rmidef/rmidef.i
    purge [.modules.vms.rmidef]rmidef_wrap.c

[.$(OBJ_DIR).modules.vms.rmidef]rmidef_wrap.obs : [.modules.vms.rmidef]rmidef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_rmidef.exe : [.$(OBJ_DIR).modules.vms.rmidef]rmidef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.regdef]regdef_wrap.c : [.modules.vms.regdef]regdef.i
    SWIG -python modules/vms/regdef/regdef.i
    purge [.modules.vms.regdef]regdef_wrap.c

[.$(OBJ_DIR).modules.vms.regdef]regdef_wrap.obs : [.modules.vms.regdef]regdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_regdef.exe : [.$(OBJ_DIR).modules.vms.regdef]regdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.rabdef]rabdef_wrap.c : [.modules.vms.rabdef]rabdef.i
    SWIG -python modules/vms/rabdef/rabdef.i
    purge [.modules.vms.rabdef]rabdef_wrap.c

[.$(OBJ_DIR).modules.vms.rabdef]rabdef_wrap.obs : [.modules.vms.rabdef]rabdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_rabdef.exe : [.$(OBJ_DIR).modules.vms.rabdef]rabdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

! [.modules.vms.accdef]accdef_wrap.c : [.modules.vms.accdef]accdef.i
!     SWIG -python modules/vms/accdef/accdef.i
!     purge [.modules.vms.accdef]accdef_wrap.c

! [.$(OBJ_DIR).modules.vms.accdef]accdef_wrap.obs : [.modules.vms.accdef]accdef_wrap.c

! [.$(OUT_DIR).$(DYNLOAD_DIR)]_accdef.exe : [.$(OBJ_DIR).modules.vms.accdef]accdef_wrap.obs
!     @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
!     $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.$(OBJ_DIR).modules.vms.accdef]accdef.obs : [.modules.vms.accdef]accdef.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_accdef.exe : [.$(OBJ_DIR).modules.vms.accdef]accdef.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT


[.modules.vms.acldef]acldef_wrap.c : [.modules.vms.acldef]acldef.i
    SWIG -python modules/vms/acldef/acldef.i
    purge [.modules.vms.acldef]acldef_wrap.c

[.$(OBJ_DIR).modules.vms.acldef]acldef_wrap.obs : [.modules.vms.acldef]acldef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_acldef.exe : [.$(OBJ_DIR).modules.vms.acldef]acldef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.acrdef]acrdef_wrap.c : [.modules.vms.acrdef]acrdef.i
    SWIG -python modules/vms/acrdef/acrdef.i
    purge [.modules.vms.acrdef]acrdef_wrap.c

[.$(OBJ_DIR).modules.vms.acrdef]acrdef_wrap.obs : [.modules.vms.acrdef]acrdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_acrdef.exe : [.$(OBJ_DIR).modules.vms.acrdef]acrdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.armdef]armdef_wrap.c : [.modules.vms.armdef]armdef.i
    SWIG -python modules/vms/armdef/armdef.i
    purge [.modules.vms.armdef]armdef_wrap.c

# *****************************************************************
[.modules.vms.secsrvdef]secsrvdef_wrap.c : [.modules.vms.secsrvdef]secsrvdef.i
    SWIG -python modules/vms/secsrvdef/secsrvdef.i
    purge [.modules.vms.secsrvdef]secsrvdef_wrap.c

[.$(OBJ_DIR).modules.vms.secsrvdef]secsrvdef_wrap.obs : [.modules.vms.secsrvdef]secsrvdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_secsrvdef.exe : [.$(OBJ_DIR).modules.vms.secsrvdef]secsrvdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT
# *****************************************************************
[.modules.vms.secsrvmsg]secsrvmsg_wrap.c : [.modules.vms.secsrvmsg]secsrvmsg.i
    SWIG -python modules/vms/secsrvmsg/secsrvmsg.i
    purge [.modules.vms.secsrvmsg]secsrvmsg_wrap.c

[.$(OBJ_DIR).modules.vms.secsrvmsg]secsrvmsg_wrap.obs : [.modules.vms.secsrvmsg]secsrvmsg_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_secsrvmsg.exe : [.$(OBJ_DIR).modules.vms.secsrvmsg]secsrvmsg_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT
# *****************************************************************

[.$(OBJ_DIR).modules.vms.armdef]armdef_wrap.obs : [.modules.vms.armdef]armdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_armdef.exe : [.$(OBJ_DIR).modules.vms.armdef]armdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.stenvdef]stenvdef_wrap.c : [.modules.vms.stenvdef]stenvdef.i
    SWIG -python modules/vms/stenvdef/stenvdef.i
    purge [.modules.vms.stenvdef]stenvdef_wrap.c

[.$(OBJ_DIR).modules.vms.stenvdef]stenvdef_wrap.obs : [.modules.vms.stenvdef]stenvdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_stenvdef.exe : [.$(OBJ_DIR).modules.vms.stenvdef]stenvdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.statedef]statedef_wrap.c : [.modules.vms.statedef]statedef.i
    SWIG -python modules/vms/statedef/statedef.i
    purge [.modules.vms.statedef]statedef_wrap.c

[.$(OBJ_DIR).modules.vms.statedef]statedef_wrap.obs : [.modules.vms.statedef]statedef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_statedef.exe : [.$(OBJ_DIR).modules.vms.statedef]statedef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.brkdef]brkdef_wrap.c : [.modules.vms.brkdef]brkdef.i
    SWIG -python modules/vms/brkdef/brkdef.i
    purge [.modules.vms.brkdef]brkdef_wrap.c

[.$(OBJ_DIR).modules.vms.brkdef]brkdef_wrap.obs : [.modules.vms.brkdef]brkdef_wrap.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_brkdef.exe : [.$(OBJ_DIR).modules.vms.brkdef]brkdef_wrap.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.sys]sys_wrap.c : [.modules.vms.sys]sys.i
    SWIG -python modules/vms/sys/sys.i
    purge [.modules.vms.sys]sys_wrap.c

[.$(OBJ_DIR).modules.vms.sys]sys_wrap.obs : [.modules.vms.sys]sys_wrap.c
   @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC)$(PY_CFLAGS_OSF_ILE)/OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).modules.vms.sys]sys.obs : [.modules.vms.sys]sys.c
   @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC)$(PY_CFLAGS_OSF_ILE)/OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OUT_DIR).$(DYNLOAD_DIR)]_sys.exe : [.$(OBJ_DIR).modules.vms.sys]sys_wrap.obs,-
[.$(OBJ_DIR).modules.vms.sys]sys.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

!! ************ DTR ***********
[.modules.vms.dtr]dtr_wrap.c : [.modules.vms.dtr]dtr.i
    SWIG -python modules/vms/dtr/dtr.i
    purge [.modules.vms.dtr]dtr_wrap.c

[.$(OBJ_DIR).modules.vms.dtr]dtr_wrap.obs : [.modules.vms.dtr]dtr_wrap.c
   @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC)$(PY_CFLAGS_DTR)/OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).modules.vms.dtr]dtr.obs : [.modules.vms.dtr]dtr.c
   @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC)$(PY_CFLAGS_DTR)/OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).modules.vms.dtr]rec.obs : [.modules.vms.dtr]rec.c
   @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC)$(PY_CFLAGS_DTR)/OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OBJ_DIR).modules.vms.dtr]rsscanf.obs : [.modules.vms.dtr]rsscanf.c
   @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC)$(PY_CFLAGS_DTR)/OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OUT_DIR).$(DYNLOAD_DIR)]_dtr.exe : [.$(OBJ_DIR).modules.vms.dtr]dtr_wrap.obs,-
[.$(OBJ_DIR).modules.vms.dtr]dtr.obs,-
[.$(OBJ_DIR).modules.vms.dtr]rec.obs,-
[.$(OBJ_DIR).modules.vms.dtr]rsscanf.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.dtr]rec_wrap.c : [.modules.vms.dtr]rec.i
    SWIG -python modules/vms/dtr/rec.i
    purge [.modules.vms.dtr]rec_wrap.c

[.$(OBJ_DIR).modules.vms.dtr]rec_wrap.obs : [.modules.vms.dtr]rec_wrap.c
   @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(CC)$(PY_CFLAGS_DTR)/OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

[.$(OUT_DIR).$(DYNLOAD_DIR)]_rec.exe : [.$(OBJ_DIR).modules.vms.dtr]rec_wrap.obs,-
[.$(OBJ_DIR).modules.vms.dtr]rec.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

!! ^^^^^^^^^^^ DTR ^^^^^^^^^^^

! RDB need sql$mod installed
[.modules.rdb]rdb_wrap.c : [.modules.rdb]rdb.i, [.modules.rdb]db.h
    SWIG -python modules/rdb/rdb.i
    purge [.modules.rdb]rdb_wrap.c

[.$(OBJ_DIR).modules.rdb]rdb_wrap.obs : [.modules.rdb]rdb_wrap.c

[.$(OBJ_DIR).modules.rdb]db.obs : [.modules.rdb]db.c, [.modules.rdb]db.h

[.$(OBJ_DIR).modules.rdb]sql.obj : [.modules.rdb]sql.sqlmod
    sqlmod [.modules.rdb]sql.sqlmod
    rename sql.obj python$build_obj:[modules.rdb]

[.$(OUT_DIR).$(DYNLOAD_DIR)]_rdb.exe : [.$(OBJ_DIR).modules.rdb]rdb_wrap.obs,-
[.$(OBJ_DIR).modules.rdb]db.obs,-
[.$(OBJ_DIR).modules.rdb]sql.obj
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.ile3]ile3_wrap.c : [.modules.vms.ile3]ile3.i
    SWIG -python modules/vms/ile3/ile3.i
    purge [.modules.vms.ile3]ile3_wrap.c

[.$(OBJ_DIR).modules.vms.ile3]ile3_wrap.obs : [.modules.vms.ile3]ile3_wrap.c

[.$(OBJ_DIR).modules.vms.ile3]ile3.obs : [.modules.vms.ile3]ile3.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_ile3.exe : [.$(OBJ_DIR).modules.vms.ile3]ile3_wrap.obs,-
[.$(OBJ_DIR).modules.vms.ile3]ile3.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.modules.vms.lib]lib_wrap.c : [.modules.vms.lib]lib.i
    SWIG -python modules/vms/lib/lib.i
    purge [.modules.vms.lib]lib_wrap.c

[.$(OBJ_DIR).modules.vms.lib]lib_wrap.obs : [.modules.vms.lib]lib_wrap.c

[.$(OBJ_DIR).modules.vms.lib]lib.obs : [.modules.vms.lib]lib.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_lib.exe : [.$(OBJ_DIR).modules.vms.lib]lib_wrap.obs,-
[.$(OBJ_DIR).modules.vms.lib]lib.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT

[.$(OBJ_DIR).Modules.vms.rms]rms.obs : [.Modules.vms.rms]rms.c

[.$(OUT_DIR).$(DYNLOAD_DIR)]_rms.exe : [.$(OBJ_DIR).Modules.vms.rms]rms.obs
    @ pipe create/dir $(DIR $(MMS$TARGET)) | copy SYS$INPUT nl:
    $(LINK)$(LINKFLAGS)/SHARE=python$build_out:[$(DYNLOAD_DIR)]$(NOTDIR $(MMS$TARGET_NAME)).EXE $(MMS$SOURCE_LIST),[.opt]$(NOTDIR $(MMS$TARGET_NAME)).opt/OPT


