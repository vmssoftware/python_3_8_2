#ifndef __DTR_H__
#define __DTR_H__

#pragma names save
#pragma names uppercase
#include "dab.h"		/* DATATRIEVE definitions */





#pragma names restore


#ifdef __cplusplus
extern "C" {
#endif

    extern unsigned int _command(unsigned int, char *, int *, unsigned short *);
    extern unsigned int _continue(unsigned int, int *, unsigned short *);
    extern unsigned int _create_udk(unsigned int, char *, short, short);
    extern unsigned int _end_udk(unsigned int);
    extern unsigned int _finish(unsigned int);
    extern unsigned int _get_port(unsigned int, void *);
    extern unsigned int _get_string(unsigned int, int, char **, char *);
    extern unsigned int _info(unsigned int, int, char, int *, char **, int);
    extern unsigned int _init(unsigned int *, int, int);
    extern unsigned int _lookup(unsigned int, char, int *, char *);
    extern unsigned int _port_eof(unsigned int);
    extern unsigned int _put_port(unsigned int, void *);
    extern unsigned int _put_value(unsigned int, char *);
    extern unsigned int _unwind(unsigned int);
    extern unsigned int _condition(unsigned int);
    extern unsigned int _state(unsigned int);
    extern char *_msg_buf(unsigned int);
    extern char *_aux_buf(unsigned int);
    extern unsigned int _get_options(unsigned int);
    extern unsigned int _set_options(unsigned int, unsigned int);
    extern unsigned int _clear_options(unsigned int, unsigned int);
    extern unsigned int _get_rec_len(unsigned int);
    extern unsigned int _get_flags(unsigned int);
    extern unsigned int _get_udk_index(unsigned int);
    extern unsigned int _dtr(unsigned int, int);
    extern unsigned int _skip(unsigned int, unsigned int);
    extern char **_row(unsigned int, char *);

#ifdef __cplusplus
}
#endif
#endif
