#pragma module SYSLOG "V1.03"

/*
**
** © Copyright 2002 Hewlett-Packard Development Company, L.P.
**
** Hewlett-Packard and the Hewlett-Packard logo are trademarks
** of Hewlett-Packard Development Company L.P. in the U.S. and/or
** other countries.
**
** Confidential computer software.
** Valid license from Hewlett-Packard required for possession, use
** or copying.  Consistent with FAR 12.211 and 12.212, Commercial
** Computer Software, Computer Software Documentation, and Technical
** Data for Commercial.  Items are licensed to the U.S. Government
** under vendor's standard commercial license.
**
** Hewlett-Packard shall not be liable for technical or editorial
** errors or omissions contained herein.  The information is provided
** "as is" without warranty of any kind and is subject to change
** without notice.  The warranties for Hewlett-Packard products are
** set forth in the express warranty statements accompanying such
** products.  Nothing herein should be construed as constituting an
** additional warranty.
**
*/

/*
**
**  FACILITY:
**
**      Secure Web Server
**
**  ABSTRACT:
**
**	Interface for openlog, syslog, closelog
**
**  AUTHOR:
**
**      Matthew Doremus
**
**
**  CREATION DATE:   August 29, 2001
**
**  MODIFICATION HISTORY:
**
**  V1.00 	        Matthew Doremus                 29-Aug-2001
**        Initial development.
**
*/

#ifndef __NEW_STARLET
#define __NEW_STARLET
#define __NEW_STARLET_SET
#endif

#include <builtins.h>
#include <starlet.h>
#include <unixlib.h>
#include <descrip.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <opcdef.h>
#include <prdef.h>
#include <stdio.h>
#include <time.h>

#include  "syslog.h"

#ifdef  __NEW_STARLET_SET
#undef  __NEW_STARLET_SET
#undef  __NEW_STARLET
#endif

/*
** Define local macro definitions
*/
#define SYSLOG_MSG_SIZE 1024
#define OPCOM_MSG_SIZE	128
#define OPCOM_HDR_SIZE	8
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

/*
** Define the code table structure
*/
typedef struct _CodeTable {
    char	*text;
    int		code;
} CodeTable;

#ifdef TEST_SYSLOG
/*
** Define the Options table array
*/
static const CodeTable OptionsTable[] = {
	"pid",		LOG_PID,
	"cons",		LOG_CONS,
	"odelay",	LOG_ODELAY,
	"ndelay",	LOG_NDELAY,
	"nowait",	LOG_NOWAIT,
	"perror",	LOG_PERROR,
	"",		-1};
#endif

/*
** Define the Facility table array
*/
static const CodeTable FacilityTable[] = {
	"kern",		LOG_KERN,
	"user",		LOG_USER,
	"mail",		LOG_MAIL,
	"daemon",	LOG_DAEMON,
	"auth",		LOG_AUTH,
	"syslog",	LOG_SYSLOG,
	"lpr",		LOG_LPR,
	"news",		LOG_NEWS,
	"uucp",		LOG_UUCP,
	"cron", 	LOG_CRON,
	"",		0,
	"",		0,
	"",		0,
	"",		0,
	"",		0,
	"",		0,
	"local0",	LOG_LOCAL0,
	"local1",	LOG_LOCAL1,
	"local2",	LOG_LOCAL2,
	"local3",	LOG_LOCAL3,
	"local4",	LOG_LOCAL4,
	"local5",	LOG_LOCAL5,
	"local6",	LOG_LOCAL6,
	"local7",	LOG_LOCAL7,
	"",		-1};

/*
** Define the Priority table array
*/
static const CodeTable PriorityTable[] = {
	"emerg",	LOG_EMERG,
	"alert",	LOG_ALERT,
	"crit",		LOG_CRIT,
	"err",		LOG_ERR,
	"warning",	LOG_WARNING,
	"notice",	LOG_NOTICE,
	"info",		LOG_INFO,
	"debug",	LOG_DEBUG,
	"",		-1};

/*
** Define the static values for syslog functions
*/
static char *SysLogIdent = NULL;
static int SysLogOpt = -1;
static int SysLogPri = -1;

/*
** Define prototypes for local functions
*/
#ifdef TEST_SYSLOG
static int ParseArg (char *, const CodeTable [], int);
#endif

#ifdef TEST_SYSLOG
/******************************************************************************/
/***                                                                        ***/
/******************************************************************************/
main (int argc, char *argv[])
{
int Opt = LOG_PERROR,
    Fac = LOG_USER,
    Pri = LOG_INFO,
    status,
    i;

/*
** Get the path name
*/
if (argc < 2 || argc > 5)
    {
    printf ("Usage: SYSLOG message [options] [facility] [priority]\n");
    printf ("\tmessage: syslog message\n");
    printf ("\toptions: openlog options (default LOG_PERROR)\n");
    for (i = 0; OptionsTable[i].code != -1; i++)
	if (strlen (OptionsTable[i].text) > 0)
	    printf ("\t\tLOG_%s\n", OptionsTable[i].text);
    printf ("\tfacility: openlog facility (default LOG_USER)\n");
    for (i = 0; FacilityTable[i].code != -1; i++)
	if (strlen (FacilityTable[i].text) > 0)
	    printf ("\t\tLOG_%s\n", FacilityTable[i].text);
    printf ("\tpriority: syslog priority (default LOG_INFO)\n");
    for (i = 0; PriorityTable[i].code != -1; i++)
	if (strlen (PriorityTable[i].text) > 0)
	    printf ("\t\tLOG_%s\n", PriorityTable[i].text);
    exit (1);
    }

/*
** Parse the Options
*/
if (argc > 2)
    Opt = ParseArg (argv[2], OptionsTable, Opt);

/*
** Parse the Facility
*/
if (argc > 3)
    Fac = ParseArg (argv[3], FacilityTable, Fac);

/*
** Parse the Priority
*/
if (argc > 4)
    Pri = ParseArg (argv[4], PriorityTable, Pri);

/*
** Call the openlog, syslog, closelog functions
*/
openlog ("TEST", Opt, Fac);
syslog (Pri, argv[1]);
closelog ();

}
/******************************************************************************/
/***                                                                        ***/
/******************************************************************************/
static int ParseArg (char *ArgStr, const CodeTable ArgTbl[], int DefVal)
{
char *StrPtr = ArgStr,
     *TokPtr = ArgStr;
int Finished = 0,
    SetVal = 0,
    RetVal = 0,
    i;

while (TokPtr)
    {
    /*
    ** Locate the next token and null terminate it
    */
    TokPtr = strstr (StrPtr, "|");
    if (TokPtr)
	*TokPtr = '\0';

    if (strncasecmp (StrPtr, "LOG_", 4) == 0)
	StrPtr = StrPtr + 4;

    for (i = 0; ArgTbl[i].code != -1; i++)
	{
	if (strcasecmp (StrPtr, ArgTbl[i].text) == 0)
	    {
	    RetVal |= ArgTbl[i].code;
	    SetVal = 1;
	    break;
	    }
	}

    /*
    ** Move past the last token
    */
    if (TokPtr)
	StrPtr = TokPtr + 1;
    }

/*
** If node code value was found then return the default value
*/
if (SetVal == 0)
    RetVal = DefVal;

/*
** Return the code value
*/
return (RetVal);

}
#endif

/******************************************************************************/
/***                                                                        ***/
/******************************************************************************/
void syslog (int priority, const char *message, ...)
{
struct dsc64$descriptor OpComDesc = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
char MsgHdr[SYSLOG_MSG_SIZE];
char MsgBuf[SYSLOG_MSG_SIZE];
char SndBuf[SYSLOG_MSG_SIZE];
OPCDEF OpComMsg;
time_t TimeBin;
char *TimeStr;
int FacilityIdx,
    PriorityIdx,
    status = 1,
    SndLen;
va_list ap;

/*
** Construct the variable length message
*/
va_start (ap, message);
vsprintf (MsgBuf, message, ap);
va_end (ap);

/*
** Initialize the syslog info if necessary
*/
if (SysLogOpt < 0 || SysLogPri < 0)
    closelog ();

/*
** Determine the Facility & Priority array indexes
*/
FacilityIdx = (LOG_FAC(priority) ? LOG_FAC(priority) : LOG_FAC(SysLogPri));
PriorityIdx = LOG_PRI(priority);

/*
** Construct the log message header
*/
sprintf (MsgHdr, "[%s] [%s] ",
	 FacilityTable[FacilityIdx].text, PriorityTable[PriorityIdx].text);
if (SysLogOpt & LOG_PID)
    sprintf (MsgHdr + strlen (MsgHdr), "(%08X) ", getpid ());
if (SysLogIdent)
    sprintf (MsgHdr + strlen (MsgHdr), "%s:", SysLogIdent);

/*
** Send the message to the console if requested
*/
if (SysLogOpt & LOG_CONS)
    {
    /*
    ** Create the opcom log message to be sent
    */
    sprintf (SndBuf, "%s%s", MsgHdr, MsgBuf);

    /*
    ** Establish the message length
    */
    SndLen = MIN(strlen (SndBuf), OPCOM_MSG_SIZE);

    /*
    ** Setup the opcom request
    */
    memset (&OpComMsg, 0, sizeof (OpComMsg));
    OpComMsg.opc$b_ms_type = OPC$_RQ_RQST;
    OpComMsg.opc$b_ms_target = OPC$M_NM_CENTRL;
    OpComMsg.opc$l_ms_rqstid = 0;
    memcpy (&OpComMsg.opc$l_ms_text, SndBuf, SndLen);

    /*
    ** Setup the opcom request descriptor
    */
    OpComDesc.dsc64$q_length = SndLen + OPCOM_HDR_SIZE;
    OpComDesc.dsc64$pq_pointer = (char *) &OpComMsg;

    /*
    ** Send the log message to opcom
    */
    status = SYS$SNDOPR (&OpComDesc, 0);
    }

/*
** Send the message to standard error if requested or console write failed
*/
if (SysLogOpt & LOG_PERROR || ! (status & 1))
    {
    /*
    ** Get the current time
    */
    TimeBin = time (NULL);
    TimeStr = ctime (&TimeBin);
    TimeStr[strlen (TimeStr) - 1] = '\0';

    /*
    ** Create the standard error log message to be written
    */
    sprintf (SndBuf, "[%s] %s%s", TimeStr, MsgHdr, MsgBuf);

    /*
    ** Write the log message to standard error
    */
    fprintf (stderr, SndBuf);
    }

}

/******************************************************************************/
/***                                                                        ***/
/******************************************************************************/
void openlog (const char *ident, int logopt, int facility)
{
unsigned __int64 CurMode = (__PAL_RD_PS() & PR$M_PS_CURMOD) >> 3;
int i;

/*
** Close any previous log info
*/
closelog ();

/*
** Save the log ident
*/
if (ident)
    SysLogIdent = strdup (ident);

/*
** Save the log options
*/
if (! ((logopt & LOG_PERROR) || (logopt & LOG_CONS)))
    SysLogOpt = logopt | LOG_PERROR;
else
    SysLogOpt = logopt;

/*
** Initialize the priority
*/
if (facility == LOG_KERN && CurMode != PR$C_PS_KERNEL)
    SysLogPri = LOG_USER;
else
    SysLogPri = facility;
for (i = 0; PriorityTable[i].code != -1; i++)
    SysLogPri |= PriorityTable[i].code;

}

/******************************************************************************/
/***                                                                        ***/
/******************************************************************************/
void closelog (void)
{
int i;

/*
** Initialize the ident
*/
if (SysLogIdent)
    free (SysLogIdent);
SysLogIdent = NULL;

/*
** Initialize the options
*/
SysLogOpt = LOG_PERROR;

/*
** Initialize the priority
*/
SysLogPri = LOG_USER;
for (i = 0; PriorityTable[i].code != -1; i++)
    SysLogPri |= PriorityTable[i].code;

}
