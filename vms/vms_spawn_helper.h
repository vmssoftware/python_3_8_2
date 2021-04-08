#ifndef VMS_SPAWN_HELPER_H
#define VMS_SPAWN_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

int vms_spawn_alloc(unsigned int **pppid, int **ppstatus, unsigned int **ppfinished);
int vms_spawn_finish(unsigned int *pfinished);
int vms_spawn_status(unsigned int pid, int *pstatus, unsigned int *pfinished, int free);

#ifdef __cplusplus
}
#endif

#endif
