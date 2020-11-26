import struct
import _rms
from vms.fabdef import *
import vms.sys

acc = FAB_M_GET
shr = FAB_M_SHRPUT + FAB_M_SHRGET + FAB_M_SHRDEL + FAB_M_SHRUPD
f = _rms.file('sysuaf',fac=acc, shr=shr)

print(f.longname)
print('Number of keys:', f.nok)
print

print('Alphabetic order')
print('%-32s %-20s  %s' %('Username', 'UIC', 'Last Interactive Login'))
print('-'*78)

for i in range(4):
    s,r = f.fetch()
    lst = struct.unpack("=i32sHHIQ32s32p32p64p64p32p32pQQHHBBBBQQQQQQQQ",
                        r[:428])
    uic = '[%o,%o]' % (lst[3], lst[2])
    sts, dt = vms.sys.asctim(lst[25], 0)
    print('%s %-16s %28s' % (lst[1], uic, dt))

print
print('UIC order')
print('%-32s %-20s  %s' %('Username', 'UIC', 'Last Interactive Login'))
print('-'*78)

f.usekey(1)
f.rewind()
i = 0
for r in f:
    i += 1
    if (i > 4): break
    lst = struct.unpack("=i32sHHIQ32s32p32p64p64p32p32pQQHHBBBBQQQQQQQQ",
                        r[:428])
    uic = '[%o,%o]' % (lst[3], lst[2])
    sts, dt = vms.sys.asctim(lst[25], 0)
    print('%s %-16s %28s' % (lst[1], uic, dt))

f.close()
