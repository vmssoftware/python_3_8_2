#include <stdio.h>
#include <str$routines.h>
#include <lib$routines.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ssdef.h>
#include <unixlib.h>
#include "dtr.h"
#include "lcl.h"


#ifndef dtr_Assert
#define dtr_Assert(x) \
    do { \
        if ((!(x))) { \
            fprintf(stderr, "Assertion failed: %s (%s: %d)\n", #x, __FILE__, __LINE__); \
            abort(); \
        } \
    } while (0)
#endif

#ifndef OKAY
#define OKAY(status) (((status) & 1) != 0)
#endif


extern void REC_load(void *, void *, unsigned short);
extern void REC_pack(void *, unsigned short, void *, unsigned short *);

#define MSG_BUF_LEN 255
#define AUX_BUF_LEN 255
#define TMP_BUF_LEN 255


unsigned int _command(unsigned int obj, char *str, int *condition, unsigned short *state)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    unsigned int status;
    struct dsc$descriptor_s str_dsc;

    dtr_Assert(dab);
    dtr_Assert(str);

    str_dsc.dsc$w_length = strlen(str);
    str_dsc.dsc$b_class = DSC$K_CLASS_S;
    str_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    str_dsc.dsc$a_pointer = str;

    status = dtr$command(dab, &str_dsc);

    if (condition) {
	*condition = dab->dab$l_condition;
    }

    if (state) {
	*state = dab->dab$w_state;
    }

    return (status);
}


unsigned int _continue(unsigned int obj, int *condition, unsigned short *state)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    unsigned int status;

    dtr_Assert(dab);
    status = dtr$continue(dab);

    if (condition) {
	*condition = dab->dab$l_condition;
    }

    if (state) {
	*state = dab->dab$w_state;
    }

    return (status);
}


unsigned int _create_udk(unsigned int obj, char *str, short index, short context)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    unsigned int status;
    struct dsc$descriptor_s str_dsc;

    dtr_Assert(dab);
    dtr_Assert(str);

    str_dsc.dsc$w_length = strlen(str);
    str_dsc.dsc$b_class = DSC$K_CLASS_S;
    str_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    str_dsc.dsc$a_pointer = str;

    status = dtr$create_udk(dab, &str_dsc, &index, &context);
    return (status);
}



unsigned int _end_udk(unsigned int obj)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    unsigned int status;

    dtr_Assert(dab);
    status = dtr$end_udk(dab);
    return (status);
}


unsigned int _finish(unsigned int obj)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    unsigned int status;

    dtr_Assert(dab);
    status = dtr$finish(dab);

    if (dab->dab$a_msg_buf) {
	free(dab->dab$a_msg_buf);
    }

    if (dab->dab$a_aux_buf) {
	free(dab->dab$a_aux_buf);
    }

    free(dab);
    return (status);
}


unsigned int _get_port(unsigned int obj, void *loc)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    unsigned int status;
    LCL_rec *rec = (LCL_rec *) loc;

    dtr_Assert(dab);
    dtr_Assert(rec);

    status = dtr$get_port(dab, rec->buf);

    if (OKAY(status)) {
	REC_load(rec, rec->buf, rec->len);
    }

    return (status);
}


unsigned int _get_string(unsigned int obj, int type, char **str, char *cmp)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    unsigned int status;
    char tmp[TMP_BUF_LEN + 1];
    unsigned short len;
    struct dsc$descriptor_s tmp_dsc;
    struct dsc$descriptor_s cmp_dsc;

    dtr_Assert(dab);
    dtr_Assert(str);

    if (cmp) {
	cmp_dsc.dsc$w_length = strlen(cmp);
	cmp_dsc.dsc$b_class = DSC$K_CLASS_S;
	cmp_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
	cmp_dsc.dsc$a_pointer = cmp;
    }

    tmp_dsc.dsc$w_length = TMP_BUF_LEN;
    tmp_dsc.dsc$b_class = DSC$K_CLASS_S;
    tmp_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    tmp_dsc.dsc$a_pointer = tmp;

    status =
	dtr$get_string(dab, (long *) &type, &tmp_dsc, &len,
		       cmp ? &cmp_dsc : NULL);

    if (OKAY(status)) {
	tmp[len] = '\0';
	dtr_Assert((*str = strdup(tmp)));
    }

    return (status);
}


unsigned int _info(unsigned int obj,
		   int id, char code, int *ret, char **str, int index)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    unsigned int status;
    char tmp[TMP_BUF_LEN + 1];
    unsigned short len;
    struct dsc$descriptor_s tmp_dsc;

    dtr_Assert(dab);
    dtr_Assert(ret);

    if (str) {
	tmp_dsc.dsc$w_length = TMP_BUF_LEN;
	tmp_dsc.dsc$b_class = DSC$K_CLASS_S;
	tmp_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
	tmp_dsc.dsc$a_pointer = tmp;
    }

    status =
	dtr$info(dab, (long *) &id, &code, (long *) ret,
		 str ? &tmp_dsc : NULL, index);

    if (OKAY(status)) {
	if (str) {
	    str$trim(&tmp_dsc, &tmp_dsc, &len);
	    tmp[len] = '\0';
	    dtr_Assert((*str = strdup(tmp)));
	}
    }

    return (status);
}


unsigned int _init(unsigned int *obj, int size, int options)
{
    DTR_access_block *dab = NULL;
    unsigned int status;

#ifdef __PYTHON
    int old = -1, idx = -1;
    if ((idx = decc$feature_get_index("DECC$UNIX_LEVEL")) >= 0) {
	old = decc$feature_set_value(idx, 1, 0);
    }
#endif

    dtr_Assert(obj);
    dtr_Assert((dab = malloc(sizeof(DTR_access_block))));

    dtr_Assert((dab->dab$a_msg_buf = malloc(MSG_BUF_LEN + 1)));
    dab->dab$w_msg_buf_len = MSG_BUF_LEN;

    dtr_Assert((dab->dab$a_aux_buf = malloc(MSG_BUF_LEN + 1)));
    dab->dab$w_aux_buf_len = MSG_BUF_LEN;

    status =
	dtr$init(dab, size ? &size : NULL, NULL, NULL,
		 options ? &options : NULL);

#ifdef __PYTHON
    if (idx >= 0) {
	decc$feature_set_value(idx, 1, old);
    }
#endif

    if (!OKAY(status)) {
	free(dab->dab$a_msg_buf);
	free(dab->dab$a_aux_buf);
	free(dab);
	return (status);
    }

    *obj = (unsigned int) dab;
    return (status);
}


unsigned int _lookup(unsigned int obj, char type, int *id, char *name)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    unsigned int status;
    struct dsc$descriptor_s name_dsc;

    dtr_Assert(dab);
    dtr_Assert(id);

    if (name) {
	name_dsc.dsc$w_length = strlen(name);
	name_dsc.dsc$b_class = DSC$K_CLASS_S;
	name_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
	name_dsc.dsc$a_pointer = name;
    }

    status = dtr$lookup(dab, &type, (long *) id, name ? &name_dsc : NULL);
    return (status);
}


unsigned int _port_eof(unsigned int obj)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    unsigned int status;

    dtr_Assert(dab);
    status = dtr$port_eof(dab);
    return (status);
}


unsigned int _put_port(unsigned int obj, void *loc)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    unsigned int status;
    LCL_rec *rec = (LCL_rec *) loc;
    unsigned short len;

    dtr_Assert(dab);
    dtr_Assert(rec);

    REC_pack(rec->buf, rec->len, loc, &len);
    status = dtr$put_port(dab, rec->buf);
    return (status);
}



unsigned int _put_value(unsigned int obj, char *val)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    unsigned int status;
    struct dsc$descriptor_s val_dsc;

    dtr_Assert(dab);

    if (val) {
	val_dsc.dsc$w_length = strlen(val);
	val_dsc.dsc$b_class = DSC$K_CLASS_S;
	val_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
	val_dsc.dsc$a_pointer = val;
    }

    status = dtr$put_value(dab, val ? &val_dsc : NULL);
    return (status);
}


unsigned int _unwind(unsigned int obj)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    dtr_Assert(dab);
    return (dtr$unwind(dab));
}


unsigned int _condition(unsigned int obj)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    dtr_Assert(dab);
    return (dab->dab$l_condition);
}


unsigned int _state(unsigned int obj)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    dtr_Assert(dab);
    return (dab->dab$w_state);
}


char *_msg_buf(unsigned int obj)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    unsigned short len;
    struct dsc$descriptor_s tmp_dsc;
    char *val;

    tmp_dsc.dsc$w_length = MSG_BUF_LEN;
    tmp_dsc.dsc$b_class = DSC$K_CLASS_S;
    tmp_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    tmp_dsc.dsc$a_pointer = dab->dab$a_msg_buf;

    str$trim(&tmp_dsc, &tmp_dsc, &len);

    dab->dab$a_msg_buf[len] = '\0';
    dtr_Assert((val = strdup(dab->dab$a_msg_buf)));
    return (val);
}


char *_aux_buf(unsigned int obj)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    unsigned short len;
    struct dsc$descriptor_s tmp_dsc;
    char *val;

    tmp_dsc.dsc$w_length = AUX_BUF_LEN;
    tmp_dsc.dsc$b_class = DSC$K_CLASS_S;
    tmp_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    tmp_dsc.dsc$a_pointer = dab->dab$a_aux_buf;

    str$trim(&tmp_dsc, &tmp_dsc, &len);

    dab->dab$a_aux_buf[len] = '\0';
    dtr_Assert((val = strdup(dab->dab$a_aux_buf)));
    return (val);
}


unsigned int _get_options(unsigned int obj)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    dtr_Assert(dab);
    return (dab->dab$l_options);
}


unsigned int _set_options(unsigned int obj, unsigned int options)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    dtr_Assert(dab);
    dab->dab$l_options = dab->dab$l_options | options;
    return (dab->dab$l_options);
}


unsigned int _clear_options(unsigned int obj, unsigned int options)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    dtr_Assert(dab);
    dab->dab$l_options = dab->dab$l_options & (~options);
    return (dab->dab$l_options);
}


unsigned int _get_rec_len(unsigned int obj)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    dtr_Assert(dab);
    return (dab->dab$w_rec_length);
}


unsigned int _get_flags(unsigned int obj)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    dtr_Assert(dab);
    return (dab->dab$l_flags);
}


unsigned int _get_udk_index(unsigned int obj)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    dtr_Assert(dab);
    return (dab->dab$w_udk_index);
}



unsigned int _dtr(unsigned int obj, int opt)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    dtr_Assert(dab);
    return (dtr$dtr(dab, (long *) &opt));
}


unsigned int _skip(unsigned int obj, unsigned int n)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    int i;
    unsigned int status = SS$_NORMAL;

    dtr_Assert(dab);

    for (i = 0; i < n; i++) {
    	if (dab->dab$w_state != DTR$K_STL_LINE) {
	    break;
	}

	status = dtr$continue(dab);

	if (! OKAY(status)) {
	    break;
	}
    }

    return (status);

}



extern int rsscanf(const char *buffer, const char *format, char ***argv);

char **_row(unsigned int obj, char *fmt)
{
    DTR_access_block *dab = (DTR_access_block *) obj;
    char **arr = NULL;

    dtr_Assert(dab);
    dtr_Assert(fmt);

    rsscanf(dab->dab$a_msg_buf, fmt, &arr);
    dtr$continue(dab);
    return (arr);
}
