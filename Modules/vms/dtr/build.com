$ set noverify
$ @sys$startup:dtrstup.com;
$ set verify
$
$ ccopt = "/nolist/names=(as_is,shortened)/define=(_USE_STD_STAT,__PYTHON)/include=([],[-],python$root:[include],dtr$library)
$
$ ! DTR
$ swig :== $swig$root:[bin]swig.exe
$ swig -python dtr.i
$ purge/log
$
$ cc'ccopt' dtr_wrap.c
$ cc'ccopt' dtr.c
$ cc'ccopt' rec.c
$ cc'ccopt' rsscanf.c
$ delete/log/noconf dtr_wrap.c;*
$
$ link/share=_dtr.exe dtr.obj,rec.obj,rsscanf.obj,dtr_wrap.obj,sys$input/opt
python$shr/share
sys$library:dtrshr.exe/share
dtr$library:termserve.olb/lib
GSMATCH=LEQUAL,1,0
case_sensitive=YES
SYMBOL_VECTOR=(PyInit__dtr=PROCEDURE)
$ purge/log [...]
$ delete/log/noconf dtr_wrap.obj;*
$
$
$ ! REC
$ swig :== $swig$root:[bin]swig.exe
$ swig -python rec.i
$ purge/log
$
$ cc'ccopt' rec_wrap.c
$ cc'ccopt' rec.c
$ delete/log/noconf rec_wrap.c;*
$
$ link/share=_rec.exe rec.obj,rec_wrap.obj,sys$input/opt
python$shr/share
GSMATCH=LEQUAL,1,0
case_sensitive=YES
SYMBOL_VECTOR=(PyInit__rec=PROCEDURE)
$ purge/log [...]
$ delete/log/noconf *.obj;*
$
$
$ exit
