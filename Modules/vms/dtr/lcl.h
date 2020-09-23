#ifndef __LCL_H__
#define __LCL_H__

#define __NEW_STARLET 1
#include <iledef.h>

typedef struct {
    unsigned short top;
    int size;
    int elem;
    ILE3 *list;
    int *types;
    unsigned short bot;
} LCL_ile3;


typedef struct {
    unsigned short ncol;
    struct {
	char *name;
	unsigned short idx;
	unsigned short type;
	unsigned short size;
	unsigned short off;
	int digits;
	int scale;
	void *val;
    } *fields;
    char *buf;
    unsigned short len;
} LCL_rec;

#endif
