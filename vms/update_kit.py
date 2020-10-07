import os
import re

def spec_replacer(match):
    if match.group(0) == ' ':
        return '^_'
    return '^' + match.group(0)

def create_content():
    python_dir = '/python$root'
    python_dir_len = len(python_dir)
    all_dirs = []
    all_files = []
    spec_pattern = re.compile('([. ^+()])')
    for root, dirs, files in os.walk(python_dir):
        inner_dirs = list(filter(lambda x: x != '', spec_pattern.sub(spec_replacer, root[python_dir_len:]).split('/')))
        kit_dir = '[' + '.'.join(['python'] + inner_dirs) + ']'
        all_dirs.append('directory "' + kit_dir + '" version limit 1;')
        for file in files:
            file_name, file_ext = os.path.splitext(file)
            if file_ext == '':
                file_ext = '.'
            file_name = spec_pattern.sub(spec_replacer, file_name)
            all_files.append('file "' + \
                kit_dir + file_name + file_ext + \
                '" source "' + \
                kit_dir + file_name + file_ext + \
                '";')
        try:
            dirs.remove('__pycache__')
        except:
            pass

    kit_template = '''
-- (C) Copyright 2020 VMS Software Inc.
--
product VSI I64VMS PYTHON {type}{major}.{minor}-{level}{edit} FULL ;

--
--  Execute the preconfigure procedure
--
    execute preconfigure "@pcsi$source:[python]python$pcsi_preconfigure.com" uses [python]python$pcsi_preconfigure.com ;

--
--  Make sure VMS V8.4 or above is installed
--
    if ((not <software VSI I64VMS VMS version minimum V8.4>) and (not <software HP I64VMS VMS version minimum V8.4>)) ;
        error NO_MIN_VMS abort ;
    end if ;

--
--  ODS-5 Disk(s) should be available on this system
--
    if (<logical name PYTHON$ODS5_AVAIL equals 0 table LNM$JOB>) ;
        error NO_ODS5_DISKS ;
    end if ;

--
-- Directories...
--

    {dirs}

--
-- Files...
--

    {files}

--
-- Start-up and shutdown scripts
--
   file "[sys$startup]python$startup.com" source "[python]python$startup.com";
   file "[sys$startup]python$shutdown.com" source "[python]python$shutdown.com";


--
-- Release notes
--
-- (none)


--
-- Do post-install tasks
--
   execute postinstall "@pcsi$source:[python]python$define_root.com" interactive uses "[python]python$define_root.com" ;

--
-- Okay, done.  Tell the user what to do next.
--
   information POST_INSTALL  phase after with helptext;

--
-- All done
--

end product;
'''
    # type, major, minor, level, edit must be the same as in pythlib.pcsi$text
    kit_content = kit_template.format(
        type='A',
        major='3',
        minor='8',
        level='2',
        edit='d20201006',
        dirs='\n    '.join(all_dirs),
        files='\n    '.join(all_files))
    with open('python.pcsi$desc', 'w') as file:
        file.write(kit_content)

if __name__ == "__main__":
    create_content()
