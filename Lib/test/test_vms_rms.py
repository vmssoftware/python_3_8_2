import sys
import os
import unittest

if sys.platform != 'OpenVMS':
    raise unittest.SkipTest('OpenVMS required')

import struct
import _rms as RMS
import vms.fabdef as FABDEF
import vms.rmsdef as RMSDEF
import vms.ssdef as SSDEF

class BaseTestCase(unittest.TestCase):

    def setUp(self):
        try:
            os.unlink('test.dat')
        except:
            pass
        os.system('CREATE/FDL=test.fdl test.dat')

    def tearDown(self):
        try:
            os.unlink('test.dat')
        except:
            pass

    def test_demo_1(self):
        acc = FABDEF.FAB_M_PUT + FABDEF.FAB_M_GET + FABDEF.FAB_M_DEL + FABDEF.FAB_M_UPD
        shr = FABDEF.FAB_M_SHRPUT + FABDEF.FAB_M_SHRGET + FABDEF.FAB_M_SHRDEL + FABDEF.FAB_M_SHRUPD
        f = RMS.file('test.dat', fac=acc, shr=shr)

        # empty file
        f.rewind()
        s = f.find()
        while(s != RMSDEF.RMS__EOF):
            f.delete()
            s = f.find()

        # Insert a few records
        records = [
            (b'AA5AA', 5, b'1234567890A'),
            (b'BB2BB', 2, b'1234567890B'),
            (b'CC4CC', 4, b'1234567890C'),
            (b'DD1DD', 1, b'1234567890D'),
            (b'EE1EE', 2, b'1234567890E'),
            (b'FF3FF', 3, b'1234567890F'),
        ]

        records_expected_key0 = [
            (b'AA5AA', 15, b'X234567890A'),
            (b'BB2BB', 12, b'X234567890B'),
            (b'CC4CC', 14, b'X234567890C'),
            (b'DD1DD', 11, b'X234567890D'),
            (b'EE1EE', 12, b'X234567890E'),
            (b'FF3FF', 13, b'X234567890F'),
        ]

        records_expected_key1 = [
            (b'DD1DD', 11, b'X234567890D'),
            (b'BB2BB', 12, b'X234567890B'),
            (b'EE1EE', 12, b'X234567890E'),
            (b'FF3FF', 13, b'X234567890F'),
            (b'CC4CC', 14, b'X234567890C'),
            (b'AA5AA', 15, b'X234567890A'),
        ]

        records_expected_key1_after_del = [
            (b'DD1DD', 11, b'X234567890D'),
            (b'FF3FF', 13, b'X234567890F'),
            (b'AA5AA', 15, b'X234567890A'),
        ]

        for rec_ in records:
            rec = struct.pack(b"=5si11s", rec_[0], rec_[1], rec_[2])
            f.put(rec)

        # Initial records
        f.rewind()
        for rec_ in records:
            r_expected = struct.pack(b"=5si11s", rec_[0], rec_[1], rec_[2])
            s, r_from_file = f.fetch()
            self.assertEqual(s, RMSDEF.RMS__NORMAL)
            self.assertEqual(r_from_file, r_expected)

        # Update all records
        f.rewind()
        for rec_ in records:
            s, r = f.fetch()
            self.assertEqual(s, RMSDEF.RMS__NORMAL)
            self.assertNotEqual(r, None)
            k, n, v = struct.unpack("=5si11s", r)
            rec = struct.pack(b"=5si11s", k, n + 10, b'X' + v[1:])
            s = f.update(rec)
            self.assertIn(s, (RMSDEF.RMS__NORMAL, RMSDEF.RMS__OK_DUP))

        # use secondary key and an iterator
        s = f.usekey(1)
        s = f.rewind()
        pos = 0
        for r in f:
            self.assertEqual(s, RMSDEF.RMS__NORMAL)
            rec_ = records_expected_key1[pos]
            pos = pos + 1
            r_expected = struct.pack(b"=5si11s", rec_[0], rec_[1], rec_[2])
            self.assertEqual(r, r_expected)

        # build a list of all records
        f.usekey(0)
        f.rewind()
        all_recs = [struct.unpack("=5si11s", r) for r in f]
        self.assertEqual(all_recs, records_expected_key0)

        f.usekey(1)
        f.rewind()
        all_recs = [struct.unpack("=5si11s", r) for r in f]
        self.assertEqual(all_recs, records_expected_key1)

        # delete a record using delete(14)
        f.usekey(1)

        key = struct.pack("=i", 14)
        s = f.delete(key)
        self.assertEqual(s, RMSDEF.RMS__NORMAL)

        #  delete all record key 12 using find(12) + delete()
        key = struct.pack("=i", 12)
        s = f.find(key)
        self.assertEqual(s, RMSDEF.RMS__NORMAL)

        while (1):
            s, r = f.fetch()
            if r:
                k, n, v = struct.unpack("=5si11s", r)
                if n == 12:
                    s = f.delete()
                else:
                    break
            else:
                break

        # build a list of all records after delete
        f.rewind()
        all_recs = [struct.unpack("=5si11s", r) for r in f]
        self.assertEqual(all_recs, records_expected_key1_after_del)

        # Close the file
        f.close()

if __name__ == "__main__":
    unittest.main(verbosity=2)
