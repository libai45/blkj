#include "kmem.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t *buffer;
    uint64_t base;
    size_t size;
    bool inited;
} kmem_mock_ctx_t;

static kmem_mock_ctx_t g_kmem = {0};

static bool kmem_translate(uint64_t kaddr, size_t access_size, size_t *out_offset) {
    if (!g_kmem.inited || !g_kmem.buffer || !out_offset) {
        return false;
    }

    if (access_size == 0) {
        return false;
    }

    if (kaddr < g_kmem.base) {
        return false;
    }

    uint64_t offset64 = kaddr - g_kmem.base;
    if (offset64 > (uint64_t)SIZE_MAX) {
        return false;
    }

    size_t offset = (size_t)offset64;
    if (offset > g_kmem.size) {
        return false;
    }

    if (access_size > g_kmem.size - offset) {
        return false;
    }

    *out_offset = offset;
    return true;
}

bool kmem_mock_init(uint64_t mock_base_addr, size_t mock_size) {
    if (mock_size == 0) {
        return false;
    }

    kmem_mock_deinit();

    uint8_t *buf = (uint8_t *)calloc(mock_size, sizeof(uint8_t));
    if (!buf) {
        return false;
    }

    g_kmem.buffer = buf;
    g_kmem.base = mock_base_addr;
    g_kmem.size = mock_size;
    g_kmem.inited = true;
    return true;
}

void kmem_mock_deinit(void) {
    if (g_kmem.buffer) {
        free(g_kmem.buffer);
    }

    g_kmem.buffer = NULL;
    g_kmem.base = 0;
    g_kmem.size = 0;
    g_kmem.inited = false;
}

bool kread_bytes(uint64_t kaddr, void *out_buf, size_t size) {
    if (!out_buf || size == 0) {
        return false;
    }

    size_t offset = 0;
    if (!kmem_translate(kaddr, size, &offset)) {
        return false;
    }

    memcpy(out_buf, g_kmem.buffer + offset, size);
    return true;
}

bool kread_int(uint64_t kaddr, int *out_value) {
    if (!out_value) {
        return false;
    }

    return kread_bytes(kaddr, out_value, sizeof(int));
}

bool kread_float(uint64_t kaddr, float *out_value) {
    if (!out_value) {
        return false;
    }

    return kread_bytes(kaddr, out_value, sizeof(float));
}

bool kmem_mock_write_bytes(uint64_t kaddr, const void *src, size_t size) {
    if (!src || size == 0) {
        return false;
    }

    size_t offset = 0;
    if (!kmem_translate(kaddr, size, &offset)) {
        return false;
    }

    memcpy(g_kmem.buffer + offset, src, size);
    return true;
}

bool kwrite_int(uint64_t kaddr, int value) {
    return kmem_mock_write_bytes(kaddr, &value, sizeof(int));
}

bool kwrite_float(uint64_t kaddr, float value) {
    return kmem_mock_write_bytes(kaddr, &value, sizeof(float));
}
