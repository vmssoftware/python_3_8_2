import unittest
import test._test_multiprocessing

import sys
from test import support

if support.PGO:
    raise unittest.SkipTest("test is not helpful for PGO")

if sys.platform in ("win32", "OpenVMS"):
    raise unittest.SkipTest("os.forkserver() is not available on Windows and OpenVMS")

test._test_multiprocessing.install_tests_in_module_dict(globals(), 'forkserver')

if __name__ == '__main__':
    unittest.main()
