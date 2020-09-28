$ set verify
$ python :== $python$root:[bin]python.exe
$
$ delete/log/noconfirm test.dat;*
$ create/fdl=rms_demo_1.fdl test.dat
$
$ write sys$output ">>> Running rms_demo_1.py..."
$ python rms_demo_1.py
$ write sys$output "<<< done."
$
$!!! TBD - this next test currently fails due to an issue with indexedfile.py
$ write sys$output ">>> Running rms_demo_2.py..."
$ python rms_demo_2.py
$ write sys$output "<<< done."
$
$ write sys$output ">>> Running rms_demo_3.py..."
$ python rms_demo_3.py
$ write sys$output "<<< done."
$
$ write sys$output ">>> Running rms_demo_4.py..."
$ python rms_demo_4.py
$ write sys$output "<<< done."
$
$ write sys$output ">>> Running rms_demo_5.py..."
$ python rms_demo_5.py
$ write sys$output "<<< done."
$
$ exit
