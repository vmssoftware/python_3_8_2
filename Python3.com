$ INSTALL_DIR = "wrk_disk:[vorfolomeev.Python3.out.]"
$ define /trans=concealed python$root 'INSTALL_DIR'
$ define PYTHONHOME "/python$root"
$ define PYTHONCASEOK "YES"
$ define python$shr python$root:[lib]python$shr.exe
$ delete/tree python$root:[lib...]*.*;*
$ backup/replace WRK_DISK:[vorfolomeev.Python3.out.debug]python3.exe python$root:[bin]
$ backup WRK_DISK:[vorfolomeev.Python3.out.debug]python$shr.exe python$root:[lib]
$ backup WRK_DISK:[vorfolomeev.Python3.lib...]*.* python$root:[lib.python3^.8...]
$ backup WRK_DISK:[vorfolomeev.Python3.out.debug.lib-dynload...]*.EXE python$root:[lib.python3^.8.lib-dynload...]*.EXE
$ ! backup WRK_DISK:[vorfolomeev.Python3.Modules.vms...]*.py python$root:[lib.python3^.8.vms]
$ ! next file must be present
$ backup WRK_DISK:[vorfolomeev.Python3.build.lib^.linux-x86_64-3^.8]_sysconfigdata__linux_x86_64-linux-gnu.py python$root:[lib.python3^.8]_sysconfigdata_m_OpenVMS_cpython-38m-ia64-openvms.py
$ python :== $python$root:[bin]python3.exe
