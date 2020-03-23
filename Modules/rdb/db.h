# ifndef __DB_H__
# define __DB_H__

# ifdef __cplusplus
	extern "C" {
#endif

extern char ** 			FetchRow(void *);
extern int 			NCol(void *);
extern int 			Attach(const char *);
extern int 			CloseCursor(void *);
extern int 			Commit();
extern char * 			Data(void *, int);
extern void * 			DeclareCursor(char *, char *);
extern int 			Detach();
extern int 			Exec(void *);
extern int 			ExecI(char *);
extern int 			Fetch(void *);
extern int 			Free(void *);
extern int 			OpenCursor(void *);
extern void * 			Prepare(char *);
extern int 			Rollback();
extern int 			SetReadonly();
extern const char *		Error();
extern int 			Sqlcode();

# ifdef __cplusplus
	}
#endif

# endif
