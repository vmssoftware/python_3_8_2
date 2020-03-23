#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <descrip.h>
#include <lib$routines.h>
#include <ssdef.h>
#include <assert.h>

#include "lib.h"

#ifndef OKAY
#define OKAY(STATUS) (((STATUS) & 1) != 0)
#endif


static const char *nil = "";

unsigned int _date_time(char **dt)
{
    char val[32];
    struct dsc$descriptor_s val_dsc;
    unsigned int status;

    assert(dt);

    val_dsc.dsc$w_length = sizeof(val) - 1;
    val_dsc.dsc$b_class = DSC$K_CLASS_S;
    val_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    val_dsc.dsc$a_pointer = val;

    status = lib$date_time(&val_dsc);

    if (OKAY(status)) {
	val[23] = '\0';
	assert((*dt = strdup(val)));
    } else {
	assert((*dt = strdup(nil)));
    }

    return (status);
}


unsigned int _free_ef(unsigned int efn)
{
    return (lib$free_ef(&efn));
}


unsigned int _get_ef(unsigned int *efn)
{
    assert(efn);
    return (lib$get_ef(efn));
}


unsigned int _get_hostname(char **hostname, unsigned int flags)
{
    char val[256];
    struct dsc$descriptor_s val_dsc;
    unsigned int status;
    unsigned short len;

    assert(hostname);

    val_dsc.dsc$w_length = sizeof(val) - 1;
    val_dsc.dsc$b_class = DSC$K_CLASS_S;
    val_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    val_dsc.dsc$a_pointer = val;

    status = lib$get_hostname(&val_dsc, &len, flags);

    if (OKAY(status)) {
	val[len] = '\0';
	assert((*hostname = strdup(val)));
    } else {
	assert((*hostname = strdup(nil)));
    }

    return (status);
}


unsigned int _getjpi(int item, unsigned int *pid, char *prn, char **ret)
{
    char val[256];
    struct dsc$descriptor_s prn_dsc;
    struct dsc$descriptor_s val_dsc;
    unsigned int status;
    unsigned short len;

    assert(ret);

    if (prn) {
	prn_dsc.dsc$w_length = strlen(prn);
	prn_dsc.dsc$b_class = DSC$K_CLASS_S;
	prn_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
	prn_dsc.dsc$a_pointer = prn;
    }

    val_dsc.dsc$w_length = sizeof(val) - 1;
    val_dsc.dsc$b_class = DSC$K_CLASS_S;
    val_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    val_dsc.dsc$a_pointer = val;

    status =
	lib$getjpi(&item, pid, (prn ? &prn_dsc : NULL), 0, &val_dsc, &len);

    if (OKAY(status)) {
	val[len] = '\0';
	assert((*ret = strdup(val)));
    } else {
	assert((*ret = strdup(nil)));
    }

    return (status);
}



unsigned int _getsyi(int item, char **ret, unsigned int *csid, char *node)
{
    char val[256];
    struct dsc$descriptor_s node_dsc, val_dsc;
    unsigned int status;
    unsigned short len;

    assert(ret);
    assert(csid);

    val_dsc.dsc$w_length = sizeof(val) - 1;
    val_dsc.dsc$b_class = DSC$K_CLASS_S;
    val_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    val_dsc.dsc$a_pointer = val;

    if (node) {
	node_dsc.dsc$w_length = strlen(node);
	node_dsc.dsc$b_class = DSC$K_CLASS_S;
	node_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
	node_dsc.dsc$a_pointer = node;
    }

    status =
	lib$getsyi(&item, NULL, &val_dsc, &len, csid,
		   (node ? &node_dsc : NULL));

    if (OKAY(status)) {
	val[len] = '\0';
	assert((*ret = strdup(val)));
    } else {
	assert((*ret = strdup(nil)));
    }

    return (status);
}


unsigned int _spawn(char *cmd, char *in, char *out, unsigned int flags,
		    char *prn, unsigned int *pid)
{
    struct dsc$descriptor_s cmd_dsc, in_dsc, out_dsc, prn_dsc;
    unsigned int status;

    if (cmd) {
	cmd_dsc.dsc$w_length = strlen(cmd);
	cmd_dsc.dsc$b_class = DSC$K_CLASS_S;
	cmd_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
	cmd_dsc.dsc$a_pointer = cmd;
    }

    if (in) {
	in_dsc.dsc$w_length = strlen(in);
	in_dsc.dsc$b_class = DSC$K_CLASS_S;
	in_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
	in_dsc.dsc$a_pointer = in;
    }

    if (out) {
	out_dsc.dsc$w_length = strlen(out);
	out_dsc.dsc$b_class = DSC$K_CLASS_S;
	out_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
	out_dsc.dsc$a_pointer = out;
    }

    if (prn) {
	prn_dsc.dsc$w_length = strlen(prn);
	prn_dsc.dsc$b_class = DSC$K_CLASS_S;
	prn_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
	prn_dsc.dsc$a_pointer = prn;
    }

    if (pid) {
	*pid = 0;
    }

    status = lib$spawn((cmd ? &cmd_dsc : NULL),
		       (in ? &in_dsc : NULL),
		       (out ? &out_dsc : NULL),
		       &flags,
		       (prn ? &prn_dsc : NULL),
		       pid, NULL, 0, NULL, NULL, NULL, NULL, NULL);

    return (status);
}


unsigned int _do_command(char *cmd)
{
    struct dsc$descriptor_s cmd_dsc;
    assert(cmd);

    cmd_dsc.dsc$w_length = strlen(cmd);
    cmd_dsc.dsc$b_class = DSC$K_CLASS_S;
    cmd_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    cmd_dsc.dsc$a_pointer = cmd;

    return (lib$do_command(&cmd_dsc));
}



unsigned int _put_common(char *str)
{
    struct dsc$descriptor_s str_dsc;

    assert(str);

    str_dsc.dsc$w_length = strlen(str);
    str_dsc.dsc$b_class = DSC$K_CLASS_S;
    str_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    str_dsc.dsc$a_pointer = str;

    return (lib$put_common(&str_dsc));
}


unsigned int _get_common(char **str)
{
    struct dsc$descriptor_s str_dsc;
    unsigned int status;
    char *tmp;
    unsigned short len;

    assert(str);

    tmp = malloc(252);
    assert(tmp);

    str_dsc.dsc$w_length = 252;
    str_dsc.dsc$b_class = DSC$K_CLASS_S;
    str_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    str_dsc.dsc$a_pointer = tmp;

    status = lib$get_common(&str_dsc, &len);

    if (OKAY(status)) {
	tmp[len] = '\0';
    } else {
	free(tmp);
	tmp = NULL;
    }

    *str = tmp;
    return (status);
}



unsigned int _create_dir(char *spec, unsigned int uic, unsigned short pe, unsigned short pv)
{
    struct dsc$descriptor_s spec_dsc;
    assert(spec);

    spec_dsc.dsc$w_length = strlen(spec);
    spec_dsc.dsc$b_class = DSC$K_CLASS_S;
    spec_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    spec_dsc.dsc$a_pointer = spec;

    return (lib$create_dir(&spec_dsc, &uic, &pe, &pv, NULL, NULL, NULL));
}

