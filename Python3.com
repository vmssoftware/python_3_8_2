$ if "''P1'" .eqs. ""
$ then
$   CONFIG := DEBUG
$ else
$   CONFIG := 'P1'
$ endif
$ write sys$output "Prepare for ''CONFIG'"
$
$ com_nam = f$environment("procedure")
$ com_dir = f$parse(com_nam,,,"directory")
$ com_dev = f$parse(com_nam,,,"device")
$ com_pat = com_dev + com_dir
$ bld_pat = com_dev + com_dir - "]" + ".out.''CONFIG']"
$ inc_pat = com_dev + com_dir - "]" + ".include]"
$ lib_pat = com_dev + com_dir - "]" + ".lib...]"
$ vms_pat = com_dev + com_dir - "]" + ".vms]"
$ dyn_pat = com_dev + com_dir - "]" + ".out.'CONFIG'.lib-dynload...]"
$ cnf_pat = com_dev + com_dir - "]" + ".build]"
$ @'com_pat'Python3_names.com
$
$ pipe delete/tree python$root:[000000...]*.*;* | copy SYS$INPUT nl:
$
$ backup 'bld_pat'python3.exe python$root:[bin]
$
$ backup 'inc_pat'*.h python$root:[include]
$ backup 'com_pat'pyconfig.h python$root:[include]
$ backup 'com_pat'py_vms.h python$root:[include]
$
$ backup 'lib_pat'*.* python$root:[lib.python3^.8...]
$ backup 'bld_pat'python$shr.exe python$root:[lib]
$
$ backup 'vms_pat'*.com python$root:[000000]
$ backup 'vms_pat'*.pcsi* python$root:[000000]
$
$ backup 'dyn_pat'*.* python$root:[lib.python3^.8.lib-dynload...]*.*
$ ! next file must be present
$ backup 'cnf_pat'_sysconfigdata__linux_x86_64-linux-gnu.py python$root:[lib.python3^.8]_sysconfigdata_m_OpenVMS_cpython-38m-ia64-openvms.py
