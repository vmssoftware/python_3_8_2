import os
import sys
import importlib

try:
    import pkg_resources
    pkg_resources_module = pkg_resources
except:
    pkg_resources_module = None

pkg_folder = None

modules = {
    'async_generator': (('setuptools',),('python setup.py install',)),
    'attrs': (('setuptools',),('python setup.py install',)),
    'automat': (('setuptools','m2r','setuptools_scm','six','attrs',),('python setup.py install',)),
    'betamax': (('setuptools','requests',),('python setup.py install',)),
    'bottle': (('setuptools',),('python setup.py install',)),
    'brotli': ((),('python setup.py install',)),
    'cffi': (('setuptools','pycparser','py'),('python setup.py install',)),
    'chardet': (('setuptools',),('python setup.py install',)),
    'cheroot': (('setuptools','setuptools_scm_git_archive','more_itertools','six',),('python setup.py install',)),
    'cherrypy': (('setuptools','setuptools_scm','jaraco.collections','zc.lockfile','portend','cheroot',),('python setup.py install',)),
    'click': (('setuptools',),('python setup.py install',)),
    'constantly': (('setuptools',),('python setup.py install',)),
    'coverage': (('setuptools',),('python setup.py install',)),
    'cryptography': (('setuptools','cffi','six',),('python setup.py install',)),
    'cython': ((),('python setup.py install',)),
    'dateutil': (('setuptools',),('python setup.py install',)),
    'decorator': (('setuptools',),('python setup.py install',)),
    'docutils': (('setuptools',),('python setup.py install',)),
    'elementpath': (('setuptools',),('python setup.py install',)),
    'flasgger': (('setuptools','six','mistune','jsonschema','pyyaml','flask',),('python setup.py install',)),
    'flask': (('setuptools','click','itsdangerous','jinja2','werkzeug',),('python setup.py install',)),
    'freezegun': (('setuptools','dateutil',),('python setup.py install',)),
    'future_fstrings': (('setuptools',),('python setup.py install',)),
    'httpbin': (('setuptools','flasgger',),('python setup.py install',)),
    'hyperlink': (('setuptools','idna',),('python setup.py install',)),
    'hypothesis': (('setuptools','sortedcontainers','attrs',),('python setup.py install',)),
    'idna': (('setuptools',),('python setup.py install',)),
    'incremental': (('setuptools',),('python setup.py install',)),
    'iso8601': (('setuptools',),('python setup.py install',)),
    'itsdangerous': (('setuptools',),('python setup.py install',)),
    'jaraco.classes': (('setuptools','setuptools_scm','more_itertools',),('python setup.py install',)),
    'jaraco.collections': (('setuptools','setuptools_scm','six','jaraco.classes','jaraco.text',),('python setup.py install',)),
    'jaraco.functools': (('setuptools','setuptools_scm','toml','more_itertools',),('python setup.py install',)),
    'jaraco.text': (('setuptools','setuptools_scm','six','jaraco.functools',),('python setup.py install',)),
    'jinja2': (('setuptools','markupsafe',),('python setup.py install',)),
    'jsonschema': (('setuptools','setuptools_scm','pyrsistent','attrs',),('python setup.py install',)),
    'm2r': (('setuptools','docutils','mistune',),('python setup.py install',)),
    'markupsafe': (('setuptools',),('python setup.py install',)),
    'mistune': (('setuptools',),('python setup.py install',)),
    'mock': (('setuptools',),('python setup.py install',)),
    'more_itertools': (('setuptools',),('python setup.py install',)),
    'numpy': (('setuptools','cython',),('python setup.py install',)),
    'outcome': (('setuptools','attrs',),('python setup.py install',)),
    'packaging': (('attrs','six','pyparsing',),('python setup.py install',)),
    'path': (('setuptools','setuptools_scm','toml',),('python setup.py install',)),
    'pluggy': (('setuptools','setuptools_scm',),('python setup.py install',)),
    'portend': (('setuptools','setuptools_scm','six','future_fstrings','tempora',),('python setup.py install',)),
    'pretend': (('setuptools',),('python setup.py install',)),
    'py': (('setuptools','setuptools_scm',),('python setup.py install',)),
    'pycparser': ((),('python setup.py install',)),
    'pygments': (('setuptools',),('python setup.py install',)),
    'pyhamcrest': (('setuptools',),('python setup.py install',)),
    'pyparsing': (('setuptools',),('python setup.py install',)),
    'pyrsistent': (('setuptools',),('python setup.py install',)),
    'py_cpuinfo': (('setuptools',),('python setup.py install',)),
    'pytest': (('setuptools','setuptools_scm','wcwidth','pluggy','more_itertools','attrs','packaging','py',),('python setup.py install',)),
    'pytest_benchmark': (('setuptools','py_cpuinfo','pytest',),('python setup.py install',)),
    'pytest_cov': (('setuptools','coverage','pytest',),('python setup.py install',)),
    'pytz': ((),('python setup.py install',)),
    'pyyaml': ((),('python setup.py install',)),
    'requests': (('setuptools',),('python setup.py install',)),
    'setuptools': ((),('python bootstrap.py','python setup.py install',)),
    'setuptools_scm': (('setuptools',),('python setup.py install',)),
    'setuptools_scm_git_archive': (('setuptools','setuptools_scm',),('python setup.py install',)),
    'simplejson': ((),('python setup.py install',)),
    'six': ((),('python setup.py install',)),
    'sniffio': (('setuptools',),('python setup.py install',)),
    'sortedcontainers': (('setuptools',),('python setup.py install',)),
    'suds_py3': (('setuptools',),('python setup.py install',)),
    'sure': (('setuptools','six','mock',),('python setup.py install',)),
    'tempora': (('setuptools','setuptools_scm','jaraco.functools','pytz',),('python setup.py install',)),
    'toml': ((),('python setup.py install',)),
    'toolbelt': (('setuptools','requests',),('python setup.py install',)),
    'twisted': (('setuptools','incremental','attrs','pyhamcrest','hyperlink','automat','constantly','zope.interface',),('python setup.py install',)),
    'werkzeug': ((),('python setup.py install',)),
    'wcwidth': (('setuptools',),('python setup.py install',)),
    'xmlschema': (('setuptools','elementpath',),('python setup.py install',)),
    'zc.lockfile': (('setuptools',),('python setup.py install',)),
    'zope.interface': (('setuptools',),('python setup.py install',)),
}

def setup_all():
    installed = set()
    in_process = set()
    for entry in modules.items():
        setup_module(entry, installed, in_process)
    for name in modules:
        if not name in installed:
            print('%s not installed' % name)

def setup_one(name):
    installed = set()
    in_process = set()
    try:
        setup_module((name, modules[name]), installed, in_process)
    except Exception as ex:
        print(str(ex))

def setup_module(entry, installed, in_process):
    name, (deps, commands) = entry
    if name in in_process:
        raise Exception('%s already in process\n%s' % repr(in_process))
    in_process.add(name)
    for dep_name in deps:
        if not dep_name in modules:
            raise Exception('%s is not configured' % dep_name)
        try:
            setup_module((dep_name, modules[dep_name]), installed, in_process)
        except Exception as ex:
            print(str(ex))
    already_installed = False
    if pkg_resources_module:
        try:
            distr = pkg_resources_module.get_distribution(name)
            if not name in sys.argv[1:]:
                already_installed = True
        except Exception as ex:
            print(str(ex))
            already_installed = False
    if not (already_installed or name in installed):
        print('\ninstalling %s' % name)
        try:
            if pkg_folder:
                os.chdir(os.path.join(pkg_folder,name))
            else:
                os.chdir(name)
            try:
                for command in commands:
                    os.system(command)
                installed.add(name)
            except Exception as ex:
                print(str(ex))
            if not pkg_folder:
                os.chdir('..')
        except Exception as ex:
            print(str(ex))
    in_process.remove(name)

def dynamic_import(module):
    try:
        module = importlib.import_module(module)
    except Exception as ex:
        print(str(ex))
    return module

if __name__ == '__main__':

    pkg_folder = os.getenv('PYTHON_LIB_REPOSITORY')

    if not pkg_resources_module:
        setup_one('setuptools')
        # add /python$root/lib/python3.8/site-packages/setuptools... into sys.path
        setuptools_appended = False
        for dir_entry in os.walk('/python$root/lib/python3.8/site-packages/'):
            if dir_entry[0].startswith('setuptools'):
                sys.path.append(dir_entry[0])
                setuptools_appended = True
                break
            for file_entry in dir_entry[2]:
                if file_entry.startswith('setuptools'):
                    sys.path.append(os.path.join(dir_entry[0], file_entry))
                    setuptools_appended = True
                    break
            if setuptools_appended:
                break
        pkg_resources_module = dynamic_import('pkg_resources')

    if len(sys.argv) > 1:
        for name in sys.argv[1:]:
            setup_one(name)
    else:
        setup_all()