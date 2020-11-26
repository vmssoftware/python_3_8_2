#define __NEW_STARLET 1

#include <descrip.h>
#include <lib$routines.h>
#include <starlet.h>
#include <ssdef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "lcl.h"

#ifndef rec_Assert
#define rec_Assert(x) \
    do { \
        if ((!((x)))) { \
            fprintf (stderr, "Assertion failed: %s (%s: %d)\n", #x, __FILE__, __LINE__); \
            abort(); \
        } \
    } while (0)
#endif

#ifndef OKAY
#define OKAY(STATUS) (((STATUS) & 1) != 0)
#endif



void *_new()
{
    LCL_rec *rec = NULL;

    rec_Assert((rec = calloc(1, sizeof(LCL_rec))));
    rec->ncol = 0;
    rec->fields = NULL;

    rec->len = 0;
    rec->buf = NULL;

    return (rec);
}


void _delete(void *addr)
{
    LCL_rec *rec = (LCL_rec *) addr;
    int i;

    rec_Assert(rec);

    if (rec->fields) {
	for (i = 0; i < rec->ncol; i++) {
	    if (rec->fields[i].name) {
		free(rec->fields[i].name);
	    }
	    if (rec->fields[i].val) {
		free(rec->fields[i].val);
	    }
	}
	free(rec->fields);
    }

    if (rec->buf) {
	free(rec->buf);
    }

    free(rec);
}


char *_getstr(void *addr, int idx)
{
    LCL_rec *rec = (LCL_rec *) addr;
    char *tmp;
    int i;
    unsigned short size;

    rec_Assert((rec->ncol));
    rec_Assert((idx < rec->ncol));
    rec_Assert((rec->fields));
    rec_Assert((rec->fields[idx].type == DSC$K_DTYPE_T));

    size = rec->fields[idx].size;
    rec_Assert((tmp = calloc((size + 1), sizeof(char))));
    memcpy(tmp, rec->fields[idx].val, size);

    i = rec->fields[idx].size - 1;

    while (i > 0 && isspace(tmp[i])) {
	i--;
    }
    i++;
    tmp[i] = '\0';

    return (tmp);
}


char *_gethex(void *addr, int idx)
{
    LCL_rec *rec = (LCL_rec *) addr;
    char tmp[32], *val;
    unsigned short type;

    rec_Assert((rec->ncol));
    rec_Assert((idx < rec->ncol));
    rec_Assert((rec->fields));

    val = rec->fields[idx].val;
    type = rec->fields[idx].type;

    switch (type) {
    case DSC$K_DTYPE_BU:
	sprintf(tmp, "0x%llx", *(unsigned char *) val);
	break;
    case DSC$K_DTYPE_B:
	sprintf(tmp, "0x%llx", *(char *) val);
	break;
    case DSC$K_DTYPE_WU:
	sprintf(tmp, "0x%llx", *(unsigned short *) val);
	break;
    case DSC$K_DTYPE_W:
	sprintf(tmp, "0x%llx", *(short *) val);
	break;
    case DSC$K_DTYPE_LU:
	sprintf(tmp, "0x%llx", *(unsigned int *) val);
	break;
    case DSC$K_DTYPE_L:
	sprintf(tmp, "0x%llx", *(int *) val);
	break;
    case DSC$K_DTYPE_QU:
	sprintf(tmp, "0x%llx", *(unsigned long long *) val);
	break;
    case DSC$K_DTYPE_Q:
	sprintf(tmp, "0x%llx", *(long long *) val);
	break;
    default:
	abort();
	break;
    }

    rec_Assert(val = strdup(tmp));
    return (val);
}


long long _getint(void *addr, int idx)
{
    LCL_rec *rec = (LCL_rec *) addr;
    void *val;
    unsigned short type;

    rec_Assert((rec->ncol));
    rec_Assert((idx < rec->ncol));
    rec_Assert((rec->fields));

    val = rec->fields[idx].val;
    type = rec->fields[idx].type;

    switch (type) {
    case DSC$K_DTYPE_BU:
	return (*(unsigned char *) val);
	break;
    case DSC$K_DTYPE_B:
	return (*(char *) val);
	break;
    case DSC$K_DTYPE_WU:
	return (*(unsigned short *) val);
	break;
    case DSC$K_DTYPE_W:
	return (*(short *) val);
	break;
    case DSC$K_DTYPE_LU:
	return (*(unsigned int *) val);
	break;
    case DSC$K_DTYPE_L:
	return (*(int *) val);
	break;
    case DSC$K_DTYPE_QU:
    case DSC$K_DTYPE_Q:
	return (*(long long *) val);
	break;
    default:
	abort();
	break;
    }

    return (0);
}


double _getnum(void *addr, int idx)
{
    LCL_rec *rec = (LCL_rec *) addr;
    struct dsc$descriptor_sd from, to;
    double tmp;
    unsigned short type;

    rec_Assert((rec->ncol));
    rec_Assert((idx < rec->ncol));
    rec_Assert((rec->fields));

    type = rec->fields[idx].type;

    switch (type) {
    case DSC$K_DTYPE_NU:
    case DSC$K_DTYPE_NL:
    case DSC$K_DTYPE_NLO:
    case DSC$K_DTYPE_NR:
    case DSC$K_DTYPE_NRO:
    case DSC$K_DTYPE_NZ:
    case DSC$K_DTYPE_P:
	from.dsc$b_class = DSC$K_CLASS_SD;
	from.dsc$w_length = rec->fields[idx].size;
	from.dsc$a_pointer = (char *) rec->fields[idx].val;
	from.dsc$b_scale = rec->fields[idx].scale;
	from.dsc$b_digits = rec->fields[idx].digits;
	from.dsc$b_dtype = type;

	to.dsc$b_class = DSC$K_CLASS_SD;
	to.dsc$w_length = sizeof(double);
	to.dsc$a_pointer = (char *) &tmp;
	to.dsc$b_scale = 0;
	to.dsc$b_digits = 0;
	to.dsc$b_dtype = DSC$K_DTYPE_FT;

	rec_Assert((OKAY
		(lib$cvt_dx_dx
		 ((unsigned int *) &from, (unsigned int *) &to))));
	return (tmp);
	break;
    default:
	abort();
	break;
    }

    return (0.0);
}



double _getfloat(void *addr, int idx)
{
    LCL_rec *rec = (LCL_rec *) addr;
    void *val;
    unsigned short type;

    rec_Assert((rec->ncol));
    rec_Assert((idx < rec->ncol));
    rec_Assert((rec->fields));

    val = rec->fields[idx].val;
    type = rec->fields[idx].type;

    switch (type) {
    case DSC$K_DTYPE_F:
	{
	    struct dsc$descriptor_sd from, to;
	    double tmp;

	    from.dsc$b_class = DSC$K_CLASS_SD;
	    from.dsc$w_length = sizeof(float);
	    from.dsc$a_pointer = (char *) val;
	    from.dsc$b_scale = 0;
	    from.dsc$b_digits = 0;
	    from.dsc$b_dtype = DSC$K_DTYPE_F;

	    to.dsc$b_class = DSC$K_CLASS_SD;
	    to.dsc$w_length = sizeof(double);
	    to.dsc$a_pointer = (char *) &tmp;
	    to.dsc$b_scale = 0;
	    to.dsc$b_digits = 0;
	    to.dsc$b_dtype = DSC$K_DTYPE_FT;

	    rec_Assert((OKAY
		    (lib$cvt_dx_dx
		     ((unsigned int *) &from, (unsigned int *) &to))));
	    return (tmp);
	}
	break;
    case DSC$K_DTYPE_FS:
	return (*(float *) val);
	break;
    case DSC$K_DTYPE_G:
	{
	    struct dsc$descriptor_sd from, to;
	    double tmp;

	    from.dsc$b_class = DSC$K_CLASS_SD;
	    from.dsc$w_length = sizeof(double);
	    from.dsc$a_pointer = (char *) val;
	    from.dsc$b_scale = 0;
	    from.dsc$b_digits = 0;
	    from.dsc$b_dtype = DSC$K_DTYPE_G;

	    to.dsc$b_class = DSC$K_CLASS_SD;
	    to.dsc$w_length = sizeof(double);
	    to.dsc$a_pointer = (char *) &tmp;
	    to.dsc$b_scale = 0;
	    to.dsc$b_digits = 0;
	    to.dsc$b_dtype = DSC$K_DTYPE_FT;

	    rec_Assert((OKAY
		    (lib$cvt_dx_dx
		     ((unsigned int *) &from, (unsigned int *) &to))));
	    return (tmp);
	}
	break;
    case DSC$K_DTYPE_FT:
	return (*(double *) val);
	break;
    default:
	abort();
	break;
    }

    return (0.0);
}


void _setstr(void *addr, char *val, int idx)
{
    LCL_rec *rec = (LCL_rec *) addr;
    unsigned short len;

    rec_Assert(rec);
    rec_Assert(idx < rec->ncol);
    rec_Assert((rec->fields[idx].type == DSC$K_DTYPE_T));

    memset(rec->fields[idx].val, ' ', rec->fields[idx].size);

    if (val) {
	len = strlen(val);
	rec_Assert((len <= rec->fields[idx].size));
	memcpy(rec->fields[idx].val, val, len);
    }
}


void _addstr(void *addr, char *val, unsigned short len, unsigned short off)
{
    LCL_rec *rec = (LCL_rec *) addr;
    int n;
    int idx;

    rec_Assert(rec);

    if (val) {
	rec_Assert(((n = strlen(val)) <= len));
    }

    idx = rec->ncol;
    rec->ncol++;
    rec_Assert((rec->fields =
	    realloc(rec->fields, sizeof(rec->fields[0]) * rec->ncol)));
    rec_Assert((rec->fields[idx].val = malloc(len)));
    memset(rec->fields[idx].val, ' ', len);

    if (val) {
	memcpy(rec->fields[idx].val, val, n);
    }

    rec->fields[idx].type = DSC$K_DTYPE_T;
    rec->fields[idx].off = off;
    rec->fields[idx].idx = idx;
    rec->fields[idx].size = len;
    rec->fields[idx].digits = -1;
    rec->fields[idx].scale = -1;
    rec->fields[idx].name = NULL;

    rec->len += len;
    rec_Assert((rec->buf = realloc(rec->buf, rec->len)));
}


void _setint(void *addr, long long val, int idx)
{
    LCL_rec *rec = (LCL_rec *) addr;
    int type;
    int size;

    rec_Assert(rec);
    rec_Assert(idx < rec->ncol);

    type = rec->fields[idx].type;
    size = rec->fields[idx].size;

    switch (type) {
    case DSC$K_DTYPE_BU:
	{
	    unsigned char tmp = (unsigned char) val;
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_B:
	{
	    char tmp = (char) val;
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_WU:
	{
	    unsigned short tmp = (unsigned short) val;
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_W:
	{
	    short tmp = (short) val;
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_LU:
	{
	    unsigned long tmp = (unsigned long) val;
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_L:
	{
	    long tmp = (long) val;
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_QU:
    case DSC$K_DTYPE_Q:
	memcpy(rec->fields[idx].val, &val, size);
	break;
    default:
	abort();
	break;
    }
}



void _addint(void *addr, int type, long long val, unsigned short off)
{
    LCL_rec *rec = (LCL_rec *) addr;
    int size;
    int idx;

    rec_Assert(rec);

    idx = rec->ncol;
    rec->ncol++;
    rec_Assert((rec->fields =
	    realloc(rec->fields, sizeof(rec->fields[0]) * rec->ncol)));

    switch (type) {
    case DSC$K_DTYPE_BU:
    case DSC$K_DTYPE_B:
	size = 1;
	break;
    case DSC$K_DTYPE_WU:
    case DSC$K_DTYPE_W:
	size = 2;
	break;
    case DSC$K_DTYPE_LU:
    case DSC$K_DTYPE_L:
	size = 4;
	break;
    case DSC$K_DTYPE_QU:
    case DSC$K_DTYPE_Q:
	size = 8;
	break;
    default:
	abort();
	break;
    }

    rec_Assert((rec->fields[idx].val = calloc(1, size)));

    switch (type) {
    case DSC$K_DTYPE_BU:
	{
	    unsigned char tmp = (unsigned char) val;
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_B:
	{
	    char tmp = (char) val;
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_WU:
	{
	    unsigned short tmp = (unsigned short) val;
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_W:
	{
	    short tmp = (short) val;
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_LU:
	{
	    unsigned long tmp = (unsigned long) val;
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_L:
	{
	    long tmp = (long) val;
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_QU:
    case DSC$K_DTYPE_Q:
     	memcpy(rec->fields[idx].val, &val, size);
	break;
    default:
	abort();
	break;
    }

    rec->fields[idx].size = size;
    rec->fields[idx].type = type;
    rec->fields[idx].off = off;
    rec->fields[idx].idx = idx;
    rec->fields[idx].digits = -1;
    rec->fields[idx].scale = -1;
    rec->fields[idx].name = NULL;

    rec->len += size;
    rec_Assert((rec->buf = realloc(rec->buf, rec->len)));
}



void _setnum(void *addr, double val, int idx)
{
    LCL_rec *rec = (LCL_rec *) addr;
    struct dsc$descriptor_sd from, to;

    rec_Assert(rec);
    rec_Assert(idx < rec->ncol);

    switch (rec->fields[idx].type) {
    case DSC$K_DTYPE_NU:
    case DSC$K_DTYPE_NL:
    case DSC$K_DTYPE_NLO:
    case DSC$K_DTYPE_NR:
    case DSC$K_DTYPE_NRO:
    case DSC$K_DTYPE_NZ:
    case DSC$K_DTYPE_P:
	break;
    default:
	abort();
	break;
    }

    from.dsc$b_class = DSC$K_CLASS_SD;
    from.dsc$w_length = sizeof(double);
    from.dsc$a_pointer = (char *) &val;
    from.dsc$b_scale = 0;
    from.dsc$b_digits = 0;
    from.dsc$b_dtype = DSC$K_DTYPE_FT;

    to.dsc$b_class = DSC$K_CLASS_SD;
    to.dsc$w_length = rec->fields[idx].size;
    to.dsc$a_pointer = rec->fields[idx].val;
    to.dsc$b_scale = rec->fields[idx].scale;
    to.dsc$b_digits = rec->fields[idx].digits;
    to.dsc$b_dtype = rec->fields[idx].type;

    rec_Assert((OKAY
	    (lib$cvt_dx_dx
	     ((unsigned int *) &from, (unsigned int *) &to))));
}



void _addnum(void *addr, int type, double val, short scale,
	     unsigned short digits, unsigned short len, unsigned short off)
{
    LCL_rec *rec = (LCL_rec *) addr;
    struct dsc$descriptor_sd from, to;
    int idx;

    rec_Assert(rec);

    switch (type) {
    case DSC$K_DTYPE_NU:
    case DSC$K_DTYPE_NL:
    case DSC$K_DTYPE_NLO:
    case DSC$K_DTYPE_NR:
    case DSC$K_DTYPE_NRO:
    case DSC$K_DTYPE_NZ:
    case DSC$K_DTYPE_P:
	break;
    default:
	abort();
	break;
    }

    idx = rec->ncol;
    rec->ncol++;
    rec_Assert((rec->fields =
	    realloc(rec->fields, sizeof(rec->fields[0]) * rec->ncol)));

    if (type == DSC$K_DTYPE_P) {
	len = digits;
    }

    rec_Assert((rec->fields[idx].val = calloc(1, len)));

    from.dsc$b_class = DSC$K_CLASS_SD;
    from.dsc$w_length = sizeof(double);
    from.dsc$a_pointer = (char *) &val;
    from.dsc$b_scale = 0;
    from.dsc$b_digits = 0;
    from.dsc$b_dtype = DSC$K_DTYPE_FT;

    to.dsc$b_class = DSC$K_CLASS_SD;
    to.dsc$w_length = len;
    to.dsc$a_pointer = rec->fields[idx].val;
    to.dsc$b_scale = scale;
    to.dsc$b_digits = digits;
    to.dsc$b_dtype = type;

    rec_Assert((OKAY
	    (lib$cvt_dx_dx
	     ((unsigned int *) &from, (unsigned int *) &to))));

    rec->fields[idx].size = len;
    rec->fields[idx].type = type;
    rec->fields[idx].off = off;
    rec->fields[idx].idx = idx;
    rec->fields[idx].digits = digits;
    rec->fields[idx].scale = scale;
    rec->fields[idx].name = NULL;

    rec->len += len;
    rec_Assert((rec->buf = realloc(rec->buf, rec->len)));
}



void _setfloat(void *addr, double val, int idx)
{
    LCL_rec *rec = (LCL_rec *) addr;
    struct dsc$descriptor_sd from, to;

    rec_Assert(rec);
    rec_Assert(idx < rec->ncol);

    switch (rec->fields[idx].type) {
    case DSC$K_DTYPE_F:
    case DSC$K_DTYPE_FS:
    case DSC$K_DTYPE_G:
    case DSC$K_DTYPE_FT:
	break;
    default:
	abort();
	break;
    }

    from.dsc$b_class = DSC$K_CLASS_SD;
    from.dsc$w_length = sizeof(double);
    from.dsc$a_pointer = (char *) &val;
    from.dsc$b_scale = 0;
    from.dsc$b_digits = 0;
    from.dsc$b_dtype = DSC$K_DTYPE_FT;

    to.dsc$b_class = DSC$K_CLASS_SD;
    to.dsc$w_length = rec->fields[idx].size;
    to.dsc$a_pointer = rec->fields[idx].val;
    to.dsc$b_scale = 0;
    to.dsc$b_digits = 0;
    to.dsc$b_dtype = rec->fields[idx].type;

    rec_Assert((OKAY
	    (lib$cvt_dx_dx
	     ((unsigned int *) &from, (unsigned int *) &to))));
}


void _addfloat(void *addr, int type, double val, unsigned short off)
{
    LCL_rec *rec = (LCL_rec *) addr;
    int size;
    int idx;

    rec_Assert(rec);

    idx = rec->ncol;
    rec->ncol++;
    rec_Assert((rec->fields =
	    realloc(rec->fields, sizeof(rec->fields[0]) * rec->ncol)));

    switch (type) {
    case DSC$K_DTYPE_F:
    case DSC$K_DTYPE_FS:
	size = 4;
	break;
    case DSC$K_DTYPE_G:
    case DSC$K_DTYPE_FT:
	size = 8;
	break;
    default:
	abort();
	break;
    }

    rec_Assert((rec->fields[idx].val = calloc(1, size)));

    switch (type) {
    case DSC$K_DTYPE_F:
	{
	    struct dsc$descriptor_sd from, to;
	    float tmp;

	    from.dsc$b_class = DSC$K_CLASS_SD;
	    from.dsc$w_length = sizeof(double);
	    from.dsc$a_pointer = (char *) &val;
	    from.dsc$b_scale = 0;
	    from.dsc$b_digits = 0;
	    from.dsc$b_dtype = DSC$K_DTYPE_FT;

	    to.dsc$b_class = DSC$K_CLASS_SD;
	    to.dsc$w_length = sizeof(float);
	    to.dsc$a_pointer = (char *) &tmp;
	    to.dsc$b_scale = 0;
	    to.dsc$b_digits = 0;
	    to.dsc$b_dtype = DSC$K_DTYPE_F;

	    rec_Assert((OKAY
		    (lib$cvt_dx_dx
		     ((unsigned int *) &from, (unsigned int *) &to))));
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_FS:
	{
	    float tmp = (float) val;
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_G:	// IEEE double to G float
	{
	    struct dsc$descriptor_sd from, to;
	    double tmp;

	    from.dsc$b_class = DSC$K_CLASS_SD;
	    from.dsc$w_length = sizeof(double);
	    from.dsc$a_pointer = (char *) &val;
	    from.dsc$b_scale = 0;
	    from.dsc$b_digits = 0;
	    from.dsc$b_dtype = DSC$K_DTYPE_FT;

	    to.dsc$b_class = DSC$K_CLASS_SD;
	    to.dsc$w_length = sizeof(double);
	    to.dsc$a_pointer = (char *) &tmp;
	    to.dsc$b_scale = 0;
	    to.dsc$b_digits = 0;
	    to.dsc$b_dtype = DSC$K_DTYPE_G;

	    rec_Assert((OKAY
		    (lib$cvt_dx_dx
		     ((unsigned int *) &from, (unsigned int *) &to))));
	    memcpy(rec->fields[idx].val, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_FT:
	{
	    memcpy(rec->fields[idx].val, &val, size);
	}
	break;
    default:
	abort();
	break;
    }

    rec->fields[idx].type = type;
    rec->fields[idx].off = off;
    rec->fields[idx].idx = idx;
    rec->fields[idx].digits = -1;
    rec->fields[idx].scale = -1;
    rec->fields[idx].name = NULL;

    rec->len += size;
    rec_Assert((rec->buf = realloc(rec->buf, rec->len)));
}


void _load(void *addr, void *data, unsigned short len)
{
    LCL_rec *rec = (LCL_rec *) addr;
    char *loc;
    char *end;
    unsigned short size;
    int i;

    rec_Assert(rec);
    rec_Assert(data);

    end = (char *) data + len;

    for (i = 0; i < rec->ncol; i++) {
	loc = (char *) data + rec->fields[i].off;
	switch (rec->fields[i].type) {
	case DSC$K_DTYPE_T:
	    size = rec->fields[i].size;
	    rec_Assert((loc + size) <= end);
	    memcpy(rec->fields[i].val, loc, size);
	    break;
	case DSC$K_DTYPE_BU:
	case DSC$K_DTYPE_B:
	    rec_Assert((size = rec->fields[i].size) == 1);
	    rec_Assert((loc + size) <= end);
	    memcpy(rec->fields[i].val, loc, size);
	    break;
	case DSC$K_DTYPE_WU:
	case DSC$K_DTYPE_W:
	    rec_Assert((size = rec->fields[i].size) == 2);
	    rec_Assert((loc + size) <= end);
	    memcpy(rec->fields[i].val, loc, size);
	    break;
	case DSC$K_DTYPE_LU:
	case DSC$K_DTYPE_L:
	    rec_Assert((size = rec->fields[i].size) == 4);
	    rec_Assert((loc + size) <= end);
	    memcpy(rec->fields[i].val, loc, size);
	    break;
	case DSC$K_DTYPE_QU:
	case DSC$K_DTYPE_Q:
	    rec_Assert((size = rec->fields[i].size) == 8);
	    rec_Assert((loc + size) <= end);
	    memcpy(rec->fields[i].val, loc, size);
	    break;
	case DSC$K_DTYPE_F:
	    {
		struct dsc$descriptor_sd from, to;
		rec_Assert((size = rec->fields[i].size) == 4);

		from.dsc$b_class = DSC$K_CLASS_SD;
		from.dsc$w_length = sizeof(float);
		from.dsc$a_pointer = (char *) loc;
		from.dsc$b_scale = 0;
		from.dsc$b_digits = 0;
		from.dsc$b_dtype = DSC$K_DTYPE_F;

		to.dsc$b_class = DSC$K_CLASS_SD;
		to.dsc$w_length = sizeof(float);
		to.dsc$a_pointer = (char *) &rec->fields[i].val;
		to.dsc$b_scale = 0;
		to.dsc$b_digits = 0;
		to.dsc$b_dtype = DSC$K_DTYPE_FS;

		rec_Assert((OKAY
			(lib$cvt_dx_dx
			 ((unsigned int *) &from, (unsigned int *) &to))));
	    }
	    break;
	case DSC$K_DTYPE_FS:
	    rec_Assert((size = rec->fields[i].size) == 4);
	    rec_Assert((loc + size) <= end);
	    memcpy(rec->fields[i].val, loc, size);
	    break;
	case DSC$K_DTYPE_G:
	    {
		struct dsc$descriptor_sd from, to;
		rec_Assert((size = rec->fields[i].size) == 8);

		from.dsc$b_class = DSC$K_CLASS_SD;
		from.dsc$w_length = sizeof(double);
		from.dsc$a_pointer = (char *) loc;
		from.dsc$b_scale = 0;
		from.dsc$b_digits = 0;
		from.dsc$b_dtype = DSC$K_DTYPE_G;

		to.dsc$b_class = DSC$K_CLASS_SD;
		to.dsc$w_length = sizeof(double);
		to.dsc$a_pointer = (char *) &rec->fields[i].val;
		to.dsc$b_scale = 0;
		to.dsc$b_digits = 0;
		to.dsc$b_dtype = DSC$K_DTYPE_FT;

		rec_Assert((OKAY
			(lib$cvt_dx_dx
			 ((unsigned int *) &from, (unsigned int *) &to))));
	    }
	    break;
	case DSC$K_DTYPE_FT:
	    rec_Assert((size = rec->fields[i].size) == 8);
	    rec_Assert((loc + size) <= end);
	    memcpy(rec->fields[i].val, loc, size);
	    break;
	case DSC$K_DTYPE_NU:
	case DSC$K_DTYPE_NL:
	case DSC$K_DTYPE_NLO:
	case DSC$K_DTYPE_NR:
	case DSC$K_DTYPE_NRO:
	case DSC$K_DTYPE_NZ:
	case DSC$K_DTYPE_P:
	    size = rec->fields[i].size;
	    rec_Assert((loc + size) <= end);
	    memcpy(rec->fields[i].val, loc, size);
	    break;
	default:
	    abort();
	    break;
	}
    }

}


void REC_load(void *addr, void *data, unsigned short len)
{
    _load(addr, data, len);
}


void REC_pack(void *data, unsigned short maxlen, void *addr,
	      unsigned short *len)
{
    LCL_rec *rec = (LCL_rec *) addr;
    char *loc;
    char *end;
    unsigned short size;
    int i;

    rec_Assert(addr);
    rec_Assert(data);

    end = (char *) data + maxlen;
    *len = 0;

    for (i = 0; i < rec->ncol; i++) {
	loc = (char *) data + rec->fields[i].off;
	size = rec->fields[i].size;
	rec_Assert((loc + size) <= end);
	memcpy(loc, rec->fields[i].val, size);
	*len += size;
    }
}
