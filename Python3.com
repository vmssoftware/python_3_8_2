$ if "''P1'" .eqs. "" 
$ then
$   CONFIG := DEBUG
$ else
$   CONFIG := 'P1'
$ endif
$ write sys$output "Prepare for ''CONFIG'"
$ INSTALL_DIR = "wrk_disk:[vorfolomeev.Python3.out.]"
$ define /trans=concealed python$root 'INSTALL_DIR'
$ define PYTHONHOME "/python$root"
$ define PYTHONCASEOK "YES"
$ define python$shr python$root:[lib]python$shr.exe
$ delete/tree python$root:[lib...]*.*;*
$ delete/tree python$root:[bin...]*.*;*
$ backup WRK_DISK:[vorfolomeev.Python3.out.'CONFIG']python3.exe python$root:[bin]
$ backup WRK_DISK:[vorfolomeev.Python3.out.'CONFIG']python$shr.exe python$root:[lib]
$ backup WRK_DISK:[vorfolomeev.Python3.lib...]*.* python$root:[lib.python3^.8...]
$ backup WRK_DISK:[vorfolomeev.Python3.out.'CONFIG'.lib-dynload...]*.* python$root:[lib.python3^.8.lib-dynload...]*.*
$ ! next file must be present
$ backup WRK_DISK:[vorfolomeev.Python3.build.lib^.linux-x86_64-3^.8]_sysconfigdata__linux_x86_64-linux-gnu.py python$root:[lib.python3^.8]_sysconfigdata_m_OpenVMS_cpython-38m-ia64-openvms.py
$ python :== $python$root:[bin]python3.exe
