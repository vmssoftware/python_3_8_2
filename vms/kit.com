$ set verify
$
$ delete/log/noconf vsi-i64vms-python-*.pcsi;*
$ delete/log/noconf vsi-i64vms-python-*.pcsi$compressed;*
$
$ product package python -
        /source=python.pcsi$desc -
        /destination=[] -
        /material=([]) -
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
$ exit

