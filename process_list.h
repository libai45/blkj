#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PROCESS_NAME_MAX_LEN 256

typedef struct {
    pid_t pid;
    char name[PROCESS_NAME_MAX_LEN];
} process_info_t;

/**
 * @brief 枚举系统进程，返回进程列表。
 *
 * @param out_list  输出：进程数组（由函数内部分配，使用 process_list_free 释放）
 * @param out_count 输出：进程数量
 * @return true 成功，false 失败
 */
bool process_list_get_all(process_info_t **out_list, size_t *out_count);

/**
 * @brief 根据进程名查找 PID（精确匹配，不区分大小写）。
 *
 * @param process_name 进程名
 * @param out_pid      输出 PID
 * @return true 找到，false 未找到或参数错误
 */
bool process_list_find_pid_by_name(const char *process_name, pid_t *out_pid);

/**
 * @brief 释放 process_list_get_all 返回的内存。
 */
void process_list_free(process_info_t *list);

#ifdef __cplusplus
}
#endif

#endif // PROCESS_LIST_H
