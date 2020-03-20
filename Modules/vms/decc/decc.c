#define __NEW_STARLET 1
#include <stdio.h>
#include <string.h>
#include <lib$routines.h>
#include <starlet.h>
#include <descrip.h>
#include <ssdef.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#include "decc.h"

extern unsigned int decc$fix_time(long long *);

unsigned int _fix_time(long long vms_time)
{
    return (decc$fix_time(&vms_time));
}


static long long epoch = 0;

#define UNIX_EPOCH "01-JAN-1970 00:00:00.00"
#define FAC 10000000L

static int offset()
{
    time_t gmt, rawtime = time(NULL);
    struct tm *ptm;

    struct tm gbuf;
    ptm = gmtime_r(&rawtime, &gbuf);
    ptm->tm_isdst = -1;
    gmt = mktime(ptm);

    return ((int) difftime(rawtime, gmt));
}

unsigned int _unixtime(long long dt)
{
    unsigned int sec;
    __int64 diff;

    if (epoch == 0)  {
    	struct dsc$descriptor_s epoch_dsc;

    	epoch_dsc.dsc$w_length = strlen(UNIX_EPOCH);
    	epoch_dsc.dsc$b_class = DSC$K_CLASS_S;
    	epoch_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    	epoch_dsc.dsc$a_pointer = UNIX_EPOCH;

	sys$bintim(&epoch_dsc, (struct _generic_64 *) &epoch);
    }

    diff = dt - epoch;
    sec = diff / FAC - offset();
    return (sec);
}



long long _vmstime(unsigned int dt)
{
    long long val;
    long long tmp;

    if (epoch == 0)  {
    	struct dsc$descriptor_s epoch_dsc;

    	epoch_dsc.dsc$w_length = strlen(UNIX_EPOCH);
    	epoch_dsc.dsc$b_class = DSC$K_CLASS_S;
    	epoch_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    	epoch_dsc.dsc$a_pointer = UNIX_EPOCH;

	sys$bintim(&epoch_dsc, (struct _generic_64 *) &epoch);
    }

    tmp = dt;
    val = epoch + (tmp * FAC);

    return (val);
}


extern int decc$from_vms(const char *, int (*)(char *, void *), int, ...);
extern int decc$to_vms(const char *, int (*)(char *, int, void *), int, int, ...);


static int cb_from_vms(char *name, void *ud)
{
    char **tmp = (char **) ud;
    *tmp = strdup(name);
    assert(*tmp);
    return (1);
}


char *_from_vms(char *path)
{
    char *tmp = NULL;

    assert(path);
    decc$from_vms(path, cb_from_vms, 0, &tmp);
    return (tmp);
}


static int cb_to_vms(char *name, int flag, void *ud)
{
    char **tmp = (char **) ud;
    *tmp = strdup(name);
    assert((*tmp));
    return (1);
}

char *_to_vms(char *path)
{
    char *tmp = NULL;

    assert(path);
    decc$to_vms(path, cb_to_vms, 0, 0, &tmp);
    return (tmp);
}


char *_getenv(char *name, char *def)
{
    char *tmp, *val = NULL;

    assert(name);
    val = getenv(name);

    if (val == NULL && def != NULL) {
       val = def;
    }

    if (val) {
        tmp = strdup(val);
	    assert(tmp);
    } else {
	    tmp = NULL;
    }

    return (tmp);
}


long _sysconf(int name)
{
    return (sysconf(name));
}


int _sleep(unsigned int nsec)
{
    return (sleep(nsec));
}

