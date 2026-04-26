#include "process_list.h"

#include <errno.h>
#include <libproc.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    pid_t pid;
    char name[PROCESS_NAME_MAX_LEN];
} process_info_internal_t;

static void safe_copy_name(char *dst, size_t dst_size, const char *src) {
    if (!dst || dst_size == 0) {
        return;
    }

    if (!src) {
        dst[0] = '\0';
        return;
    }

    size_t n = strnlen(src, dst_size - 1);
    memcpy(dst, src, n);
    dst[n] = '\0';
}

bool process_list_get_all(process_info_t **out_list, size_t *out_count) {
    if (!out_list || !out_count) {
        return false;
    }

    *out_list = NULL;
    *out_count = 0;

    int bytes_needed = proc_listallpids(NULL, 0);
    if (bytes_needed <= 0) {
        return false;
    }

    size_t pid_capacity = (size_t)bytes_needed / sizeof(pid_t);
    if (pid_capacity == 0) {
        return false;
    }

    pid_t *pids = (pid_t *)calloc(pid_capacity, sizeof(pid_t));
    if (!pids) {
        return false;
    }

    int bytes_filled = proc_listallpids(pids, (int)(pid_capacity * sizeof(pid_t)));
    if (bytes_filled <= 0) {
        free(pids);
        return false;
    }

    size_t pid_count = (size_t)bytes_filled / sizeof(pid_t);

    process_info_t *list = (process_info_t *)calloc(pid_count, sizeof(process_info_t));
    if (!list) {
        free(pids);
        return false;
    }

    size_t valid_count = 0;
    for (size_t i = 0; i < pid_count; i++) {
        pid_t pid = pids[i];
        if (pid <= 0) {
            continue;
        }

        char name_buf[PROCESS_NAME_MAX_LEN] = {0};
        int ret = proc_name(pid, name_buf, (uint32_t)sizeof(name_buf));
        if (ret <= 0) {
            continue;
        }

        list[valid_count].pid = pid;
        safe_copy_name(list[valid_count].name, sizeof(list[valid_count].name), name_buf);
        valid_count++;
    }

    free(pids);

    if (valid_count == 0) {
        free(list);
        return false;
    }

    process_info_t *shrunk = (process_info_t *)realloc(list, valid_count * sizeof(process_info_t));
    if (shrunk) {
        list = shrunk;
    }

    *out_list = list;
    *out_count = valid_count;
    return true;
}

bool process_list_find_pid_by_name(const char *process_name, pid_t *out_pid) {
    if (!process_name || !out_pid || process_name[0] == '\0') {
        return false;
    }

    process_info_t *list = NULL;
    size_t count = 0;
    if (!process_list_get_all(&list, &count)) {
        return false;
    }

    bool found = false;
    for (size_t i = 0; i < count; i++) {
        if (strcasecmp(list[i].name, process_name) == 0) {
            *out_pid = list[i].pid;
            found = true;
            break;
        }
    }

    process_list_free(list);
    return found;
}

void process_list_free(process_info_t *list) {
    free(list);
}
