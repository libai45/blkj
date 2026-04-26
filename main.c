#include <inttypes.h>
#include <stdio.h>

#include "kmem.h"

static void print_bytes(const unsigned char *buf, size_t n) {
    for (size_t i = 0; i < n; i++) {
        printf("%02X", buf[i]);
        if (i + 1 < n) {
            printf(" ");
        }
    }
    printf("\n");
}

int main(void) {
    const uint64_t kbase = 0xFFFFFFF007000000ULL;
    const size_t ksize = 0x1000;

    if (!kmem_mock_init(kbase, ksize)) {
        fprintf(stderr, "[Error] kmem_mock_init failed.\n");
        return 1;
    }

    uint64_t addr_int = kbase + 0x100;
    uint64_t addr_float = kbase + 0x200;
    uint64_t addr_bytes = kbase + 0x300;

    if (!kwrite_int(addr_int, 123456)) {
        fprintf(stderr, "[Error] kwrite_int failed.\n");
        kmem_mock_deinit();
        return 1;
    }

    if (!kwrite_float(addr_float, 3.14159f)) {
        fprintf(stderr, "[Error] kwrite_float failed.\n");
        kmem_mock_deinit();
        return 1;
    }

    const unsigned char sample[8] = {0x41, 0x42, 0x43, 0x44, 0x11, 0x22, 0x33, 0x44};
    if (!kmem_mock_write_bytes(addr_bytes, sample, sizeof(sample))) {
        fprintf(stderr, "[Error] kmem_mock_write_bytes failed.\n");
        kmem_mock_deinit();
        return 1;
    }

    int int_val = 0;
    float float_val = 0.0f;
    unsigned char out[8] = {0};

    if (!kread_int(addr_int, &int_val)) {
        fprintf(stderr, "[Error] kread_int failed.\n");
        kmem_mock_deinit();
        return 1;
    }

    if (!kread_float(addr_float, &float_val)) {
        fprintf(stderr, "[Error] kread_float failed.\n");
        kmem_mock_deinit();
        return 1;
    }

    if (!kread_bytes(addr_bytes, out, sizeof(out))) {
        fprintf(stderr, "[Error] kread_bytes failed.\n");
        kmem_mock_deinit();
        return 1;
    }

    printf("[OK] int   @0x%" PRIx64 " = %d\n", addr_int, int_val);
    printf("[OK] float @0x%" PRIx64 " = %.5f\n", addr_float, float_val);
    printf("[OK] bytes @0x%" PRIx64 " = ", addr_bytes);
    print_bytes(out, sizeof(out));

    kmem_mock_deinit();
    return 0;
}
