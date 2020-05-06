$ set nover
$ del/noconf [.python...__pycache__]*.*;*
$ del/noconf [.python...]__pycache__.dir;
$ top = "python"
$
$ pwd = f$environment("default")
$ dir_name = f$parse(pwd,,, "directory", "no_conceal")
$ dev_name = f$parse(pwd,,, "device", "no_conceal")
$ dir_name = dir_name - "]["
$ dir_name = f$edit(dir_name, "lowercase")
$ dir_name = dir_name - "]" + ".]"
$
$ tmp = dev_name + dir_name
$ define/process/nolog/translation=(concealed) tmp$root 'tmp'
$
$ open/write fp files.dat
$ current_dir = "[''top']"
$
$ write fp ""
$ write fp "--"
$ write fp "-- Directories..."
$ write fp "--"
$ write fp ""
$
$ write fp "   directory ""''current_dir'"" ;"
$
$ loop1:
$    file = f$search("tmp$root:[''top'...]*.*")
$    if file .eqs. "" then goto end_loop1
$    name = f$parse(file,,,"NAME")
$    if f$file_attributes(file, "DIRECTORY")
$    then
$        current_dir = f$parse(file,,,"DIRECTORY") - "000000." - "]" + ".''name']"
$        write fp "   directory ""''current_dir'"" ;"
$    endif
$    goto loop1
$ end_loop1:
$!
$ write fp ""
$ write fp "--"
$ write fp "-- Files..."
$ write fp "--"
$ write fp ""
$
$ loop2:
$    file = f$search("tmp$root:[''top'...]*.*")
$    if file .eqs. "" then goto end_loop2
$    name = f$parse(file,,,"NAME")
$    if .not. f$file_attributes(file, "DIRECTORY")
$    then
$        current_dir = f$parse(file,,,"DIRECTORY") - "000000."
$        size = f$file_attributes(file, "ALQ")
$        type = f$parse(file,,,"TYPE")
$        if type .nes. ".log" .and. type .nes. ".LOG"
$        then
$           write fp "   file ""''current_dir'''name'''type'"" source ""''current_dir'''name'''type'"";"
$        else
$           write sys$output "Excluding ""''current_dir'''name'''type'"""
$        endif
$    endif
$    goto loop2
$ end_loop2:
$!
$ close fp
$ deassign/proc tmp$root
$ exit

