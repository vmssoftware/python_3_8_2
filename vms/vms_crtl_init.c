#include <stdio.h>
#include <errno.h>
#include <unixlib.h>


/*
** Sets current value for a feature
*/
static void set(char *name, int value)
{
     int index;
     errno = 0;

     index = decc$feature_get_index(name);

     if (index > 0)
	decc$feature_set_value(index, 1, value);
}

/*
** Sets default value for a feature
*/
static void set_default(char *name, int value)
{
     int index;
     errno = 0;

     index = decc$feature_get_index(name);

     if (index > 0)
	decc$feature_set_value (index, 0, value);
}

static void set_coe ( void )
{
    set ("DECC$UNIX_LEVEL", 100);
    set ("DECC$ARGV_PARSE_STYLE", 1);
    set ("DECC$DETACHED_CHILD_PROCESS", 0);
    set ("DECC$EFS_CASE_PRESERVE", 1);
    set ("DECC$EFS_CASE_SPECIAL", 0);
    set ("DECC$EFS_CHARSET", 1);
    set ("DECC$ENABLE_GETENV_CACHE", 1);
    set ("DECC$EXIT_AFTER_FAILED_EXEC", 1);
    set ("DECC$FILE_SHARING", 1);
    set ("DECC$MAILBOX_CTX_STM", 1);
    set ("DECC$POPEN_NO_CRLF_REC_ATTR", 1);
    set ("DECC$POSIX_SEEK_STREAM_FILE", 1);
    set ("DECC$POSIX_STYLE_UID", 0);    // else getpwuid() doesn't work
    set ("DECC$READDIR_DROPDOTNOTYPE", 1);
}

#pragma extern_model save
#pragma extern_model strict_refdef "LIB$INITIALIZE" nowrt, long
#if __INITIAL_POINTER_SIZE
#    pragma __pointer_size __save
#    pragma __pointer_size 32
#else
#    pragma __required_pointer_size __save
#    pragma __required_pointer_size 32
#endif
void (* const iniarray[])() = {set_coe, } ;	/* Set our contribution to the LIB$INITIALIZE array */
#if __INITIAL_POINTER_SIZE
#    pragma __pointer_size __restore
#else
#    pragma __required_pointer_size __restore
#endif
#pragma extern_model restore

int LIB$INITIALIZE();
/* globaldef */ int (*lib_init_ref)() = LIB$INITIALIZE;
