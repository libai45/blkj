#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    pid_t pid;
    char name[128];
} process_info_t;

/**
 * Enumerate all visible processes on the device.
 *
 * The caller owns the returned array and must free it with free_process_list().
 *
 * @param count Output count of returned processes.
 * @return Heap-allocated array of process_info_t, or NULL on failure.
 */
process_info_t *copy_process_list(int *count);

/**
 * Free an array returned by copy_process_list().
 *
 * @param list Array to free.
 */
void free_process_list(process_info_t *list);

#ifdef __cplusplus
}
#endif

#endif /* PROCESS_LIST_H */
