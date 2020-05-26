$ set nover
$
$ com_nam = f$environment("procedure")
$ com_dir = f$parse(com_nam,,,"directory")
$ com_dev = f$parse(com_nam,,,"device")
$ dir_name = com_dev + com_dir - "vms]" + "out.]"
$
$ top = "python"
$
$ define/process/nolog/translation=(concealed) tmp$root 'dir_name'
$
$ del/noconf tmp$root:['top'...__pycache__]*.*;*
$ del/noconf tmp$root:['top'...]__pycache__.dir;
$
$ del/noconf files.dat;*
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

