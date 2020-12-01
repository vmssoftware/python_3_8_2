"""distutils.openvmsccompiler

Contains the OpenVMSCCompiler class, a subclass of CCompiler that handles
the OpenVMS-style command-line C compiler:
  * macros defined with /DEFINE=(...) qualifier
  * macros undefined with /UNDEFINE=(...) qualifier
  * include search directories specified with /INCLUDE_DIRECTORY=(...)
  * libraries
  * library search directories
  * compile handled by 'CC' (or similar) executable:
    compiles .c to .obj
  * link static library handled by 'LIBRARY' command
  * link shared library handled by 'LINK /SHAREABLE'
"""

import os, sys, re
import tempfile
import stat

from distutils import sysconfig
from distutils.dep_util import newer
from distutils.ccompiler import \
     CCompiler, gen_lib_options
from distutils.errors import \
     DistutilsExecError, CompileError, LibError, LinkError
from distutils import log

import subprocess

if sys.platform == 'OpenVMS':
    import vms.decc


class OpenVMSCCompiler(CCompiler):

    compiler_type = 'openvms'

    # These are used by CCompiler in two places: the constructor sets
    # instance attributes 'preprocessor', 'compiler', etc. from them, and
    # 'set_executable()' allows any of these to be set.  The defaults here
    # are pretty generic; they will probably have to be set by an outsider
    # (eg. using information discovered by the sysconfig about building
    # Python extensions).
    executables = {
                   'archiver'     : ["LIBRARY"],
                   'compiler'     : ["CC"],
                   'compiler_c'   : ["CC"],
                   'compiler_cxx' : ["CXX"],
                   'compiler_so'  : ["CC"],
                   'linker'       : ["LINK"],
                   'linker_exe'   : ["LINK"],
                   'linker_so'    : ["LINK"],
                   'preprocessor' : None,
                   'ranlib'       : None,
                  }

    # Needed for the filename generation methods provided by the base
    # class, CCompiler.  NB. whoever instantiates/uses a particular
    # UnixCCompiler instance should set 'shared_lib_ext' -- we set a
    # reasonable common default here, but it's not necessarily used on all
    # Unices!

    src_extensions = [".c",".C",".cc",".cxx",".cpp"]
    obj_extension = ".obj"
    static_lib_extension = ".olb"
    shared_lib_extension = ".exe"
    dylib_lib_extension = ".exe"
    xcode_stub_lib_extension = ".tbd"
    static_lib_format = shared_lib_format = dylib_lib_format = "%s%s"
    xcode_stub_lib_format = dylib_lib_format
    exe_extension = ".exe"

    def _setup_compile(self, outdir, macros, incdirs, sources, depends,
                       extra):
        """Process arguments and decide which source files to compile."""
        if outdir is None:
            outdir = self.output_dir
        elif not isinstance(outdir, str):
            raise TypeError("'output_dir' must be a string or None")

        if macros is None:
            macros = self.macros
        elif isinstance(macros, list):
            macros = macros + (self.macros or [])
        else:
            raise TypeError("'macros' (if supplied) must be a list of tuples")

        if incdirs is None:
            incdirs = self.include_dirs
        elif isinstance(incdirs, (list, tuple)):
            incdirs = list(incdirs) + (self.include_dirs or [])
        else:
            raise TypeError(
                  "'include_dirs' (if supplied) must be a list of strings")

        if extra is None:
            extra = []

        # Get the list of expected output (object) files
        objects = self.object_filenames(sources, strip_dir=0,
                                        output_dir=outdir)
        assert len(objects) == len(sources)

        pp_opts = self._gen_preprocess_options(macros, incdirs)

        build = {}
        for i in range(len(sources)):
            src = sources[i]
            obj = objects[i]
            ext = os.path.splitext(src)[1]
            self.mkpath(os.path.dirname(obj))
            build[obj] = (src, ext)

        return macros, objects, extra, pp_opts, build

    def _gen_preprocess_options(self, macros, include_dirs):
        """ Generate C pre-processor options
        """
        pp_define = []
        pp_undefine = []
        for macro in macros:
            if not (isinstance(macro, tuple) and 1 <= len(macro) <= 2):
                raise TypeError(
                    "bad macro definition '%s': "
                    "each element of 'macros' list must be a 1- or 2-tuple"
                    % macro)

            if len(macro) == 1:        # undefine this macro
                pp_undefine.append(macro[0])
            elif len(macro) == 2:
                if macro[1] in (None, ''):    # define with no explicit value
                    pp_define.append(macro[0])
                else:
                    if isinstance(macro[1], str) and macro[1].startswith('"'):
                        macro_value = '"""' + macro[1][1:-1] + '"""'
                        pp_define.append("%s=%s" % (macro[0], macro_value))
                    else:
                        pp_define.append("%s=%s" % macro)

        pp_opts = [ \
            '/NAMES=(AS_IS,SHORTENED)',
            '/WARNINGS=WARNINGS=ALL',
            # '/WARNINGS=ERRORS=IMPLICITFUNC',
            # '/L_DOUBLE_SIZE=64',    # float128(nan) == inf
            ]
        if len(pp_undefine):
            pp_opts.append("/UNDEFINE=(" + ",".join(pp_undefine) + ")")
        if len(pp_define):
            pp_opts.append("/DEFINE=(" + ",".join(pp_define) + ")")

        if len(include_dirs):
            def fix_dir(directory):
                if not '[]' in directory:
                    return '"' + directory + '"'
                return directory
            pp_opts.append(f"/INCLUDE_DIRECTORY=({','.join(map(fix_dir, include_dirs))})")

        return pp_opts

    def _get_cc_args(self, pp_opts, debug, before):
        """ Generate C compiler qualifiers
        """
        cc_args = []
        if debug:
            cc_args.append("/DEBUG/NOOPTIMIZE")
        else:
            cc_args.append("/NODEBUG/OPTIMIZE")

        if before:
            cc_args[:0] = before

        return cc_args

    def _compile(self, obj, src, ext, cc_args, extra_postargs, pp_opts):
        lang = self.detect_language(src)
        if lang == "c++":
            compiler = self.compiler_cxx
        else:
            compiler = self.compiler_c

        try:
            src_vms = vms.decc.to_vms(src, 0, 0)[0]
            obj_vms = vms.decc.to_vms(obj, 0, 0)[0]
            cmd_list = compiler + cc_args + pp_opts + extra_postargs + [src_vms, '/OBJECT=' + obj_vms]
            self.spawn(cmd_list)
        except DistutilsExecError as msg:
            raise CompileError(msg)

    def create_static_lib(self, objects, output_libname,
                          output_dir=None, debug=0, target_lang=None):
        objects, output_dir = self._fix_object_args(objects, output_dir)

        output_filename = \
            self.library_filename(output_libname, output_dir=output_dir)

        if self._need_link(objects, output_filename):
            self.mkpath(os.path.dirname(output_filename))
            output_filename_vms = vms.decc.to_vms(output_filename, 0, 0)[0]
            self.spawn(self.archiver + ['/CREATE', output_filename_vms])
            for input_name in objects + self.objects:
                input_name_vms = vms.decc.to_vms(input_name, 0, 0)[0]
                self.spawn(self.archiver + [output_filename_vms, input_name_vms])
        else:
            log.debug("skipping %s (up-to-date)", output_filename)

    def link(self, target_desc, objects,
             output_filename, output_dir=None, libraries=None,
             library_dirs=None, runtime_library_dirs=None,
             export_symbols=None, debug=0, extra_preargs=None,
             extra_postargs=None, build_temp=None, target_lang=None):
        objects, output_dir = self._fix_object_args(objects, output_dir)
        fixed_args = self._fix_lib_args(libraries, library_dirs,
                                        runtime_library_dirs)
        libraries, library_dirs, runtime_library_dirs = fixed_args

        # lib_opts = gen_lib_options(self, library_dirs, runtime_library_dirs,
        #                            libraries)

        if not isinstance(output_dir, (str, type(None))):
            raise TypeError("'output_dir' must be a string or None")
        if output_dir is not None:
            output_filename = os.path.join(output_dir, output_filename)

        if self._need_link(objects, output_filename):
            # ld_args = (objects + self.objects +
            #            lib_opts + [link_output_qualifier, output_filename])
            opt_file = tempfile.NamedTemporaryFile(suffix='.OPT', delete=False)
            opt_lines = []

            for obj_file in objects:
                obj_file_vms = vms.decc.to_vms(obj_file, 0, 0)[0]
                opt_lines.append(obj_file_vms)

            vms_libraries_set = set()

            verbose = False # True # '_multiarray_umath' in output_filename

            for lib_file in libraries:
                lib_file_vms = None
                _, lib_file_ext = os.path.splitext(lib_file)
                if lib_file_ext:
                    # looks like full path
                    lib_file_ext = lib_file_ext.upper()
                    if lib_file_ext in ('.OLB', '.EXE'):
                        if re.search(r'[:\[\]]', lib_file):
                            lib_file_vms = lib_file
                        else:
                            lib_file_vms = vms.decc.to_vms(lib_file, 0, 0)[0]
                if not lib_file_vms:
                    # find the library in the library_dirs
                    for lib_dir in library_dirs:
                        for lib_ext in ['','.OLB','.EXE']:
                            try:
                                lib_path = os.path.join(lib_dir, lib_file + lib_ext)
                                st = os.stat(lib_path)
                                if not stat.S_ISDIR(st.st_mode):
                                    lib_file_ext = lib_ext
                                    lib_file_vms = vms.decc.to_vms(lib_path, 0, 0)[0]
                                    break
                            except:
                                pass
                        else:
                            continue
                        break
                if lib_file_vms and lib_file_vms.lower() not in vms_libraries_set:
                    # write it to the OPT
                    opt_line = lib_file_vms + ('/LIBRARY' if lib_file_ext == '.OLB' else '/SHAREABLE')
                    opt_lines.append(opt_line)
                    vms_libraries_set.add(lib_file_vms.lower())

            opt_lines.append('GSMATCH=LEQUAL,1,0')
            opt_lines.append('CASE_SENSITIVE=YES')

            proc_names = dict()
            try:
                import _rms
                from vms.fabdef import FAB_M_GET, FAB_M_SHRGET
                from vms.rmsdef import RMS__EOF
                repository = _rms.file('[.CXX_REPOSITORY]CXX$DEMANGLER_DB', fac=FAB_M_GET, shr=FAB_M_SHRGET)
                while(1):
                    s, r = repository.fetch()
                    if r == None or s == RMS__EOF:
                        break
                    r = r.decode()
                    full_name = r[31:]
                    short_name = r[:31]
                    proc_names[full_name] = short_name
                repository.close()
            except Exception as ex:
                if verbose:
                    print(ex)
                pass

            def shorten_name(name):
                if len(name) <= 31:
                    return name
                try:
                    return proc_names[name]
                except:
                    return name[:31]

            if export_symbols and len(export_symbols):
                opt_lines.append('SYMBOL_VECTOR=( -')
                for export_symbol in export_symbols[:-1]:
                    opt_lines.append(shorten_name(export_symbol) + '=PROCEDURE, -')
                for export_symbol in export_symbols[-1:]:
                    opt_lines.append(shorten_name(export_symbol) + '=PROCEDURE )')

            if verbose:
                print("--- OPT START\n")
                print('\n'.join(opt_lines))
                print("--- OPT END\n")

            opt_file.write(('\n'.join(opt_lines)).encode())
            opt_file.close()
            opt_name_vms = vms.decc.to_vms(opt_file.name, 0, 0)[0]

            link_output_qualifier = '/EXECUTABLE=' if target_desc == CCompiler.EXECUTABLE else '/SHAREABLE='
            output_filename_vms = vms.decc.to_vms(output_filename, 0, 0)[0]
            ld_args = [link_output_qualifier + output_filename_vms, opt_name_vms + "/OPTIONS"]

            if debug:
                map_file, _ = os.path.splitext(output_filename)
                map_file_vms = vms.decc.to_vms(map_file + '.MAP', 0, 0)[0]
                ld_args[:0] = ['/DEBUG/MAP=' + map_file_vms]

            if extra_preargs:
                ld_args[:0] = extra_preargs

            if extra_postargs:
                ld_args.extend(extra_postargs)

            self.mkpath(os.path.dirname(output_filename))

            try:
                linker = self.linker[:]
                self.spawn(linker + ld_args)
            except DistutilsExecError as msg:
                raise LinkError(msg)
            finally:
                os.unlink(opt_file.name)
        else:
            log.debug("skipping %s (up-to-date)", output_filename)

    # -- Miscellaneous methods -----------------------------------------
    # These are all used by the 'gen_lib_options() function, in
    # ccompiler.py.

    def library_dir_option(self, dir):
        # TODO: add them to .OPT file?
        return dir

    def _is_gcc(self, compiler_name):
        return False

    def runtime_library_dir_option(self, dir):
        return dir

    def library_option(self, lib):
        return lib

    def find_library_file(self, dirs, lib, debug=0):

        if lib.lower() == 'python$shr.exe':
            return os.path.join('/python$root/lib', lib)

        # shared_f = self.library_filename(lib, lib_type='shared')
        # dylib_f = self.library_filename(lib, lib_type='dylib')
        # xcode_stub_f = self.library_filename(lib, lib_type='xcode_stub')
        # static_f = self.library_filename(lib, lib_type='static')

        # for dir in dirs:
        #     shared = os.path.join(dir, shared_f)
        #     dylib = os.path.join(dir, dylib_f)
        #     static = os.path.join(dir, static_f)
        #     xcode_stub = os.path.join(dir, xcode_stub_f)

        #     # We're second-guessing the linker here, with not much hard
        #     # data to go on: GCC seems to prefer the shared library, so I'm
        #     # assuming that *all* Unix C compilers do.  And of course I'm
        #     # ignoring even GCC's "-static" option.  So sue me.
        #     if os.path.exists(dylib):
        #         return dylib
        #     elif os.path.exists(xcode_stub):
        #         return xcode_stub
        #     elif os.path.exists(shared):
        #         return shared
        #     elif os.path.exists(static):
        #         return static

        # Oops, didn't find it in *any* of 'dirs'
        return None
