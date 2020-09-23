'''
execute in parent folder of python libraries
'''
import os

modules = {
    'async_generator': (('setuptools',),('python setup.py install',)),
    'attrs': (('setuptools',),('python setup.py install',)),
    'automat': (('setuptools','m2r','setuptools_scm','six','attrs',),('python setup.py install',)),
    'betamax': (('setuptools','requests',),('python setup.py install',)),
    'bottle': (('setuptools',),('python setup.py install',)),
    'brotli': ((),('python setup.py install',)),
    'cffi': (('setuptools','pycparser','py','pytest',),('python setup.py install',)),
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
    'freezegun': (('setuptools','dateutil',),('python setup.py install',)),
    'future_fstrings': (('setuptools',),('python setup.py install',)),
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
    'jsonschema': (('setuptools','pyrsistent','attrs',),('python setup.py install',)),
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
    os.chdir('..')
    for entry in modules.items():
        setup_module(entry, installed, in_process)
    for name in modules:
        if not name in installed:
            print('%s not installed' % name)

def setup_module(entry, installed, in_process):
    name, (deps, commands) = entry
    if name in in_process:
        raise repr(in_process)
    in_process.add(name)
    for dep_name in deps:
        try:
            setup_module((dep_name, modules[dep_name]), installed, in_process)
        except:
            pass
    if not name in installed:
        print('\ninstalling %s' % name)
        try:
            os.chdir(name)
            try:
                for command in commands:
                    os.system(command)
                installed.add(name)
            except:
                pass
            os.chdir('..')
        except:
            pass
    in_process.remove(name)

if __name__ == '__main__':
    setup_all()