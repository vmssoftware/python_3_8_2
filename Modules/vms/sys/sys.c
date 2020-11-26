#define __NEW_STARLET 1
#include <starlet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <descrip.h>
#include <lib$routines.h>
#include <iosbdef.h>
#include <efndef.h>
#include <ssdef.h>
#include <gen64def.h>
#include <assert.h>
#include <iodef.h>

#include "sys.h"
#include "lcl.h"

#ifndef OKAY
#define OKAY(STATUS) (((STATUS) & 1) != 0)
#endif

#ifndef DEF_TABNAM
#define DEF_TABNAM "LNM$FILE_DEV"
#endif


static const char *nil = "";

unsigned int _asctim(long long timaddr, char **timbuf, char cvtflg)
{
    char val[64];
    struct dsc$descriptor_s val_dsc;
    unsigned int status;
    unsigned short len;

    assert(timbuf);

    val_dsc.dsc$w_length = sizeof(val) - 1;
    val_dsc.dsc$b_class = DSC$K_CLASS_S;
    val_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    val_dsc.dsc$a_pointer = val;

    status =
	sys$asctim(&len, &val_dsc, (struct _generic_64 *) &timaddr,
		   cvtflg);
    if (OKAY(status)) {
        val[len] = '\0';
        *timbuf = strdup(val);
        assert(*timbuf);
    } else {
        *timbuf = strdup(nil);
    	assert(*timbuf);
    }

    return (status);
}


unsigned int _bintim(char *timbuf, long long *timadr)
{
    struct dsc$descriptor_s tim_dsc;

    assert(timbuf);
    assert(timadr);

    tim_dsc.dsc$w_length = strlen(timbuf);
    tim_dsc.dsc$b_class = DSC$K_CLASS_S;
    tim_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    tim_dsc.dsc$a_pointer = timbuf;

    return (sys$bintim(&tim_dsc, (struct _generic_64 *) timadr));
}



unsigned int _hiber()
{
    return (sys$hiber());
}

unsigned int _schdwk(unsigned int *pidadr, char *prcnam, long long daytim, long long reptim) {
    unsigned int status = 0;
    struct dsc$descriptor_s prcnam_dsc;

    if (prcnam) {
        prcnam_dsc.dsc$w_length = strlen(prcnam);
        prcnam_dsc.dsc$b_class = DSC$K_CLASS_S;
        prcnam_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
        prcnam_dsc.dsc$a_pointer = prcnam;
    }

    status = sys$schdwk(pidadr, prcnam ? &prcnam_dsc : NULL, (struct _generic_64 *)&daytim, (struct _generic_64 *)&reptim);

    return status;
}


unsigned int _assign(char *devnam, unsigned short int *chan,
		     unsigned int acmode, char *mbxnam, unsigned int flags)
{
    struct dsc$descriptor_s dev_dsc;
    struct dsc$descriptor_s mbx_dsc;
    unsigned int status;

    assert(devnam);
    assert(chan);

    dev_dsc.dsc$w_length = strlen(devnam);
    dev_dsc.dsc$b_class = DSC$K_CLASS_S;
    dev_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    dev_dsc.dsc$a_pointer = devnam;

    if (mbxnam != NULL) {
	mbx_dsc.dsc$w_length = strlen(mbxnam);
	mbx_dsc.dsc$b_class = DSC$K_CLASS_S;
	mbx_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
	mbx_dsc.dsc$a_pointer = mbxnam;
    }

    status =
	sys$assign(&dev_dsc, chan, acmode, mbxnam ? &mbx_dsc : NULL,
		   flags);

    return (status);
}


unsigned int _dassgn(unsigned short int chan)
{
    return (sys$dassgn(chan));
}


unsigned int _cancel(unsigned short int chan)
{
    return (sys$cancel(chan));
}



unsigned int _finish_rdb(unsigned int *context)
{
    assert(context);
    return (sys$finish_rdb(context));
}


unsigned int _find_held(unsigned int holder, unsigned int *id,
			unsigned int *attrib, unsigned int *context)
{
    unsigned int qw[2];
    assert(id);
    assert(attrib);
    assert(context);

    qw[0] = holder;
    qw[1] = 0;

    return (sys$find_held
	    ((struct _generic_64 *) &qw[0], id, attrib, context));
}



unsigned int _asctoid(char *name, unsigned int *id, unsigned int *attrib)
{
    struct dsc$descriptor_s name_dsc;

    assert(name);
    assert(id);
    assert(attrib);

    name_dsc.dsc$w_length = strlen(name);
    name_dsc.dsc$b_class = DSC$K_CLASS_S;
    name_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    name_dsc.dsc$a_pointer = name;

    return (sys$asctoid(&name_dsc, id, attrib));
}



unsigned int _getmsg(unsigned int msgid, char **msg, unsigned int flags,
		     unsigned int *outadr)
{
    char val[128];
    struct dsc$descriptor_s val_dsc;
    unsigned int status;
    unsigned short len;

    assert(msg);
    assert(outadr);

    val_dsc.dsc$w_length = sizeof(val) - 1;
    val_dsc.dsc$b_class = DSC$K_CLASS_S;
    val_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    val_dsc.dsc$a_pointer = val;

    status =
	sys$getmsg(msgid, &len, &val_dsc, flags, (unsigned char *) outadr);

    if (OKAY(status)) {
        val[len] = '\0';
        *msg = strdup(val);
        assert(*msg);
    } else {
        *msg = strdup(nil);
    	assert(*msg);
    }

    return (status);
}



unsigned int _idtoasc(unsigned int id, char **nambuf, unsigned int *resid,
		      unsigned int *attrib, unsigned int *ctxt)
{
    char val[128];
    struct dsc$descriptor_s val_dsc;
    unsigned int status;
    unsigned short len;

    assert(nambuf);
    assert(resid);
    assert(attrib);
    assert(ctxt);

    val_dsc.dsc$w_length = sizeof(val) - 1;
    val_dsc.dsc$b_class = DSC$K_CLASS_S;
    val_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    val_dsc.dsc$a_pointer = val;

    status = sys$idtoasc(id, &len, &val_dsc, resid, attrib, ctxt);

    if (OKAY(status)) {
        val[len] = '\0';
        *nambuf = strdup(val);
        assert(*nambuf);
    } else {
        *nambuf = strdup(nil);
	    assert(*nambuf);
    }

    return (status);
}



unsigned int _crembx(char prmflg, unsigned short int *chan,
		     unsigned int maxmsg, unsigned int bufquo,
		     unsigned int promsk, unsigned int acmode,
		     char *lognam, unsigned int flags)
{
    struct dsc$descriptor_s lnm_dsc;
    unsigned int status;

    assert(chan);

    if (lognam != NULL) {
	lnm_dsc.dsc$w_length = strlen(lognam);
	lnm_dsc.dsc$b_class = DSC$K_CLASS_S;
	lnm_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
	lnm_dsc.dsc$a_pointer = lognam;
    }

    status = sys$crembx(prmflg,
			chan,
			maxmsg,
			bufquo,
			promsk, acmode, lognam ? &lnm_dsc : NULL, flags,
			0);

    return (status);
}


unsigned int _delmbx(unsigned short int chan)
{
    return (sys$delmbx(chan));
}


unsigned int _getrmi(void *addr)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    unsigned long status;
    IOSB iosb;
    unsigned int efn;

    assert(obj);

    if (!(status = lib$get_ef(&efn))) {
	return (status);
    }

    if (!OKAY(status = sys$getrmi(efn, 0, 0, obj->list, &iosb, 0, 0))) {
	goto hell;
    }

    if (!OKAY(status = sys$synch(efn, &iosb))) {
	goto hell;
    }

    if (!OKAY(iosb.iosb$l_getxxi_status)) {
	status = iosb.iosb$l_getxxi_status;
	goto hell;
    }

  hell:
    lib$free_ef(&efn);
    return (status);
}



unsigned int _getuai(char *user, void *addr)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    struct dsc$descriptor_s user_dsc;

    assert(obj);
    assert(user);

    user_dsc.dsc$w_length = strlen(user);
    user_dsc.dsc$b_class = DSC$K_CLASS_S;
    user_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    user_dsc.dsc$a_pointer = user;

    return (sys$getuai(0, NULL, &user_dsc, obj->list, NULL, NULL, 0));
}


unsigned int _setuai(char *user, void *addr)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    struct dsc$descriptor_s user_dsc;

    assert(obj);
    assert(user);

    user_dsc.dsc$w_length = strlen(user);
    user_dsc.dsc$b_class = DSC$K_CLASS_S;
    user_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    user_dsc.dsc$a_pointer = user;

    return (sys$setuai(0, NULL, &user_dsc, obj->list, NULL, NULL, 0));
}



unsigned int _getqui(unsigned short func, int *context, void *addr)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    IOSB iosb;
    unsigned long status;

    // assert(obj);
    assert(context);

    status = sys$getquiw(EFN$C_ENF, func, (unsigned int *) context, obj ? obj->list : NULL, &iosb, NULL, 0);

    if (!OKAY(status)) {
        return (status);
    }

    if (!OKAY(iosb.iosb$l_getxxi_status)) {
        status = iosb.iosb$l_getxxi_status;
    }

    return (status);
}


unsigned int _getsyi(int *csidadr, char *node, void *addr)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    struct dsc$descriptor_s node_dsc;
    IOSB iosb;
    unsigned long status;

    assert(obj);

    if (node) {
        node_dsc.dsc$w_length = strlen(node);
        node_dsc.dsc$b_class = DSC$K_CLASS_S;
        node_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
        node_dsc.dsc$a_pointer = node;
    }

    status =
        sys$getsyiw(EFN$C_ENF, node ? NULL : (unsigned int *) csidadr,
                node ? &node_dsc : NULL, obj->list, &iosb, NULL, 0);

    if (!OKAY(status)) {
        return (status);
    }

    if (!OKAY(iosb.iosb$l_getxxi_status)) {
        status = iosb.iosb$l_getxxi_status;
    }

    return (status);
}



unsigned int _getjpi(int *pidadr, char *prcnam, void *addr)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    struct dsc$descriptor_s prcnam_dsc;
    IOSB iosb;
    unsigned long status;

    assert(obj);

    if (prcnam) {
        prcnam_dsc.dsc$w_length = strlen(prcnam);
        prcnam_dsc.dsc$b_class = DSC$K_CLASS_S;
        prcnam_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
        prcnam_dsc.dsc$a_pointer = prcnam;
    }

    status =
        sys$getjpiw(EFN$C_ENF, (unsigned int *) pidadr,
                prcnam ? &prcnam_dsc : NULL, obj->list, &iosb, NULL,
                0);

    if (!OKAY(status)) {
        return (status);
    }

    if (!OKAY(iosb.iosb$l_getxxi_status)) {
        status = iosb.iosb$l_getxxi_status;
    }

    return (status);
}


unsigned int _getdvi(char *dev, void *addr)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    struct dsc$descriptor_s dev_dsc;
    IOSB iosb;
    unsigned long status;

    assert(obj);
    assert(dev);

    dev_dsc.dsc$w_length = strlen(dev);
    dev_dsc.dsc$b_class = DSC$K_CLASS_S;
    dev_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    dev_dsc.dsc$a_pointer = dev;

    status = sys$getdviw(EFN$C_ENF, 0, &dev_dsc, obj->list, &iosb, NULL, 0, NULL);

    if (!OKAY(status)) {
        return (status);
    }

    if (!OKAY(iosb.iosb$l_getxxi_status)) {
        status = iosb.iosb$l_getxxi_status;
    }

    return (status);
}


unsigned int _device_scan(char **name, char *patt, void *addr,
			  unsigned long long *ctxt)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    char val[128];
    struct dsc$descriptor_s dev_dsc;
    struct dsc$descriptor_s ret_dsc;
    unsigned short len;
    unsigned long status;

    assert(name);
    assert(ctxt);
    assert(addr);

    if (patt) {
        dev_dsc.dsc$w_length = strlen(patt);
        dev_dsc.dsc$b_class = DSC$K_CLASS_S;
        dev_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
        dev_dsc.dsc$a_pointer = patt;
    }

    ret_dsc.dsc$w_length = sizeof(val) - 1;
    ret_dsc.dsc$b_class = DSC$K_CLASS_S;
    ret_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    ret_dsc.dsc$a_pointer = val;

    status =
        sys$device_scan(&ret_dsc, &len, patt ? &dev_dsc : NULL, obj->list,
                (struct _generic_64 *) ctxt);

    if (OKAY(status)) {
        val[len] = '\0';
        *name = strdup(val);
        assert(*name);
    } else {
        *name = strdup(nil);
        assert(*name);
    }

    return (status);
}

unsigned int _dellnm(char *tabnam, char *lognam, unsigned char acmode) {
    struct dsc$descriptor_s tabnam_dsc;
    struct dsc$descriptor_s lognam_dsc;
    assert(lognam);

    if (tabnam == NULL) {
        tabnam_dsc.dsc$w_length = strlen(DEF_TABNAM);
        tabnam_dsc.dsc$b_class = DSC$K_CLASS_S;
        tabnam_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
        tabnam_dsc.dsc$a_pointer = DEF_TABNAM;
    } else {
        tabnam_dsc.dsc$w_length = strlen(tabnam);
        tabnam_dsc.dsc$b_class = DSC$K_CLASS_S;
        tabnam_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
        tabnam_dsc.dsc$a_pointer = tabnam;
    }

    lognam_dsc.dsc$w_length = strlen(lognam);
    lognam_dsc.dsc$b_class = DSC$K_CLASS_S;
    lognam_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    lognam_dsc.dsc$a_pointer = lognam;

    return sys$dellnm(&tabnam_dsc, &lognam_dsc, &acmode);

}


unsigned int _trnlnm(unsigned int attr, char *tabnam, char *lognam,
                unsigned char acmode, void *addr)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    struct dsc$descriptor_s tabnam_dsc;
    struct dsc$descriptor_s lognam_dsc;

    assert(lognam);
    assert(addr);

    if (tabnam == NULL) {
        tabnam_dsc.dsc$w_length = strlen(DEF_TABNAM);
        tabnam_dsc.dsc$b_class = DSC$K_CLASS_S;
        tabnam_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
        tabnam_dsc.dsc$a_pointer = DEF_TABNAM;
    } else {
        tabnam_dsc.dsc$w_length = strlen(tabnam);
        tabnam_dsc.dsc$b_class = DSC$K_CLASS_S;
        tabnam_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
        tabnam_dsc.dsc$a_pointer = tabnam;
    }

    lognam_dsc.dsc$w_length = strlen(lognam);
    lognam_dsc.dsc$b_class = DSC$K_CLASS_S;
    lognam_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    lognam_dsc.dsc$a_pointer = lognam;

    return sys$trnlnm(&attr, &tabnam_dsc, &lognam_dsc, &acmode, obj->list);
}



unsigned int _forcex(int pid, char *prcnam, unsigned int code)
{
    struct dsc$descriptor_s prcnam_dsc;

    if (prcnam) {
	prcnam_dsc.dsc$w_length = strlen(prcnam);
	prcnam_dsc.dsc$b_class = DSC$K_CLASS_S;
	prcnam_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
	prcnam_dsc.dsc$a_pointer = prcnam;
    }

    return (sys$forcex
	    (pid <= 0 ? NULL : (unsigned int *) &pid,
	     prcnam ? &prcnam_dsc : NULL, code));
}



unsigned int _rem_ident(unsigned int id)
{
    return (sys$rem_ident(id));
}


unsigned int _add_ident(char *name, unsigned int id, unsigned int attrib,
			unsigned int *resid)
{
    struct dsc$descriptor_s name_dsc;
    assert(name);

    name_dsc.dsc$w_length = strlen(name);
    name_dsc.dsc$b_class = DSC$K_CLASS_S;
    name_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    name_dsc.dsc$a_pointer = name;

    return (sys$add_ident(&name_dsc, id, attrib, resid));
}


unsigned int _rem_holder(unsigned int id, unsigned long long holder)
{
    return (sys$rem_holder(id, (struct _generic_64 *) &holder));
}


unsigned int _add_holder(unsigned int id, unsigned long long holder,
			 unsigned int attrib)
{
    return (sys$add_holder(id, (struct _generic_64 *) &holder, attrib));
}



unsigned int _getlki(unsigned int *lkidadr, void *addr)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    IOSB iosb;
    unsigned long status;

    assert(lkidadr);
    assert(obj);

    status =
        sys$getlkiw(EFN$C_ENF, lkidadr, obj->list, &iosb,
                NULL, 0, 0);

    if (!OKAY(status)) {
        return (status);
    }

    if (!OKAY(iosb.iosb$l_getxxi_status)) {
        status = iosb.iosb$l_getxxi_status;
    }

    return (status);
}


unsigned int _uicstr(long int val, char **res, int flag)
{
    char str[32], fmt[16];
    struct dsc$descriptor_s str_dsc, fmt_dsc;
    unsigned short len;
    unsigned long status;
    char *tmp;

    assert(res);

    if (flag) {
        strcpy(fmt, "!%U");
    } else {
        strcpy(fmt, "!%I");
    }

    fmt_dsc.dsc$w_length = sizeof(fmt) - 1;
    fmt_dsc.dsc$b_class = DSC$K_CLASS_S;
    fmt_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    fmt_dsc.dsc$a_pointer = fmt;

    str_dsc.dsc$w_length = sizeof(str) - 1;
    str_dsc.dsc$b_class = DSC$K_CLASS_S;
    str_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    str_dsc.dsc$a_pointer = str;

    if (OKAY((status = sys$fao(&fmt_dsc, &len, &str_dsc, val)))) {
        str[len] = '\0';
        *res = strdup(str);
        assert(*res);
    } else {
        *res = strdup(nil);
    	assert(*res);
    }

    return (status);
}


unsigned int _gettim(long long *timadr)
{
    assert(timadr);
    return (sys$gettim((struct _generic_64 *) timadr));
}


unsigned int _crelnm(unsigned int attr, char *tabnam, char *lognam,
		     unsigned char acmode, void *addr)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    struct dsc$descriptor_s tabnam_dsc;
    struct dsc$descriptor_s lognam_dsc;

    assert(lognam);
    assert(addr);

    if (tabnam == NULL) {
        tabnam_dsc.dsc$w_length = strlen(DEF_TABNAM);
        tabnam_dsc.dsc$b_class = DSC$K_CLASS_S;
        tabnam_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
        tabnam_dsc.dsc$a_pointer = DEF_TABNAM;
    } else {
        tabnam_dsc.dsc$w_length = strlen(tabnam);
        tabnam_dsc.dsc$b_class = DSC$K_CLASS_S;
        tabnam_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
        tabnam_dsc.dsc$a_pointer = tabnam;
    }

    lognam_dsc.dsc$w_length = strlen(lognam);
    lognam_dsc.dsc$b_class = DSC$K_CLASS_S;
    lognam_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    lognam_dsc.dsc$a_pointer = lognam;

    return (sys$crelnm
	    (&attr, &tabnam_dsc, &lognam_dsc, &acmode, obj->list));
}




unsigned int _show_intrusion(char *criteria, char **intruder,
			     unsigned long long *expires,
			     unsigned int *type, unsigned int *count,
			     unsigned int flags, unsigned int *context)
{
    unsigned int status;
    struct dsc$descriptor_s criteria_dsc, intruder_dsc;
    char tmp[1059];
    unsigned short len;
    struct {
        unsigned int type;
        unsigned int count;
        unsigned long long expires;
    } blk;

    assert(criteria);
    assert(intruder);
    assert(expires);
    assert(type);
    assert(count);
    assert(context);

    criteria_dsc.dsc$w_length = strlen(criteria);
    criteria_dsc.dsc$b_class = DSC$K_CLASS_S;
    criteria_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    criteria_dsc.dsc$a_pointer = criteria;

    intruder_dsc.dsc$w_length = sizeof(tmp) - 1;
    intruder_dsc.dsc$b_class = DSC$K_CLASS_S;
    intruder_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    intruder_dsc.dsc$a_pointer = tmp;

    status = sys$show_intrusion(&criteria_dsc, &intruder_dsc, &len, &blk, flags, context);

    if (status == SS$_NORMAL) {
    	*expires = blk.expires;
    	*type = blk.type;
    	*count = blk.count;
    	tmp[len] = '\0';
        *intruder = strdup(tmp);
    	assert(*intruder);
    }

    return (status);
}

unsigned int _readvblk(unsigned short int chan, void *rbuffer, long long *rlen, unsigned short *iostatus, long long p3, unsigned int func_mod) {
    struct _iosb iosb;
    int status = SYS$QIOW(
        0,                          /* efn - event flag */
        chan,                       /* chan - channel number */
        IO$_READVBLK | func_mod,    /* func - function modifier */
        &iosb,                      /* iosb - I/O status block */
        0,                          /* astadr - AST routine */
        0,                          /* astprm - AST parameter */
        rbuffer,                    /* p1 - input buffer */
        *rlen,                      /* p2 - size of buffer */
        p3,                         /* starting vblock */
        0,0,0);                     /* p4-p6*/
    *rlen = iosb.iosb$w_bcnt;
    *iostatus = iosb.iosb$w_status;
    return status;
}

unsigned int _writevblk(unsigned short int chan, void *wbuffer, long long *wlen, unsigned short *iostatus, long long p3, unsigned int func_mod) {
    struct _iosb iosb;
    int status = SYS$QIOW(
        0,                          /* efn - event flag */
        chan,                       /* chan - channel number */
        IO$_WRITEVBLK | func_mod,   /* func - function modifier */
        &iosb,                      /* iosb - I/O status block */
        0,                          /* astadr - AST routine */
        0,                          /* astprm - AST parameter */
        wbuffer,                    /* p1 - input buffer */
        *wlen,                      /* p2 - size of buffer */
        p3,                         /* starting vblock */
        0,0,0);                     /* p4-p6*/
    *wlen = iosb.iosb$w_bcnt;
    *iostatus = iosb.iosb$w_status;
    return status;
}
