import struct
import _rms
from vms.fabdef import *
import vms.rmsdef
acc = FAB_M_GET
shr = FAB_M_SHRPUT + FAB_M_SHRGET + FAB_M_SHRDEL + FAB_M_SHRUPD
f = _rms.file('sysuaf',fac=acc, shr=shr)


print('Alphabetic order')

for i in range(10):
    s,r = f.fetch()
    lst = struct.unpack("=i32shhiQ32s32p", r[:116])
    print('%s [%o,%o]' % (lst[1], lst[3], lst[2]))

print
print('UIC order')

f.usekey(1)
f.rewind()
i = 0
for r in f:
    i += 1
    if (i > 10): break
    lst = struct.unpack("=i32shhiQ32s32p", r[:116])
    print('%s [%o,%o]' % (lst[1], lst[3], lst[2]))


print
print('Alphabetic order')

f.usekey(0)
f.rewind()
for r in f:
    lst = struct.unpack("=i32shhiQ32s32p", r[:116])
    print('%s [%o,%o]' % (lst[1], lst[3], lst[2]))


f.close()
