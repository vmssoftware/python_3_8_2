#ifndef __RTL_H__
#define __RTL_H__

# ifdef __cplusplus
        extern "C" {
#endif

extern unsigned int _date_time(char **);
extern unsigned int _free_ef(unsigned int);
extern unsigned int _get_ef(unsigned int *);
extern unsigned int _get_hostname(char **, unsigned int);
extern unsigned int _getjpi(int, unsigned int *, char *, char **);
extern unsigned int _getsyi(int, char **, unsigned int *, char *);
extern unsigned int _spawn(char *, char *, char *, unsigned int, char *, unsigned int *);
extern unsigned int _do_command(char *);
extern unsigned int _put_common(char *);
extern unsigned int _get_common(char **);
extern unsigned int _create_dir(char *, unsigned int *, unsigned short, unsigned short);
extern unsigned int _set_symbol(char *, char *);
extern unsigned int _get_symbol(char *, char **);
extern unsigned int _delete_symbol(char *);

#ifdef __cplusplus
        }
#endif
#endif
