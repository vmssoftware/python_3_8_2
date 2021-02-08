%module lib

%{
#include "lib.h"
%}

%include "typemaps.i"
%include <cstring.i>
%cstring_output_allocate(char **OUTPUT, free(*$1));

%typemap(in) unsigned int *owner_uic (unsigned int ouic) {
   if (PyInt_Check($input)) {
        ouic = PyInt_AsLong($input);
        $1 = &ouic;
    } else {
        $1 = NULL;
    }
}

%exception {
    Py_BEGIN_ALLOW_THREADS
    $action
    Py_END_ALLOW_THREADS
}

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
%rename(set_symbol) _set_symbol;
%rename(get_symbol) _get_symbol;
%rename(delete_symbol) _delete_symbol;

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
extern unsigned int _create_dir(char *, unsigned int *owner_uic, unsigned short, unsigned short);
extern unsigned int _set_symbol(char *, char *);
extern unsigned int _get_symbol(char *, char **OUTPUT);
extern unsigned int _delete_symbol(char *);

#define LIB_FACILITY 21
#define LIB_NORMAL 1409025
#define LIB_STRTRU 1409041
#define LIB_ONEENTQUE 1409049
#define LIB_KEYALRINS 1409057
#define LIB_ERRROUCAL 1409065
#define LIB_RESTART 1409073
#define LIB_BADSTA 1409540
#define LIB_FATERRLIB 1409548
#define LIB_INSVIRMEM 1409556
#define LIB_INPSTRTRU 1409564
#define LIB_INVSTRDES 1409572
#define LIB_INTLOGERR 1409580
#define LIB_INVARG 1409588
#define LIB_AMBKEY 1409596
#define LIB_UNRKEY 1409604
#define LIB_PUSSTAOVE 1409612
#define LIB_USEFLORES 1409620
#define LIB_SIGNO_ARG 1409628
#define LIB_BADBLOADR 1409636
#define LIB_BADBLOSIZ 1409644
#define LIB_NOTFOU 1409652
#define LIB_ATTCONSTO 1409660
#define LIB_SYNTAXERR 1409668
#define LIB_INVTYPE 1409676
#define LIB_INSEF 1409684
#define LIB_EF_ALRFRE 1409692
#define LIB_EF_ALRRES 1409700
#define LIB_EF_RESSYS 1409708
#define LIB_INSLUN 1409716
#define LIB_LUNALRFRE 1409724
#define LIB_LUNRESSYS 1409732
#define LIB_SCRBUFOVF 1409740
#define LIB_INVSCRPOS 1409748
#define LIB_SECINTFAI 1409756
#define LIB_INVCHA 1409764
#define LIB_QUEWASEMP 1409772
#define LIB_STRIS_INT 1409780
#define LIB_KEYNOTFOU 1409788
#define LIB_INVCVT 1409796
#define LIB_INVDTYDSC 1409804
#define LIB_INVCLADSC 1409812
#define LIB_INVCLADTY 1409820
#define LIB_INVNBDS 1409828
#define LIB_DESSTROVF 1409836
#define LIB_INTOVF 1409844
#define LIB_DECOVF 1409852
#define LIB_FLTOVF 1409860
#define LIB_FLTUND 1409868
#define LIB_ROPRAND 1409876
#define LIB_WRONUMARG 1409884
#define LIB_NOSUCHSYM 1409892
#define LIB_INSCLIMEM 1409900
#define LIB_AMBSYMDEF 1409908
#define LIB_NOCLI 1409916
#define LIB_UNECLIERR 1409924
#define LIB_INVSYMNAM 1409932
#define LIB_ATTREQREF 1409940
#define LIB_INVFILSPE 1409948
#define LIB_INVXAB 1409956
#define LIB_NO_STRACT 1409964
#define LIB_BADTAGVAL 1409972
#define LIB_INVOPEZON 1409980
#define LIB_PAGLIMEXC 1409988
#define LIB_NOTIMP 1409996
#define LIB_BADZONE 1410004
#define LIB_IVTIME 1410012
#define LIB_ONEDELTIM 1410020
#define LIB_NEGTIM 1410028
#define LIB_INVARGORD 1410036
#define LIB_ABSTIMREQ 1410044
#define LIB_DELTIMREQ 1410052
#define LIB_INVOPER 1410060
#define LIB_REENTRANCY 1410068
#define LIB_UNRFORCOD 1410076
#define LIB_ILLINISTR 1410084
#define LIB_NUMELEMENTS 1410092
#define LIB_ELETOOLON 1410100
#define LIB_ILLCOMPONENT 1410108
#define LIB_AMBDATTIM 1410116
#define LIB_INCDATTIM 1410124
#define LIB_ILLFORMAT 1410132
#define LIB_UNSUPVER 1410140
#define LIB_BADFORMAT 1410148
#define LIB_NODTOOLNG 1410156
#define LIB_NOHOSNAM 1410164
#define LIB_OUTSTRTRU 1413120
#define LIB_FILFAIMAT 1413128
#define LIB_NEGANS 1413136
#define LIB_QUIPRO 1413144
#define LIB_QUICONACT 1413152
#define LIB_ENGLUSED 1413160
#define LIB_DEFFORUSE 1413168
#define LIB_MEMALRFRE 1413176
#define LIB_BADCCC 1425408
#define LIB_EOMERROR 1425416
#define LIB_EOMFATAL 1425424
#define LIB_EOMWARN 1425432
#define LIB_GSDTYP 1425440
#define LIB_ILLFMLCNT 1425448
#define LIB_ILLMODNAM 1425456
#define LIB_ILLPSCLEN 1425464
#define LIB_ILLRECLEN 1425472
#define LIB_ILLRECLN2 1425480
#define LIB_ILLRECTYP 1425488
#define LIB_ILLRECTY2 1425496
#define LIB_ILLSYMLEN 1425504
#define LIB_NOEOM 1425512
#define LIB_RECTOOSML 1425520
#define LIB_SEQUENCE 1425528
#define LIB_SEQUENCE2 1425536
#define LIB_STRLVL 1425544
#define LIB_NOWILD 1380650
#define LIB_ACTIMAGE 1381050
#define LIB_OPENIN 1380506
#define LIB_CLOSEIN 1380432
#define LIB_READERR 1380530
