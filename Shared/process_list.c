#include "process_list.h"

#include <libproc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void copy_name(char *dst, size_t dst_size, const char *src) {
    if (dst == NULL || dst_size == 0) {
        return;
    }

    if (src == NULL) {
        dst[0] = '\0';
        return;
    }

    snprintf(dst, dst_size, "%s", src);
    dst[dst_size - 1] = '\0';
}

process_info_t *copy_process_list(int *count) {
    if (count == NULL) {
        return NULL;
    }

    *count = 0;

    int proc_count = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0);
    if (proc_count <= 0) {
        return NULL;
    }

    pid_t *pids = (pid_t *)malloc((size_t)proc_count * sizeof(pid_t));
    if (pids == NULL) {
        return NULL;
    }

    int filled = proc_listpids(PROC_ALL_PIDS, 0, pids, proc_count * (int)sizeof(pid_t));
    if (filled <= 0) {
        free(pids);
        return NULL;
    }

    int max_items = filled / (int)sizeof(pid_t);
    process_info_t *list = (process_info_t *)calloc((size_t)max_items, sizeof(process_info_t));
    if (list == NULL) {
        free(pids);
        return NULL;
    }

    int out_index = 0;
    for (int i = 0; i < max_items; i++) {
        pid_t pid = pids[i];
        if (pid <= 0) {
            continue;
        }

        char name_buf[PROC_PIDPATHINFO_MAXSIZE] = {0};
        int ret = proc_name(pid, name_buf, sizeof(name_buf));
        if (ret <= 0 || name_buf[0] == '\0') {
            char path_buf[PROC_PIDPATHINFO_MAXSIZE] = {0};
            if (proc_pidpath(pid, path_buf, sizeof(path_buf)) > 0) {
                const char *base = strrchr(path_buf, '/');
                copy_name(list[out_index].name, sizeof(list[out_index].name), base ? base + 1 : path_buf);
            } else {
                copy_name(list[out_index].name, sizeof(list[out_index].name), "Unknown");
            }
        } else {
            copy_name(list[out_index].name, sizeof(list[out_index].name), name_buf);
        }

        list[out_index].pid = pid;
        out_index++;
    }

    free(pids);

    if (out_index == 0) {
        free(list);
        return NULL;
    }

    *count = out_index;
    return list;
}

void free_process_list(process_info_t *list) {
    free(list);
}
