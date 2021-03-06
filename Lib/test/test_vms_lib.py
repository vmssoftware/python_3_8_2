import sys
import unittest
import os

if sys.platform != 'OpenVMS':
    raise unittest.SkipTest('OpenVMS required')

import vms.lib as LIB
import vms.ssdef as SS
import vms.jpidef as JPI
import vms.syidef as SYI

class BaseTestCase(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_create_dir(self):
        """ tests creating directory """
        dirname = 'kill_this_temporary_directory'
        status = LIB.create_dir('[.' + dirname + ']', 0, 0xffff, 0)
        self.assertIn(status, (SS.SS__CREATED, SS.SS__NORMAL))
        if status == SS.SS__CREATED:
            os.rmdir(dirname)
        status = LIB.create_dir('[.' + dirname + ']', None, 0xffff, 0)
        self.assertIn(status, (SS.SS__CREATED, SS.SS__NORMAL))
        if status == SS.SS__CREATED:
            os.rmdir(dirname)

    def test_date_time(self):
        """ tests date_time """
        status, date_str = LIB.date_time()
        self.assertEqual(1, status)
        self.assertNotIn(date_str, ('', None))

    def test_ef(self):
        """ tests ef """
        status, ef = LIB.get_ef()
        self.assertEqual(1, status)
        status = LIB.free_ef(ef)
        self.assertEqual(1, status)

    def test_common(self):
        """ tests common """
        common_value = "-=Python=-"
        status = LIB.put_common(common_value)
        self.assertEqual(1, status)
        status, got_common_value = LIB.get_common()
        self.assertEqual(1, status)
        self.assertEqual(got_common_value, common_value)

        common_value = "-=Python=-" * 32
        status = LIB.put_common(common_value)
        self.assertEqual(1409041, status)   # truncated
        status, got_common_value = LIB.get_common()
        self.assertEqual(1, status)
        self.assertEqual(got_common_value, common_value[:252])

    def test_get_hostname(self):
        """ tests get_hostname """
        status, host_name = LIB.get_hostname(0)
        self.assertEqual(1, status)
        self.assertNotIn(host_name, ('', None))
        status, host_name = LIB.get_hostname(1)
        self.assertEqual(1, status)
        self.assertNotIn(host_name, ('', None))

    def test_getjpi(self):
        """ tests getjpi """
        status, pagecount_str = LIB.getjpi(JPI.JPI__PPGCNT, 0, None)
        self.assertEqual(1, status)
        self.assertIsInstance(pagecount_str, str)
        pagecount_int = int(pagecount_str)
        self.assertEqual(str(pagecount_int), pagecount_str)
        self.assertGreater(pagecount_int, 0)

    def test_getsyi(self):
        """ tests getsyi """
        status, pagesize_str, _ = LIB.getsyi(SYI.SYI__PAGE_SIZE, None)
        self.assertEqual(1, status)
        self.assertIsInstance(pagesize_str, str)
        pagesize_int = int(pagesize_str)
        self.assertEqual(str(pagesize_int), pagesize_str)
        self.assertGreater(pagesize_int, 0)

    def test_spawn(self):
        """ tests spawn """
        status, pid = LIB.spawn('show time', None, None, 0, None)
        self.assertEqual(1, status)

if __name__ == "__main__":
    unittest.main(verbosity=2)
