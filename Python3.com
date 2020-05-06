$ @WRK_DISK:[vorfolomeev.Python3]Python3_names.com
$ if "''P1'" .eqs. ""
$ then
$   CONFIG := DEBUG
$ else
$   CONFIG := 'P1'
$ endif
$ write sys$output "Prepare for ''CONFIG'"
$
$ pipe delete/tree python$root:[000000...]*.*;* | copy SYS$INPUT nl:
$
$ backup WRK_DISK:[vorfolomeev.Python3.out.'CONFIG']python3.exe python$root:[bin]
$
$ backup WRK_DISK:[vorfolomeev.Python3.include]*.h python$root:[include]
$ backup WRK_DISK:[vorfolomeev.Python3]pyconfig.h python$root:[include]
$
$ backup WRK_DISK:[vorfolomeev.Python3.lib...]*.* python$root:[lib.python3^.8...]
$ ! pipe delete/tree python$root:[lib...]*.exe;* | copy SYS$INPUT nl:
$ ! pipe delete/tree python$root:[...test...]*.*;* | copy SYS$INPUT nl:
$ ! pipe delete/tree python$root:[...idle_test...]*.*;* | copy SYS$INPUT nl:
$ ! pipe delete/tree python$root:[...unittest...]*.*;* | copy SYS$INPUT nl:
$ ! pipe delete/tree python$root:[...__pycache__...]*.*;* | copy SYS$INPUT nl:
$ backup WRK_DISK:[vorfolomeev.Python3.out.'CONFIG']python$shr.exe python$root:[lib]
$
$ backup WRK_DISK:[vorfolomeev.Python3.vms]*.com python$root:[000000]
$ backup WRK_DISK:[vorfolomeev.Python3.vms]*.pcsi* python$root:[000000]
$
$ backup WRK_DISK:[vorfolomeev.Python3.out.'CONFIG'.lib-dynload...]*.* python$root:[lib.python3^.8.lib-dynload...]*.*
$ ! next file must be present
$ backup WRK_DISK:[vorfolomeev.Python3.build]_sysconfigdata__linux_x86_64-linux-gnu.py python$root:[lib.python3^.8]_sysconfigdata_m_OpenVMS_cpython-38m-ia64-openvms.py
