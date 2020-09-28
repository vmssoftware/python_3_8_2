import struct
from vms.rmsdef import RMS__SUC, RMS__RNF, RMS__OK_DUP
from vms.fabdef import FAB_M_PUT, FAB_M_GET, FAB_M_DEL, FAB_M_UPD, \
       FAB_M_SHRPUT, FAB_M_SHRGET, FAB_M_SHRDEL, FAB_M_SHRUPD
# from _rms.IndexedFile import IndexedFile, Record
from vms.indexedfile import IndexedFile, Record

class TestRec(Record):
    _field = [
              ('f1', '5s'),
              ('f2', 'i'),
              ('f3', '11s'),
             ]
    _fmt = '=' + ''.join([x[1] for x in _field])
    _fixsize = struct.calcsize(_fmt)

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

if __name__=='__main__':
    f = TestFile()
    print
    print ('Reset file')
    f.reset()

    print
    print ('Insert a few records')
    rec = TestRec(('AA5AA', 5, '1234567890A'))
    f.put(rec)
    rec = TestRec(('BB2BB', 2, '1234567890B'))
    f.put(rec)
    rec = TestRec(('CC4CC', 4, '1234567890C'))
    f.put(rec)
    rec = TestRec(('DD1DD', 1, '1234567890D'))
    f.put(rec)
    rec = TestRec(('EE1EE', 2, '1234567890E'))
    f.put(rec)
    rec = TestRec(('FF3FF', 3, '1234567890F'))
    f.put(rec)

    print
    print ('Initial records using an iterator, primary key order')
    for rec in f:
        print( rec.f1, rec.f2, rec.f3)

    print
    print ('Initial records using an iterator, secondary key order')
    f.iterator_keynum(1)
    for rec in f:
        print( rec.f1, rec.f2, rec.f3)

    print
    print ('Update all records using an iterator')
    # iterator key is automatically reset to primary key
    for rec in f:
        rec.f2 = rec.f2 + 10
        f.update_current(rec)

    print
    print ('Updated records using an iterator')
    for rec in f:
        print( rec.f1, rec.f2, rec.f3)

    print
    print ('build a list of all records')
    lst = f.fetchall(0)
    print (len(lst))

### Not implemented
###    print
###    print ('delete a record using secondary key value 14')
###    f.delete(1, 14)

    print
    print ('Records using an iterator, secondary key order')
    f.iterator_keynum(1)
    for rec in f:
        print (rec.f1, rec.f2, rec.f3)

    print ('delete all records using secondary key value 12')
    f.open(acc = FAB_M_GET + FAB_M_DEL)
    f.usekey(1)
    f.rewind()
    f.find(None, f.pack_key(1, 12))
    for rec in f:
        if rec.f2 == 12:
           f.delete_current()
        else:
           break
    f.close()

    print
    print ('Records using an iterator, primary key order')
    for rec in f:
        print( rec.f1, rec.f2, rec.f3)
