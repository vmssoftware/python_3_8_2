import struct
import _rms
import vms.fabdef
import vms.rmsdef

__version__ = '1.0'
__doc__ = 'get user information from sysuaf'

class user:
    def __init__(self, username, uic_member, uic_group, sub_id, parent_id,
                 account, owner, defdev, defdir, lgicmd, defcli, clitables,
                 pwd, pwd2, logfails, salt, encrypt, encrypt2, pwd_length,
                 expiration, pwd_lifetime, pwd_date, pwd2_date, lastlogin_i,
                 lastlogin_n, priv, def_priv, min_class, max_class, flags,
                 hourly_access, primedays, pri, quepri, maxjobs, maxacctjobs,
                 maxdetach, prccnt, biolm, diolm, tqcnt, astlm, enqlm, fillm,
                 shrfillm, wsquota, dfwscnt, wsextent, pgflquota, cputim,
                 bytlm, pbytlm, jtquota, proxy_lim, proxies, account_lim,
                 accounts, def_class):
        self.username = username
        self.uic_member = uic_member
        self.uic_group = uic_group
        self.sub_id = sub_id
        self.parent_id = parent_id
        self.account = account
        self.owner = owner
        self.defdev = defdev
        self.defdir = defdir
        self.lgicmd = lgicmd
        self.defcli = defcli
        self.clitables = clitables
        self.pwd = pwd
        self.pwd2 = pwd2
        self.logfails = logfails
        self.salt = salt
        self.encrypt = encrypt
        self.encrypt2 = encrypt2
        self.pwd_length = pwd_length
        self.expiration = expiration
        self.pwd_lifetime = pwd_lifetime
        # pre-expired pwd
        if pwd_date == 18446744073709551615:
            self.pwd_date = None
        else:
            self.pwd_date = pwd_date
        # pre-expired pwd
        if pwd2_date == 18446744073709551615:
            self.pwd2_date = None
        else:
            self.pwd2_date = pwd2_date
        self.lastlogin_i = lastlogin_i
        self.lastlogin_n = lastlogin_n
        self.priv = priv
        self.def_priv = def_priv
        self.min_class = min_class
        self.max_class = max_class
        self.flags = flags
        self.hourly_access = hourly_access
        self.primedays = primedays
        self.pri = pri
        self.quepri = quepri
        self.maxjobs = maxjobs
        self.maxacctjobs = maxacctjobs
        self.maxdetach = maxdetach
        self.prccnt = prccnt
        self.biolm = biolm
        self.diolm = diolm
        self.tqcnt = tqcnt
        self.astlm = astlm
        self.enqlm = enqlm
        self.fillm = fillm
        self.shrfillm = shrfillm
        self.wsquota = wsquota
        self.dfwscnt = dfwscnt
        self.wsextent = wsextent
        self.pgflquota = pgflquota
        self.cputim = cputim
        self.bytlm = bytlm
        self.pbytlm = pbytlm
        self.jtquota = jtquota
        self.proxy_lim = proxy_lim
        self.proxies = proxies
        self.account_lim = account_lim
        self.accounts = accounts
        self.def_class = def_class

def all_users():
    acc = vms.fabdef.FAB_M_PUT + vms.fabdef.FAB_M_GET + vms.fabdef.FAB_M_DEL +\
          vms.fabdef.FAB_M_UPD
    shr = vms.fabdef.FAB_M_SHRPUT + vms.fabdef.FAB_M_SHRGET +\
          vms.fabdef.FAB_M_SHRDEL + vms.fabdef.FAB_M_SHRUPD
    f = _rms.file('sysuaf',fac=acc, shr=shr)
    res = {}
    format = "=4x32sHHIQ32s32p32p64p64p32p32pQQHHBBBB8xQQQQQQQ20s20sI30s12xBx"
    format += "BBHHHHHHHHHHHLLLLLLLLHHHH20s"
    for r in f:
        lst = struct.unpack(format, r[:600])
        usr = user(*lst)
        res[usr.username] = usr
    f.close()
    return res


l = all_users()
print(*l, sep = "\n")
