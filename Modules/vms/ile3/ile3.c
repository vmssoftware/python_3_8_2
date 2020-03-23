#define __NEW_STARLET 1

#include <descrip.h>
#include <starlet.h>
#include <ssdef.h>
#include <iledef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "ile3.h"
#include "lcl.h"

#ifndef OKAY
#define OKAY(STATUS) (((STATUS) & 1) != 0)
#endif



void *_new()
{
    LCL_ile3 *obj = NULL;

    obj = calloc(1, sizeof(LCL_ile3));
    assert(obj);
    obj->list = calloc(1, sizeof(ILE3));
    assert(obj->list);
    obj->types = calloc(1, sizeof(int));
    assert(obj->types);
    obj->size = 1;
    obj->elem = 0;
    return (obj);
}


void _delete(void *addr)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    int i;
    ILE3 *item;

    for (i = 0; i < obj->size; i++) {
	item = &obj->list[i];
	if (item) {
	    free(item->ile3$ps_bufaddr);
	    free(item->ile3$ps_retlen_addr);
	    free(item);
	}
	free(obj->types);
    }

    free(obj);
}


int _getbyte(void *addr, int idx, int n)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    ILE3 *item;
    unsigned short len;
    char *tmp;

    assert((idx >= 0) && (idx <= obj->elem));
    item = &obj->list[idx];
    len = *item->ile3$ps_retlen_addr;
    assert(len);
    assert(len > n);
    tmp = (char *) item->ile3$ps_bufaddr;
    return (tmp[n]);
}


char *_getstr(void *addr, int idx, int flag)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    ILE3 *item;
    unsigned short len;
    char *tmp;
    char *ptr;

    assert((idx >= 0) && (idx <= obj->elem));
    assert((obj->types[idx] == DSC$K_DTYPE_T));
    item = &obj->list[idx];

    ptr = (char *) item->ile3$ps_bufaddr;

    if (flag) {
	len = ptr[0];
	ptr++;
    } else {
	len = *item->ile3$ps_retlen_addr;
    }

    tmp = malloc(len + 1);
    assert(tmp);
    memcpy(tmp, ptr, len);
    tmp[len] = '\0';

    if (!flag) {
	char *cp = &tmp[len - 1];
	while ((isspace(*cp)) && cp >= tmp) {
	    *cp = '\0';
	    cp--;
	}
    }

    return (tmp);
}


long long _getint(void *addr, int idx)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    ILE3 *item;
    unsigned short size;
    int type;

    assert((idx >= 0) && (idx <= obj->elem));
    item = &obj->list[idx];
    type = obj->types[idx];

    switch (type) {
    case DSC$K_DTYPE_BU:
	return (*(unsigned char *) item->ile3$ps_bufaddr);
	break;
    case DSC$K_DTYPE_B:
	return (*(char *) item->ile3$ps_bufaddr);
	break;
    case DSC$K_DTYPE_WU:
	return (*(unsigned short *) item->ile3$ps_bufaddr);
	break;
    case DSC$K_DTYPE_W:
	return (*(short *) item->ile3$ps_bufaddr);
	break;
    case DSC$K_DTYPE_LU:
	return (*(unsigned int *) item->ile3$ps_bufaddr);
	break;
    case DSC$K_DTYPE_L:
	return (*(int *) item->ile3$ps_bufaddr);
	break;
    case DSC$K_DTYPE_QU:
    case DSC$K_DTYPE_Q:
	return (*(long long *) item->ile3$ps_bufaddr);
	break;
    default:
	abort();
	break;
    }

    return (0);
}


char *_gethex(void *addr, int idx)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    ILE3 *item;
    unsigned short size;
    int type;
    char tmp[32], *val;

    assert((idx >= 0) && (idx <= obj->elem));
    item = &obj->list[idx];
    type = obj->types[idx];

    switch (type) {
    case DSC$K_DTYPE_BU:
	sprintf(tmp, "0x%llx", *(unsigned char *) item->ile3$ps_bufaddr);
	break;
    case DSC$K_DTYPE_B:
	sprintf(tmp, "0x%llx", *(char *) item->ile3$ps_bufaddr);
	break;
    case DSC$K_DTYPE_WU:
	sprintf(tmp, "0x%llx", *(unsigned short *) item->ile3$ps_bufaddr);
	break;
    case DSC$K_DTYPE_W:
	sprintf(tmp, "0x%llx", *(short *) item->ile3$ps_bufaddr);
	break;
    case DSC$K_DTYPE_LU:
	sprintf(tmp, "0x%llx", *(unsigned int *) item->ile3$ps_bufaddr);
	break;
    case DSC$K_DTYPE_L:
	sprintf(tmp, "0x%llx", *(int *) item->ile3$ps_bufaddr);
	break;
    case DSC$K_DTYPE_QU:
	sprintf(tmp, "0x%llx",
		*(unsigned long long *) item->ile3$ps_bufaddr);
	break;
    case DSC$K_DTYPE_Q:
	sprintf(tmp, "0x%llx", *(long long *) item->ile3$ps_bufaddr);
	break;
    default:
	abort();
	break;
    }

    val = strdup(tmp);
    assert(val);
    return (val);
}


void _addstr(void *addr, int code, char *val, unsigned short len)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    ILE3 *item;

    assert(obj);

    obj->size++;
    obj->list = realloc(obj->list, sizeof(ILE3) * obj->size);
    assert(obj->list);
    obj->types = realloc(obj->types, sizeof(int) * obj->size);
    assert(obj->types);
    item = &obj->list[obj->elem];

    item->ile3$w_code = code;
    item->ile3$ps_retlen_addr = calloc(1, 2);
    assert(item->ile3$ps_retlen_addr);

    if (val) {
	item->ile3$w_length = strlen(val);
    item->ile3$ps_bufaddr = strdup(val);
	assert(item->ile3$ps_bufaddr);
    } else {
	item->ile3$w_length = len;
    item->ile3$ps_bufaddr = calloc(1, len);
	assert(item->ile3$ps_bufaddr);
    }

    obj->types[obj->elem] = DSC$K_DTYPE_T;
    obj->elem++;
}


void _addint(void *addr, int code, int type, long long val)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    ILE3 *item;
    unsigned short size;

    assert(obj);

    obj->size++;
    obj->list = realloc(obj->list, sizeof(ILE3) * obj->size);
    assert(obj->list);
    obj->types = realloc(obj->types, sizeof(int) * obj->size);
    assert(obj->types);
    item = &obj->list[obj->elem];

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

    item->ile3$w_length = size;
    item->ile3$w_code = code;
    item->ile3$ps_bufaddr = calloc(1, size);
    assert(item->ile3$ps_bufaddr);
    item->ile3$ps_retlen_addr = calloc(1, 2);
    assert(item->ile3$ps_retlen_addr);

    switch (type) {
    case DSC$K_DTYPE_BU:
	{
	    unsigned char tmp = (unsigned char) val;
	    memcpy(item->ile3$ps_bufaddr, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_B:
	{
	    char tmp = (char) val;
	    memcpy(item->ile3$ps_bufaddr, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_WU:
	{
	    unsigned short tmp = (unsigned short) val;
	    memcpy(item->ile3$ps_bufaddr, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_W:
	{
	    short tmp = (short) val;
	    memcpy(item->ile3$ps_bufaddr, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_LU:
	{
	    unsigned long tmp = (unsigned long) val;
	    memcpy(item->ile3$ps_bufaddr, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_L:
	{
	    long tmp = (long) val;
	    memcpy(item->ile3$ps_bufaddr, &tmp, size);
	}
	break;
    case DSC$K_DTYPE_QU:
    case DSC$K_DTYPE_Q:
	memcpy(item->ile3$ps_bufaddr, &val, size);
	break;
    default:
	abort();
	break;
    }

    obj->types[obj->elem] = type;
    memcpy(item->ile3$ps_retlen_addr, &size, sizeof(unsigned short));
    obj->elem++;

    item = &obj->list[obj->elem];
    item->ile3$w_length = 0;
    item->ile3$w_code = 0;
    item->ile3$ps_bufaddr = NULL;
    item->ile3$ps_retlen_addr = NULL;
}



void _addstrd(void *addr, int code, char *val, unsigned char len)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    ILE3 *item;
    int n;
    char *ptr;

    assert(obj);
    assert(val);

    obj->size++;
    obj->list = realloc(obj->list, sizeof(ILE3) * obj->size);
    assert(obj->list);
    obj->types = realloc(obj->types, sizeof(int) * obj->size);
    assert(obj->types);
    item = &obj->list[obj->elem];

    item->ile3$w_code = code;
    item->ile3$ps_retlen_addr = calloc(1, 2);
    assert(item->ile3$ps_retlen_addr);

    item->ile3$w_length = len;
    item->ile3$ps_bufaddr = calloc(1, len);
    assert(item->ile3$ps_bufaddr);
    memset(item->ile3$ps_bufaddr, ' ', len);

    ptr = item->ile3$ps_bufaddr;
    n = strlen(val);
    *ptr = n;
    ptr++;
    memcpy(ptr, val, n);

    obj->types[obj->elem] = DSC$K_DTYPE_T;
    obj->elem++;
}



void _addstrn(void *addr, int code, char *val, unsigned short len)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    ILE3 *item;

    assert(obj);
    assert(val);

    obj->size++;
    obj->list = realloc(obj->list, sizeof(ILE3) * obj->size);
    assert(obj->list);
    obj->types = realloc(obj->types, sizeof(int) * obj->size);
    assert(obj->types);
    item = &obj->list[obj->elem];

    item->ile3$w_code = code;
    item->ile3$ps_retlen_addr = calloc(1, 2);
    assert(item->ile3$ps_retlen_addr);

    item->ile3$w_length = len;
    item->ile3$ps_bufaddr = calloc(1, len);
    assert(item->ile3$ps_bufaddr);
    memset(item->ile3$ps_bufaddr, ' ', len);
    memcpy(item->ile3$ps_bufaddr, val, strlen(val));

    obj->types[obj->elem] = DSC$K_DTYPE_T;
    obj->elem++;
}


void _addbin(void *addr, int code, long long val, unsigned short off, unsigned short num)
{
    LCL_ile3 *obj = (LCL_ile3 *) addr;
    ILE3 *item;
    long long *ptr;

    assert(obj);
    assert((off < num));
    assert(((off + num) <= sizeof(val)));

    obj->size++;
    obj->list = realloc(obj->list, sizeof(ILE3) * obj->size);
    assert(obj->list);
    obj->types = realloc(obj->types, sizeof(int) * obj->size);
    assert(obj->types);
    item = &obj->list[obj->elem];

    item->ile3$w_code = code;
    item->ile3$ps_retlen_addr = calloc(1, 2);
    assert(item->ile3$ps_retlen_addr);

    item->ile3$w_length = num;
    item->ile3$ps_bufaddr = malloc(num);
    assert(item->ile3$ps_bufaddr);
    ptr = &val + off;
    memcpy(item->ile3$ps_bufaddr, ptr, num);

    obj->types[obj->elem] = DSC$K_DTYPE_T;
    obj->elem++;
}

