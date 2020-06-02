import unittest
import test._test_multiprocessing

import sys
from test import support

if sys.platform in ("OpenVMS"):
    raise unittest.SkipTest("os.spawn() is not available on OpenVMS")

if support.PGO:
    raise unittest.SkipTest("test is not helpful for PGO")

test._test_multiprocessing.install_tests_in_module_dict(globals(), 'spawn')

if __name__ == '__main__':
    unittest.main()
