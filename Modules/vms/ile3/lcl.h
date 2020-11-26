#ifndef __LCL_H__
#define __LCL_H__

#define __NEW_STARLET 1
#include <iledef.h>

typedef struct {
    int size;
    int elem;
    ILE3 *list;
    int *types;
} LCL_ile3;


#endif
