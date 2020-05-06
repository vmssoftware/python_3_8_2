$! 'f$verify(0)
$
$! Create python$define_logicals.com such that python$root is corectly defined
$!
$
$ if "" .nes. f$trnlnm("python$verify") then set verify
$
$ root = f$trnlmn("pcsi$destination") - "]" + "python.]"
$
$ open/write fd sys$common:[sysmgr]python$define_logicals.com
$ write fd "$ define/system/trans=concealed python$root ''root'"
$ write fd "$ define/system python$shr python$root:[lib]python$shr.exe"
$ write fd "$ define/system PYTHONHOME ""/python$root"""
$ write fd "$ define/system PYTHONCASEOK ""YES"""
$ write fd "$ exit"
$ close fd
$ purge/nolog sys$startup:python$define_logicals.com
$
$ exit
