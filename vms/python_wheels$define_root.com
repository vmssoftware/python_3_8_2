$! 'f$verify(0)
$
$! Create python_wheels$define_logicals.com such that python_wheels$root is corectly defined
$!
$
$ root = f$trnlmn("pcsi$destination") - "]" + "wheels.]"
$
$ open/write fd sys$common:[sysmgr]python_wheels$define_logicals.com
$ write fd "$ define/system/trans=concealed python_wheels$root ''root'"
$ write fd "$ define/system PIP_FIND_LINKS ""/PYTHON_WHEELS$ROOT"""
$ write fd "$ exit"
$ close fd
$ purge/nolog sys$startup:python_wheels$define_logicals.com
$
$ exit
