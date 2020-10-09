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
    executables = {'compiler_c'   : ["CC"],
                   'compiler_cxx' : ["CXX"],
                   'linker'       : ["LINK"],
                   'archiver'     : ["LIBRARY"],
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
    static_lib_format = shared_lib_format = dylib_lib_format = "lib%s%s"
    xcode_stub_lib_format = dylib_lib_format
    exe_extension = ".exe"

    sys_defines = dict()

    def set_sys_defines(self, sys_defines):
        self.sys_defines = dict()
        for name, value in self.sys_defines.items():
            self.sys_defines[name] = value

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
                if macro[1] is None:    # define with no explicit value
                    pp_define.append(macro[0])
                else:
                    if isinstance(macro[1], str):
                        pp_define.append("%s=\"\"\"%s\"\"\"" % macro)
                    else:
                        pp_define.append("%s=%s" % macro)

        pp_opts = [ \
            '/NAMES=(AS_IS,SHORTENED)',
            '/WARNINGS=WARNINGS=ALL',
            '/WARNINGS=ERRORS=IMPLICITFUNC'
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

        cmd_file = None

        try:
            src_vms = vms.decc.to_vms(src, 0, 0)[0]
            obj_vms = vms.decc.to_vms(obj, 0, 0)[0]
            cmd_list = compiler + cc_args + pp_opts + [src_vms, '/OBJECT=' + obj_vms] + extra_postargs
            if len(self.sys_defines) > 0:
                cmd_file = tempfile.NamedTemporaryFile(suffix='.COM', delete=False)
                for name, value in self.sys_defines.items():
                    define_line = '$' + 'define ' + name + ' "' + value + '"\n'
                    cmd_file.write(define_line.encode())
                cmd_line = '$' + ' '.join(cmd_list) + '\n'
                cmd_file.write(cmd_line.encode())
                cmd_file.close()
                self.spawn(['@' + vms.decc.to_vms(cmd_file.name, 0, 0)[0]])
            else:
                self.spawn(cmd_list)
        except DistutilsExecError as msg:
            raise CompileError(msg)
        finally:
            if cmd_file:
                os.unlink(cmd_file.name)

    def create_static_lib(self, objects, output_libname,
                          output_dir=None, debug=0, target_lang=None):
        objects, output_dir = self._fix_object_args(objects, output_dir)

        output_filename = \
            self.library_filename(output_libname, output_dir=output_dir)

        if self._need_link(objects, output_filename):
            self.mkpath(os.path.dirname(output_filename))
            self.spawn(self.archiver +
                       [output_filename] +
                       objects + self.objects)
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

            for obj_file in objects:
                obj_file_vms = vms.decc.to_vms(obj_file, 0, 0)[0]
                opt_file.write(obj_file_vms.encode())
                opt_file.write(b'\n')

            for lib_file in libraries:
                _, ext = os.path.splitext(lib_file)
                if ext:
                    ext = ext.upper()
                    if ext in ('.OLB', '.EXE'):
                        if not '[]' in lib_file:
                            lib_file_vms = vms.decc.to_vms(lib_file, 0, 0)[0]
                        else:
                            lib_file_vms = lib_file
                        opt_file.write(lib_file_vms.encode())
                        opt_file.write(b'/LIBRARY\n' if ext == '.OLB' else b'/SHAREABLE\n' )

            opt_file.write(b'GSMATCH=LEQUAL,1,0\nCASE_SENSITIVE=YES\n')

            proc_names = dict()
            try:
                repository = open('CXX_REPOSITORY/CXX$DEMANGLER_DB')
                for line in repository:
                    full_name = line[31:-1]
                    short_name = line[:31]
                    proc_names[full_name] = short_name
                repository.close()
            except:
                pass

            def shorten_name(name):
                if len(name) <= 31:
                    return name
                try:
                    return proc_names[name]
                except:
                    return name[:31]

            if export_symbols and len(export_symbols):
                opt_file.write(('SYMBOL_VECTOR=(' + \
                    '\n'.join(list(shorten_name(x) + '=PROCEDURE' for x in export_symbols)) + \
                    ')\n').encode())

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
