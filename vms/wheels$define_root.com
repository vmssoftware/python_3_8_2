$ root = f$trnlmn("pcsi$destination") - "]" + "wheels.]"
$
$ open/write fd sys$common:[sysmgr]python_wheels$define_logicals.com
$ write fd "$ define/system/trans=concealed python_wheels$root ''root'"
$ write fd "$ define/system PIP_FIND_LINKS "/python_wheels$root"
$ write fd "$ exit"
$ close fd
$ purge/nolog sys$startup:python_wheels$define_logicals.com
$
$ exit
