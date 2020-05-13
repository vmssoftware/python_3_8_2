$ com_nam = f$environment("procedure")
$ com_dir = f$parse(com_nam,,,"directory")
$ com_dev = f$parse(com_nam,,,"device")
$ com_pat = com_dev + com_dir
$ INSTALL_DIR = com_pat - "]" + ".out.python.]"
$ pipe create/dir 'INSTALL_DIR' | copy SYS$INPUT nl:
$ define /trans=concealed python$root 'INSTALL_DIR'
$ define PYTHONHOME "/python$root"
$ define PYTHONCASEOK "YES"
$ define python$shr python$root:[lib]python$shr.exe
$ python :== $python$root:[bin]python3.exe
