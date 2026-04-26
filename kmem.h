#ifndef KMEM_H
#define KMEM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化 mock 内核内存环境。
 *
 * @param mock_base_addr 模拟“内核基地址”（用于地址映射）
 * @param mock_size      模拟内存大小（字节）
 * @return true 成功，false 失败
 */
bool kmem_mock_init(uint64_t mock_base_addr, size_t mock_size);

/**
 * @brief 释放 mock 内核内存环境。
 */
void kmem_mock_deinit(void);

/**
 * @brief 读取内核地址的 int32_t（mock）。
 */
bool kread_int(uint64_t kaddr, int *out_value);

/**
 * @brief 读取内核地址的 float（mock）。
 */
bool kread_float(uint64_t kaddr, float *out_value);

/**
 * @brief 读取内核地址的任意字节（mock）。
 */
bool kread_bytes(uint64_t kaddr, void *out_buf, size_t size);

/**
 * @brief 写入 int32_t 到内核地址（mock）。
 */
bool kwrite_int(uint64_t kaddr, int value);

/**
 * @brief 写入 float 到内核地址（mock）。
 */
bool kwrite_float(uint64_t kaddr, float value);

/**
 * @brief （测试辅助）把字节写入 mock 内存。
 */
bool kmem_mock_write_bytes(uint64_t kaddr, const void *src, size_t size);

#ifdef __cplusplus
}
#endif

#endif // KMEM_H
