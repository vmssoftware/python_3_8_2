import pprint
import struct
import _rms
from vms.fabdef import *
import vms.rmsdef

acc = FAB_M_PUT + FAB_M_GET + FAB_M_DEL + FAB_M_UPD
shr = FAB_M_SHRPUT + FAB_M_SHRGET + FAB_M_SHRDEL + FAB_M_SHRUPD
f = _rms.file('test.dat',fac=acc, shr=shr)

print
print('Empty file')
f.rewind()
s = f.find()
while(s != vms.rmsdef.RMS__EOF):
    f.delete()
    s = f.find()


print
print('Insert a few records')
rec = struct.pack(b"=5si11s", b'AA5AA', 5, b'1234567890A')
f.put(rec)
rec = struct.pack(b"=5si11s", b'BB2BB', 2, b'1234567890B')
f.put(rec)
rec = struct.pack(b"=5si11s", b'CC4CC', 4, b'1234567890C')
f.put(rec)
rec = struct.pack(b"=5si11s", b'DD1DD', 1, b'1234567890D')
f.put(rec)
rec = struct.pack(b"=5si11s", b'EE1EE', 2, b'1234567890E')
f.put(rec)
rec = struct.pack(b"=5si11s", b'FF3FF', 3, b'1234567890F')
f.put(rec)


print
print('Initial records')
f.rewind()
while(1):
    s, r = f.fetch()
    if r == None: break
    print(struct.unpack("=5si11s", r))


print
print('Update all records')
f.rewind()
while(1):
    s, r = f.fetch()
    if r == None: break
    k,n,v = struct.unpack("=5si11s", r)
    rec = struct.pack(b"=5si11s", k, n+10, b'X' + v[1:])
    s = f.update(rec)


print
print('use secondary key and an iterator')
s = f.usekey(1)
s = f.rewind()
for r in f:
    print(struct.unpack("=5si11s", r))


print
print('build a list of all records')
f.usekey(0)
f.rewind()
pprint.pprint([struct.unpack("=5si11s", r) for r in f])


f.usekey(1)

print
print('delete a record using delete(14)')
key = struct.pack("=i", 14)
s = f.delete(key)

print
print('delete all record key 12 using find(12) + delete()')
key = struct.pack("=i", 12)
s = f.find(key)
while (1):
    s, r = f.fetch()
    if r:
        k,n,v = struct.unpack("=5si11s", r)
        if n == 12:
            s = f.delete()
        else:
            break
    else:
        break

print
print('build a list of all records after delete')
f.rewind()
pprint.pprint([struct.unpack("=5si11s", r) for r in f])


print('Close the file')
f.close()
