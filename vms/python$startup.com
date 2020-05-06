$ ! PYTHON$STARTUP.COM
$ !+
$ ! 06-May-2020
$ ! Startup file for Python 3.8.2 on OpenVMS
$ !
$ !-
$
$ set noon
$
$ file = f$search("sys$startup:python$define_logicals.com")
$
$ if file .eqs. ""
$ then
$    root = f$trnlmn("pcsi$destination")
$    if "''root'" .eqs. ""
$    then
$       write sys$output "The logical name python$root is not defined; check installation."
$       exit
$    endif
$
$    root = "''root" - "]" + "python.]"
$    define/system/trans=concealed python$root 'root
$    define/system python$shr python$root:[lib]python$shr.exe
$ else
$    @sys$startup:python$define_logicals.com
$ endif
$
$!! if f$file_attributes("PYTHON$SHR","KNOWN")
$!! then
$!!    install replace python$shr
$!! else
$!!    install add python$shr /open /header_res /share
$!! endif
$
$ exit
