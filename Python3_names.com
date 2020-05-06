$ INSTALL_DIR = "wrk_disk:[vorfolomeev.Python3.out.python.]"
$ pipe create/dir 'INSTALL_DIR' | copy SYS$INPUT nl:
$ pipe define /trans=concealed python$root 'INSTALL_DIR' | copy SYS$INPUT nl:
$ pipe define PYTHONHOME "/python$root" | copy SYS$INPUT nl:
$ pipe define PYTHONCASEOK "YES" | copy SYS$INPUT nl:
$ pipe define python$shr python$root:[lib]python$shr.exe | copy SYS$INPUT nl:
$ python :== $python$root:[bin]python3.exe
