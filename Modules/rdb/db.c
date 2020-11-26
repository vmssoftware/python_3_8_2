#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <descrip.h>

#pragma names save
#pragma names uppercase
#include <sql_rdb_headers.h>
#include <sql_literals.h>
#include <sql_sqlca.h>
#pragma names restore

#include <starlet.h>
#include <str$routines.h>
#include <lib$routines.h>
#include <assert.h>
#include <unixlib.h>

#include "db.h"

#ifndef OKAY
#define OKAY(STATUS) (((STATUS) & 1) != 0)
#endif


#define MAXARGS        100

#pragma nomember_alignment

typedef struct
{
	char  	sqldaid[8];
	int   	i0;
	short 	sqln;
	short 	sqld;
	struct  sqlvar
    	{
    	   	short 	sqltype;
	   	long  	sqldlen;
           	int  	i0;
	   	char *  sqldata;
	   	int *   sqlnind;
           	int  	i1;
           	int  	i2;
	   	short 	l0;
    	   	char  	d0[128];
           	char  	d1[128];
           	char  	d2[128];
           	char  	d3[128];
	}
	sqlvar[MAXARGS];
}
sqlda_t;

typedef struct {
    	sqlda_t 		sqlda_i;
	sqlda_t 		sqlda_o;
	long 			id;
	char 			cursor[32];
}
stmt_t;


static struct SQLCA SQLCA = {
	"SQLCA  ", 128, 0, {0, ""}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, ""
	};



typedef long  	vc_len_t;
typedef short 	sc_len_t;


# define MAXVLEN 8192

typedef struct
{
	unsigned long 	dlen;
	char 		data[MAXVLEN];
}
vc_t;


#pragma names save
#pragma names uppercase
extern void eib$$db_attach(void *, char *);
extern void eib$$db_detach(void *);
extern void eib$$db_commit(void *);
extern void eib$$db_rollback(void *);
extern void eib$$db_exec_immediate(void *, char *);
extern void eib$$db_exec(void *, long *, void *, void *);
extern void eib$$db_set_readonly(void *);
extern void eib$$db_release(void *, long *);
extern void eib$$db_prepare(void *, long *, char *);
extern void eib$$db_open_cursor(void *, char *, void *);
extern void eib$$db_close_cursor(void *, char *);
extern void eib$$db_declare(void *, char *, long *);
extern void eib$$db_describe_select(void *, long *, void *);
extern void eib$$db_describe_markers(void *, long *, void *);
extern void eib$$db_fetch(void *, char *, void *);
#pragma names restore



static char errstr[256] = "";




static char *eib_DB_ErrorString()
{
	static char 			str[256];
	unsigned short 			pos;
	unsigned short 			len;
	struct dsc$descriptor_s 	dsc;

    	dsc.dsc$w_length  = sizeof(str) - 1;
    	dsc.dsc$b_class   = DSC$K_CLASS_S;
    	dsc.dsc$b_dtype   = DSC$K_DTYPE_T;
    	dsc.dsc$a_pointer = str;

    	sql$get_error_text(&dsc);

	str$trim(&dsc, &dsc, &len);
	str[len] = '\0';
    	return ( (char *)str );
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

int Sqlcode()
{
	return (SQLCA.SQLCODE);
}

const char *Error()
{
	return (errstr);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

int Attach(const char *db)
{
	char *			tmp = NULL;
	int 			rv = 0;

//
	int old = -1, idx = -1;
   	if ((idx = decc$feature_get_index("DECC$UNIX_LEVEL")) >= 0)
    	{
           old = decc$feature_set_value(idx, 1, 0);
        }
//

	errstr[0] = '\0';

	if (db == NULL || ! strlen(db))
	{
	   sprintf(errstr, "Invalid database name");
	   rv = -1;
	   goto hell;
	}

	tmp = malloc((strlen(db) + 16) * sizeof(char));
    assert(tmp);
	sprintf(tmp, "filename %s", db);

	eib$$db_attach(&SQLCA, tmp);
	free(tmp);

	if (SQLCA.SQLCODE != SQLCODE_SUCCESS) {
	   strcpy(errstr, eib_DB_ErrorString());
	   rv = -1;
	   goto hell;
	}

hell:
   	if (idx >= 0)
	{
           decc$feature_set_value(idx, 1, old);
	}

	return (rv);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

int Rollback()
{
	errstr[0] = '\0';

	eib$$db_rollback(&SQLCA);

	if (SQLCA.SQLCODE != SQLCODE_SUCCESS) {
	   strcpy(errstr, eib_DB_ErrorString());
	   return (-1);
	}

	return (0);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

int Commit()
{
	errstr[0] = '\0';

	eib$$db_commit(&SQLCA);

	if (SQLCA.SQLCODE != SQLCODE_SUCCESS) {
	   strcpy(errstr, eib_DB_ErrorString());
	   return (-1);
	}

	return (0);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

int Exec(void *addr)
{
	stmt_t * 		sp = (stmt_t *) addr;

	eib$$db_exec(&SQLCA, &sp->id, &sp->sqlda_i, &sp->sqlda_o);

	if (SQLCA.SQLCODE != SQLCODE_SUCCESS) {
	   strcpy(errstr, eib_DB_ErrorString());
	   return (-1);
	}

	return (0);
}

int ExecI(char *stmt)
{
	errstr[0] = '\0';
	eib$$db_exec_immediate(&SQLCA, stmt);

	if (SQLCA.SQLCODE != SQLCODE_SUCCESS) {
	   strcpy(errstr, eib_DB_ErrorString());
	   return (-1);
	}

	return (0);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

int Detach()
{
	errstr[0] = '\0';

	eib$$db_detach(&SQLCA);

	if (SQLCA.SQLCODE != SQLCODE_SUCCESS) {
	   strcpy(errstr, eib_DB_ErrorString());
	   return (-1);
	}

	return (0);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

static int eib_DB_Alloc(sqlda_t *sqlda_o)
{
	short 			type;
    	int 			i;

	errstr[0] = '\0';

	for (i = 0; i < sqlda_o->sqld; i++)
	{
	   sqlda_o->sqlvar[i].sqlnind = malloc(sizeof(int));
       assert(sqlda_o->sqlvar[i].sqlnind);
	   type = sqlda_o->sqlvar[i].sqltype;

	   switch (type)
	   {
	      case SQLDA_VARCHAR:
          sqlda_o->sqlvar[i].sqldata = malloc(sizeof(vc_t));
		 assert(sqlda_o->sqlvar[i].sqldata);
	         break;

	      case SQLDA_CHAR:
	      case SQLDA_INTEGER:
	      case SQLDA_SMALLINT:
	      case SQLDA_TINYINT:
	      case SQLDA_FLOAT:
              case SQLDA_SEGSTRING:
              case SQLDA_QUADWORD:
	      case SQLDA_DATE:
	      case SQLDA2_DATETIME:
          sqlda_o->sqlvar[i].sqldata = malloc(sqlda_o->sqlvar[i].sqldlen);
		 assert(sqlda_o->sqlvar[i].sqldata);
		 break;

	      default:
	         abort(); 		/* Unknown data type */
		 break;
	   }
	}

	return (0);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

int SetReadonly()
{
	errstr[0] = '\0';

        eib$$db_set_readonly(&SQLCA);

	if (SQLCA.SQLCODE != SQLCODE_SUCCESS) {
	   strcpy(errstr, eib_DB_ErrorString());
	   return (-1);
	}

	return (0);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

int Free(void *addr)
{
	stmt_t * 		sp = (stmt_t *) addr;
        int                     i;

        if (sp->id != 0)
        {
           eib$$db_release(&SQLCA, &sp->id);
        }

        sp->id = 0;

        for (i = 0; i < MAXARGS; i++)
        {
           if (sp->sqlda_o.sqlvar[i].sqlnind != NULL)
           {
              free(sp->sqlda_o.sqlvar[i].sqlnind);
           }

           if (sp->sqlda_o.sqlvar[i].sqldata != NULL)
           {
              free(sp->sqlda_o.sqlvar[i].sqldata);
           }
        }

        for (i = 0; i < MAXARGS; i++)
        {
           if (sp->sqlda_i.sqlvar[i].sqlnind != NULL)
           {
              free(sp->sqlda_i.sqlvar[i].sqlnind);
           }

           if (sp->sqlda_i.sqlvar[i].sqldata != NULL)
           {
              free(sp->sqlda_i.sqlvar[i].sqldata);
           }
        }

	free(sp);
	return (0);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

int OpenCursor(void *addr)
{
	stmt_t * 		sp = (stmt_t *) addr;

	errstr[0] = '\0';

	eib$$db_open_cursor(&SQLCA, sp->cursor, &sp->sqlda_i);

	if (SQLCA.SQLCODE != SQLCODE_SUCCESS) {
	   strcpy(errstr, eib_DB_ErrorString());
	   return (-1);
	}

	return (0);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

int CloseCursor(void *addr)
{
	stmt_t * 		sp = (stmt_t *) addr;

	errstr[0] = '\0';

	eib$$db_close_cursor(&SQLCA, sp->cursor);

	if (SQLCA.SQLCODE != SQLCODE_SUCCESS) {
	   strcpy(errstr, eib_DB_ErrorString());
	   return (-1);
	}

	return (0);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

void *Prepare(char *stmt)
{
	stmt_t *sp = NULL;
    	int 			i;
	int 			ss;		/* Not really used */

     	errstr[0] = '\0';

	sp = calloc(1, sizeof(stmt_t));
    assert(sp);

	sp->sqlda_i.sqln = MAXARGS;
	sp->sqlda_o.sqln = MAXARGS;

        strncpy(&sp->sqlda_i.sqldaid[0], "SQLDA2  ", 8);
        strncpy(&sp->sqlda_o.sqldaid[0], "SQLDA2  ", 8);

	eib$$db_prepare(&SQLCA, &sp->id, stmt);

 	if (SQLCA.SQLCODE != SQLCODE_SUCCESS)
	{
	   strcpy(errstr, eib_DB_ErrorString());
	   free(sp);
	   return (NULL);
	}

	eib$$db_describe_select(&SQLCA, &sp->id, &sp->sqlda_o);

 	if (SQLCA.SQLCODE != SQLCODE_SUCCESS)
	{
	   strcpy(errstr, eib_DB_ErrorString());
	   free(sp);
	   return (NULL);
	}

	eib$$db_describe_markers(&SQLCA, &sp->id, &sp->sqlda_i);

	if (SQLCA.SQLCODE != SQLCODE_SUCCESS)
	{
	   strcpy(errstr, eib_DB_ErrorString());
	   free(sp);
	   return (NULL);
	}

	ss = SQLCA.SQLERRD[1];

        for (i = 0; i < sp->sqlda_i.sqld; i++)
       	{
               sp->sqlda_i.sqlvar[i].sqldata = malloc(sp->sqlda_i.sqlvar[i].sqldlen * sizeof(char));
	   assert(sp->sqlda_i.sqlvar[i].sqldata);
	   sp->sqlda_i.sqlvar[i].sqlnind = NULL;
	}

	for (i = 0; i < sp->sqlda_o.sqld; i++)
	{
	   if (sp->sqlda_o.sqlvar[i].sqltype == SQLDA2_INTERVAL)
	   {
	      sp->sqlda_o.sqlvar[i].sqltype = SQLDA_VARCHAR;
    	      sp->sqlda_o.sqlvar[i].sqldlen = 100;
	   }
	}

    	return ((void *) sp);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

void *DeclareCursor(char *cursor, char *stmt)
{
	stmt_t * 		sp = NULL;

	errstr[0] = '\0';

#pragma message disable (MAYLOSEDATA2)		/* We know we're getting a 32-bit address */
	if ((sp = Prepare(stmt)) == NULL)
	{
	   return (NULL);
	}
#pragma message enable (MAYLOSEDATA2)

	if (eib_DB_Alloc(&sp->sqlda_o) != 0)
	{
           Free((void *) sp);
	   return (NULL);
	}

	strcpy(sp->cursor, cursor);
	eib$$db_declare(&SQLCA, sp->cursor, &sp->id);

	if (SQLCA.SQLCODE != SQLCODE_SUCCESS)
	{
	   strcpy(errstr, eib_DB_ErrorString());
           Free((void *) sp);
	   return (NULL);
	}

	return ((void *) sp);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

void eib_DB_FreeRow(char **arr)
{
	int i = 0;

	if (arr != NULL)
	{
	   while (arr[i])
	   {
	      free(arr[i]);
	      i++;
	   }
	   free(arr);
       	}
}



char **FetchRow(void *addr)
{
	stmt_t * 		sp = (stmt_t *) addr;
	char ** 		arr = NULL;
	int 			i;
	int 			nc;

	if (Fetch(addr) != 1)
	{
	   return (NULL);
	}

	nc = sp->sqlda_o.sqld;
    arr = calloc((nc + 1), sizeof(char *));
	assert(arr);

	for (i = 0; i < nc; i++)
	{
        arr[i] = strdup(Data(addr, i));
	   assert(arr[i]);
	}

	arr[i] = NULL;
	return (arr);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

int Fetch(void *addr)
{
	stmt_t * 		sp = (stmt_t *) addr;

	eib$$db_fetch(&SQLCA, sp->cursor, &sp->sqlda_o);

	switch (SQLCA.SQLCODE)
	{
	   case SQLCODE_SUCCESS:
	      return (1);
	      break;

	   case SQLCODE_EOS:
	      return (0);
	      break;

	   default:
	      return (-1);
	      break;
	}
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

int NCol(void *addr)
{
	stmt_t *		sp = (stmt_t *) addr;
	return (sp->sqlda_o.sqld);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

static unsigned long dbl_to_str(double val, char *str, int len)
{
	struct dsc$descriptor_sd 	data_desc;
	struct dsc$descriptor_s 	strn_desc;
	unsigned long 			status;

	data_desc.dsc$b_class   = DSC$K_CLASS_SD;
	data_desc.dsc$w_length  = sizeof(double);
	data_desc.dsc$a_pointer = (char *) &val;
	data_desc.dsc$b_scale   = 0;
	data_desc.dsc$b_digits  = 0;
	data_desc.dsc$b_dtype   = DSC$K_DTYPE_G; 	/* G-Floating 64-bit */

	strn_desc.dsc$w_length  = len;
	strn_desc.dsc$b_dtype   = DSC$K_DTYPE_T;
	strn_desc.dsc$b_class   = DSC$K_CLASS_S;
	strn_desc.dsc$a_pointer = str;

	status = lib$cvt_dx_dx(&data_desc, &strn_desc);

	if (OKAY(status))
	{
	   str[strn_desc.dsc$w_length] = '\0';
	}

	return (status);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

static unsigned long flt_to_str(float val, char *str, int len)
{
	struct dsc$descriptor_sd 	data_desc;
	struct dsc$descriptor_s 	strn_desc;
	unsigned long 			status;

	data_desc.dsc$b_class   = DSC$K_CLASS_SD;
	data_desc.dsc$w_length  = sizeof(double);
	data_desc.dsc$a_pointer = (char *) &val;
	data_desc.dsc$b_scale   = 0;
	data_desc.dsc$b_digits  = 0;
	data_desc.dsc$b_dtype   = DSC$K_DTYPE_F; 	/* F-Floating 32-bit */

	strn_desc.dsc$w_length  = len;
	strn_desc.dsc$b_dtype   = DSC$K_DTYPE_T;
	strn_desc.dsc$b_class   = DSC$K_CLASS_S;
	strn_desc.dsc$a_pointer = str;

	status = lib$cvt_dx_dx(&data_desc, &strn_desc);

	if (OKAY(status))
	{
	   str[strn_desc.dsc$w_length] = '\0';
	}

	return (status);
}

/* ------------------------------------------------------------------------------------------------------------------------------ */

char *Data(void *addr, int n)
{
	stmt_t * 			sp = (stmt_t *) addr;
        struct dsc$descriptor_s         dd;
        vc_len_t                	len;
        sc_len_t *                      p;
        char *                  	q;
	struct dsc$descriptor_sd        qd;
	static char 			str[MAXVLEN];
	struct sqlvar * 		var;

	var = &sp->sqlda_o.sqlvar[n];

	if (*var->sqlnind < 0)
        {
	   strcpy(str, "<null>");
        }
        else
        {
           switch (var->sqltype)
           {
              case SQLDA_VARCHAR:
                 len = (vc_len_t) *var->sqldata;

                 if (len < 0)             		/* Hack */
                 {
                    len = 256 + len;
                 }

                 str[len] = '\0';
                 strncpy(str, (char *)(var->sqldata + sizeof(vc_len_t)), len);

                 for (q = str + len - 1; q >= str && *q == ' ' ; q--)
                 {
                    *q = '\0';
                 }

	         break;

	      case SQLDA_CHAR:
                 len = var->sqldlen;
                 str[len] = '\0';
                 strncpy(str, var->sqldata, len);

                 for (q = str + len - 1 ; q >= str && *q == ' ' ; q--)
                 {
                    *q = '\0';
                 }

	         break;

	      case SQLDA_TINYINT:
                 sprintf(str, "%d", *(char *) var->sqldata);
	         break;

	      case SQLDA_FLOAT:
                 if (var->sqldlen == 8)
                 {
                    dbl_to_str(*(double *) var->sqldata, str, sizeof(str) - 1);
                 }
                 else
                 {
                    flt_to_str(*(float  *) var->sqldata, str, sizeof(str) - 1);
                 }

	         break;

	      case SQLDA_DATE:
                 dd.dsc$w_length  = 32;
                 dd.dsc$b_dtype   = DSC$K_DTYPE_T;
                 dd.dsc$b_class   = DSC$K_CLASS_S;
                 dd.dsc$a_pointer = str;

                 sys$asctim(0, &dd, var->sqldata, 0);
	         str[20] = '\0';
                 if (str[0] == ' ') str[0] = '0';

	         break;

              case SQLDA_SMALLINT:
              case SQLDA_QUADWORD:
              case SQLDA_INTEGER:
                 p = (sc_len_t *) &var->sqldlen;

                 dd.dsc$w_length  = 64;
                 dd.dsc$b_dtype   = DSC$K_DTYPE_T;
                 dd.dsc$b_class   = DSC$K_CLASS_S;
                 dd.dsc$a_pointer = str;

                 qd.dsc$b_class   = DSC$K_CLASS_SD;
                 qd.dsc$w_length  = p[0];
                 qd.dsc$a_pointer = var->sqldata;
                 qd.dsc$b_scale   = -1 * p[1];
                 qd.dsc$b_digits  = 0;

                 switch (var->sqltype)
                 {
                    case SQLDA_SMALLINT:
                       qd.dsc$b_dtype = DSC$K_DTYPE_W;
                       break;

                    case SQLDA_QUADWORD:
                       qd.dsc$b_dtype = DSC$K_DTYPE_Q;
                       break;

                    case SQLDA_INTEGER:
                       qd.dsc$b_dtype = DSC$K_DTYPE_L;
                       break;
                 }

                 lib$cvt_dx_dx(&qd, &dd, &dd.dsc$w_length);
                 str[dd.dsc$w_length] = '\0';
	         break;

	      default:
	         strcpy(str, "<null>");
	         break;
	   }
	}

	return (str);
}

