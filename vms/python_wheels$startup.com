$ ! PYTHON_WHEELS$STARTUP.COM
$ !+
$ ! 02-Nov-2020
$ ! Startup file for wheels for Python 3.8.2 on OpenVMS
$ !
$ !-
$
$ set noon
$
$ file = f$search("sys$startup:python_wheels$define_logicals.com")
$
$ if file .eqs. ""
$ then
$    root = f$trnlmn("pcsi$destination")
$    if "''root'" .eqs. ""
$    then
$       write sys$output "The logical name python_wheels$root is not defined; check installation."
$       exit
$    endif
$
$    root = "''root" - "]" + "wheels.]"
$    define/system/trans=concealed PYTHON_WHEELS$ROOT 'root
$    define/system PIP_FIND_LINKS "/PYTHON_WHEELS$ROOT"
$ else
$    @sys$startup:python_wheels$define_logicals.com
$ endif
$
$ exit
