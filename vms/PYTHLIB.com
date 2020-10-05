$ set verify
$
$ delete/log/noconf vsi-i64vms-PYTHLIB-*.pcsi;*
$ delete/log/noconf vsi-i64vms-PYTHLIB-*.pcsi$compressed;*
$
$ product package PYTHLIB -
        /source=PYTHLIB.pcsi$desc -
        /destination=[] -
        /material=PYTHON$ROOT:[lib.python3^.8.site-packages] -
        /format=sequential -
        /opt=noconf -
        /log -
        /producer=VSI
$
$ product copy PYTHLIB/dest=[]/format=compressed/opt=noconf
$ purge/log
$
$ purge/log [...]
$
$ set noverify
$
$ exit

