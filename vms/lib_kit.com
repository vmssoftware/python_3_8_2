$ set verify
$
$ delete/log/noconf vsi-i64vms-python-libs-*.pcsi;*
$ delete/log/noconf vsi-i64vms-python-libs-*.pcsi$compressed;*
$
$ src_pat = python$root:[lib.python3^.8.site-packages]
$
$ product package python_libs -
        /source=python.pcsi$desc -
        /destination='src_pat' -
        /material=('src_pat') -
        /format=sequential -
        /opt=noconf -
        /log -
        /producer=VSI
$
$ product copy python/dest=[]/format=compressed/opt=noconf
$ purge/log
$
$ purge/log [...]
$
$ set noverify
$
$ exit

