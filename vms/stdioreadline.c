#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <descrip.h>
#include <lib$routines.h>
#include <smg$routines.h>				/* SMG$name   */
#include <smgdef.h>					/* SMG$x_name */
#include <smgmsg.h>					/* SMG$_name  */
#include <ssdef.h>					/* SS$_NAME   */
#include <trmdef.h>
#include <stsdef.h>

#define S_PASSWORD 255  /* size of password buffer */

/* SMG$ support */
static unsigned long                             pyvms_gl_keyboard_id;   /* SMG$ */
static unsigned long                             pyvms_gl_key_table_id;  /* SMG$ */
static char LineBuffer[256];

char *
vms__StdioReadline(FILE *sys_stdin, FILE *sys_stdout, char *prompt)
{
	int n;
	struct dsc$descriptor_s	inputD;
	struct dsc$descriptor_s	promptD;
	struct dsc$descriptor_s* promptPtr;
	unsigned short int reslen;
	unsigned long flags = 0;
	int s;
	char promptLocal[32];

	if (pyvms_gl_key_table_id == 0) {
		s = smg$create_key_table(&pyvms_gl_key_table_id);
		if (!$VMS_STATUS_SUCCESS(s)) {
			perror("SMG$CREATE_KEY_TABLE() failed\n");
			exit(s);
		}
	}
	if (pyvms_gl_keyboard_id == 0) {
		s = smg$create_virtual_keyboard(&pyvms_gl_keyboard_id);
		if (!$VMS_STATUS_SUCCESS(s)) {
			perror("SMG$CREATE_VIRTUAL_KEYBOARD() failed\n");
			exit(s);
    		}
	}

	fflush(stdout);

	/* set up descriptors */
	inputD.dsc$w_length  = sizeof(LineBuffer) - 2;
	inputD.dsc$b_dtype   = DSC$K_DTYPE_T;
	inputD.dsc$b_class   = DSC$K_CLASS_S; /* @@ use a Dyndesc ? */
	inputD.dsc$a_pointer = LineBuffer;

	if (prompt)
	{
	    strcpy(promptLocal, prompt);
	    promptD.dsc$w_length  = strlen(prompt);
	    promptD.dsc$b_dtype   = DSC$K_DTYPE_T;
	    promptD.dsc$b_class   = DSC$K_CLASS_S;
	    promptD.dsc$a_pointer = promptLocal;
	    promptPtr = &promptD;
	}
	else
	{
	    promptPtr = 0;			/* no prompt */
	}

	s = smg$read_composed_line
		(&pyvms_gl_keyboard_id		/* keyboard-id            */
		,&pyvms_gl_key_table_id		/* [key-table-id]         */
		,&inputD			/* resultant-string       */
		,promptPtr			/* [prompt-string]        */
		,&reslen	 		/* [resultant-length]     */
		,0				/* [display-id]           */
		,&flags				/* [flags]                */
		,0				/* [initial-string]       */
		,0				/* [timeout]              */
		,0				/* [rendition-set]        */
		,0				/* [rendition-complement] */
		,0				/* [word-terminator-code] */
		);

	if ((s != SS$_NORMAL) &&
	    (s != SS$_ABORT)  &&
	    (s != SS$_CANCEL) &&
	    (s != SMG$_EOF)
	   )
	{
	    return NULL;
	}

	if (s == SMG$_EOF) {
	    LineBuffer[reslen] = '\0';
	} else if ((s == SS$_ABORT)  || (s == SS$_CANCEL)) {
	    return NULL;
	} else {
	    LineBuffer[reslen] = '\n';
      	    LineBuffer[reslen+1] = '\0';
	}

	return LineBuffer;
}

