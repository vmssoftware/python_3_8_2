import _rms
import vms.fabdef
import vms.rmsdef
import struct
from decimal import Decimal

class BCD:
    def __init__(self, ipart, dpart):
        self._ipart = ipart
        self._dpart = dpart
    def decode(self, val):
        return Decimal(_rms.BCD2Tuple(val, self._dpart))
    def pack(self, val):
        return _rms.Tuple2BCD(val.as_tuple(), self._ipart,
                                 self._dpart)

class IndexedFile:
    def __init__(self, filename, item_class, auto_open_close=True):
        self.filename = filename
        self._status = None
        self._item_class = item_class
        self._auto_init = self._auto = auto_open_close
        self._file = None
        self._iterkeynum = self.primary_keynum()

    def __del__(self):
        if self._file:
            self.close()

    def primary_keynum(self):
        raise NotImplementedError('IndexedFile.primary_keynum')

    def iterator_keynum(self, keynum = None):
        prev = self._iterkeynum
        if keynum is not None:
            self._iterkeynum = keynum
        return prev

    def __iter__(self):
        if self._auto and not self._file:
            self.open()
            self._file.usekey(self._iterkeynum)
            self._file.rewind()
            self._open_by_iterator = True
        else:
            self._open_by_iterator = False
        return self

    def __next__(self):
        try:
            rec = self._file.__next__()
        except StopIteration:
            if self._open_by_iterator:
                self.close()
            raise
        return self._item_class.unpack(rec, self._item_class)

    def next(self):
        return self.__next__()

    def usekey(self, keynum):
        self._file.usekey(keynum)

    def rewind(self):
        self._file.rewind()

    def find(self, keynum, keypack):
        if not self._auto:
            f = self._file
        else:
            acc = vms.fabdef.FAB_M_GET
            shr = vms.fabdef.FAB_M_SHRPUT + vms.fabdef.FAB_M_SHRGET + vms.fabdef.FAB_M_SHRDEL + vms.fabdef.FAB_M_SHRUPD
            f = _rms.file(self.filename, fac=acc, shr=shr)
        if keynum is not None:
            f.usekey(keynum)
        try:
            f.find(keypack)
        except _rms.error as e:
            if self._auto:
                f.close()
            if e.errno == vms.rmsdef.RMS__RNF:
                return False
            else:
                raise
        return True

    def open(self,
             acc = vms.fabdef.FAB_M_PUT + vms.fabdef.FAB_M_GET + vms.fabdef.FAB_M_DEL + vms.fabdef.FAB_M_UPD,
             shr = vms.fabdef.FAB_M_SHRPUT + vms.fabdef.FAB_M_SHRGET + vms.fabdef.FAB_M_SHRDEL + vms.fabdef.FAB_M_SHRUPD,
             fop = 0):
        self._auto = False
        self._status = None
        self._file = _rms.file(self.filename, fac=acc, shr=shr, fop=fop)

    def close(self):
        self._file.close()
        self._file = None
        self._auto = self._auto_init
        self._iterkeynum = self.primary_keynum()

    def status(self):
        return self._status

    def put(self, rec):
        if not self._auto:
            f = self._file
        else:
            # add record to the file
            acc = vms.fabdef.FAB_M_PUT + vms.fabdef.FAB_M_GET + vms.fabdef.FAB_M_DEL + vms.fabdef.FAB_M_UPD
            shr = vms.fabdef.FAB_M_SHRPUT + vms.fabdef.FAB_M_SHRGET + vms.fabdef.FAB_M_SHRDEL + vms.fabdef.FAB_M_SHRUPD
            fop = 0
            f = _rms.file(self.filename, fac=acc, shr=shr, fop=fop)
        self._status = None
        rec = rec.pack()
        try:
            self._status = f.put(rec)
        except _rms.error:
            if self._auto:
                f.close()
            raise
        else:
            if self._auto:
                f.close()

    def delete(self, keynum, keyval):
        if not self._auto:
            f = self._file
        else:
            acc = vms.fabdef.FAB_M_GET + vms.fabdef.FAB_M_DEL
            shr = vms.fabdef.FAB_M_SHRPUT + vms.fabdef.FAB_M_SHRGET + vms.fabdef.FAB_M_SHRDEL + vms.fabdef.FAB_M_SHRUPD
            f = _rms.file(self.filename, fac=acc, shr=shr)
        self._status = None
        f.usekey(keynum)
        key = self._item_class.pack_key(keynum, keyval)
        try:
            self._status = f.find(key)
            self._status = f.delete()
        except _rms.error:
            if self._auto:
                f.close()
            raise
        else:
            if self._auto:
                f.close()

    def delete_current(self):
        if not self._file:
            raise "File is not open"
        self._status = None
        try:
            self._status = self._file.delete()
        except _rms.error:
            raise

    def fetch(self, keynum, keyval, item_class = None):
        if item_class is None:
            item_class = self._item_class
        if not self._auto:
            f = self._file
        else:
            acc = vms.fabdef.FAB_M_GET
            shr = vms.fabdef.FAB_M_SHRPUT + vms.fabdef.FAB_M_SHRGET + vms.fabdef.FAB_M_SHRDEL + vms.fabdef.FAB_M_SHRUPD
            f = _rms.file(self.filename, fac=acc, shr=shr)
        f.usekey(keynum)
        self._status = None
        key = self._item_class.pack_key(keynum, keyval)
        try:
            self._status, r = f.fetch(key)
        except _rms.error:
            if self._auto:
                f.close()
            raise
        else:
            if self._auto:
                f.close()
        if self._status == vms.rmsdef.RMS__RNF:
             return None
        return item_class.unpack(r, item_class)

    def update(self, rec):
        if not self._auto:
            f = self._file
        else:
            acc = vms.fabdef.FAB_M_GET + vms.fabdef.FAB_M_UPD
            shr = vms.fabdef.FAB_M_SHRPUT + vms.fabdef.FAB_M_SHRGET + vms.fabdef.FAB_M_SHRDEL + vms.fabdef.FAB_M_SHRUPD
            f = _rms.file(self.filename, fac=acc, shr=shr)
        self._status = None
        f.usekey(self.primary_keynum())
        key = self._item_class.pack_key(self.primary_keynum(),
                                        rec.keyval(self.primary_keynum()))
        try:
            self._status, r = f.fetch(key)
            rec = rec.pack()
            self._status = f.update(rec)
        except _rms.error:
            if self._auto:
                f.close()
            raise
        else:
            if self._auto:
                f.close()

    def update_current(self, rec):
        if not self._file:
            raise "File is not open"
        self._status = None
        try:
            rec = rec.pack()
            self._status = self._file.update(rec)
        except _rms.error:
            raise

    def reset(self):
        if not self._auto:
            f = self._file
        else:
            acc = vms.fabdef.FAB_M_PUT + vms.fabdef.FAB_M_GET + vms.fabdef.FAB_M_DEL + vms.fabdef.FAB_M_UPD
            shr = vms.fabdef.FAB_M_SHRPUT + vms.fabdef.FAB_M_SHRGET + vms.fabdef.FAB_M_SHRDEL + vms.fabdef.FAB_M_SHRUPD
            f = _rms.file(self.filename, fac=acc, shr=shr)
        self._status = None
        f.rewind()
        s = f.find()
        while(s != vms.rmsdef.RMS__EOF):
            f.delete()
            s = f.find()
        if self._auto:
            f.close()

    def fetchall(self, keynum, keypack = None, stop = None, limit = None):
        if not self._auto:
            f = self._file
        else:
            acc = vms.fabdef.FAB_M_GET
            shr = vms.fabdef.FAB_M_SHRPUT + vms.fabdef.FAB_M_SHRGET + vms.fabdef.FAB_M_SHRDEL + vms.fabdef.FAB_M_SHRUPD
            f = _rms.file(self.filename, fac=acc, shr=shr)
        self._status = None
        f.usekey(keynum)
        f.rewind()
        if keypack:
            try:
                f.find(keypack)
            except _rms.error as e:
                if self._auto:
                    f.close()
                if e.errno == vms.rmsdef.RMS__RNF:
                    return []
                else:
                    raise
        res = []
        n = 0
        for r in f:
            rec = self._item_class.unpack(r, self._item_class)
            if stop and stop(rec):
                break
            res.append(rec)
            n += 1
            if limit is not None and n >= limit:
                break
        if self._auto:
            f.close()
        return res

class Record:
    autoDecodeRecord = True
    _field = []
    _fixsize = 0
    _fmt = ''
    _varsize = 0
    def __init__(self, data, conv = True, rec = None, opt = None):
        for i in range(len(self._field)):
            setattr(self, self._field[i][0], data[i])
        self._varrec = []
#        self._rec = rec
        self._conv = conv
        if self._varsize > 0:
            vartbl = data[-1]
            for i in range(len(vartbl) / self._varsize):
                self._varrec.append(
                    self._varclass.unpack(vartbl[i * self._varsize : (i + 1) * self._varsize],
                                          self._varclass))
        if self.autoDecodeRecord:
            self.decode()

    def pack_key(keynum, keyval):
        raise NotImplementedError('Record.pack_key')
    pack_key = staticmethod(pack_key)

    def keyval(self, keynum):
        raise NotImplementedError('Record.keyval')

    def decode(self):
        if not self._conv:
            for i in range(len(self._field)):
                if len(self._field[i]) > 2:
                    v = getattr(self, self._field[i][0])
                    v = self._field[i][2].decode(v)
                    setattr(self, self._field[i][0], v)
            self._conv = True

    def pack(self):
        val = []
        for i in range(len(self._field)):
            v = getattr(self, self._field[i][0])
            if len(self._field[i]) > 2:
                if self._conv:
                    v = self._field[i][2].pack(v)
            if isinstance(v, str):
                 val.append(v.encode())
            else:
                 val.append(v)
        # print(*val)
        lst = [struct.pack(self._fmt, *val)]
        for v in self._varrec:
            lst.append(v.pack())
        r = b''.join(lst)
#        if (r != self._rec):
#            print 'OOps!'
#            print repr(r)
#            print repr(self.rec)

        return r

    def unpack(rec, classe, opt = None):
        varfmt = ''
        if len(rec) > classe._fixsize:
            varfmt = '%ds' % (len(rec) - classe._fixsize)
        lst = list(struct.unpack(classe._fmt + varfmt, rec))
        return classe(lst, False, rec, opt)
    unpack = staticmethod(unpack)
