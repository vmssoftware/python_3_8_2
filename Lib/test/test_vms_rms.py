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
        if os.access('test.fdl'):
            os.system('CREATE/FDL=test.fdl test.dat')
        else:
            os.system('CREATE/FDL=python$root:[lib.python3^.8.test]test.fdl test.dat')

        self.records = [
            (b'AA5AA', 5, b'1234567890A'),
            (b'BB2BB', 2, b'1234567890B'),
            (b'CC4CC', 4, b'1234567890C'),
            (b'DD1DD', 1, b'1234567890D'),
            (b'EE1EE', 2, b'1234567890E'),
            (b'FF3FF', 3, b'1234567890F'),
        ]

        self.records_expected_key1_unmodified = [
            (b'DD1DD', 1, b'1234567890D'),
            (b'BB2BB', 2, b'1234567890B'),
            (b'EE1EE', 2, b'1234567890E'),
            (b'FF3FF', 3, b'1234567890F'),
            (b'CC4CC', 4, b'1234567890C'),
            (b'AA5AA', 5, b'1234567890A'),
        ]

        self.records_expected_key0_modified = [
            (b'AA5AA', 15, b'X234567890A'),
            (b'BB2BB', 12, b'X234567890B'),
            (b'CC4CC', 14, b'X234567890C'),
            (b'DD1DD', 11, b'X234567890D'),
            (b'EE1EE', 12, b'X234567890E'),
            (b'FF3FF', 13, b'X234567890F'),
        ]

        self.records_expected_key1_modified = [
            (b'DD1DD', 11, b'X234567890D'),
            (b'BB2BB', 12, b'X234567890B'),
            (b'EE1EE', 12, b'X234567890E'),
            (b'FF3FF', 13, b'X234567890F'),
            (b'CC4CC', 14, b'X234567890C'),
            (b'AA5AA', 15, b'X234567890A'),
        ]

        self.records_expected_key0_after_del_12 = [
            (b'AA5AA', 15, b'X234567890A'),
            (b'CC4CC', 14, b'X234567890C'),
            (b'DD1DD', 11, b'X234567890D'),
            (b'FF3FF', 13, b'X234567890F'),
        ]

        self.records_expected_key1_after_del_14_12 = [
            (b'DD1DD', 11, b'X234567890D'),
            (b'FF3FF', 13, b'X234567890F'),
            (b'AA5AA', 15, b'X234567890A'),
        ]


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
        for rec_ in self.records:
            rec = struct.pack(b"=5si11s", rec_[0], rec_[1], rec_[2])
            f.put(rec)

        # Initial records
        f.rewind()
        for rec_ in self.records:
            r_expected = struct.pack(b"=5si11s", rec_[0], rec_[1], rec_[2])
            s, r_from_file = f.fetch()
            self.assertEqual(s, RMSDEF.RMS__NORMAL)
            self.assertEqual(r_from_file, r_expected)

        # Update all records
        f.rewind()
        for rec_ in self.records:
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
            rec_ = self.records_expected_key1_modified[pos]
            pos = pos + 1
            r_expected = struct.pack(b"=5si11s", rec_[0], rec_[1], rec_[2])
            self.assertEqual(r, r_expected)

        # build a list of all records
        f.usekey(0)
        f.rewind()
        all_recs = [struct.unpack("=5si11s", r) for r in f]
        self.assertEqual(all_recs, self.records_expected_key0_modified)

        f.usekey(1)
        f.rewind()
        all_recs = [struct.unpack("=5si11s", r) for r in f]
        self.assertEqual(all_recs, self.records_expected_key1_modified)

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
        self.assertEqual(all_recs, self.records_expected_key1_after_del_14_12)

        # Close the file
        f.close()



    def test_demo_2(self):

        from vms.indexedfile import IndexedFile, Record

        class TestRec(Record):
            _field = [
                    ('f1', '5s'),
                    ('f2', 'i'),
                    ('f3', '11s'),
                    ]
            _fmt = '=' + ''.join([x[1] for x in _field])
            _fixsize = struct.calcsize(_fmt)

            def __eq__(self, other):
                return self.pack() == other.pack()

            def keyval(self, keynum):
                if keynum == 0:
                    return self.f1
                elif keynum == 1:
                    return self.f2
                else:
                    raise "Invalid keynum parameter"


        class TestFile(IndexedFile):
            Name = 'test.dat'
            def __init__(self):
                IndexedFile.__init__(self, TestFile.Name, TestRec)

            def primary_keynum(self):
                return 0

            def pack_key(self, keynum, keyval):
                if keynum == 0:
                    return struct.pack("=5s", keyval)
                elif keynum == 1:
                    return struct.pack("=i", keyval)
                else:
                    raise KeyError

        f = TestFile()
        # Reset file
        f.reset()

        # Insert a few records
        for rec_ in self.records:
            rec = TestRec(rec_)
            f.put(rec)

        # Initial records using an iterator, primary key order
        pos = 0
        for rec in f:
            self.assertEqual(rec, TestRec(self.records[pos]))
            pos = pos + 1

        # Initial records using an iterator, secondary key order
        f.iterator_keynum(1)
        pos = 0
        for rec in f:
            self.assertEqual(rec, TestRec(self.records_expected_key1_unmodified[pos]))
            pos = pos + 1

        # Update all records using an iterator
        # iterator key is automatically reset to primary key
        for rec in f:
            rec.f2 = rec.f2 + 10
            rec.f3 = b'X' + rec.f3[1:]
            f.update_current(rec)

        # Updated records using an iterator
        pos = 0
        for rec in f:
            self.assertEqual(rec, TestRec(self.records_expected_key0_modified[pos]))
            pos = pos + 1

        # build a list of all records
        lst = f.fetchall(0)
        self.assertEqual(lst, list(TestRec(x) for x in self.records_expected_key0_modified))

    ### Not implemented
    ###    print
    ###    print ('delete a record using secondary key value 14')
    ###    f.delete(1, 14)

        # Records using an iterator, secondary key order
        f.iterator_keynum(1)
        pos = 0
        for rec in f:
            self.assertEqual(rec, TestRec(self.records_expected_key1_modified[pos]))
            pos = pos + 1

        # delete all records using secondary key value 12
        f.open(acc = FABDEF.FAB_M_GET + FABDEF.FAB_M_DEL)
        f.usekey(1)
        f.rewind()
        f.find(None, f.pack_key(1, 12))
        for rec in f:
            if rec.f2 == 12:
                f.delete_current()
            else:
                break
        f.close()

        # Records using an iterator, primary key order
        pos = 0
        for rec in f:
            self.assertEqual(rec, TestRec(self.records_expected_key0_after_del_12[pos]))
            pos = pos + 1


    def test_demo_3(self):
        acc = FABDEF.FAB_M_GET
        shr = FABDEF.FAB_M_SHRPUT + FABDEF.FAB_M_SHRGET + FABDEF.FAB_M_SHRDEL + FABDEF.FAB_M_SHRUPD
        try:
            f = RMS.file('sysuaf',fac=acc, shr=shr)
        except:
            raise unittest.SkipTest('High privileges required')

        # Alphabetic order

        for i in range(10):
            s,r = f.fetch()
            lst = struct.unpack("=i32shhiQ32s32p", r[:116])
            self.assertIsNotNone(lst)
            # print('%s [%o,%o]' % (lst[1], lst[3], lst[2]))

        # UIC order

        f.usekey(1)
        f.rewind()
        i = 0
        for r in f:
            i += 1
            if (i > 10): break
            lst = struct.unpack("=i32shhiQ32s32p", r[:116])
            self.assertIsNotNone(lst)
            # print('%s [%o,%o]' % (lst[1], lst[3], lst[2]))


        # Alphabetic order

        f.usekey(0)
        f.rewind()
        for r in f:
            lst = struct.unpack("=i32shhiQ32s32p", r[:116])
            self.assertIsNotNone(lst)
            # print('%s [%o,%o]' % (lst[1], lst[3], lst[2]))

        f.close()


if __name__ == "__main__":
    unittest.main(verbosity=2)
