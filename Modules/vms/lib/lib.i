%module lib

%{
#include "lib.h"
%}

%include "typemaps.i"
%include <cstring.i>
%cstring_output_allocate(char **OUTPUT, free(*$1));


%rename(date_time) _date_time;
%rename(free_ef) _free_ef;
%rename(get_ef) _get_ef;
%rename(get_hostname) _get_hostname;
%rename(getjpi) _getjpi;
%rename(getsyi) _getsyi;
%rename(spawn) _spawn;
%rename(do_command) _do_command;
%rename(put_common) _put_common;
%rename(get_common) _get_common;
%rename(create_dir) _create_dir;

extern unsigned int _date_time(char **OUTPUT);
extern unsigned int _free_ef(unsigned int);
extern unsigned int _get_ef(unsigned int *OUTPUT);
extern unsigned int _get_hostname(char **OUTPUT, unsigned int);
extern unsigned int _getjpi(int, unsigned int *INPUT, char *, char **OUTPUT);
extern unsigned int _getsyi(int, char **OUTPUT, unsigned int *OUTPUT, char *);
extern unsigned int _spawn(char *, char *, char *, unsigned int, char *, unsigned int *OUTPUT);
extern unsigned int _do_command(char *);
extern unsigned int _put_common(char *);
extern unsigned int _get_common(char **OUTPUT);
extern unsigned int _create_dir(char *, unsigned int, unsigned short, unsigned short);

