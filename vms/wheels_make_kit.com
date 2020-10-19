$ set verify
$
$ delete/log/noconf vsi-i64vms-pythwhls-*.pcsi;*
$ delete/log/noconf vsi-i64vms-pythwhls-*.pcsi$compressed;*
$
$ product package pythwhls -
        /source=pythwhls.pcsi$desc -
        /destination=[] -
        /material=(python_wheels$root:) -
        /format=sequential -
        /opt=noconf -
        /log -
        /producer=VSI
$
$ product copy pythwhls/source=[]/dest=[]/format=compressed/opt=noconf
$ purge/log
$
$ purge/log [...]
$
$ set noverify
$
$ exit

