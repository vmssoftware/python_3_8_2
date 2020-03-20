$ INSTALL_DIR = "wrk_disk:[vorfolomeev.Python3.out.]"
$ define /trans=concealed python$root 'INSTALL_DIR'
$ define PYTHONHOME "/python$root"
$ define PYTHONCASEOK "YES"
$ define python$shr python$root:[lib]python$shr.exe
$ python :== $python$root:[bin]python3.exe
