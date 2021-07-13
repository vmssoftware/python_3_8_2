#ifndef VMS_SLEEP_H
#define VMS_SLEEP_H

#ifdef __cplusplus
extern "C" {
#endif

int vms_sleep(int microsecs);
void* vms_waiter_init();
void vms_waiter_free(void* waiter);
void vms_waiter_set(void* waiter);
void vms_waiter_clr(void* waiter);
int vms_waiter_wait(void* waiter, int timeout_microseconds);

#ifdef __cplusplus
}
#endif

#endif
