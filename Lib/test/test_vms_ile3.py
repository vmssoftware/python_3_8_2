import sys
import unittest

if sys.platform != 'OpenVMS':
    raise unittest.SkipTest('OpenVMS required')

import vms.ile3 as ILE3
import vms.sys as SYS
import vms.syidef as SYI
import vms.dscdef as DSC

class BaseTestCase(unittest.TestCase):

    def setUp(self):
        self.il = None

    def tearDown(self):
        if self.il:
            ILE3.delele(self.il)

    def test_getstr(self):
        """ tests getting str """
        self.il = ILE3.new()
        self.assertEqual(0, ILE3.size(self.il))
        ILE3.addstr(self.il, SYI.SYI__HW_NAME, None, 64)
        self.assertEqual(1, ILE3.size(self.il))
        status, _ = SYS.getsyi(0, None, self.il)
        self.assertEqual(1, status)
        self.assertEqual(1, ILE3.size(self.il))
        hw_name = ILE3.getstr(self.il, 0, 0)
        self.assertEqual(1, ILE3.size(self.il))
        ILE3.delete(self.il)
        self.il = None
        self.assertNotIn(hw_name, (None, ''))

    def test_addbin_getbyte(self):
        """ tests add binary data and get byte """
        self.il = ILE3.new()
        self.assertEqual(0, ILE3.size(self.il))
        ILE3.addbin(self.il, SYI.SYI__ARCH_TYPE, 0x0102030405060708, 2, 5)
        self.assertEqual(1, ILE3.size(self.il))
        self.assertEqual(0x04, ILE3.getbyte(self.il, 0, 2))
        ILE3.delete(self.il)
        self.il = None

    def test_addint_getint(self):
        """ tests add int and get int """
        self.il = ILE3.new()
        self.assertEqual(0, ILE3.size(self.il))
        ILE3.addint(self.il, SYI.SYI__ARCH_TYPE, DSC.DSC_K_DTYPE_QU, 0x0102030405060708)
        self.assertEqual(1, ILE3.size(self.il))
        self.assertEqual(0x0102030405060708, ILE3.getint(self.il, 0))
        ILE3.delete(self.il)
        self.il = None

    def test_addstrd(self):
        """ tests add strd """
        self.il = ILE3.new()
        self.assertEqual(0, ILE3.size(self.il))
        ILE3.addstrd(self.il, SYI.SYI__HW_NAME, '1234567890', 5)    # one byte reserved for length
        self.assertEqual(1, ILE3.size(self.il))
        hw_name = ILE3.getstr(self.il, 0, 1)
        self.assertEqual(hw_name, '1234')
        ILE3.delete(self.il)
        self.il = None

    def test_addstrn(self):
        """ tests add strn """
        self.il = ILE3.new()
        self.assertEqual(0, ILE3.size(self.il))
        ILE3.addstrn(self.il, SYI.SYI__HW_NAME, '1234567890', 5)
        self.assertEqual(1, ILE3.size(self.il))
        hw_name = ILE3.getstr(self.il, 0, 0)
        self.assertEqual(hw_name, '12345')
        ILE3.delete(self.il)
        self.il = None

if __name__ == "__main__":
    unittest.main(verbosity=2)
