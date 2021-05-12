import os
import re
import sys

def spec_replacer(match):
    if match.group(0) == ' ':
        return '^_'
    return '^' + match.group(0)

def create_content(type, major, minor, level, edit):
    re_is_compiled = re.compile(r'([^-]*)-([^-]*)-([^-]*)-([^-]*)-openvms(.*?).whl', flags=re.IGNORECASE)
    python_wheels_dir = '/python_wheels$root'
    python_wheels_dir_len = len(python_wheels_dir)
    all_dirs = []
    all_files = []
    spec_pattern = re.compile('([. ^+()])')
    for root, dirs, files in os.walk(python_wheels_dir):
        inner_dirs = list(filter(lambda x: x != '', spec_pattern.sub(spec_replacer, root[python_wheels_dir_len:]).split('/')))
        kit_dir = '[' + '.'.join(['wheels'] + inner_dirs) + ']'
        src_dir = '[000000]'
        if len(inner_dirs):
            src_dir = '[' + '.'.join(inner_dirs) + ']'
        all_dirs.append('directory "' + kit_dir + '" version limit 1;')
        for file in files:
            file_name, file_ext = os.path.splitext(file)
            if file_ext == '':
                file_ext = '.'
            file_name = spec_pattern.sub(spec_replacer, file_name)
            kit_file_name = file_name
            # test if module is compiled
            matched = re_is_compiled.match(file)
            if matched:
                lib_name = matched.group(1)
                lib_version = matched.group(2)
                lib_version_splitted = lib_version.split('.')
                lib_version = []
                for v_str in lib_version_splitted:
                    v_num_found = re.search(r'^\d+', v_str)
                    if v_num_found:
                        lib_version.append(v_num_found[0])
                    else:
                        break
                if len(lib_version):
                    lib_version = '.'.join(lib_version)
                else:
                    lib_version = '0.0.0'
                lib_new_full_name = '-'.join([
                    lib_name,
                    lib_version,
                    'py2.py3',
                    'none',
                    'any',
                ])
                kit_file_name = spec_pattern.sub(spec_replacer, lib_new_full_name)
            all_files.append('file "' + \
                kit_dir + kit_file_name + file_ext + \
                '" source "' + \
                src_dir + file_name + file_ext + \
                '";')
        try:
            dirs.remove('__pycache__')
        except:
            pass

    kit_template = '''--
-- (C) Copyright 2021 VMS Software Inc.
--
product VSI I64VMS PYTHWHLS {type}{major}.{minor}-{level}{edit} FULL ;

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
--    file "[sys$startup]python_wheels$define_root.com" source "[000000]python_wheels$define_root.com";
--    file "[sys$startup]python_wheels$startup.com" source "[000000]python_wheels$startup.com";

--
-- Do post-install tasks
--

    execute postinstall (
        "root = f$trnlmn(""pcsi$destination"") - ""]"" + ""wheels.]""",
        "define/system/trans=concealed python_wheels$root 'root'",
        "define/system PIP_FIND_LINKS ""/python_wheels$root""",
        "open/write fd sys$startup:wheels$startup.com",
        "write fd ""$!Define logical names for Python wheels packages...""",
        "write fd ""$define/system/trans=concealed python_wheels$root ''root'""",
        "write fd ""$define/system PIP_FIND_LINKS """"/python_wheels$root""""",
        "write fd ""$exit""",
        "close fd"
     );

--    execute postinstall (
--        "root = f$trnlmn(""pcsi$destination"") - ""]"" + ""wheels.]""",
--        "define/system/trans=concealed python_wheels$root 'root'",
--        "define/system PIP_FIND_LINKS ""/python_wheels$root""",
--        "write sys$output ""% Add next commands to the system startup file:""",
--        "write sys$output ""%     $define/system/trans=concealed python_wheels$root ''root'""",
--        "write sys$output ""%     $define/system PIP_FIND_LINKS """"/python_wheels$root"""""
--     ) interactive ;

--    execute
--        postinstall
--            "@pcsi$source:[000000]python_wheels$define_root.com"
--            interactive
--            uses [000000]python_wheels$define_root.com
--    ;

--
-- Okay, done.  Tell the user what to do next.
--
   information POST_INSTALL  phase after with helptext;

end product;
'''
    # type, major, minor, level, edit must be the same as in pythlib.pcsi$text
    kit_content = kit_template.format(
        type=type,
        major=major,
        minor=minor,
        level=level,
        edit=edit,
        dirs='\n    '.join(all_dirs),
        files='\n    '.join(all_files))
    with open('pythwhls.pcsi$desc', 'w') as file:
        file.write(kit_content)

    text_template = '''=product VSI I64VMS PYTHWHLS {type}{major}.{minor}-{level}{edit} full
1 'PRODUCT
=prompt Python wheels collection for OpenVMS ({edit})

1 'PRODUCER
=prompt VSI Software Inc.

1 'NOTICE
=prompt (C) Copyright 2021 VMS Software Inc.

1 POST_INSTALL
=prompt Post-installation tasks are required.
To define the Wheels for Python runtime at system boot time, add the
following lines to SYS$MANAGER:SYSTARTUP_VMS.COM:

    $ file := sys$startup:wheels$startup.com
    $ if f$search("''file'") .nes. "" then @'file'

'''
    text_content = text_template.format(
        type=type,
        major=major,
        minor=minor,
        level=level,
        edit=edit,
        dirs='\n    '.join(all_dirs),
        files='\n    '.join(all_files))
    with open('pythwhls.pcsi$text', 'w') as file:
        file.write(text_content)

if __name__ == "__main__":

    import getopt
    import datetime

    opts, args = getopt.getopt(sys.argv[1:], '', ['type=', 'major=', 'minor=', 'level=', 'edit='])

    type = 'A'
    major = '0'
    minor = '1'
    level = '1'
    edit = ''   # 'd' + datetime.date.today().strftime('%Y%m%d')

    for opt, optarg in opts:
        if opt in ['--type']:
            type = optarg
        elif opt in ['--major']:
            major = optarg
        elif opt in ['--minor']:
            minor = optarg
        elif opt in ['--level']:
            level = optarg
        elif opt in ['--edit']:
            edit = optarg
        else:
            print('Unknown option %s' % opt)

    create_content(
        type,
        major,
        minor,
        level,
        edit,
    )
