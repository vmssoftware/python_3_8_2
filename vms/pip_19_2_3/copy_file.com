$ com_nam = f$environment("procedure")
$ com_dir = f$parse(com_nam,,,"directory")
$ com_dev = f$parse(com_nam,,,"device")
$ src_pat = com_dev + com_dir - "]" + "._internal]"
$ copy 'src_pat'configuration.py PYTHON$ROOT:[lib.python3^.8.site-packages.pip._internal]