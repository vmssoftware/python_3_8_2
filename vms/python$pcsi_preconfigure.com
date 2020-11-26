$ verify = f$verify(0)
$ set noon
$
$ define/job/nolog python$ods5_avail 0
$
$ loop:
$    dev = f$device("*", "DISK")
$    if dev .nes. ""
$    then
$       if f$getdvi("''dev'", "ACPTYPE") .eqs. "F11V5"
$       then
$          define/job/nolog python$ods5_avail 1
$       else
$          goto loop
$       endif
$    endif
$
$ verify = f$verify(verify)
$ exit
