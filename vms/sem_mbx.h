#ifndef _SEM_MBX_DEFINED
#define _SEM_MBX_DEFINED

#ifdef __cplusplus
extern "C" {}
#endif

/* Data structure describing a semaphores.  */
typedef struct {
    unsigned short int channel;
    int fd;
} sem_t_mbx;

#define SEM_FAILED_MBX ((sem_t_mbx *)-1)
#define SEM_VALUE_MAX_MBX   INT_MAX

/* Open a named semaphore */
sem_t_mbx *sem_open_mbx (const char *name, int oflag, ...);
/* close the semaphore opened through sem_open */
int sem_close_mbx (sem_t_mbx *sem);
/* Lock the semaphore (blocking) */
int sem_wait_mbx (sem_t_mbx *sem);
/* Lock the specified semaphore (non-blocking) */
int sem_trywait_mbx (sem_t_mbx *sem);
/* Unlock the specified semaphore */
int sem_post_mbx (sem_t_mbx *sem);
/* Get the semaphore value */
int sem_getvalue_mbx (sem_t_mbx *sem, int *sval);

#ifdef __cplusplus
}
#endif

#endif // _SEM_MBX_DEFINED
