$ ! INSTALL_DIR must have 'LIB/PYTHON3.8' folder with all *.PY files from 'LIB'
$ INSTALL_DIR = "wrk_disk:[vorfolomeev.Python3.out.]"
$ define python$root 'INSTALL_DIR'
$ ! backup/replace WRK_DISK:[vorfolomeev.Python3.lib...]*.*;* python$root:[lib.python3^.8...]
$ define python$shr python$root:[debug]python$shr.exe
$ define PYTHONHOME "/python$root"
