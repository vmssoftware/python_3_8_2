$ set verify
$ hell:
$ ! RMS interface
$ define cc$include python$root:[include]
$ ccopt = "/nolist/noopt/names=(as_is,shortened)/define=(_OSF_SOURCE,_USE_STD_STAT)/include=cc$include/warn=disable=(QUESTCOMPARE,QUESTCOMPARE1)"
$
$!! cc'ccopt' [.modules.vms.rms]rms.c
$ cc'ccopt' rms.c
$ link/share=[---.lib]_rms.exe rms.obj,sys$input/opt
sys$library:pthread$rtl.exe/share
python$shr/share
GSMATCH=LEQUAL,1,0
CASE_SENSITIVE=YES
SYMBOL_VECTOR=(PyInit__rms=PROCEDURE)
$ purge/log [...]
$ delete/log/noconf rms.obj;*
$
$
$!!! copy _rms.exe PYTHON$ROOT:[000000.lib.python3^.5.lib-dynload]
$!!! purge/log PYTHON$ROOT:[000000.lib.python3^.5.lib-dynload]
$ exit

