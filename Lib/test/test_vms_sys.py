import sys
import unittest
import time
import os

if sys.platform != 'OpenVMS':
    raise unittest.SkipTest('OpenVMS required')

import vms.sys as SYS
import vms.ssdef as SS
import vms.jpidef as JPI
import vms.syidef as SYI
import vms.iodef as IO

class BaseTestCase(unittest.TestCase):

    def setUp(self):
        self.context = 0

    def tearDown(self):
        if self.context:
            SYS.finish_rdb(self.context)

    def test_asctim(self):
        """ tests asctim """
        vms_time = 51022884689600846
        status, result = SYS.asctim(vms_time, 0)
        self.assertEqual(1, status)
        self.assertEqual('24-JUL-2020 06:21:08.96', result)
        status, result = SYS.asctim(vms_time, 1)
        self.assertEqual(1, status)
        self.assertEqual('06:21:08.96', result)

    def test_bintim(self):
        """ tests bintim """
        time_str = '24-JUL-2020 06:21:08.96'
        status, vms_time =  SYS.bintim(time_str)
        self.assertEqual(1, status)
        self.assertEqual(51022884689600000, vms_time)

    def test_id(self):
        """ tests asctoid and idtoasc """
        name = 'SYSTEM'
        status, result_id, result_attrib = SYS.asctoid(name)
        self.assertEqual(1, status)
        self.assertGreater(result_id, 0)
        self.assertNotEqual(result_attrib, None)
        status, result_name, result_id, result_attrib, self.context = SYS.idtoasc(result_id, 0)
        self.assertEqual(1, status)
        self.assertNotEqual(self.context, 0)
        self.assertEqual(result_name, name)
        status = SYS.finish_rdb(self.context)
        self.context = 0
        self.assertEqual(1, status)

    def test_mbx(self):
        """ test mailbox """
        test_bytes = b'test bytes'
        status, mbx_channel = SYS.crembx( \
            0,      # temporary/permanent
            0,      # maxmsg
            0,      # bufquo
            0,      # promsk
            0,      # acmode
            None,   # logname
            0)      # flags
        self.assertEqual(1, status)
        status, written = SYS.writevblk(mbx_channel, test_bytes, 0, IO.IO_M_NOW | IO.IO_M_STREAM)
        self.assertEqual(1, status)
        self.assertEqual(len(test_bytes), written)
        status, read_bytes = SYS.readvblk(mbx_channel, len(test_bytes), 0, IO.IO_M_NOW | IO.IO_M_STREAM)
        self.assertEqual(1, status)
        self.assertEqual(read_bytes, test_bytes)
        status = SYS.delmbx(mbx_channel)
        self.assertEqual(1, status)

if __name__ == "__main__":
    unittest.main(verbosity=2)
