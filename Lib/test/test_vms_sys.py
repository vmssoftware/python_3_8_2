import sys
import unittest
import time
import os
import threading

if sys.platform != 'OpenVMS':
    raise unittest.SkipTest('OpenVMS required')

import vms.sys as SYS
import vms.ile3 as ILE3
import vms.ssdef as SS
import vms.jpidef as JPI
import vms.syidef as SYI
import vms.iodef as IO
import vms.lnmdef as LNM
import vms.dscdef as DSC
import vms.psldef as PSL

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
        mbx_name = "PythonTest"
        status, mbx_channel = SYS.crembx( \
            0,      # temporary/permanent
            0,      # maxmsg
            0,      # bufquo
            0,      # promsk
            0,      # acmode
            mbx_name,   # logname
            0)      # flags
        self.assertEqual(1, status)
        status, written, iostatus = SYS.writevblk(mbx_channel, test_bytes, 0, IO.IO_M_NOW | IO.IO_M_STREAM)
        self.assertEqual(1, status)
        self.assertEqual(1, iostatus)
        self.assertEqual(len(test_bytes), written)
        status, read_channel = SYS.assign(mbx_name, 0, None, 0)
        self.assertEqual(1, status)
        status, read_bytes, iostatus = SYS.readvblk(read_channel, len(test_bytes), 0, IO.IO_M_NOW | IO.IO_M_STREAM)
        self.assertEqual(1, status)
        self.assertEqual(1, iostatus)
        self.assertEqual(read_bytes, test_bytes)
        status = SYS.dassgn(read_channel)
        self.assertEqual(1, status)
        status = SYS.delmbx(mbx_channel)
        self.assertEqual(1, status)

    def test_cancel(self):
        """ test cancel """
        test_bytes = b'test bytes'
        mbx_name = "PythonTest"
        status, mbx_channel = SYS.crembx(0, 0, 0, 0, 0, mbx_name, 0)
        self.assertEqual(1, status)

        def cancel_thread_fn(retstat):
            time.sleep(1.0)
            retstat.append(SYS.cancel(mbx_channel))

        cancel_thread_retstat = []
        cancel_thread = threading.Thread(target=cancel_thread_fn, args=(cancel_thread_retstat,))
        cancel_thread.start()

        status, read_bytes, iostatus = SYS.readvblk(mbx_channel, len(test_bytes), 0, IO.IO_M_STREAM)
        self.assertEqual(1, status)
        self.assertIn(iostatus, (44, 2096)) # abort or cancel
        self.assertEqual(0, len(read_bytes))

        cancel_thread.join()
        self.assertEqual(1, len(cancel_thread_retstat))
        self.assertEqual(1, cancel_thread_retstat[0])

        status = SYS.delmbx(mbx_channel)
        self.assertEqual(1, status)

    def test_logical_name(self):
        """ test create, translate and delete logical name """
        log_name = 'PythonTestCRELNM'
        log_value = 'Value of PythonTestCRELNM'
        log_table = 'LNM$PROCESS_TABLE'
        il = ILE3.new()
        ILE3.addstrn(il, LNM.LNM__STRING, log_value, len(log_value))
        status = SYS.crelnm(0, log_table, log_name, PSL.PSL_C_USER, il)
        ILE3.delete(il)
        self.assertIn(status, (SS.SS__NORMAL, SS.SS__SUPERSEDE))

        il = ILE3.new()
        ILE3.addint(il, LNM.LNM__INDEX, DSC.DSC_K_DTYPE_LU, 0)
        ILE3.addint(il, LNM.LNM__ATTRIBUTES, DSC.DSC_K_DTYPE_LU, 0)
        ILE3.addint(il, LNM.LNM__LENGTH, DSC.DSC_K_DTYPE_LU, 0)
        ILE3.addstr(il, LNM.LNM__STRING, None, 255)
        ILE3.addstr(il, LNM.LNM__TABLE, None, 32)
        status = SYS.trnlnm(LNM.LNM_M_CASE_BLIND, None, log_name, PSL.PSL_C_USER, il)
        attributes = ILE3.getint(il, 1)
        value_length = ILE3.getint(il, 2)
        value_str = ILE3.getstr(il, 3, 0)
        table_str = ILE3.getstr(il, 4, 0)
        ILE3.delete(il)
        self.assertEqual(1, status)
        self.assertEqual(attributes & LNM.LNM_M_EXISTS, LNM.LNM_M_EXISTS)
        self.assertEqual(value_length, len(log_value))
        self.assertEqual(value_str, log_value)
        self.assertEqual(table_str, log_table)

        status = SYS.dellnm(log_table, log_name, PSL.PSL_C_USER)
        self.assertEqual(1, status)


if __name__ == "__main__":
    unittest.main(verbosity=2)
