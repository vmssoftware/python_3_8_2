#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <descrip.h>
#include <lib$routines.h>
#include <smg$routines.h>       /* SMG$name   */
#include <smgdef.h>             /* SMG$x_name */
#include <smgmsg.h>             /* SMG$_name  */
#include <ssdef.h>              /* SS$_NAME   */
#include <trmdef.h>
#include <stsdef.h>

/* SMG$ support */
static unsigned long                             pyvms_gl_keyboard_id;   /* SMG$ */
static unsigned long                             pyvms_gl_key_table_id;  /* SMG$ */

#define MAX_LINE_BUFFER 1024

void *PyMem_RawMalloc(size_t size);
void  PyMem_RawFree(void *ptr);

char* vms_SMG_Readline(FILE *stdin, FILE *stdout, const char *prompt) {
    struct dsc$descriptor_s inputD;
    struct dsc$descriptor_s promptD;
    struct dsc$descriptor_s *promptPtr;

    unsigned short int reslen = 0;
    unsigned long flags = 0;
    int status = 0;

    if (pyvms_gl_key_table_id == 0) {
        status = smg$create_key_table(&pyvms_gl_key_table_id);
        if (!$VMS_STATUS_SUCCESS(status)) {
            return NULL;
            // perror("SMG$CREATE_KEY_TABLE() failed\n");
            // exit(status);
        }
    }
    if (pyvms_gl_keyboard_id == 0) {
        status = smg$create_virtual_keyboard(&pyvms_gl_keyboard_id);
        if (!$VMS_STATUS_SUCCESS(status)) {
            return NULL;
            // perror("SMG$CREATE_VIRTUAL_KEYBOARD() failed\n");
            // exit(status);
        }
    }

    fflush(stdout);

    /* set up descriptors */
    inputD.dsc$w_length  = MAX_LINE_BUFFER - 2;
    inputD.dsc$a_pointer = (void*)PyMem_RawMalloc(MAX_LINE_BUFFER);
    if (inputD.dsc$a_pointer == NULL) {
        return NULL;
    }
    inputD.dsc$b_dtype   = DSC$K_DTYPE_T;
    inputD.dsc$b_class   = DSC$K_CLASS_S; /* @@ use a Dyndesc ? */

    if (prompt)	{
        promptD.dsc$w_length  = strlen(prompt);
        promptD.dsc$b_dtype   = DSC$K_DTYPE_T;
        promptD.dsc$b_class   = DSC$K_CLASS_S;
        promptD.dsc$a_pointer = (char*)prompt;
        promptPtr = &promptD;
    } else {
        promptPtr = 0;      /* no prompt */
    }

    status = smg$read_composed_line(
         &pyvms_gl_keyboard_id      /* keyboard-id            */
        ,&pyvms_gl_key_table_id     /* [key-table-id]         */
        ,&inputD                    /* resultant-string       */
        ,promptPtr                  /* [prompt-string]        */
        ,&reslen                    /* [resultant-length]     */
        ,0                          /* [display-id]           */
        ,&flags                     /* [flags]                */
        ,0                          /* [initial-string]       */
        ,0                          /* [timeout]              */
        ,0                          /* [rendition-set]        */
        ,0                          /* [rendition-complement] */
        ,0                          /* [word-terminator-code] */
    );

    switch (status) {
        case SS$_NORMAL:
            inputD.dsc$a_pointer[reslen++] = '\n';
        case SMG$_EOF:
            inputD.dsc$a_pointer[reslen++] = '\0';
            break;
        default:
            PyMem_RawFree(inputD.dsc$a_pointer);
            inputD.dsc$a_pointer = NULL;
            break;
    }

    return inputD.dsc$a_pointer;
}

