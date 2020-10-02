import os
import sys
import importlib

try:
    import pkg_resources
    pkg_resources_module = pkg_resources
except:
    pkg_resources_module = None

pkg_folder = None

# modules is dictionary
# key is the folder
# value is the tuple:
#   tuple of dependencies
#   tuple of pre-install commands
#   name of the module
modules = {
    'async_generator':
        (   (   'setuptools',
            ),
            (),
            'async_generator',
        ),
    'attrs':
        (   (   'setuptools',
            ),
            (),
            'attrs',
        ),
    'apispec':
        (   (   'setuptools',
            ),
            (),
            'apispec',
        ),
    'apispec_webframeworks':
        (   (   'setuptools',
                'apispec',
                'pyyaml',
            ),
            (),
            'apispec_webframeworks',
        ),
    'automat':
        (   (   'setuptools',
                'm2r',
                'setuptools_scm',
                'six',
                'attrs',
            ),
            (),
            'automat',
        ),
    'betamax':
        (   (   'setuptools',
                'requests',
            ),
            (),
            'betamax',
        ),
    'bottle':
        (   (   'setuptools',
            ),
            (),
            'bottle',
        ),
    'brotli':
        (   (),
            (),
            'brotli',
        ),
    'cffi':
        (   (   'setuptools',
                'pycparser',
                'py',
            ),
            (),
            'cffi',
        ),
    'chardet':
        (   (   'setuptools',
            ),
            (),
            'chardet',
        ),
    'cheroot':
        (   (   'setuptools',
                'setuptools_scm_git_archive',
                'more_itertools',
                'six',
            ),
            (),
            'cheroot',
        ),
    'cherrypy':
        (   (   'setuptools',
                'setuptools_scm',
                'jaraco.collections',
                'zc.lockfile',
                'portend',
                'cheroot',
            ),
            (),
            'cherrypy',
        ),
    'click':
        (   (   'setuptools',
            ),
            ('  python setup.py install',
            ),
            'click',
        ),
    'constantly':
        (   (   'setuptools',
            ),
            (),
            'constantly',
        ),
    'coverage':
        (   (   'setuptools',
            ),
            (),
            'coverage',
        ),
    'cryptography':
        (   (   'setuptools',
                'cffi',
                'six',
            ),
            (),
            'cryptography',
        ),
    'cryptography/vectors':
        (   (   'cryptography',
            ),
            (),
            'cryptography-vectors',
        ),
    'cython':
        (   (),
            (),
            'cython',
        ),
    'dateutil':
        (   (   'setuptools',
            ),
            (),
            'python-dateutil',
        ),
    'decorator':
        (   (   'setuptools',
            ),
            (),
            'decorator',
        ),
    'docutils':
        (   (   'setuptools',
            ),
            (),
            'docutils',
        ),
    'elementpath':
        (   (   'setuptools',
            ),
            (),
            'elementpath',
        ),
    'flasgger':
        (   (   'setuptools',
                'six',
                'mistune',
                'jsonschema',
                'pyyaml',
                'flask',
            ),
            (),
            'flasgger',
        ),
    'flask':
        (   (   'setuptools',
                'click',
                'itsdangerous',
                'jinja2',
                'werkzeug',
            ),
            (),
            'flask',
        ),
    'freezegun':
        (   (   'setuptools',
                'dateutil',
            ),
            (),
            'freezegun',
        ),
    'ftputil':
        (   (   'setuptools',
            ),
            (),
            'ftputil',
        ),
    'future_fstrings':
        (   (   'setuptools',
            ),
            (),
            'future_fstrings',
        ),
    'httpbin':
        (   (   'setuptools',
                'flasgger',
                'decorator',
            ),
            (),
            'httpbin',
        ),
    'hyperlink':
        (   (   'setuptools',
                'idna',
            ),
            (),
            'hyperlink',
        ),
    'hypothesis':
        (   (   'setuptools',
                'sortedcontainers',
                'attrs',
            ),
            (),
            'hypothesis',
        ),
    'idna':
        (   (   'setuptools',
            ),
            (),
            'idna',
        ),
    'incremental':
        (   (   'setuptools',
            ),
            (),
            'incremental',
        ),
    'iso8601':
        (   (   'setuptools',
            ),
            (),
            'iso8601',
        ),
    'itsdangerous':
        (   (   'setuptools',
            ),
            (),
            'itsdangerous',
        ),
    'jaraco.classes':
        (   (   'setuptools',
                'setuptools_scm',
                'more_itertools',
            ),
            (),
            'jaraco.classes',
        ),
    'jaraco.collections':
        (   (   'setuptools',
                'setuptools_scm',
                'six',
                'jaraco.classes',
                'jaraco.text',
            ),
            (),
            'jaraco.collections',
        ),
    'jaraco.functools':
        (   (   'setuptools',
                'setuptools_scm',
                'toml',
                'more_itertools',
            ),
            (),
            'jaraco.functools',
        ),
    'jaraco.text':
        (   (   'setuptools',
                'setuptools_scm',
                'six',
                'jaraco.functools',
            ),
            (),
            'jaraco.text',
        ),
    'jinja2':
        (   (   'setuptools',
                'markupsafe',
            ),
            (),
            'jinja2',
        ),
    'jsonschema':
        (   (   'setuptools',
                'setuptools_scm',
                'pyrsistent',
                'attrs',
            ),
            (),
            'jsonschema',
        ),
    'm2r':
        (   (   'setuptools',
                'docutils',
                'mistune',
            ),
            (),
            'm2r',
        ),
    'markupsafe':
        (   (   'setuptools',
            ),
            (),
            'markupsafe',
        ),
    'marshmallow':
        (   (   'setuptools',
            ),
            (),
            'marshmallow',
        ),
    'mistune':
        (   (   'setuptools',
            ),
            (),
            'mistune',
        ),
    'mock':
        (   (   'setuptools',
            ),
            (),
            'mock',
        ),
    'more_itertools':
        (   (   'setuptools',
            ),
            (),
            'more-itertools',
        ),
    'outcome':
        (   (   'setuptools',
                'attrs',
            ),
            (),
            'outcome',
        ),
    'packaging':
        (   (   'attrs',
                'six',
                'pyparsing',
            ),
            (),
            'packaging',
        ),
    'path':
        (   (   'setuptools',
                'setuptools_scm',
                'toml',
            ),
            (),
            'path',
        ),
    'pluggy':
        (   (   'setuptools',
                'setuptools_scm',
            ),
            (),
            'pluggy',
        ),
    'prance':
        (   (   'setuptools',
            ),
            (),
            'prance',
        ),
    'portend':
        (   (   'setuptools',
                'setuptools_scm',
                'six',
                'future_fstrings',
                'tempora',
            ),
            (),
            'portend',
        ),
    'pretend':
        (   (   'setuptools',
            ),
            (),
            'pretend',
        ),
    'py':
        (   (   'setuptools',
                'setuptools_scm',
            ),
            (),
            'py',
        ),
    'pycparser':
        (   (),
            (),
            'pycparser',
        ),
    'pygments':
        (   (   'setuptools',
            ),
            (),
            'pygments',
        ),
    'pyhamcrest':
        (   (   'setuptools',
            ),
            (),
            'pyhamcrest',
        ),
    'pyparsing':
        (   (   'setuptools',
            ),
            (),
            'pyparsing',
        ),
    'pyrsistent':
        (   (   'setuptools',
            ),
            (),
            'pyrsistent',
        ),
    'py_cpuinfo':
        (   (   'setuptools',
            ),
            (),
            'py-cpuinfo',
        ),
    'pytest':
        (   (   'setuptools',
                'setuptools_scm',
                'wcwidth',
                'pluggy',
                'more_itertools',
                'attrs',
                'packaging',
                'py',
            ),
            (),
            'pytest',
        ),
    'pytest_benchmark':
        (   (   'setuptools',
                'py_cpuinfo',
                'pytest',
            ),
            (),
            'pytest-benchmark',
        ),
    'pytest_cov':
        (   (   'setuptools',
                'coverage',
                'pytest',
            ),
            (),
            'pytest-cov',
        ),
    'pytest_flask':
        (   (   'setuptools',
                'pytest',
                'flask',
                'werkzeug',
            ),
            (),
            'pytest_flask',
        ),
    'pytest_httpbin':
        (   (   'setuptools',
                'pytest',
                'httpbin',
            ),
            (),
            'pytest_httpbin',
        ),
    'pytz':
        (   (),
            (),
            'pytz',
        ),
    'pyyaml':
        (
            (),
            (),
            'pyyaml'
        ),
    'requests':
        (   (   'setuptools',
            ),
            (),
            'requests',
        ),
    'setuptools':
        (   (),
            (   'python bootstrap.py',
            ),
            'setuptools',
        ),
    'setuptools_scm':
        (   (   'setuptools',
            ),
            (),
            'setuptools_scm',
        ),
    'setuptools_scm_git_archive':
        (   (   'setuptools',
                'setuptools_scm',
            ),
            (),
            'setuptools_scm_git_archive',
        ),
    'simplejson':
        (   (),
            (),
            'simplejson',
        ),
    'six':
        (   (),
            (),
            'six',
        ),
    'sniffio':
        (   (   'setuptools',
            ),
            (),
            'sniffio',
        ),
    'sortedcontainers':
        (   (   'setuptools',
            ),
            (),
            'sortedcontainers',
        ),
    'suds_py3':
        (   (   'setuptools',
            ),
            (),
            'suds_py3',
        ),
    'sure':
        (   (   'setuptools',
                'six',
                'mock',
            ),
            (),
            'sure',
        ),
    'tempora':
        (   (   'setuptools',
                'setuptools_scm',
                'jaraco.functools',
                'pytz',
            ),
            (),
            'tempora',
        ),
    'toml':
        (   (),
            (),
            'toml',
        ),
    'toolbelt':
        (   (   'setuptools',
                'requests',
            ),
            (),
            'requests-toolbelt',
        ),
    'trustme':
        (   (   'setuptools',
                'idna',
                'cryptography',
            ),
            (),
            'trustme',
        ),
    'twisted':
        (   (   'setuptools',
                'incremental',
                'attrs',
                'pyhamcrest',
                'hyperlink',
                'automat',
                'constantly',
                'zope.interface',
            ),
            (),
            'twisted',
        ),
    'werkzeug':
        (   (),
            (),
            'werkzeug',
        ),
    'wcwidth':
        (   (   'setuptools',
            ),
            (),
            'wcwidth',
        ),
    'xmlschema':
        (   (   'setuptools',
                'elementpath',
            ),
            (),
            'xmlschema',
        ),
    'zc.lockfile':
        (   (   'setuptools',
            ),
            (),
            'zc.lockfile',
        ),
    'zope.interface':
        (   (   'setuptools',
            ),
            (),
            'zope.interface',
        ),
}

def setup_all(options):
    installed = set()
    in_process = set()
    for entry in modules.items():
        setup_module(entry, installed, in_process, options)
    for folder in modules:
        if not folder in installed:
            print('%s not installed' % folder)

def setup_one(folder, options):
    installed = set()
    in_process = set()
    try:
        setup_module((folder, modules[folder]), installed, in_process, options)
    except Exception as ex:
        print(str(ex))

def setup_module(entry, installed, in_process, options):
    folder, (deps, commands, module_name) = entry
    if folder in in_process:
        raise Exception('%s already in process\n%s' % repr(in_process))
    in_process.add(folder)
    for dep_folder in deps:
        if not dep_folder in modules:
            raise Exception('%s is not configured' % dep_folder)
        try:
            setup_module((dep_folder, modules[dep_folder]), installed, in_process, options)
        except Exception as ex:
            print(str(ex))
    if pkg_resources_module:
        try:
            distr = pkg_resources_module.get_distribution(module_name)
            if not folder in sys.argv[1:]:
                installed.add(folder)
        except Exception as ex:
            print(str(ex))
    if not folder in installed:
        print('\nInstalling from %s' % folder)
        try:
            if pkg_folder:
                os.chdir(os.path.join(pkg_folder, folder))
            else:
                cur_folder = os.getcwd()
                os.chdir(folder)
            try:
                # pre-install
                for command in commands:
                    os.system(command)

                install_cmd = 'python setup.py install ' + ' '.join(options)
                # test for version option
                for opt in options:
                    if opt == '--version':
                        install_cmd = 'python setup.py --version'
                        break
                os.system(install_cmd)

                # test if it has been installed
                if pkg_resources_module:
                    try:
                        distr = pkg_resources_module.get_distribution(module_name)
                        # add to installed if distribution is found
                        installed.add(folder)
                    except Exception as ex:
                        pass
                else:
                    # add to installed in any case
                    installed.add(folder)

            except Exception as ex:
                print(str(ex))
            if not pkg_folder:
                os.chdir(cur_folder)
        except Exception as ex:
            print(str(ex))
    in_process.remove(folder)

def dynamic_import(module):
    try:
        module = importlib.import_module(module)
    except Exception as ex:
        print(str(ex))
    return module

if __name__ == '__main__':

    folders = []
    options = []
    for arg in sys.argv[1:]:
        if arg.startswith('-'):
            options.append(arg)
        else:
            folders.append(arg)

    pkg_folder = os.getenv('PYTHON_LIB_REPOSITORY')

    if not pkg_resources_module:
        setup_one('setuptools', options)
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
        if setuptools_appended:
            pkg_resources_module = dynamic_import('pkg_resources')

    if len(folders) > 0:
        for folder in folders:
            setup_one(folder, options)
    else:
        setup_all(options)